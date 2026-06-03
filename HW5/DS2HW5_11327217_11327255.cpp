// 11327217 蔡易勳 11327255 許頌恩 
// 資料結構與演算法：外部合併排序與主索引建構 (高效能 Block I/O 版)

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <cmath>  // 處理浮點數絕對值與誤差比較

using namespace std;

// ========== 全域輔助函式宣告 ==========
void PrintTitle();           // 顯示主選單與輸入提示
void SkipSpace(string &str); // 移除字串前後的多餘空格
string ReadInput();          // 讀取並處理使用者輸入的整行字串

// =========================================================
// 資料結構定義區
// =========================================================

// 訊息記錄結構：用於外部合併排序時，作為暫存在緩衝區的資料單位
// 10 + 10 + 4 = 24 bytes，天然符合記憶體對齊，不會產生 struct padding
struct Record {
    char putID[10];  // 發送者學號（10 bytes）
    char getID[10];  // 接收者學號（10 bytes）
    float weight;    // 訊息量權重（4 bytes）
};

// 內部排序使用的包裹結構，方便自訂比較函數並保持 Stable Sort
struct SortRecord {
    Record rec;
    int originalOrder; // 紀錄讀入時的順序，用於 Stable Sort

    bool operator<(const SortRecord& other) const {
        if (rec.weight != other.rec.weight) {
            return rec.weight > other.rec.weight; // 權重大的排前面
        }
        // 若權重相等，原本在檔案中較早出現的排前面
        return originalOrder < other.originalOrder; 
    }
};

// =========================================================
// 核心邏輯類別：負責外部合併排序與主索引構建
// =========================================================
class GraphManager {
 private:
    // 輔助函數：執行兩路合併 (2-way Merge) - 具備 Block I/O 讀取優化
    void mergeTwoFiles(const string& file1, const string& file2, const string& outputFile) {
        ifstream f1(file1, ios::binary);
        ifstream f2(file2, ios::binary);
        ofstream out(outputFile, ios::binary);
        
        Record rec1, rec2;
        bool has1 = (bool)f1.read(reinterpret_cast<char*>(&rec1), sizeof(Record));
        bool has2 = (bool)f2.read(reinterpret_cast<char*>(&rec2), sizeof(Record));
        
        // Block I/O 輸出緩衝區
        const int OUT_BUFFER_SIZE = 300;
        vector<Record> outBuffer;
        outBuffer.reserve(OUT_BUFFER_SIZE);

        auto flushBuffer = [&]() {
            if (!outBuffer.empty()) {
                out.write(reinterpret_cast<const char*>(outBuffer.data()), outBuffer.size() * sizeof(Record));
                outBuffer.clear();
            }
        };

        // 使用類似 Merge Sort 合併陣列的雙指標寫法
        while (has1 || has2) {
            // 條件設定為 >= 確保當 weight 相同時，優先取 file1 的資料，維持 Stable Sort 特性
            if (has1 && (!has2 || rec1.weight >= rec2.weight)) {
                outBuffer.push_back(rec1);
                has1 = (bool)f1.read(reinterpret_cast<char*>(&rec1), sizeof(Record));
            } else {
                outBuffer.push_back(rec2);
                has2 = (bool)f2.read(reinterpret_cast<char*>(&rec2), sizeof(Record));
            }

            if (outBuffer.size() == OUT_BUFFER_SIZE) {
                flushBuffer();
            }
        }
        
        flushBuffer(); // 清空剩餘的資料
        
        f1.close();
        f2.close();
        out.close();
    }

 public:
    // GraphManager 建構子
    GraphManager() {}

    // ========== 任務一：外部合併排序 (External Merge Sort) ==========
    // 功能：將無法一次載入記憶體的大型二進位檔案，透過分批讀取與合併的方式進行排序
    void externalMergeSort(string file_num) {
        auto totalStartTime = chrono::high_resolution_clock::now();
        
        string inputFile = "pairs" + file_num + ".bin";
        string outputFile = "order" + file_num + ".bin";
        
        ifstream infile(inputFile, ios::binary);
        if (!infile) {
            cout << "\n### " << inputFile << " does not exist! ###\n";
            return;
        }
        
        const int BUFFER_SIZE = 300;  // 記憶體限制：每次最多載入 300 筆紀錄
        
        // ---------------------------------------------------------
        // 階段一：內部排序 (Internal Sort) - 使用 Block I/O 讀取
        // ---------------------------------------------------------
        auto internalStartTime = chrono::high_resolution_clock::now();
        
        vector<string> tempFiles;
        vector<SortRecord> buffer;
        buffer.reserve(BUFFER_SIZE);
        
        int fileIndex = 0;
        Record tempRec;
        int orderCounter = 0;
        
        // 一次讀取完整的 24 bytes (Block I/O 概念)，取代三次細碎的 read
        while (infile.read(reinterpret_cast<char*>(&tempRec), sizeof(Record))) {
            
            buffer.push_back({tempRec, orderCounter++});
            
            // 當緩衝區滿了，進行穩定排序 (保持權重相同者的原始次序)，並輸出為暫存檔 (.bin)
            if (buffer.size() == BUFFER_SIZE) {
                // 使用 std::sort 搭配 originalOrder 來達成 Stable Sort 效果，速度比 stable_sort 快
                sort(buffer.begin(), buffer.end());
                
                string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
                tempFiles.push_back(tempFile);
                
                // Block I/O 寫入：把整個 Vector 記憶體區塊一次寫入硬碟
                ofstream outtemp(tempFile, ios::binary);
                for(const auto& sr : buffer) {
                    outtemp.write(reinterpret_cast<const char*>(&sr.rec), sizeof(Record));
                }
                outtemp.close();
                
                buffer.clear();
                orderCounter = 0;
                fileIndex++;
            }
        }
        
        // 處理未滿 BUFFER_SIZE 的最後一批殘餘紀錄
        if (!buffer.empty()) {
            sort(buffer.begin(), buffer.end());
            
            string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
            tempFiles.push_back(tempFile);
            
            ofstream outtemp(tempFile, ios::binary);
            for(const auto& sr : buffer) {
                outtemp.write(reinterpret_cast<const char*>(&sr.rec), sizeof(Record));
            }
            outtemp.close();
        }
        
        infile.close();
        
        auto internalEndTime = chrono::high_resolution_clock::now();
        auto internalDuration = chrono::duration_cast<chrono::milliseconds>(internalEndTime - internalStartTime);
        
        cout << "\nThe internal sort is completed. Check the initial sorted runs! \n";
        cout << "\nNow there are " << tempFiles.size() << " runs.\n\n";
        
        // ---------------------------------------------------------
        // 階段二：外部合併 (External Merge) - 兩兩合併暫存檔直到剩下一個
        // ---------------------------------------------------------
        auto externalStartTime = chrono::high_resolution_clock::now();
        
        int mergePass = 0;
        // 只要暫存檔數量大於 1，就繼續進行合併回合 (Pass)
        while (tempFiles.size() > 1) {
            vector<string> newTempFiles;
            
            // 每次取出兩個相鄰的檔案進行合併
            for (size_t i = 0; i < tempFiles.size(); i += 2) {
                if (i + 1 < tempFiles.size()) {
                    string outputTemp = "temp_" + file_num + "_p" + to_string(mergePass) + "_" + to_string(newTempFiles.size()) + ".bin";
                    mergeTwoFiles(tempFiles[i], tempFiles[i+1], outputTemp);
                    newTempFiles.push_back(outputTemp);
                    
                    // 合併完成後，刪除舊的暫存檔以節省硬碟空間
                    remove(tempFiles[i].c_str());
                    remove(tempFiles[i+1].c_str());
                } else {
                    // 若該回合為奇數個檔案，最後一個檔案無法配對，直接複製留到下一回合
                    string outputTemp = "temp_" + file_num + "_p" + to_string(mergePass) + "_" + to_string(newTempFiles.size()) + ".bin";
                    ifstream src(tempFiles[i], ios::binary);
                    ofstream dst(outputTemp, ios::binary);
                    dst << src.rdbuf(); // 快速複製檔案技巧
                    dst.close();
                    src.close();
                    newTempFiles.push_back(outputTemp);
                    remove(tempFiles[i].c_str());
                }
            }
            
            tempFiles = newTempFiles; // 更新暫存檔清單為新產生的一批
            mergePass++;
            cout << "Now there are " << tempFiles.size() << " runs.\n";
            if (tempFiles.size() > 1) printf("\n");
        }
        
        auto externalEndTime = chrono::high_resolution_clock::now();
        auto externalDuration = chrono::duration_cast<chrono::milliseconds>(externalEndTime - externalStartTime);
        
        // 最終合併完成，將唯一的暫存檔重新命名（複製）為正式的輸出檔 orderXXX.bin
        if (tempFiles.size() == 1) {
            // 直接更名檔案 (Rename)，比用 rdbuf() 複製快上無數倍！
            remove(outputFile.c_str()); // 確保舊檔案不存在
            rename(tempFiles[0].c_str(), outputFile.c_str());
        }
        
        auto totalEndTime = chrono::high_resolution_clock::now();
        auto totalDuration = chrono::duration_cast<chrono::milliseconds>(totalEndTime - totalStartTime);
        
        // 輸出效能測量結果 (強制保留至小數點後三位)
        cout << "\nThe execution time ...\n";
        cout << fixed << setprecision(3);
        cout << "Internal Sort = " << (double)internalDuration.count() << " ms\n";
        cout << "External Sort = " << (double)externalDuration.count() << " ms\n";
        cout << "Total Execution Time = " << (double)totalDuration.count() << " ms\n";
    }
    
    // ========== 任務二：構建主索引 (Primary Index) ==========
    // 功能：掃描排序完成的檔案，記錄每種「量化權重」第一次出現時的「檔案位址(筆數)」
    void buildPrimaryIndex(string file_num) {
        string sortedFile = "order" + file_num + ".bin";
        
        ifstream fin(sortedFile, ios::binary);
        if (!fin) {
            cout << "\n### " << sortedFile << " does not exist! ###\n";
            cout << "Please run external sort first!\n";
            return;
        }
        
        struct IndexEntry {
            float key;     // 索引鍵值：量化權重
            int offset;    // 檔案位址：紀錄的 Index (第幾筆，從 0 開始)
        };
        
        vector<IndexEntry> index;
        Record tempRec;
        int offset = 0;
        float lastWeight = -1.0;
        
        // 同樣採用 Block I/O 概念，一次讀出整個 24 bytes
        while (fin.read(reinterpret_cast<char*>(&tempRec), sizeof(Record))) {
            // 當偵測到權重改變時（新權重群組的開頭），即將當下 offset 寫入索引表
            if (tempRec.weight != lastWeight) {
                index.push_back({tempRec.weight, offset});
                lastWeight = tempRec.weight;
            }
            offset++;  // 累加紀錄筆數
        }
        
        fin.close();
        
        cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "Mission 2: Build the primary index \n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "\n<Primary index>: (key, offset)\n";
        
        // 依規定輸出前 100 筆索引紀錄
        int count = min(100, (int)index.size());
        for (int i = 0; i < count; i++) {
            cout << "[" << (i + 1) << "] (";
            
            // 輸出格式化：利用 defaultfloat 還原預設浮點數行為，並限制最多顯示到小數第二位。
            // 這樣 C++ 會自動移除結尾無意義的 0 (例如: 0.90 -> 0.9，1.00 -> 1)
            cout << defaultfloat << setprecision(2) << index[i].key;
            cout << ", " << index[i].offset << ")\n";
        }
    }
};

// =========================================================
// 主程式與全域 UI / 字串處理函式
// =========================================================

int main() {
    // 釋放 C++ 標準串流的效能束縛，大幅提升 cout 速度
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    GraphManager gm;  
  
    while (true) {
        PrintTitle();  // 顯示選單與「Input the file name: [0]Quit」
        
        string cmd;
        // 內層迴圈：專門處理讀取與驗證檔案名稱的狀態
        while (true) {
            cmd = ReadInput();  
            
            if (cmd == "0") {  
                break;  // 使用者輸入 0，跳出讀檔程序，進入是否結束程式的最後詢問
            } 
            
            bool fileExists = false;
            
            // 防呆檢查步驟一：驗證字串是否剛好為 3 位數的純數字
            if (cmd.length() == 3 && cmd[0] >= '0' && cmd[0] <= '9' && 
                cmd[1] >= '0' && cmd[1] <= '9' && cmd[2] >= '0' && cmd[2] <= '9') {
                
                // 防呆檢查步驟二：實際嘗試開啟對應的 pairsXXX.bin 檔案
                // 這是為了滿足「若輸入格式正確但檔案不存在，需重新輸入」的測資要求
                string filename = "pairs" + cmd + ".bin";
                ifstream testFile(filename, ios::binary);
                if (testFile.is_open()) {
                    testFile.close();
                    fileExists = true;  // 確認實體檔案確實存在
                }
            }
            
            // 根據驗證結果決定後續流程
            if (fileExists) {
                // 檔案存在且有效，執行指定的任務功能
                gm.externalMergeSort(cmd);
                gm.buildPrimaryIndex(cmd);
                
                break;  // 任務執行完畢，跳出讀檔迴圈，進入是否繼續的詢問
            } else {
                // 檔案無效 (無論是格式錯或實體找不到)，僅印出錯誤提示並等待下一輪重新輸入
                cout << "\npairs" << cmd << ".bin does not exist!!!\n\n";
                cout << "Input the file name: [0]Quit\n";
            }
        }
        
        // 任務完成，或使用者在輸入階段輸入了 0 中斷，都會統一在此進行確認
        cout << "\n[0]Quit or [Any other key]continue?\n";
        string continueCmd = ReadInput();
        
        if (continueCmd == "0") {
            return 0; // 正式結束程式 (不另外印出結束訊息以符合測資要求)
        } 
        
        cout << endl;  
    }
}

// 讀取終端機整行輸入，並忽略掉純空白或按下的 Enter (空字串)
string ReadInput() {
    string input;
    while (1) {
        getline(cin, input);  
        SkipSpace(input);     
        if (input.empty()) continue;  
        else break;
    }
    return input;
}

// 修剪傳入字串頭尾的空白字元 (類似其他語言的 trim 函數)
void SkipSpace(string &str) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] != ' ') break;
        if (str[i] == ' ') {
            str.erase(str.begin() + i);
            i--;
        }
    }
    for (int i = str.size()-1; i >= 0; i--) {
        if (str[i] != ' ') break;
        if (str[i] == ' ') {
            str.erase(str.begin() + i);
        }
    }
}

// 根據作業規定，強制且唯一顯示在程式開頭的選單介面
void PrintTitle () {
    cout << "* Data Structures and Algorithms *\n";
    cout << "**********************************\n";
    cout << "* 1. External merge sort on file *\n";
    cout << "* 2: Construct the primary index *\n";
    cout << "**********************************\n";
    cout << "*** The buffer size is 300\n";
    cout << "##################################\n";
    cout << "Mission 1: External merge sort \n";
    cout << "##################################\n\n";
    cout << "Input the file name: [0]Quit\n";
}