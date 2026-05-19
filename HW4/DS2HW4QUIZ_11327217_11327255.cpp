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

// ========================================ㄋ=================
// 資料結構定義區 (加入建構子，讓初始化更簡潔)
// =========================================================

// 相鄰串列的節點：紀錄互動關係的收訊者與權重
struct AdjNode {
    string getID;    
    float weight;    
    AdjNode* next;   

    // AdjNode 建構子：建立節點時直接賦值
    AdjNode(string id, float w) : getID(id), weight(w), next(nullptr) {}
};

// 主陣列的節點：紀錄每一位學生及其發送訊息的串列開頭
struct StudentNode {
    string studentID; 
    AdjNode* head;    

    // StudentNode 建構子：建立學生時直接賦值，並將串列預設為空
    StudentNode(string id) : studentID(id), head(nullptr) {}
};

// =========================================================
// 核心邏輯類別：負責圖的建立、儲存與輸出
// =========================================================
class GraphManager {
private:
    vector<StudentNode> students; // 動態主陣列
    int totalNodes;               // 總有向邊數

    // 輔助函式：安全地將 char[12] 轉換為 string
    string convertToString(const char* arr) {
        size_t len = 0;
        while (len < 12 && arr[len] != '\0') {
            len++;
        }
        return string(arr, len);
    }

    // 輔助函式：尋找學生，若無則動態新增並回傳索引值
    int findOrInsertStudent(const string& id) {
        for (size_t i = 0; i < students.size(); ++i) {
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

public:
    // GraphManager 建構子：初始化系統
    GraphManager() {
        totalNodes = 0;
    }

    // 執行新任務前，清空舊資料與釋放記憶體
    void reSet() {
        for (size_t i = 0; i < students.size(); ++i) {
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
            for (size_t i = 0; i < students.size(); ++i) {
                const StudentNode& student = students[i];
                fout << student.studentID << " :";
                
                AdjNode* curr = student.head;
                if (curr == nullptr) {
                    fout << " (無發訊紀錄)";
                } else {
                    while (curr != nullptr) {
                        fout << " -> [" << curr->getID << ", " << fixed << setprecision(2) << curr->weight << "]";
                        curr = curr->next;
                    }
                }
                fout << "\n";
            }
            fout.close();
        }

        cout << "\n<<< There are " << students.size() << " IDs in total. >>>\n";
        cout << "\n<<< There are " << totalNodes << " nodes in total. >>>\n";
    }

    // 任務分配器
    void doTask(string cmd) { 
        if (cmd == "1") {
            string file_num;
            while (1) {
                cout << "\nInput a file number ([0] Quit): ";
                file_num = ReadInput();
                if (file_num == "0") {
                    break;
                }
                buildAdjacencyLists(file_num);
                break; 
            }
        } else if (cmd == "2") {
            cout << "\nNot implemented yet.\n"; 
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