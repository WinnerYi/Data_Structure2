// 11327217 蔡易勳 11327255 許頌恩 
// 圖形資料處理系統：用於分析學生訊息傳遞網絡的連通性和影響力

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <functional>
#include <algorithm>

using namespace std;

// ========== 全域輔助函式宣告 ==========
void PrintTitle();           // 顯示主選單
void SkipSpace(string &str); // 移除字串前後空格
string ReadInput();          // 讀取並處理使用者輸入

// =========================================================
// 資料結構定義區 - 圖的相鄰表表示法
// =========================================================

// 相鄰串列節點：用於存儲邊的目標學生ID和邊權值
// getID: 接收訊息的學生ID
// weight: 訊息傳遞的強度（0.0~1.0之間的浮點數）
// next: 指向下一個邊的指標
struct AdjNode {
    string getID;    
    float weight;    
    AdjNode* next;   

    // 建構子：在建立節點時直接初始化成員變數
    AdjNode(string id, float w)  {
        getID = id;
        weight = w;
        next = nullptr;
    }
};

// 學生節點：用於存儲每位學生的資訊和他發出的訊息
// studentID: 學生的學號
// head: 指向該學生發出的第一條邊的指標
struct StudentNode {
    string studentID; 
    AdjNode* head;    

    // 建構子：初始化學生資訊，相鄰串列設為空
    StudentNode(string id){
        studentID = id; 
        head = nullptr;
    }
};

// 訊息記錄：用於外部排序
// putID: 發送者學號（10字符）
// getID: 接收者學號（10字符）
// weight: 訊息量權重（0, 1]之間的浮點數）
struct Record {
    char putID[12];
    char getID[12];
    float weight;
    
    // 比較函數：用於排序（按weight由大到小）
    bool operator>(const Record& other) const {
        return weight > other.weight;
    }
};

// =========================================================
// 核心邏輯類別：負責整個圖形資料的建立、管理與查詢
// =========================================================
// 該類別使用相鄰表實現有向圖，支持邊權值
// 主要功能：1) 建立相鄰表；2) 計算連通數；3) 估計影響值；4) 找前K名影響力最高的學生
class GraphManager {
 private:
    vector<StudentNode> students; // 存儲所有學生的動態陣列，已排序
    int totalNodes;               // 圖中邊的總數
    string currentFileNum;        // 當前所讀檔案的編號

    // ===== 私有輔助函式 =====
    
    // 功能：將固定長度字符陣列轉換為C++ string，移除null終止符
    // 參數：arr - 來自檔案讀取的char陣列（12字節）
    // 回傳：轉換後的字串
    string convertToString(const char* arr) {
        int len = 0;
        while (len < 12 && arr[len] != '\0') {
            len++;
        }
        return string(arr, len);
    }

    // 功能：尋找指定學號的學生，若不存在則新增到students陣列
    // 參數：id - 要尋找或新增的學生ID
    // 回傳：學生在students陣列中的索引值
    // 時間複雜度：O(n) 其中n為當前學生數量
    int findOrInsertStudent(const string& id) {
        for (int i = 0; i < students.size(); i++) {
            if (students[i].studentID == id) {
                return i;
            }
        }
        
        // 學生不存在，新增到陣列
        students.push_back(StudentNode(id));
        return students.size() - 1; 
    }

    // 功能：將邊插入到指定學生的相鄰串列中，保持升序排列
    // 參數：student - 發送訊息的學生；getID - 接收訊息的學生ID；weight - 邊的權值
    // 說明：採用有序插入，使相鄰串列按getID升序排列，便於後續處理
    void insertEdge(StudentNode& student, string getID, float weight) {
        // 建立新的邊節點
        AdjNode* newNode = new AdjNode(getID, weight);
        
        // 情況1：相鄰串列為空，或新節點應插在首位
        if (student.head == nullptr || student.head->getID > getID) {
            newNode->next = student.head;
            student.head = newNode;
            return;
        }
        
        // 情況2：在已存在的節點中尋找合適的插入位置
        AdjNode* curr = student.head;
        while (curr->next != nullptr && curr->next->getID < getID) {
            curr = curr->next;
        }
        
        newNode->next = curr->next;
        curr->next = newNode;
    }

    // 功能：使用快速排序算法將students陣列按studentID升序排列
    // 說明：排序後可以使用二元搜尋來加快查詢效率
    void sortStudents() {
        sort(students.begin(), students.end(), [](const StudentNode& a, const StudentNode& b) {
            return a.studentID < b.studentID;
        });
    }

    // 功能：使用二元搜尋在已排序的students陣列中尋找指定學號的索引
    // 參數：id - 要尋找的學生ID
    // 回傳：找到時返回索引值；未找到時返回-1
    // 時間複雜度：O(log n) 其中n為學生數量
    int getStudentIndex(const string& id) {
        int left = 0, right = students.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (students[mid].studentID == id) return mid;
            else if (students[mid].studentID < id) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

 public:
    // ===== 公有成員函式 =====
    
    // GraphManager 建構子：初始化系統
    GraphManager() {
        totalNodes = 0;
        currentFileNum = "";
    }

    // 功能：清空所有資料並釋放動態分配的記憶體
    // 說明：執行新任務前應先調用此函式清理舊資料
    void reSet() {
        // 釋放每個學生的相鄰串列
        for (int i = 0; i < students.size(); i++) {
            AdjNode* curr = students[i].head;
            while (curr != nullptr) {
                AdjNode* temp = curr;
                curr = curr->next;
                delete temp;  // 釋放每個邊節點
            }
        }
        students.clear();  // 清空學生陣列
        totalNodes = 0;    // 重置邊計數
    }

    // ========== 任務一：建立相鄰表 ==========
    // 功能：讀取二進位檔案，建立有向圖的相鄰表表示，並輸出到檔案
    // 流程：1) 逐筆讀取邊資料
    //      2) 動態新增節點（若不存在）
    //      3) 有序插入邊到相鄰串列
    //      4) 排序學生陣列
    //      5) 輸出相鄰表到檔案
    void buildAdjacencyLists(string file_num) {
        string filename = "pairs" + file_num;
        string inFile = filename + ".bin";
        string outFile = filename + ".adj";

        ifstream fin(inFile, ios::binary);
        if (fin.fail()) {
            cout << "\n### " << inFile << " does not exist! ###\n";
            return;
        }

        reSet(); // 每次讀檔前清空上一筆資料
        currentFileNum = file_num;

        char putID[12];  // 發送者ID
        char getID[12];  // 接收者ID
        float weight;    // 邊的權值

        // 逐欄位讀取避開 Struct Padding，不需使用 #pragma
        // 每筆記錄：putID(12 bytes) + getID(12 bytes) + weight(4 bytes)
        while (fin.read(putID, 12) && 
               fin.read(getID, 12) && 
               fin.read(reinterpret_cast<char*>(&weight), sizeof(float))) {
            
            // 轉換char陣列為string
            string strPutID = convertToString(putID);
            string strGetID = convertToString(getID);

            // 尋找或新增發送者
            int putIdx = findOrInsertStudent(strPutID);
            // 尋找或新增接收者
            findOrInsertStudent(strGetID); 

            // 將邊插入發送者的相鄰串列（有序）
            insertEdge(students[putIdx], strGetID, weight);
            totalNodes++;  // 邊數加一
        }
        fin.close();

        // 排序主陣列以便後續使用二元搜尋
        sortStudents();

        // 輸出相鄰表到檔案
        ofstream fout(outFile);
        if (fout) {
            fout << "<<< There are " << students.size() << " IDs in total. >>>\n";
            for (int i = 0; i < students.size(); i++) {
                const StudentNode& student = students[i];
                fout << "[" << setw(3) << right << (i + 1) << "] " << student.studentID << ": \n";
                
                AdjNode* curr = student.head;
                if (curr != nullptr) {
                    fout << "\t";
                    int edgeCount = 1;
                    // 輸出所有出邊（最多12個為一行）
                    while (curr != nullptr) {
                        // 依照指定格式輸出：(序號) 接收者ID,權值
                        fout << "(" << setw(2) << right << edgeCount << ") " << curr->getID << "," << setw(7) << right << curr->weight;
                        if (edgeCount % 12 == 0) {fout << "\n";}  // 每12個邊換行
                        curr = curr->next;
                        if (curr != nullptr) fout << "\t";
                        
                            
                        
                        edgeCount++;
                    }
                    fout << "\n";
                }
            }
            fout << "<<< There are " << totalNodes << " nodes in total. >>>\n";
            fout.close();
        }

        cout << "\n<<< There are " << students.size() << " IDs in total. >>>\n";
        cout << "\n<<< There are " << totalNodes << " nodes in total. >>>\n";
    }

    // ========== 任務二：計算連通數 ==========
    // 功能：對每個學生執行BFS，找出他能連通到的所有學生數量
    // 輸出格式：按連通數由大到小排序（連通數相同則按學號由小到大）
    void computeConnectionCounts() {
        if (students.empty() || currentFileNum.empty()) {
            cout << "### There is no graph and choose 1 first. ###\n";
            return;
        }

        // 結果結構：存儲發訊者、連通數和連通到的接收者
        struct ResultNode {
            string senderID;
            int count;
            vector<string> receivers;
        };

        vector<ResultNode> results(students.size());

        // 為每個學生進行BFS
        for (int i = 0; i < students.size(); i++) {
            results[i].senderID = students[i].studentID;
            
            vector<bool> visited(students.size(), false);  // 記錄已訪問的節點
            vector<string> q;   // BFS隊列，存儲學生ID
            int head = 0;  // 隊列頭指標
            
            // 初始化：從第i個學生開始BFS
            q.push_back(students[i].studentID);
            visited[i] = true;

            // BFS主迴圈
            while (head < q.size()) {
                string currID = q[head++];
                int currIdx = getStudentIndex(currID);
                
                if (currIdx != -1) {
                    // 訪問currIdx的所有出邊
                    AdjNode* curr = students[currIdx].head;
                    while (curr != nullptr) {
                        int nextIdx = getStudentIndex(curr->getID);
                        
                        // 若未訪問過，標記為已訪問並加入隊列
                        if (nextIdx != -1 && !visited[nextIdx]) {
                            visited[nextIdx] = true;
                            q.push_back(students[nextIdx].studentID);
                            results[i].receivers.push_back(students[nextIdx].studentID);
                        }
                        curr = curr->next;
                    }
                }
            }
            results[i].count = results[i].receivers.size();
            
            // 將收訊者學號依照字串由小到大排序
            sort(results[i].receivers.begin(), results[i].receivers.end());
        }
        
        // 將發訊者學號依照連通數由大到小排序 (若連通數相同則以學號由小到大)
        sort(results.begin(), results.end(), [](const ResultNode& a, const ResultNode& b) {
            if (a.count != b.count) return a.count > b.count;
            return a.senderID < b.senderID;
        });
        
        // 寫入檔案
        string outFile = "pairs" + currentFileNum + ".cnt";
        ofstream fout(outFile);
        if (fout) {
            fout << "<<< There are " << results.size() << " IDs in total. >>>\n";
            for (int i = 0; i < results.size(); i++) {
                fout << "[" << setw(3) << right << (i + 1) << "] " << results[i].senderID << "(" << results[i].count << "): \n";
                if (results[i].count > 0) {
                    fout << "\t";
                    // 輸出所有連通到的接收者（最多12個為一行）
                    for (int j = 0; j < results[i].receivers.size(); j++) {
                        
                        fout << "(" << setw(2) << right << (j + 1) << ") " << results[i].receivers[j];
                        if ((j + 1) % 12 == 0) {fout << "\n";}
                        if (j != results[i].receivers.size() - 1) {
                            
                            fout << "\t";
                        }
                    }
                    fout << "\n";
                }
            }
            fout.close();
            cout << "\n<<< There are " << students.size() << " IDs in total. >>>\n";
        } else {
            cout << "\n### Failed to write to " << outFile << " ###\n";
        }
    }

    // ========== 任務一（新）：外部合併排序 ==========
    // 功能：使用外部合併排序方法按weight由大到小排序二進位檔案
    // 參數：file_num - 檔案編號（如"501"會讀取pairs501.bin）
    //      buffer_size - 緩衝區大小（固定為300條記錄）
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
        
        const int BUFFER_SIZE = 300;  // 緩衝區大小（記錄數）
        
        // 第一階段：讀取原始檔案，分割為多個排序後的臨時檔案（內部排序）
        auto internalStartTime = chrono::high_resolution_clock::now();
        
        vector<string> tempFiles;
        vector<Record> buffer;
        ifstream infile(inputFile, ios::binary);
        int fileIndex = 0;
        
        char putID[12], getID[12];
        float weight;
        
        while (infile.read(putID, 12) && 
               infile.read(getID, 12) && 
               infile.read(reinterpret_cast<char*>(&weight), sizeof(float))) {
            
            Record rec;
            memcpy(rec.putID, putID, 12);
            memcpy(rec.getID, getID, 12);
            rec.weight = weight;
            buffer.push_back(rec);
            
            // 緩衝區滿時，排序並寫入臨時檔案
            if (buffer.size() == BUFFER_SIZE) {
                // 按weight由大到小排序（穩定排序）
                stable_sort(buffer.begin(), buffer.end(), 
                    [](const Record& a, const Record& b) {
                        return a.weight > b.weight;
                    });
                
                string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
                tempFiles.push_back(tempFile);
                
                ofstream outtemp(tempFile, ios::binary);
                for (const auto& rec : buffer) {
                    outtemp.write(rec.putID, 12);
                    outtemp.write(rec.getID, 12);
                    outtemp.write(reinterpret_cast<const char*>(&rec.weight), sizeof(float));
                }
                outtemp.close();
                
                buffer.clear();
                fileIndex++;
            }
        }
        
        // 處理最後一批記錄
        if (!buffer.empty()) {
            stable_sort(buffer.begin(), buffer.end(), 
                [](const Record& a, const Record& b) {
                    return a.weight > b.weight;
                });
            
            string tempFile = "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
            tempFiles.push_back(tempFile);
            
            ofstream outtemp(tempFile, ios::binary);
            for (const auto& rec : buffer) {
                outtemp.write(rec.putID, 12);
                outtemp.write(rec.getID, 12);
                outtemp.write(reinterpret_cast<const char*>(&rec.weight), sizeof(float));
            }
            outtemp.close();
        }
        
        infile.close();
        
        auto internalEndTime = chrono::high_resolution_clock::now();
        auto internalDuration = chrono::duration_cast<chrono::milliseconds>(internalEndTime - internalStartTime);
        
        cout << "\nThe internal sort is completed. Check the initial sorted runs! \n";
        cout << "\nNow there are " << tempFiles.size() << " runs.\n";
        
        // 第二階段：逐步合併排序後的臨時檔案（外部排序）
        auto externalStartTime = chrono::high_resolution_clock::now();
        
        int runCount = tempFiles.size();
        int mergePass = 0;
        while (tempFiles.size() > 1) {
            vector<string> newTempFiles;
            
            // 每次合併2個檔案
            for (size_t i = 0; i < tempFiles.size(); i += 2) {
                if (i + 1 < tempFiles.size()) {
                    // 合併tempFiles[i]和tempFiles[i+1]
                    string outputTemp = "temp_" + file_num + "_p" + to_string(mergePass) + "_" + to_string(newTempFiles.size()) + ".bin";
                    mergeTwoFiles(tempFiles[i], tempFiles[i+1], outputTemp);
                    newTempFiles.push_back(outputTemp);
                    
                    // 刪除原始檔案
                    remove(tempFiles[i].c_str());
                    remove(tempFiles[i+1].c_str());
                } else {
                    // 奇數個檔案，最後一個直接複製
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
            
            tempFiles = newTempFiles;
            mergePass++;
            cout << "Now there are " << tempFiles.size() << " runs.\n";
        }
        
        auto externalEndTime = chrono::high_resolution_clock::now();
        auto externalDuration = chrono::duration_cast<chrono::milliseconds>(externalEndTime - externalStartTime);
        
        // 複製最終結果
        if (tempFiles.size() == 1) {
            ifstream src(tempFiles[0], ios::binary);
            if (!src) {
                cout << "Error: Cannot open temp file " << tempFiles[0] << "\n";
                return;
            }
            ofstream dst(outputFile, ios::binary);
            if (!dst) {
                cout << "Error: Cannot create output file " << outputFile << "\n";
                return;
            }
            dst << src.rdbuf();
            dst.close();
            src.close();
            remove(tempFiles[0].c_str());
        } else if (tempFiles.empty()) {
            cout << "Error: No temp files remain after merge!\n";
        }
        
        auto totalEndTime = chrono::high_resolution_clock::now();
        auto totalDuration = chrono::duration_cast<chrono::milliseconds>(totalEndTime - totalStartTime);
        
        cout << "\nThe execution time ...\n";
        cout << fixed << setprecision(3);
        cout << "Internal Sort = " << (double)internalDuration.count() << " ms\n";
        cout << "External Sort = " << (double)externalDuration.count() << " ms\n";
        cout << "Total Execution Time = " << (double)totalDuration.count() << " ms\n";
    }
    
    // 輔助函數：合併兩個排序檔案
    void mergeTwoFiles(const string& file1, const string& file2, const string& outputFile) {
        ifstream f1(file1, ios::binary);
        ifstream f2(file2, ios::binary);
        ofstream out(outputFile, ios::binary);
        
        Record rec1, rec2;
        bool has1 = readRecord(f1, rec1);
        bool has2 = readRecord(f2, rec2);
        
        while (has1 || has2) {
            if (has1 && (!has2 || rec1.weight > rec2.weight)) {
                out.write(rec1.putID, 12);
                out.write(rec1.getID, 12);
                out.write(reinterpret_cast<const char*>(&rec1.weight), sizeof(float));
                has1 = readRecord(f1, rec1);
            } else {
                out.write(rec2.putID, 12);
                out.write(rec2.getID, 12);
                out.write(reinterpret_cast<const char*>(&rec2.weight), sizeof(float));
                has2 = readRecord(f2, rec2);
            }
        }
        
        f1.close();
        f2.close();
        out.close();
    }
    
    // 輔助函數：讀取一筆記錄
    bool readRecord(ifstream& fin, Record& rec) {
        if (fin.read(rec.putID, 12) && 
            fin.read(rec.getID, 12) && 
            fin.read(reinterpret_cast<char*>(&rec.weight), sizeof(float))) {
            return true;
        }
        return false;
    }
    
    // ========== 任務二：構建主索引 ==========
    // 功能：根據排序後的檔案構建主索引（key, offset）
    // 主要功能：記錄每個不同weight值第一次出現的位置
    void buildPrimaryIndex(string file_num) {
        string sortedFile = "order" + file_num + ".bin";
        
        ifstream fin(sortedFile, ios::binary);
        if (!fin) {
            cout << "\n### " << sortedFile << " does not exist! ###\n";
            cout << "Please run external sort first!\n";
            return;
        }
        
        struct IndexEntry {
            float key;
            int offset;
        };
        
        vector<IndexEntry> index;
        char putID[12], getID[12];
        float weight;
        int offset = 0;
        float lastWeight = -1.0;
        
        // 讀取排序檔案，記錄每個新weight值的位置
        while (fin.read(putID, 12) && 
               fin.read(getID, 12) && 
               fin.read(reinterpret_cast<char*>(&weight), sizeof(float))) {
            
            // 如果weight改變，記錄此位置為索引
            if (weight != lastWeight) {
                index.push_back({weight, offset});
                lastWeight = weight;
            }
            
            offset += 28;  // putID(12) + getID(12) + weight(4) = 28 bytes
        }
        
        fin.close();
        
        cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "Mission 2: Build the primary index \n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "\n<Primary index>: (key, offset)\n";
        
        // 輸出索引（最多100個）
        int count = min(100, (int)index.size());
        for (int i = 0; i < count; i++) {
            cout << "[" << setw(3) << right << (i + 1) << "] (" 
                 << fixed << setprecision(2) << index[i].key << ", " 
                 << index[i].offset << ")\n";
        }
    }

  
};

// =========================================================
// 主程式與全域 UI / 字串處理函式
// =========================================================

// 主程式：程序的入口點
int main() {
    GraphManager gm;  // 建立圖形管理器實例
  
    // 無限迴圈，除非使用者輸入0退出
    while (true) {
        PrintTitle();  // 顯示主選單
        string cmd = ReadInput();  // 讀取使用者命令
        
        if (cmd == "0") {  // 退出程序
            cout << "程式已退出\n";
            return 0; 
        } else {
            // 檢查是否為有效的檔案編號（3位數字）
            if (cmd.length() == 3 && cmd[0] >= '0' && cmd[0] <= '9' && 
                cmd[1] >= '0' && cmd[1] <= '9' && cmd[2] >= '0' && cmd[2] <= '9') {
                // 執行外部合併排序
                gm.externalMergeSort(cmd);
                
                // 排序完成後，詢問是否繼續進行主索引構建
                cout << "\n[0]Quit or [Any other key]continue?\n";
                string continueCmd = ReadInput();
                
                if (continueCmd == "0") {
                    cout << "程式已退出\n";
                    return 0;
                } else {
                    // 構建主索引
                    gm.buildPrimaryIndex(cmd);
                }
            } else {
                cout << "\nInvalid input! Please enter a 3-digit file number or 0 to quit.\n";
            }
        }
        cout << endl;  
    }
}

// 功能：讀取使用者輸入並去除前後空格
// 回傳：處理後的字串
string ReadInput() {
    string input;
    while (1) {
        getline(cin, input);  // 讀取整行輸入
        SkipSpace(input);     // 移除前後空格
        if (input.empty()) continue;  // 如果為空，重新輸入
        else break;
    }
    return input;
}

// 功能：移除字串前後的空格
// 說明：修改傳入的字串引數
void SkipSpace(string &str) {
    // 移除前面的空格
    for (int i = 0; i < str.size(); i++) {
        if (str[i] != ' ') break;
        if (str[i] == ' ') {
            str.erase(str.begin() + i);
            i--;
        }
    }
    // 移除後面的空格
    for (int i = str.size()-1; i >= 0; i--) {
        if (str[i] != ' ') break;
        if (str[i] == ' ') {
            str.erase(str.begin() + i);
        }
    }
}

// 功能：顯示程式的主選單
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