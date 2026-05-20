// 11327217 蔡易勳 11327255 許頌恩 

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>

using namespace std;

// 全域輔助函式宣告
void PrintTitle();
void SkipSpace(string &str);
string ReadInput();

// =========================================================
// 資料結構定義區 (加入建構子，讓初始化更簡潔)
// =========================================================

// 相鄰串列的節點：紀錄互動關係的收訊者與權重
struct AdjNode {
    string getID;    
    float weight;    
    AdjNode* next;   

    // AdjNode 建構子：建立節點時直接賦值
    AdjNode(string id, float w)  {
        getID = id;
        weight = w;
        next = nullptr;
    }
};

// 主陣列的節點：紀錄每一位學生及其發送訊息的串列開頭
struct StudentNode {
    string studentID; 
    AdjNode* head;    

    // StudentNode 建構子：建立學生時直接賦值，並將串列預設為空
    StudentNode(string id){
        studentID = id; 
        head = nullptr;

    }
};

// =========================================================
// 核心邏輯類別：負責圖的建立、儲存與輸出
// =========================================================
class GraphManager {
 private:
    vector<StudentNode> students; // 動態主陣列
    int totalNodes;               // 總有向邊數
    string currentFileNum;        // 記錄目前的檔案編號

    // 輔助函式：安全地將 char[12] 轉換為 string
    string convertToString(const char* arr) {
        int len = 0;
        while (len < 12 && arr[len] != '\0') {
            len++;
        }
        return string(arr, len);
    }

    // 輔助函式：尋找學生，若無則動態新增並回傳索引值
    int findOrInsertStudent(const string& id) {
        for (int i = 0; i < students.size(); i++) {
            if (students[i].studentID == id) {
                return i;
            }
        }
        
        // 使用建構子，一行完成初始化並推入陣列
        students.push_back(StudentNode(id));
        return students.size() - 1; 
    }

    // 輔助函式：將互動紀錄插入相鄰串列 (由小到大排序)
    void insertEdge(StudentNode& student, string getID, float weight) {
        // 使用建構子，一行完成動態配置與初始化
        AdjNode* newNode = new AdjNode(getID, weight);
        
        if (student.head == nullptr || student.head->getID > getID) {
            newNode->next = student.head;
            student.head = newNode;
            return;
        }
        
        AdjNode* curr = student.head;
        while (curr->next != nullptr && curr->next->getID < getID) {
            curr = curr->next;
        }
        
        newNode->next = curr->next;
        curr->next = newNode;
    }

    // 輔助函式：手刻選擇排序 (取代 std::sort)
    void sortStudents() {
        int n = students.size();
        for (int i = 0; i < n - 1; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                if (students[j].studentID < students[minIdx].studentID) {
                    minIdx = j; 
                }
            }
            if (minIdx != i) {
                StudentNode temp = students[i];
                students[i] = students[minIdx];
                students[minIdx] = temp;
            }
        }
    }

    // 輔助函式：二元搜尋取得學生索引值
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
    // GraphManager 建構子：初始化系統
    GraphManager() {
        totalNodes = 0;
        currentFileNum = "";
    }

    // 執行新任務前，清空舊資料與釋放記憶體
    void reSet() {
        for (int i = 0; i < students.size(); i++) {
            AdjNode* curr = students[i].head;
            while (curr != nullptr) {
                AdjNode* temp = curr;
                curr = curr->next;
                delete temp; 
            }
        }
        students.clear(); 
        totalNodes = 0;   
    }

    // 任務一：讀取二進位檔，建立並輸出相鄰串列
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

        char putID[12];
        char getID[12];
        float weight;

        // 逐欄位讀取避開 Struct Padding，不需使用 #pragma
        while (fin.read(putID, 12) && 
               fin.read(getID, 12) && 
               fin.read(reinterpret_cast<char*>(&weight), sizeof(float))) {
            
            string strPutID = convertToString(putID);
            string strGetID = convertToString(getID);

            int putIdx = findOrInsertStudent(strPutID);
            findOrInsertStudent(strGetID); 

            insertEdge(students[putIdx], strGetID, weight);
            totalNodes++; 
        }
        fin.close();

        // 排序主陣列
        sortStudents();

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
                    while (curr != nullptr) {
                        
                        // 依照指定格式輸出
                        fout << "(" << setw(2) << right << edgeCount << ") " << curr->getID << "," << setw(7) << right << curr->weight;
                        if (edgeCount % 12 == 0) {fout << "\n";}
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

    // 任務二：計算連通數
    void computeConnectionCounts() {
        if (students.empty() || currentFileNum.empty()) {
            cout << "### There is no graph and choose 1 first. ###\n";
            return;
        }

        struct ResultNode {
            string senderID;
            int count;
            vector<string> receivers;
        };

        vector<ResultNode> results(students.size());

        for (int i = 0; i < students.size(); i++) {
            results[i].senderID = students[i].studentID;
            
            vector<bool> visited(students.size(), false);
            vector<string> q;
            int head = 0;
            
            q.push_back(students[i].studentID);
            visited[i] = true;

            while (head < q.size()) {
                string currID = q[head++];
                int currIdx = getStudentIndex(currID);
                
                if (currIdx != -1) {
                    AdjNode* curr = students[currIdx].head;
                    while (curr != nullptr) {
                        string nextID = curr->getID;
                        int nextIdx = getStudentIndex(nextID);
                        
                        if (nextIdx != -1 && !visited[nextIdx]) {
                            visited[nextIdx] = true;
                            q.push_back(nextID);
                            results[i].receivers.push_back(nextID);
                        }
                        curr = curr->next;
                    }
                }
            }
            results[i].count = results[i].receivers.size();
            
            // 將收訊者學號依照字串由小到大排序
            for (int r1 = 0; r1 < results[i].count; r1++) {
                int minR = r1;
                for (int r2 = r1 + 1; r2 < results[i].count; r2++) {
                    if (results[i].receivers[r2] < results[i].receivers[minR]) {
                        minR = r2;
                    }
                }
                if (minR != r1) {
                    string temp = results[i].receivers[r1];
                    results[i].receivers[r1] = results[i].receivers[minR];
                    results[i].receivers[minR] = temp;
                }
            }
        }
        
        // 將發訊者學號依照連通數由大到小排序 (若連通數相同則以學號由小到大)
        for (int i = 0; i < results.size(); i++) {
            int maxIdx = i;
            for (int j = i + 1; j < results.size(); j++) {
                if (results[j].count > results[maxIdx].count) {
                    maxIdx = j;
                } else if (results[j].count == results[maxIdx].count && results[j].senderID < results[maxIdx].senderID) {
                    maxIdx = j;
                }
            }
            if (maxIdx != i) {
                ResultNode temp = results[i];
                results[i] = results[maxIdx];
                results[maxIdx] = temp;
            }
        }
        
        // 寫入檔案
        string outFile = "pairs" + currentFileNum + ".cnt";
        ofstream fout(outFile);
        if (fout) {
            fout << "<<< There are " << results.size() << " IDs in total. >>>\n";
            for (int i = 0; i < results.size(); i++) {
                fout << "[" << setw(3) << right << (i + 1) << "] " << results[i].senderID << "(" << results[i].count << "): \n";
                if (results[i].count > 0) {
                    fout << "\t";
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

    // 任務分配器
    void doTask(string cmd) { 
        if (cmd == "1") {
            string file_num;
            while (1) {
                cout << "\nInput a file number ([0] Quit): ";
                file_num = ReadInput();
                if (file_num == "0") {
                    reSet();
                    break;
                }
                buildAdjacencyLists(file_num);
                break; 
            }
        } else if (cmd == "2") {
            computeConnectionCounts();
        }
    }
};

// =========================================================
// 主程式與全域 UI / 字串處理函式
// =========================================================
int main() {
    GraphManager gm; 
  
    while (true) {
        PrintTitle();
        string cmd = ReadInput(); 
        
        if (cmd == "0") { 
            return 0; 
        } else if (cmd == "1" || cmd == "2"){
            gm.doTask(cmd); 
        } else {
            cout << "\nCommand does not exist!\n"; 
        }
        cout << endl;  
    }
}

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

void PrintTitle () {
    cout << "* Data Structures and Algorithms *\n";
    cout << "**** Graph data manipulation *****\n";
    cout << "* 0. QUIT                        *\n";
    cout << "* 1. Build adjacency lists       *\n";
    cout << "* 2. Compute connection counts   *\n";
    cout << "**********************************\n";
    cout << "Input a choice(0, 1, 2): ";
}