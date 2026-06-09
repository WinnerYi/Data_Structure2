// 11327217 蔡易勳 11327255 許頌恩
// 資料結構與演算法：外部合併排序與主索引建構、範圍檢索與輔助索引

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ========== 全域輔助函式宣告 ==========
void PrintTitle();           
void SkipSpace(string &str); 
string ReadInput();          
void PrintMission1();
void PrintMission3();

int GetBufferSize();
bool ProcessMission1And2(class GraphManager &gm, int buffer_size);
void GetRangeSearchValues(float &f_num1, float &f_num2);
void ProcessMission3And4(class GraphManager &gm, int buffer_size);

// =========================================================
// 資料結構定義區
// =========================================================

struct Record {
  char putID[10]; 
  char getID[10]; 
  float weight;   

  bool operator>(const Record &other) const { return weight > other.weight; }
};

struct SortRecord {
    Record rec;
    int originalOrder; 

    bool operator<(const SortRecord& other) const {
        if (rec.weight != other.rec.weight) {
            return rec.weight > other.rec.weight; 
        }
        return originalOrder < other.originalOrder; 
    }
};

struct IndexEntry {
    float key;
    int offset;
};

// =========================================================
// 核心邏輯類別
// =========================================================
class GraphManager {
private:
  int currentBufSize;                     
  string sortedFile;                      
  vector<IndexEntry> primaryIndex;        
  map<string, vector<int>> secondaryIndex;

  void mergeTwoFiles(const string &file1, const string &file2,
                     const string &outputFile) {
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);
    ofstream out(outputFile, ios::binary);

    Record rec1, rec2;
    bool has1 = readRecord(f1, rec1);
    bool has2 = readRecord(f2, rec2);

    vector<Record> outBuffer;
    outBuffer.reserve(currentBufSize);

    auto flushBuffer = [&]() {
        if (!outBuffer.empty()) {
            out.write(reinterpret_cast<const char*>(outBuffer.data()), outBuffer.size() * sizeof(Record));
            outBuffer.clear();
        }
    };

    while (has1 || has2) {
      if (has1 && (!has2 || rec1.weight >= rec2.weight)) {
          outBuffer.push_back(rec1);
          has1 = readRecord(f1, rec1);
      } else {
          outBuffer.push_back(rec2);
          has2 = readRecord(f2, rec2);
      }
      
      if (outBuffer.size() == (size_t)currentBufSize) {
          flushBuffer();
      }
    }
    flushBuffer(); 

    f1.close();
    f2.close();
    out.close();
  }

  bool readRecord(ifstream &fin, Record &rec) {
    if (fin.read(reinterpret_cast<char *>(&rec), sizeof(Record))) {
      return true;
    }
    return false;
  }
  
  // 【修正】：正確清除 char array 中的 null bytes 和空白，避免亂碼
  string extractID(const char* idArray) {
      // 先找到第一個 null byte 或非可見字元作為結束
      int len = 0;
      for (int i = 0; i < 10; i++) {
          if (idArray[i] == '\0') break;
          if ((unsigned char)idArray[i] < 0x20) break; // 非可見字元
          len = i + 1;
      }
      string s(idArray, len);
      // 再去掉尾部空白
      while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n'))
          s.pop_back();
      return s;
  }

public:
  GraphManager() : currentBufSize(300) {}
  
  void setBufferSize(int size) {
      currentBufSize = size;
  }

  // ========== 任務一：外部合併排序 ==========
  void externalMergeSort(string file_num, int new_buffer_size) {
    setBufferSize(new_buffer_size);
    auto totalStartTime = chrono::high_resolution_clock::now();

    string inputFile = "pairs" + file_num + ".bin";
    sortedFile = "order" + file_num + ".bin";

    ifstream fin(inputFile, ios::binary);
    if (!fin) {
      cout << "\n### " << inputFile << " does not exist! ###\n";
      return;
    }
    fin.close();

    auto internalStartTime = chrono::high_resolution_clock::now();

    vector<string> tempFiles;
    vector<SortRecord> buffer;
    buffer.reserve(currentBufSize);
    ifstream infile(inputFile, ios::binary);
    int fileIndex = 0;
    Record tempRec;
    int orderCounter = 0;

    while (readRecord(infile, tempRec)) {
      buffer.push_back({tempRec, orderCounter++});

      if (buffer.size() == (size_t)currentBufSize) {
        sort(buffer.begin(), buffer.end());

        string tempFile =
            "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
        tempFiles.push_back(tempFile);

        ofstream outtemp(tempFile, ios::binary);
        for (const auto &sr : buffer) {
          outtemp.write(reinterpret_cast<const char *>(&sr.rec), sizeof(Record));
        }
        outtemp.close();

        buffer.clear();
        orderCounter = 0;
        fileIndex++;
      }
    }

    if (!buffer.empty()) {
      sort(buffer.begin(), buffer.end());

      string tempFile =
          "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
      tempFiles.push_back(tempFile);

      ofstream outtemp(tempFile, ios::binary);
      for (const auto &sr : buffer) {
        outtemp.write(reinterpret_cast<const char *>(&sr.rec), sizeof(Record));
      }
      outtemp.close();
    }

    infile.close();

    auto internalEndTime = chrono::high_resolution_clock::now();
    auto internalDuration = chrono::duration_cast<chrono::milliseconds>(
        internalEndTime - internalStartTime);

    cout << "\nThe internal sort is completed. Check the initial sorted runs! \n";
    cout << "\nNow there are " << tempFiles.size() << " runs.\n";

    auto externalStartTime = chrono::high_resolution_clock::now();

    int mergePass = 0;
    while (tempFiles.size() > 1) {
      vector<string> newTempFiles;

      for (size_t i = 0; i < tempFiles.size(); i += 2) {
        if (i + 1 < tempFiles.size()) {
          string outputTemp = "temp_" + file_num + "_p" + to_string(mergePass) +
                              "_" + to_string(newTempFiles.size()) + ".bin";
          mergeTwoFiles(tempFiles[i], tempFiles[i + 1], outputTemp);
          newTempFiles.push_back(outputTemp);

          remove(tempFiles[i].c_str());
          remove(tempFiles[i + 1].c_str());
        } else {
          string outputTemp = "temp_" + file_num + "_p" + to_string(mergePass) +
                              "_" + to_string(newTempFiles.size()) + ".bin";
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
      
      cout << "\nNow there are " << tempFiles.size() << " runs.\n";
    }

    auto externalEndTime = chrono::high_resolution_clock::now();
    auto externalDuration = chrono::duration_cast<chrono::milliseconds>(
        externalEndTime - externalStartTime);

    if (tempFiles.size() == 1) {
      remove(sortedFile.c_str());
      rename(tempFiles[0].c_str(), sortedFile.c_str());
    }

    auto totalEndTime = chrono::high_resolution_clock::now();
    auto totalDuration = chrono::duration_cast<chrono::milliseconds>(
        totalEndTime - totalStartTime);

    cout << "\nThe execution time ...\n";
    cout << fixed << setprecision(3);
    cout << "Internal Sort = " << (double)internalDuration.count() << " ms\n";
    cout << "External Sort = " << (double)externalDuration.count() << " ms\n";
    cout << "Total Execution Time = " << (double)totalDuration.count()
         << " ms\n";
  }

  // ========== 任務二：構建主索引 ==========
  void buildPrimaryIndex(string file_num) {
    ifstream fin(sortedFile, ios::binary);
    if (!fin) {
      cout << "\n### " << sortedFile << " does not exist! ###\n";
      cout << "Please run external sort first!\n";
      return;
    }
    
    primaryIndex.clear(); 
    Record tempRec;
    int offset = 0;
    float lastWeight = -1.0;

    while (readRecord(fin, tempRec)) {
      if (tempRec.weight != lastWeight) {
        primaryIndex.push_back({tempRec.weight, offset});
        lastWeight = tempRec.weight;
      }
      offset++;
    }

    fin.close();

    cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    cout << "* 2: Construct the primary index *\n";
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    cout << "\n<Primary index>: (key, offset)\n";

    int count = min(100, (int)primaryIndex.size());
    for (int i = 0; i < count; i++) {
      cout << "[" << (i + 1) << "] (";
      cout << defaultfloat << setprecision(2) << primaryIndex[i].key;
      cout << ", " << primaryIndex[i].offset << ")\n";
    }
  }

  // ========== 任務三：範圍檢索與建立輔助索引 ==========
  // 【修正】：資料是降序排列(weight 大 -> 小)，搜尋邏輯須對應調整
  void buildSecondaryIndex(float min_w, float max_w) {
      secondaryIndex.clear(); 
      
      // primaryIndex 是降序：[0] 最大值, [last] 最小值
      // 找第一個 key <= max_w 的 entry 作為起點
      int start_offset = -1;
      for (const auto& entry : primaryIndex) {
          if (entry.key <= max_w + 1e-5f) { 
              start_offset = entry.offset;
              break;
          }
      }
      
      if (start_offset == -1) {
          cout << "There are 0 records in total.\n";
          cout << "There are 0 senders in total.\n";
          return;
      }

      ifstream fin(sortedFile, ios::binary);
      fin.seekg((long long)start_offset * sizeof(Record), ios::beg);

      vector<Record> buffer(currentBufSize);
      int absolute_offset = start_offset;
      bool done = false;
      int total_records = 0;

      while (!done && fin) {
          fin.read(reinterpret_cast<char*>(buffer.data()), currentBufSize * sizeof(Record));
          int count = fin.gcount() / sizeof(Record);
          if (count == 0) break;

          for (int i = 0; i < count; ++i) {
              float w = buffer[i].weight;
              // 資料降序：遇到比 min_w 小的就可以停了
              if (w < min_w - 1e-5f) {
                  done = true;
                  break;
              }
              // 在 [min_w, max_w] 範圍內才收錄
              if (w >= min_w - 1e-5f && w <= max_w + 1e-5f) {
                  string putID = extractID(buffer[i].putID);
                  secondaryIndex[putID].push_back(absolute_offset);
                  total_records++;
              }
              absolute_offset++;
          }
      }
      fin.close();

      cout << "\nThere are " << total_records << " records in total.\n";
      cout << "There are " << secondaryIndex.size() << " senders in total.\n";
      
      int seq = 1;
      for (const auto& pair : secondaryIndex) {
          // 【修正】：']' 後直接 setw(11) right（自帶空格padding），tab 後 2 空格
          cout << "[" << setw(4) << seq++ << "]"
               << setw(11) << right << pair.first
               << "\t" << setw(5) << right << pair.second.size() << "\n";
      }
  }

  // ========== 任務四：使用輔助索引查找資料 ==========
  void searchSecondaryIndex(string targetID) {
      if (secondaryIndex.find(targetID) == secondaryIndex.end()) {
          cout << "Sender " << targetID << " does not exist.\n";
          return;
      }

      const auto& offsets = secondaryIndex[targetID];
      cout << "Sender " << targetID << " has " << offsets.size() << " records.\n";
      
      ifstream fin(sortedFile, ios::binary);
      
      int seq = 1;
      for (int offset : offsets) {
          fin.seekg((long long)offset * sizeof(Record), ios::beg);
          Record rec;
          fin.read(reinterpret_cast<char*>(&rec), sizeof(Record));
          
          cout << "[" << setw(3) << seq++ << "]    " 
               << setw(10) << left << extractID(rec.getID) << right 
               << "    " << fixed << setprecision(2) << rec.weight << "\n";
      }
      fin.close();
      cout << defaultfloat;
  }
};

// =========================================================
// 主程式與流程控制
// =========================================================

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  GraphManager gm;

  while (true) {
    PrintTitle();

    int new_buffer_size = GetBufferSize();

    cout << "\n";
    PrintMission1();

    if (!ProcessMission1And2(gm, new_buffer_size)) {
      return 0;
    }

    ProcessMission3And4(gm, new_buffer_size);

    cout << "\n[0]Quit or [Any other key]continue?\n";
    string continueCmd = ReadInput();
    if (continueCmd == "0") return 0;
  }
  return 0;
}

// =========================================================
// 全域 UI / 字串與輸入處理函式實作
// =========================================================

int GetBufferSize() {
  int size;
  string input;
  while (true) {
    input = ReadInput();
    try {
        size = stoi(input);
        
        if (size == 0) exit(0); 

        if (size < 300 || size > 60000) {
          cout << "### It is NOT in [300,60000] ###\n\n";
          cout << "Input a new buffer size in [300, 60000]: ";
          continue;
        }
        break;
    } catch (...) {
        cout << "### It is NOT in [300,60000] ###\n\n";
        cout << "Input a new buffer size in [300, 60000]: ";
    }
  }
  return size;
}

bool ProcessMission1And2(GraphManager &gm, int buffer_size) {
  string cmd;
  while (true) {
    cmd = ReadInput();

    if (cmd == "0") {
      cout << "\n[0]Quit or [Any other key]continue?\n";
      string continueCmd = ReadInput();
      if (continueCmd == "0") {
        return false; 
      } else {
        cout << "\nInput the file name: [0]Quit\n";
        continue; 
      }
    }

    bool fileExists = false;

    if ((cmd.length() == 3 || cmd.length() == 4) && cmd[0] >= '0' && cmd[0] <= '9' && cmd[1] >= '0' &&
        cmd[1] <= '9' && cmd[2] >= '0' && cmd[2] <= '9') {
      string filename = "pairs" + cmd + ".bin";
      ifstream testFile(filename, ios::binary);
      if (testFile.is_open()) {
        testFile.close();
        fileExists = true;
      }
    }

    if (fileExists) {
      gm.externalMergeSort(cmd, buffer_size);
      gm.buildPrimaryIndex(cmd);
      return true; 
    } else {
      cout << "\npairs" << cmd << ".bin does not exist!!!\n\n";
      cout << "Input the file name: [0]Quit\n";
    }
  }
}

// 【修正】：使用 cout << "\n" 讓錯誤訊息換行，符合 answer 格式
void GetRangeSearchValues(float &f_num1, float &f_num2) {
  while (true) {
    cout << "Input a floating number in [0.01, 1]: ";
    string line = ReadInput();
    try {
        size_t pos;
        float val = stof(line, &pos);
        if (pos != line.size()) throw invalid_argument("");
        if (val < 0.01f || val > 1.00f) {
            cout << "\n### It is NOT in [0.01,1] ###\n\n";
            continue;
        }
        f_num1 = val;
        break;
    } catch (...) {
        cout << "\n### It is NOT in [0.01,1] ###\n\n";
    }
  }

  while (true) {
    cout << "\nInput a floating number in [0.01, 1]: ";
    string line = ReadInput();
    try {
        size_t pos;
        float val = stof(line, &pos);
        if (pos != line.size()) throw invalid_argument("");
        if (val < 0.01f || val > 1.00f) {
            cout << "\n### It is NOT in [0.01,1] ###\n\n";
            continue;
        }
        f_num2 = val;
        break;
    } catch (...) {
        cout << "\n### It is NOT in [0.01,1] ###\n\n";
    }
  }
}

void ProcessMission3And4(GraphManager &gm, int buffer_size) {
    while (true) {
        printf("\n");
        PrintMission3();
        
        float min_w, max_w;
        GetRangeSearchValues(min_w, max_w);
        
        if (min_w > max_w) swap(min_w, max_w);
        
        gm.buildSecondaryIndex(min_w, max_w);
        
        while (true) {
            cout << "\nInput a student ID ([4] Quit): ";
            string m4cmd = ReadInput();
            if (m4cmd == "4") break;

            gm.searchSecondaryIndex(m4cmd);
        }
        
        cout << "\n[3]Quit or [Any other key]continue?\n";
        string m3cmd = ReadInput();
        if (m3cmd == "3") {
            break;
        }
    }
}

string ReadInput() {
  string input;
  while (true) {
    if (!getline(cin, input)) {
        exit(0); 
    }
    SkipSpace(input);
    if (!input.empty()) {
      break;
    }
  }
  return input;
}

void SkipSpace(string &str) {
  for (int i = 0; i < (int)str.size(); i++) {
    if (str[i] != ' ')
      break;
    if (str[i] == ' ') {
      str.erase(str.begin() + i);
      i--;
    }
  }
  for (int i = str.size() - 1; i >= 0; i--) {
    if (str[i] != ' ')
      break;
    if (str[i] == ' ') {
      str.erase(str.begin() + i);
    }
  }
}

void PrintTitle() {
  cout << "* Data Structures and Algorithms *\n";
  cout << "**********************************\n";
  cout << "* 1. External merge sort on file *\n";
  cout << "* 2: Construct the primary index *\n";
  cout << "* 3: Range search to build index *\n";
  cout << "* 4: Retrieve records from index *\n";
  cout << "**********************************\n";
  cout << "*** The buffer size is 300\n";
  cout << "Input a new buffer size in [300, 60000]: ";
}

void PrintMission1() {
  cout << "##################################\n";
  cout << "* 1. External merge sort on file *\n";
  cout << "##################################\n\n";
  cout << "Input the file name: [0]Quit\n";
}

void PrintMission3() {
  cout << "\n##################################\n";
  cout << "* 3: Range search to build index *\n";
  cout << "##################################\n\n";
  cout << "Input two values in (0,1] for range search.\n\n";
}