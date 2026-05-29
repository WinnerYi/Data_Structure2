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

    // ========== 任務三：估計影響值 ==========
    // 功能：根據用戶輸入的權重門檻，使用DFS計算每個學生的影響力
    // 影響力定義：通過權重 >= 門檻的邊能到達的學生數量
    void estimateInfluenceValues() {
        if (students.empty() || currentFileNum.empty()) {
            cout << "### There is no graph and choose 1 first. ###\n";
            return;
        }

        // 讀取並驗證門檻值 [0.9, 1.0]
        float threshold;
        while (1) {
            cout << "\nInput a real number in [0.9,1.0]: ";
            string input = ReadInput();
            
            // 嘗試解析浮點數
            try {
                size_t idx = 0;
                threshold = stof(input, &idx);
                
                // 檢查是否整個字串都被解析為數字
                if (idx != input.length()) {
                    cout << "\n### It is NOT in [0.9,1.0] ###\n";
                    continue;
                }
                
                // 檢查是否在有效範圍內
                if (threshold <= 0.9 || threshold > 1.0) {
                    cout << "\n### It is NOT in [0.9,1.0] ###\n";
                    continue;
                }
                
                break;
            } catch (...) {
                // 解析失敗，重新輸入
                continue;
            }
        }

        // 發訊者資訊結構
        struct SenderInfo {
            string senderID;           // 發訊者ID
            vector<string> reachableIDs;  // 可到達的接收者清單
            int influenceValue;        // 影響力值（可到達的接收者數量）
        };

        vector<SenderInfo> senders;

        // 為每個學生進行DFS計算影響力
        for (int i = 0; i < students.size(); i++) {
            vector<bool> visited(students.size(), false);  // 記錄已訪問的節點
            vector<string> reachable;  // 存儲可到達的學生
            
            // DFS：訪問所有可到達的學生（透過權重 >= 門檻的邊）
            function<void(int)> dfs = [&](int idx) {
                visited[idx] = true;
                
                // 遍歷此節點的所有出邊
                AdjNode* curr = students[idx].head;
                while (curr != nullptr) {
                    // 只在邊的權重 >= 門檻時才傳遞訊息
                    if (curr->weight >= threshold) {
                        int nextIdx = getStudentIndex(curr->getID);
                        if (nextIdx != -1 && !visited[nextIdx]) {
                            reachable.push_back(students[nextIdx].studentID);
                            dfs(nextIdx);
                        }
                    }
                    curr = curr->next;
                }
            };
            
            dfs(i);
            
            // 只記錄有影響力的發訊者（可到達 >= 1 個收訊者）
            if (reachable.size() > 0) {
                // 排序收訊者學號（由小到大）
                sort(reachable.begin(), reachable.end());
                
                senders.push_back({students[i].studentID, reachable, (int)reachable.size()});
            }
        }

        // 按影響力由大到小排序，相同則按學號由小到大排序
        sort(senders.begin(), senders.end(), [](const SenderInfo& a, const SenderInfo& b) {
            if (a.influenceValue != b.influenceValue) return a.influenceValue > b.influenceValue;
            return a.senderID < b.senderID;
        });

        // 寫入 .inf 檔案
        string outFile = "pairs" + currentFileNum + ".inf";
        ofstream fout(outFile);
        if (fout) {
            for (int i = 0; i < senders.size(); i++) {
                fout << senders[i].senderID << ": " << senders[i].influenceValue << " {";
                for (int j = 0; j < senders[i].reachableIDs.size(); j++) {
                    if (j > 0) fout << ", ";
                    fout << senders[i].reachableIDs[j];
                }
                fout << "}\n";
            }
            fout.close();
        }

        cout << "\n<<< There are " << senders.size() << " IDs in total. >>>\n";
    }

    // ========== 任務四：找前K名影響力最高的學生 ==========
    // 功能：以固定門檻0.9找出前K名影響力最高的學生，並輸出所有與第K名影響力相等的學生
    // 說明：此任務會計時，測量DFS演算法的執行效能
    void findTopKInfluenceValues() {
        if (students.empty() || currentFileNum.empty()) {
            cout << "### There is no graph and choose 1 first. ###\n";
            return;
        }

        // 計算開始時間
        auto startTime = chrono::high_resolution_clock::now();

        float threshold = 0.9; // 固定門檻

        // 結構：存儲發訊者及其影響力
        struct SenderInfo {
            string senderID;
            int influenceValue;
        };

        vector<SenderInfo> senders;

        // 為每個學生進行DFS計算影響力
        for (int i = 0; i < students.size(); i++) {
            vector<bool> visited(students.size(), false);  // 記錄已訪問的節點
            int reachableCount = 0;  // 可到達的學生計數
            
            // DFS：訪問所有可到達的學生（透過權重 >= 0.9 的邊）
            function<void(int)> dfs = [&](int idx) {
                visited[idx] = true;
                
                // 遍歷此節點的所有出邊
                AdjNode* curr = students[idx].head;
                while (curr != nullptr) {
                    // 只在邊的權重 >= 0.9 時才傳遞訊息
                    if (curr->weight >= threshold) {
                        int nextIdx = getStudentIndex(curr->getID);
                        if (nextIdx != -1 && !visited[nextIdx]) {
                            reachableCount++;  // 增加可到達計數
                            dfs(nextIdx);
                        }
                    }
                    curr = curr->next;
                }
            };
            
            dfs(i);
            
            // 只記錄有影響力的發訊者（可到達 >= 1 個收訊者）
            if (reachableCount > 0) {
                senders.push_back({students[i].studentID, reachableCount});
            }
        }

        // 計算結束時間
        auto endTime = chrono::high_resolution_clock::now();
        long long elapsedMs = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
        cout << "\n[Elapsed time] " << elapsedMs << " ms\n";  // 輸出執行時間

        // 按影響力由大到小排序，相同則按學號由小到大排序
        sort(senders.begin(), senders.end(), [](const SenderInfo& a, const SenderInfo& b) {
            if (a.influenceValue != b.influenceValue) return a.influenceValue > b.influenceValue;
            return a.senderID < b.senderID;
        });

        // 請求用戶輸入K值
        int k;
        while (1) {
            cout << "\nInput an integer to show top-K in [1," << senders.size() << "]: ";
            string input = ReadInput();
            
            try {
                size_t idx = 0;
                k = stoi(input, &idx);

                // 檢查是否為負數
                if (input[0] == '-') continue;
                
                // 檢查整個字串是否都被解析
                if (idx != input.length()) {
                    cout << "\n### " << input << " is NOT in [1," << senders.size() << "] ###\n";
                    continue;
                }
                
                // 檢查K值是否在有效範圍內
                if (k < 1 || k > senders.size()) {
                    cout << "\n### " << k << " is NOT in [1," << senders.size() << "] ###\n";
                    continue;
                }
                
                break;
            } catch (...) {
                // 解析異常，重新輸入
            }
        }

        // 找出第K個的影響力值，以及所有等於此值的發訊者
        if (k > 0 && k <= senders.size()) {
            int kthInfluence = senders[k - 1].influenceValue;  // 第K名的影響力值
            int rankCounter = 1;
            printf("\n");
            
            // 輸出所有影響力 >= K-th值的發訊者
            // 這樣可以顯示所有並列排名的學生
            for (int i = 0; i < senders.size(); i++) {
                if (senders[i].influenceValue >= kthInfluence) {
                    cout << "<" << rankCounter << "> " << senders[i].senderID << ": " << senders[i].influenceValue << "\n";
                    rankCounter++;
                } else {
                    break;  // 影響力遞減，可以停止
                }
            }
        }
    }

    // 任務分配器：根據使用者輸入的命令執行對應的任務
    void doTask(string cmd) { 
        if (cmd == "1") {
            // 任務1：建立相鄰表
            string file_num;
            while (1) {
                cout << "\nInput a file number ([0] Quit): ";
                file_num = ReadInput();
                if (file_num == "0") {
                    reSet();  // 清空舊資料
                    break;
                }
                buildAdjacencyLists(file_num);
                break; 
            }
        } else if (cmd == "2") {
            // 任務2：計算連通數
            computeConnectionCounts();
        } else if (cmd == "3") {
            // 任務3：估計影響值
            estimateInfluenceValues();
        } else if (cmd == "4") {
            // 任務4：找前K名影響力最高的學生
            findTopKInfluenceValues();
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
            return 0; 
        } else if (cmd == "1" || cmd == "2" || cmd == "3" || cmd == "4"){
            // 執行對應的任務
            gm.doTask(cmd); 
        } else {
            cout << "\nCommand does not exist!\n";  // 無效命令
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
    cout << "**** Graph data manipulation *****\n";
    cout << "* 0. QUIT                        *\n";
    cout << "* 1. Build adjacency lists       *\n";
    cout << "* 2. Compute connection counts   *\n";
    cout << "* 3. Estimate influence values   *\n";
    cout << "* 4. Find top-k influence values *\n";
    cout << "**********************************\n";
    cout << "Input a choice(0, 1, 2, 3, 4): ";
}