// 11327217 蔡易勳   11327255許頌恩
///TODO: 任務一   嘗試找到空位（最多查詢tableSize次以避免無限迴圈）不確定這樣是不是對的 ??
// input996.txt 是測試找不到空位的檔案


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

// 哈希表項結構
struct HashItem {
  int hvalue;        // 哈希值
  char sId[10];      // 學號
  char sName[10];    // 姓名
  float mean;        // 平均分數
  bool isEmpty;      // 是否為空
  
  HashItem()  {
    hvalue = -1;
    isEmpty = true;
    mean = 0.0f;
    strcpy(sId, "");
    strcpy(sName, "");
  }
};

struct StudentData { // struct 結構
  char sId[10];
  char sName[10];
  unsigned char score1, score2, score3, score4, score5, score6;
  float average;
};

class GraduateInfo{ 
 private:
  int id;
  StudentData data;

 public:
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
  vector<GraduateInfo> info;
  vector<HashItem> hashTable;
  int tableSize;
  
 public:
  void reSet() {
    info.clear();
    hashTable.clear();
    tableSize = 0;
  }

  int getInfoCount() {
    return info.size();
  }
  
  // 計算學號的哈希值：ASCII編碼相乘
  long long computeHash(string sid) {
    long long hashVal = 1;
    for (char c : sid) {
      hashVal *= (int)c;
    }
    return hashVal;
  }
  
  // Quadratic Probing：使用平方探測找空位
  int quadraticProbe(long long hashVal, int idx) {
    return (hashVal + idx * idx) % tableSize;
  }
  
  // 建立Quadratic Probing哈希表
  void buildQuadraticHashTable(string file_num) {
    // 從二進制檔讀取資料
    string bin_path = "input" + file_num + ".bin";
    ifstream in(bin_path, ios::binary);
    
    if (in.fail()) {
      cout << "\n### " << bin_path << " does not exist! ###\n";
      readTextSaveBin(file_num);
      // 重新開啟二進制檔
      in.open(bin_path, ios::binary);
      if (in.fail()) {
        return;
      }
    }
    
    reSet();
    
    vector<GraduateInfo> tempData;
    GraduateInfo g;
    while (in.read((char*)&g, sizeof(GraduateInfo))) {
      tempData.push_back(g);
    }
    in.close();
    
    // if (tempData.empty()) {
    //   cout << "沒東西\n";
    //   return;
    // }
    
    // 計算哈希表大小：>= 1.15 * 資料數的最小質數
    int minSize = (int)(tempData.size() * 1.15) + 1;
    tableSize = findMinPrime(minSize);
    
    // 初始化哈希表
    hashTable.resize(tableSize);
    
    int dataInserted = 0;
    int dataSkipped = 0;
    
    // 將資料插入哈希表
    for (const auto& gData : tempData) {
      string sid = gData.getSId();
      long long hashVal = computeHash(sid);
      int idx = 0;
      bool inserted = false;
      
      // 嘗試找到空位（最多查詢tableSize次以避免無限迴圈）不確定這樣是不是對的
      while (idx < tableSize) {
        int pos = quadraticProbe(hashVal, idx);
        
        if (hashTable[pos].isEmpty) {
          // 找到空位，插入資料
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
        dataSkipped++;
        cout << "sId: " << sid << " 找不到空位\n"; //這邊demo也跑不出來 所以我也不知道怎麼寫
      }
    }
    
    cout << "\nHash table has been successfully created by Quadratic probing\n";
    
    // 計算搜尋統計
    calculateSearchStatistics(dataInserted);
    
    // 輸出到檔案
    outputHashTableToFile(file_num);
  }
  
  // 計算搜尋統計
  void calculateSearchStatistics(int validRecords) {
    if (validRecords == 0) {
      cout << "No valid records to search\n";
      return;
    }
    
    double successfulTotal = 0;
    int successfulCount = 0;
    
    // 搜尋存在的值：計算每個有效記錄的查詢次數
    for (int i = 0; i < tableSize; i++) {
      if (!hashTable[i].isEmpty) {
        const char* sid_in_table = hashTable[i].sId;
        long long hashVal = computeHash(sid_in_table);
        int idx = 0;
        int comparisons = 0;
        
        // 使用平方探測查找該記錄
        while (idx < tableSize) {
          int pos = quadraticProbe(hashVal, idx);
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
    
    // 搜尋不存在的值：對每個位置計算通過平方探測到達空位需要多少次比較，然後除以表大小
    double unsuccessfulTotal = 0;
    
    for (int startPos = 0; startPos < tableSize; startPos++) {
      int idx = 0;
      int comparisons = 0;
      
      // 用平方探測找第一個空位
      while (idx < tableSize) {
        int pos = (startPos + idx * idx) % tableSize;
        if (hashTable[pos].isEmpty) {
          comparisons++;
          break;
        }
        comparisons++;
        idx++;
      }
      unsuccessfulTotal += comparisons;
    }
    
    double unsuccessfulAvg = (tableSize > 0) ? unsuccessfulTotal / tableSize - 1.0 : 0;
    
    cout << fixed << setprecision(4);
    cout << "unsuccessful search: " << unsuccessfulAvg << " comparisons on average\n";
    cout << "successful search: " << successfulAvg << " comparisons on average\n";
  }
  
  // 輸出哈希表到檔案
  void outputHashTableToFile(string file_num) {
    string output_path = "quadratic" + file_num + ".txt";
    ofstream out(output_path);
    
    out << "--- Hash table created by Quadratic probing ---\n";
    
    for (int i = 0; i < tableSize; i++) {
      out << "[ " << i << "]";
      if (!hashTable[i].isEmpty) {
        out << " " << hashTable[i].hvalue << ", " 
            << hashTable[i].sId << ", " 
            << hashTable[i].sName << ", " 
            << fixed << setprecision(2) << hashTable[i].mean;
      }
      out << "\n";
    }
    out << "-----------------------------------------------\n";
    out.close();
  }

  

// 讀取文本檔案並解析數據
  void readTextSaveBin(string file_num) {
    string txt_path = "input" + file_num + ".txt";
    ifstream in(txt_path);
    
    if (in.fail()) {
        cout << "\n### " << txt_path << " does not exist! ###\n\n";
        return;
    }
    
    reSet(); // 讀新檔案前清空舊資料
    
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string sid, sname, score1_str, score2_str, score3_str, score4_str, score5_str, score6_str, avg_str;

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
     // 保存數據為二進制檔案
    string bin_path = "input" + file_num + ".bin";
    ofstream out(bin_path, ios::binary);
    for (GraduateInfo g : info) {
        out.write((char*)&g, sizeof(GraduateInfo)); // 必須將位址轉型為 char*，並提供資料大小 !!!
    }
    out.close();
  
  }
  
 

void doTask(string cmd) { 
  if (cmd == "1") {
    string file_num;
    while (1) {
        cout << "\nInput a file number ([0] Quit): ";
        file_num = ReadInput();
        if (file_num == "0") break;
        buildQuadraticHashTable(file_num);
        break;
    }
  } else if (cmd == "2") {
    
  } else if (cmd == "3") { 
   
  } else if (cmd == "4") {
    
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
    } else if (cmd == "3") {
      uc.doTask(cmd);
    } else if (cmd == "4") {
      uc.doTask(cmd);
    } else cout << "\nCommand does not exist!\n";
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

int safeStoi(string s) {
    string cleanStr = "";
    for (char c : s) { 
        if (c >= '0' && c <= '9') {
            cleanStr += c;
        }
    }
    if (cleanStr.empty()) {
        return 0;
    }
    return stoi(cleanStr);
}

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
    if (cleanStr.empty()) {
        return 0.0f;
    }
    return stof(cleanStr);
}

void stringToChar(string src, char dst[]) {
    int i;
    for (i = 0; i < src.length() ; i++) {
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
  // cout << "* 3. Top-K max search on 23 tree *\n";
  // cout << "* 4. Exact search on AVL tree    *\n";
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

// 找大於等於n的最小質數
int findMinPrime(int n) {
  if (n < 2) return 2;
  while (!isPrime(n)) {
    n++;
  }
  return n;
}