// 11327217 蔡易勳 11327255 許頌恩 
// 資料結構與演算法：外部合併排序與主索引建構

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
    
    // 比較函數：用於內部排序（按 weight 由大到小）
    bool operator>(const Record& other) const {
        return weight > other.weight;
    }
};

// =========================================================
// 核心邏輯類別：負責外部合併排序與主索引構建
// =========================================================
class GraphManager {
 private:
    // 輔助函數：執行兩路合併 (2-way Merge)，將兩個已排序的檔案合併為一個排序檔
    void mergeTwoFiles(const string& file1, const string& file2, const string& outputFile) {
        ifstream f1(file1, ios::binary);
        ifstream f2(file2, ios::binary);
        ofstream out(outputFile, ios::binary);
        
        Record rec1, rec2;
        bool has1 = readRecord(f1, rec1);
        bool has2 = readRecord(f2, rec2);
        
        // 使用類似 Merge Sort 合併陣列的雙指標寫法
        while (has1 || has2) {
            // 條件設定為 >= 確保當 weight 相同時，優先取 file1 的資料，維持 Stable Sort 特性
            if (has1 && (!has2 || rec1.weight >= rec2.weight)) {
                // 單筆 Block I/O，一次寫入 24 bytes
                out.write(reinterpret_cast<const char*>(&rec1), sizeof(Record));
                has1 = readRecord(f1, rec1);
            } else {
                // 單筆 Block I/O，一次寫入 24 bytes
                out.write(reinterpret_cast<const char*>(&rec2), sizeof(Record));
                has2 = readRecord(f2, rec2);
            }
        }
        
        f1.close();
        f2.close();
        out.close();
    }
    
    // 輔助函數：從二進位檔案讀取單筆 24 bytes 的紀錄
    bool readRecord(ifstream& fin, Record& rec) {
        // 單筆 Block I/O，一次讀取 24 bytes
        if (fin.read(reinterpret_cast<char*>(&rec), sizeof(Record))) {
            return true;
        }
        return false;
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
        
        ifstream fin(inputFile, ios::binary);
        if (!fin) {
            cout << "\n### " << inputFile << " does not exist! ###\n";
            return;
        }
        fin.close();
        
        const int BUFFER_SIZE = 300;  // 記憶體限制：每次最多載入 300 筆紀錄
        
        // ---------------------------------------------------------
        // 階段一：內部排序 (Internal Sort) - 分段生成排序好的 Run 檔案
        // ---------------------------------------------------------
        auto internalStartTime = chrono::high_resolution_clock::now();
        
        vector<string> tempFiles;
        vector<Record> buffer;
        ifstream infile(inputFile, ios::binary);
        int fileIndex = 0;
        
        Record tempRec;
        
        // 持續讀取直到檔案結尾
        // 利用優化後的 readRecord 進行單筆 Block I/O 讀取
        while (readRecord(infile, tempRec)) {
            
            buffer.push_back(tempRec);
            
            // 當緩衝區滿了，進行穩定排序 (保持權重相同者的原始次序)，並輸出為暫存檔 (.bin)
            if (buffer.size() == BUFFER_SIZE) {
                stable_sort(buffer.begin(), buffer.end(), 
                    [](const Record& a, const Record& b) {
                        return a.weight > b.weight;
                    });
                
                string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
                tempFiles.push_back(tempFile);
                
                ofstream outtemp(tempFile, ios::binary);
                for (const auto& rec : buffer) {
                    // 單筆 Block I/O，一次寫出 24 bytes
                    outtemp.write(reinterpret_cast<const char*>(&rec), sizeof(Record));
                }
                outtemp.close();
                
                buffer.clear();
                fileIndex++;
            }
        }
        
        // 處理未滿 BUFFER_SIZE 的最後一批殘餘紀錄
        if (!buffer.empty()) {
            stable_sort(buffer.begin(), buffer.end(), 
                [](const Record& a, const Record& b) {
                    return a.weight > b.weight;
                });
            
            string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
            tempFiles.push_back(tempFile);
            
            ofstream outtemp(tempFile, ios::binary);
            for (const auto& rec : buffer) {
                // 單筆 Block I/O，一次寫出 24 bytes
                outtemp.write(reinterpret_cast<const char*>(&rec), sizeof(Record));
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
                    dst << src.rdbuf();
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
        
        // 最終合併完成，將唯一的暫存檔轉正
        if (tempFiles.size() == 1) {
            // 【優化點 2】：Rename 取代最後一輪複製
            remove(outputFile.c_str()); // 確保目標檔不存在，避免 rename 失敗
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
        
        // 循序分批讀取排序檔，避免將整個檔案載入記憶體
        // 【優化點 1】：利用優化後的 readRecord 進行單筆 Block I/O 讀取
        while (readRecord(fin, tempRec)) {
            
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
            return 0;
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

// 修剪傳入字串頭尾的空白字元
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