// 11327217 蔡易勳   11327255許頌恩
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
using namespace std;

void PrintTitle();
void SkipSpace(string &str);
string ReadInput();
int safeStoi(string s);
float safeStof(string s);
void stringToChar(string src, char dst[]);
bool isPrime(int n);
int findMinPrime(int dataCount);

struct HashItem {
    int hvalue;        // 雜湊值
    char sId[10];      // 學號
    char sName[10];    // 姓名
    float mean;        // 平均分數
    bool isEmpty;      // 判斷此位置是否為空
    
    HashItem()  {
        hvalue = -1;
        isEmpty = true;
        mean = 0.0f;
        strcpy(sId, "");
        strcpy(sName, "");
    }
};

struct StudentData { 
    char sId[10];
    char sName[10];
    unsigned char score1, score2, score3, score4, score5, score6; // 節省記憶體的無號字元
    float average;
};

class GraduateInfo{ 
private:
    StudentData data;

public:
    // 將外部傳入的資料寫入結構中
    void setGraduateInfo(string sid, string sname, int s1, int s2, int s3, int s4, int s5, int s6, float avg) {
        stringToChar(sid, data.sId);
        stringToChar(sname, data.sName);
        data.score1 = (unsigned char)s1;
        data.score2 = (unsigned char)s2;
        data.score3 = (unsigned char)s3;
        data.score4 = (unsigned char)s4;
        data.score5 = (unsigned char)s5;
        data.score6 = (unsigned char)s6;
        data.average = avg;
    } 

    // 提供外部讀取資料的 Getters
    const char* getSId() const { return data.sId; }
    const char* getSName() const { return data.sName; }
    unsigned char getScore(int idx) const { 
        if(idx == 1) return data.score1;
        if(idx == 2) return data.score2;
        if(idx == 3) return data.score3;
        if(idx == 4) return data.score4;
        if(idx == 5) return data.score5;
        if(idx == 6) return data.score6;
        return 0;
    }
    float getAverage() const { return data.average; }
};

class UniversityCatalog {
private:
    vector<GraduateInfo> info;    // 存放從檔案讀取的原始學生資料
    vector<HashItem> hashTable;   // 雜湊表本身
    int tableSize;                // 雜湊表的大小
    string currentFileNum;        // 記憶目前讀取的檔案編號，供任務二輸出使用
    int currentProbeMethod;       // 0: Quadratic Probing, 1: Double Hashing
    int maxStepForDoubleHash;     // 雙重雜湊的最大步階
  
public:
    // 初始化系統狀態，清空舊資料
    void reSet() {
        info.clear();
        hashTable.clear();
        tableSize = 0;
        currentFileNum = "";
        currentProbeMethod = -1;
        maxStepForDoubleHash = 0;
    }

    int getInfoCount() {
        return info.size();
    }
    
    // 邊乘邊取餘數，避免 ASCII 相乘導致整數溢位
    long long computeHash(string sid) {
        long long hashVal = 1;
        for (char c : sid) {
            hashVal = (hashVal * (unsigned char)c) % tableSize; 
        }
        if (hashVal < 0) hashVal += tableSize; // 防禦負數
        return hashVal;
    }

    // 計算雙重雜湊 (Double Hashing) 的跳躍步階 (Step)
    long long computeStep(string sid, int maxStep) {
        long long stepVal = 1;
        for (char c : sid) {
            stepVal = (stepVal * (unsigned char)c) % maxStep;
        }
        if (stepVal < 0) stepVal += maxStep; // 防禦負數
        
        long long step = maxStep - stepVal;
        if (step <= 0) step = 1; // 步階絕對不能是 0 (會導致無窮迴圈)
        return step;
    }
  
    // 計算二次探測 (Quadratic Probing) 的新位置： H(x) + idx^2
    int quadraticProbe(long long hashVal, int idx) {
        return (hashVal + idx * idx) % tableSize;
    }
  
    // 任務一：建立二次探測 (Quadratic Probing) 雜湊表
    void buildQuadraticHashTable(string file_num) {
        string bin_path = "input" + file_num + ".bin";
        ifstream in(bin_path, ios::binary);
        
        // 1. 若二進位檔不存在，先讀取文字檔並建立二進位檔
        if (in.fail()) {
            cout << "\n### " << bin_path << " does not exist! ###\n";
            readTextSaveBin(file_num);
            in.clear();
            in.open(bin_path, ios::binary);
            if (in.fail()) return;
        }
        
        reSet();
        currentFileNum = file_num; 
        
        // 2. 將資料讀入記憶體
        GraduateInfo g;
        while (in.read((char*)&g, sizeof(GraduateInfo))) {
            info.push_back(g);
        }
        in.close();
        
        // 3. 設定雜湊表大小： >= 1.15 * 資料數 的最小質數
        int minSize = (int)(info.size() * 1.15) + 1;
        tableSize = findMinPrime(minSize);
        hashTable.resize(tableSize);
        
        int dataInserted = 0;
        int dataSkipped = 0;
        
        // 二次探測的最高探測次數限制，防陷入死迴圈
        int maxProbes = (tableSize + 1) / 2;
        
        // 4. 逐筆插入資料至雜湊表
        for (int i = 0; i < info.size(); i++) {
            GraduateInfo gData = info[i]; // 透過索引取得當前學生資料
            
            string sid = gData.getSId();
            long long hashVal = computeHash(sid);
            int idx = 0;
            bool inserted = false;
            
            // 處理碰撞：尋找空位
            while (idx < maxProbes) {
                int pos = quadraticProbe(hashVal, idx);
                
                if (hashTable[pos].isEmpty) {
                    hashTable[pos].hvalue = hashVal % tableSize;
                    stringToChar(sid, hashTable[pos].sId);
                    stringToChar(gData.getSName(), hashTable[pos].sName);
                    hashTable[pos].mean = gData.getAverage();
                    hashTable[pos].isEmpty = false;
                    inserted = true;
                    dataInserted++;
                    break;
                }
               
                idx++; // 發生碰撞，增加探測次數
            }
            
            if (!inserted) {
                dataSkipped++;
                cout << "### Failed at [" << i << "]. ###\n"; 
            }
        }
        
        cout << "\nHash table has been successfully created by Quadratic probing\n";
        calculateSearchStatistics(dataInserted);
        outputHashTableToFile("quadratic", file_num);
        currentProbeMethod = 0; // 記錄使用二次探測
        searchStudentLoop();
    }

    // 任務二：建立雙重雜湊 (Double Hashing) 雜湊表
    void buildDoubleHashTable() {
        if (info.empty() || currentFileNum == "") {
            cout << "### Command 1 first. ###\n\n";
            return;
        }

        int dataCount = info.size();
        int minSize = (int)(dataCount * 1.15) + 1;
        tableSize = findMinPrime(minSize);

        // 任務二規定：計算最高步階
        int maxStepLimit = (dataCount / 5) + 1;
        int maxStep = findMinPrime(maxStepLimit);

        hashTable.clear();
        hashTable.resize(tableSize);

        int dataInserted = 0;

        // 逐筆插入資料
        for (int i = 0; i < info.size(); i++) {
            GraduateInfo gData = info[i]; // 透過索引取得當前學生資料
            
            string sid = gData.getSId();
            long long hashVal = computeHash(sid);
            long long step = computeStep(sid, maxStep);
            int idx = 0;
            bool inserted = false;

            // 處理碰撞：依照步階跳躍尋找空位
            while (idx < tableSize) {
                int pos = (hashVal + idx * step) % tableSize;

                if (hashTable[pos].isEmpty) {
                    hashTable[pos].hvalue = hashVal % tableSize;
                    stringToChar(sid, hashTable[pos].sId);
                    stringToChar(gData.getSName(), hashTable[pos].sName);
                    hashTable[pos].mean = gData.getAverage();
                    hashTable[pos].isEmpty = false;
                    inserted = true;
                    dataInserted++;
                    break;
                }
                idx++;
            }

            if (!inserted) {
                cout << "sId: " << sid << " 找不到空位\n";
            }
        }

        cout << "\nHash table has been successfully created by Double hashing   \n";
        calculateDoubleSearchStatistics(maxStep);
        outputHashTableToFile("double", currentFileNum);
        currentProbeMethod = 1; // 記錄使用雙重雜湊
        maxStepForDoubleHash = maxStep;
        searchStudentLoop();
    }
    
    // 計算任務一 (二次探測) 的搜尋成功與失敗平均次數
    void calculateSearchStatistics(int validRecords) {
        if (validRecords == 0) {
            cout << "No valid records to search\n";
            return;
        }
        
        double successfulTotal = 0;
        int successfulCount = 0;
        int maxProbes = (tableSize + 1) / 2;
        
        // 1. 計算成功搜尋：找尋目前存在表中的每一筆資料
        for (int i = 0; i < tableSize; i++) {
            if (!hashTable[i].isEmpty) {
                const char* sid_in_table = hashTable[i].sId;
                long long hashVal = computeHash(sid_in_table);
                int idx = 0;
                int comparisons = 0;
                
                while (idx < maxProbes) {
                    int pos = quadraticProbe(hashVal, idx);
                    comparisons++; // 每次探測都算一次比較
                    if (!hashTable[pos].isEmpty && strcmp(hashTable[pos].sId, sid_in_table) == 0) {
                        break; // 找到了
                    }
                    idx++;
                }
                successfulTotal += comparisons;
                successfulCount++;
            }
        }
        
        double successfulAvg = (successfulCount > 0) ? successfulTotal / successfulCount : 0;
        
        // 2. 計算失敗搜尋：從每一個位置開始找，直到遇到空位
        double unsuccessfulTotal = 0;
        
        for (int startPos = 0; startPos < tableSize; startPos++) {
            int idx = 0;
            int comparisons = 0;
            
            while (idx < maxProbes) {
                int pos = quadraticProbe(startPos, idx);
                if (hashTable[pos].isEmpty) {
                    comparisons++; // 檢查到空位也算一次比較
                    break;
                }
                comparisons++;
                idx++;
            }
            unsuccessfulTotal += comparisons;
        }
        
        double unsuccessfulAvg = (tableSize > 0) ? (unsuccessfulTotal) / tableSize - 1.0 : 0;
        
        cout << fixed << setprecision(4);
        cout << "unsuccessful search: " << unsuccessfulAvg << " comparisons on average\n";
        cout << "successful search: " << successfulAvg << " comparisons on average\n";
    }

    // 計算任務二 (雙重雜湊) 的搜尋成功平均次數
    void calculateDoubleSearchStatistics(int maxStep) {
        double successfulTotal = 0;
        int successfulCount = 0;

        for (int i = 0; i < tableSize; i++) {
            if (!hashTable[i].isEmpty) {
                const char* sid_in_table = hashTable[i].sId;
                long long hashVal = computeHash(sid_in_table);
                long long step = computeStep(sid_in_table, maxStep);
                int idx = 0;
                int comparisons = 0;
                
                while (idx < tableSize) {
                    int pos = (hashVal + idx * step) % tableSize;
                    comparisons++;
                    if (!hashTable[pos].isEmpty && strcmp(hashTable[pos].sId, sid_in_table) == 0) {
                        break;
                    }
                    idx++;
                }
                successfulTotal += comparisons;
                successfulCount++;
            }
        }

        double successfulAvg = (successfulCount > 0) ? successfulTotal / successfulCount : 0;
        cout << fixed << setprecision(4);
        cout << "successful search: " << successfulAvg << " comparisons on average\n";
    }
  
    // 將建好的雜湊表匯出為文字檔
    void outputHashTableToFile(string prefix, string file_num) {
        string output_path = prefix + file_num + ".txt";
        ofstream out(output_path);
        
        if(prefix == "quadratic")
            out << "--- Hash table created by Quadratic probing ---\n";
        else
            out << "--- Hash table created by Double hashing    ---\n";
        
        for (int i = 0; i < tableSize; i++) {
            out << "[" << setw(3) << right << i << "] ";
            
            if (!hashTable[i].isEmpty) {
                out << setw(10) << hashTable[i].hvalue << "," 
                    << setw(11) << hashTable[i].sId << "," 
                    << setw(11) << hashTable[i].sName << "," 
                    << setw(11) << hashTable[i].mean;
            }
            out << "\n";
        }
        out << " -----------------------------------------------------\n";
        out.close();
    }
    
    // 二次探測搜尋特定學生
    void searchByQuadraticProbing(string sid) {
        long long hashVal = computeHash(sid);
        int idx = 0;
        int maxProbes = (tableSize + 1) / 2;
        int probeCount = 0;
        
        while (idx < maxProbes) {
            int pos = quadraticProbe(hashVal, idx);
            probeCount++;
            
            if (!hashTable[pos].isEmpty && strcmp(hashTable[pos].sId, sid.c_str()) == 0) {
    // 找到了
              cout << "\n{ " << hashTable[pos].sId << ", " 
                  << hashTable[pos].sName << ", ";
                  
              // 判斷 mean 是否為整數 (例如 85.00 == 85)
              if (hashTable[pos].mean == (int)hashTable[pos].mean) {
                  // 如果是整數，轉型成整數輸出，就不會有小數點
                  cout << (int)hashTable[pos].mean; 
              } else {
                  // 如果帶有小數，則強制保留兩位小數
                  cout << fixed << setprecision(2) << hashTable[pos].mean;
              }
              
              cout << " } is found after " << probeCount << " probes.\n\n";
              return;
          }
            
            if (hashTable[pos].isEmpty) {
                // 遇到空位，表示沒有找到
                cout << "\n" << sid << " is not found after " << probeCount << " probes.\n\n";
                return;
            }
            
            idx++;
        }
        
        cout << "\n\n" << sid << " is not found after " << probeCount << " probes.\n\n";
    }
    
    // 雙重雜湊搜尋特定學生
    void searchByDoubleHashing(string sid) {
        long long hashVal = computeHash(sid);
        long long step = computeStep(sid, maxStepForDoubleHash);
        int idx = 0;
        int probeCount = 0;
        
        while (idx < tableSize) {
            int pos = (hashVal + idx * step) % tableSize;
            probeCount++;
            
            if (!hashTable[pos].isEmpty && strcmp(hashTable[pos].sId, sid.c_str()) == 0) {
                // 找到了
                cout << "\n{ " << hashTable[pos].sId << ", " 
                    << hashTable[pos].sName << ", ";
                    
                // 判斷 mean 是否為整數
                if (hashTable[pos].mean == (int)hashTable[pos].mean) {
                    cout << (int)hashTable[pos].mean; 
                } else {
                    cout << fixed << setprecision(2) << hashTable[pos].mean;
                }
                
                cout << " } is found after " << probeCount << " probes.\n\n";
                return;
            }
            
            if (hashTable[pos].isEmpty) {
                // 遇到空位，表示沒有找到
                cout << "\n" << sid << " is not found after " << probeCount << " probes.\n\n";
                return;
            }
            
            idx++;
        }
        
        cout << "\n\n" << sid << " is not found after " << probeCount << " probes.\n\n";
    }
    
    // 搜尋學生訪問迴圈
    void searchStudentLoop() {
        if (currentProbeMethod == -1) {
            cout << "### Command 1 first. ###\n\n";
            return;
        }
        
        while (true) {
            cout << "Input a student ID to search ([0] Quit): ";
            string sid = ReadInput();
            
            if (sid == "0") {
                printf("\n");
                break;
            }
            
            if (currentProbeMethod == 0) {
                searchByQuadraticProbing(sid);
            } else if (currentProbeMethod == 1) {
                searchByDoubleHashing(sid);
            }
        }
    }

    // 任務零：讀取文字檔並解析儲存為二進位檔
    void readTextSaveBin(string file_num) {
        string txt_path = "input" + file_num + ".txt";
        ifstream in(txt_path);
        
        if (in.fail()) {
            cout << "\n### " << txt_path << " does not exist! ###\n\n";
            currentFileNum = "";
            return;
        }
        
        reSet(); 
        
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string sid, sname, score1_str, score2_str, score3_str, score4_str, score5_str, score6_str, avg_str;

            // 以 Tab 鍵為分隔符號讀取欄位
            getline(ss, sid, '\t');
            getline(ss, sname, '\t');
            getline(ss, score1_str, '\t');
            getline(ss, score2_str, '\t');
            getline(ss, score3_str, '\t');
            getline(ss, score4_str, '\t');
            getline(ss, score5_str, '\t');
            getline(ss, score6_str, '\t');
            getline(ss, avg_str);
            
            int s1 = safeStoi(score1_str);
            int s2 = safeStoi(score2_str);
            int s3 = safeStoi(score3_str);
            int s4 = safeStoi(score4_str);
            int s5 = safeStoi(score5_str);
            int s6 = safeStoi(score6_str);
            float avg = safeStof(avg_str);
            
            GraduateInfo g;
            g.setGraduateInfo(sid, sname, s1, s2, s3, s4, s5, s6, avg);
            info.push_back(g);
        }
        in.close();
        
        // 將動態陣列中的資料寫入為 .bin 檔
        string bin_path = "input" + file_num + ".bin";
        ofstream out(bin_path, ios::binary);
        
        //迴圈讀取陣列寫入檔案
        for (int i = 0; i < info.size(); i++) {
            out.write((char*)&info[i], sizeof(GraduateInfo)); 
        }
        out.close();
    }
  
    // 依據使用者選擇執行對應任務
    void doTask(string cmd) { 
        if (cmd == "1") {
            string file_num;
            while (1) {
                cout << "\nInput a file number ([0] Quit): ";
                file_num = ReadInput();
                if (file_num == "0") {
                    printf("\n");
                    break;
                }
                buildQuadraticHashTable(file_num);
                break;
            }
        } else if (cmd == "2") {
            buildDoubleHashTable();
        }
    }
};

int main() {
    UniversityCatalog uc;
  
    while (true) {
        PrintTitle();
        string cmd = ReadInput(); 
        if (cmd == "0") { 
            return 0;
        } else if (cmd == "1"){
            uc.doTask(cmd);
        }  else if (cmd == "2"){ 
            uc.doTask(cmd);
        } else {
            cout << "\nCommand does not exist!\n\n";
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

// 去除字串前後端多餘的空白字元
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

// 安全字串轉整數 (防呆過濾非數字字元)
int safeStoi(string s) {
    string cleanStr = "";
    for (char c : s) { 
        if (c >= '0' && c <= '9') {
            cleanStr += c;
        }
    }
    if (cleanStr.empty()) return 0;
    return stoi(cleanStr);
}

// 安全字串轉浮點數 (防呆過濾非數字及小數點)
float safeStof(string s) {
    string cleanStr = "";
    bool hasDecimal = false;
    for (char c : s) { 
        if (c >= '0' && c <= '9') {
            cleanStr += c;
        } else if (c == '.' && !hasDecimal) {
            cleanStr += c;
            hasDecimal = true;
        }
    }
    if (cleanStr.empty()) return 0.0f;
    return stof(cleanStr);
}

void stringToChar(string src, char dst[]) {
    int i;
    for (i = 0; i < src.length() && i < 9; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

void PrintTitle () {
    cout << "* Data Structures and Algorithms *\n";
    cout << "************ Hash Table **********\n";
    cout << "* 0. QUIT                        *\n";
    cout << "* 1. Quadratic probing           *\n";
    cout << "* 2. Double hashing              *\n";
    cout << "**********************************\n";
    cout << "Input a choice(0, 1, 2): ";
}

// 檢查是否為質數
bool isPrime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// 尋找大於等於 n 的最小質數
int findMinPrime(int n) {
    if (n < 2) return 2;
    while (!isPrime(n)) {
        n++;
    }
    return n;
}