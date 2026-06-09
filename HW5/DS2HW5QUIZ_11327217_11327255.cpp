// 11327217 蔡易勳 11327255 許頌恩
// 資料結構與演算法：外部合併排序與主索引建構

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ========== 全域輔助函式宣告 ==========
void PrintTitle();           // 顯示主選單與輸入提示
void SkipSpace(string &str); // 移除字串前後的多餘空格
string ReadInput();          // 讀取並處理使用者輸入的整行字串
void PrintMission1();
void PrintMission3();

// 新增的拆分函式
int GetBufferSize();
bool ProcessMission1And2(class GraphManager &gm, int buffer_size);
void GetRangeSearchValues(float &f_num1, float &f_num2);

// =========================================================
// 資料結構定義區
// =========================================================

// 訊息記錄結構：用於外部合併排序時，作為暫存在緩衝區的資料單位
// 10 + 10 + 4 = 24 bytes，天然符合記憶體對齊，不會產生 struct padding
struct Record {
  char putID[10]; // 發送者學號（10 bytes）
  char getID[10]; // 接收者學號（10 bytes）
  float weight;   // 訊息量權重（4 bytes）

  // 比較函數：用於內部排序（按 weight 由大到小）
  bool operator>(const Record &other) const { return weight > other.weight; }
};

// =========================================================
// 核心邏輯類別：負責外部合併排序與主索引構建
// =========================================================
class GraphManager {
private:
  // 輔助函數：執行兩路合併 (2-way Merge)，將兩個已排序的檔案合併為一個排序檔
  void mergeTwoFiles(const string &file1, const string &file2,
                     const string &outputFile) {
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);
    ofstream out(outputFile, ios::binary);

    Record rec1, rec2;
    bool has1 = readRecord(f1, rec1);
    bool has2 = readRecord(f2, rec2);

    // 使用類似 Merge Sort 合併陣列的雙指標寫法
    while (has1 || has2) {
      // 條件設定為 >= 確保當 weight 相同時，優先取 file1 的資料，維持 Stable
      // Sort 特性
      if (has1 && (!has2 || rec1.weight >= rec2.weight)) {
        out.write(reinterpret_cast<const char *>(&rec1), sizeof(Record));
        has1 = readRecord(f1, rec1);
      } else {
        out.write(reinterpret_cast<const char *>(&rec2), sizeof(Record));
        has2 = readRecord(f2, rec2);
      }
    }

    f1.close();
    f2.close();
    out.close();
  }

  // 輔助函數：從二進位檔案讀取單筆 24 bytes 的紀錄
  bool readRecord(ifstream &fin, Record &rec) {
    if (fin.read(reinterpret_cast<char *>(&rec), sizeof(Record))) {
      return true;
    }
    return false;
  }

public:
  GraphManager() {}

  // ========== 任務一：外部合併排序 (External Merge Sort) ==========
  void externalMergeSort(string file_num, int new_buffer_size) {
    auto totalStartTime = chrono::high_resolution_clock::now();

    string inputFile = "pairs" + file_num + ".bin";
    string outputFile = "order" + file_num + ".bin";

    ifstream fin(inputFile, ios::binary);
    if (!fin) {
      cout << "\n### " << inputFile << " does not exist! ###\n";
      return;
    }
    fin.close();

    // ---------------------------------------------------------
    // 階段一：內部排序 (Internal Sort) - 分段生成排序好的 Run 檔案
    // ---------------------------------------------------------
    auto internalStartTime = chrono::high_resolution_clock::now();

    vector<string> tempFiles;
    vector<Record> buffer;
    ifstream infile(inputFile, ios::binary);
    int fileIndex = 0;
    Record tempRec;

    while (readRecord(infile, tempRec)) {
      buffer.push_back(tempRec);

      if (buffer.size() == new_buffer_size) {
        stable_sort(buffer.begin(), buffer.end(),
                    [](const Record &a, const Record &b) {
                      return a.weight > b.weight;
                    });

        string tempFile =
            "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
        tempFiles.push_back(tempFile);

        ofstream outtemp(tempFile, ios::binary);
        for (const auto &rec : buffer) {
          outtemp.write(reinterpret_cast<const char *>(&rec), sizeof(Record));
        }
        outtemp.close();

        buffer.clear();
        fileIndex++;
      }
    }

    if (!buffer.empty()) {
      stable_sort(
          buffer.begin(), buffer.end(),
          [](const Record &a, const Record &b) { return a.weight > b.weight; });

      string tempFile =
          "temp_" + file_num + "_" + to_string(fileIndex) + ".bin";
      tempFiles.push_back(tempFile);

      ofstream outtemp(tempFile, ios::binary);
      for (const auto &rec : buffer) {
        outtemp.write(reinterpret_cast<const char *>(&rec), sizeof(Record));
      }
      outtemp.close();
    }

    infile.close();

    auto internalEndTime = chrono::high_resolution_clock::now();
    auto internalDuration = chrono::duration_cast<chrono::milliseconds>(
        internalEndTime - internalStartTime);

    cout << "\nThe internal sort is completed. Check the initial sorted runs! "
            "\n";
    cout << "\nNow there are " << tempFiles.size() << " runs.\n\n";

    // ---------------------------------------------------------
    // 階段二：外部合併 (External Merge) - 兩兩合併暫存檔直到剩下一個
    // ---------------------------------------------------------
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
      cout << "Now there are " << tempFiles.size() << " runs.\n";
      if (tempFiles.size() > 1)
        printf("\n");
    }

    auto externalEndTime = chrono::high_resolution_clock::now();
    auto externalDuration = chrono::duration_cast<chrono::milliseconds>(
        externalEndTime - externalStartTime);

    if (tempFiles.size() == 1) {
      remove(outputFile.c_str());
      rename(tempFiles[0].c_str(), outputFile.c_str());
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

  // ========== 任務二：構建主索引 (Primary Index) ==========
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
    Record tempRec;
    int offset = 0;
    float lastWeight = -1.0;

    while (readRecord(fin, tempRec)) {
      if (tempRec.weight != lastWeight) {
        index.push_back({tempRec.weight, offset});
        lastWeight = tempRec.weight;
      }
      offset++;
    }

    fin.close();

    cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    cout << "Mission 2: Build the primary index \n";
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
    cout << "\n<Primary index>: (key, offset)\n";

    int count = min(100, (int)index.size());
    for (int i = 0; i < count; i++) {
      cout << "[" << (i + 1) << "] (";
      cout << defaultfloat << setprecision(2) << index[i].key;
      cout << ", " << index[i].offset << ")\n";
    }
  }
};

// =========================================================
// 主程式與流程控制
// =========================================================

int main() {
  GraphManager gm;

  while (true) {
    PrintTitle();

    // 1. 取得 Buffer Size
    int new_buffer_size = GetBufferSize();

    cout << "\n";
    PrintMission1();

    // 2. 處理檔案輸入並執行 Mission 1 & 2
    // 如果回傳 false，代表使用者選擇 Quit，直接結束程式
    if (!ProcessMission1And2(gm, new_buffer_size)) {
      return 0;
    }

    printf("\n");
    PrintMission3();

    // 3. 處理 Mission 3 的範圍搜尋輸入
    float f_num1, f_num2;
    GetRangeSearchValues(f_num1, f_num2);
  }
  return 0;
}

// =========================================================
// 全域 UI / 字串與輸入處理函式實作
// =========================================================

int GetBufferSize() {
  int size;
  while (true) {
    cin >> size;
    if (size < 300 || size > 60000) {
      cout << "\n### It is NOT in [300,60000] ###\n\n";
      cout << "Input a new buffer size in [300, 60000]: ";
      continue;
    }
    break;
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
        return false; // 回傳 false 通知 main 結束程式
      } else {
        cout << "\nInput the file name: [0]Quit\n";
        continue; // 使用者不想退出，重新提示輸入檔名
      }
    }

    bool fileExists = false;

    // 防呆檢查步驟一與二：驗證檔名並確認檔案存在
    if (cmd.length() == 3 && cmd[0] >= '0' && cmd[0] <= '9' && cmd[1] >= '0' &&
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
      return true; // 任務成功執行，回傳 true 繼續後續流程
    } else {
      cout << "\npairs" << cmd << ".bin does not exist!!!\n\n";
      cout << "Input the file name: [0]Quit\n";
    }
  }
}

void GetRangeSearchValues(float &f_num1, float &f_num2) {
  while (true) {
    cin >> f_num1;
    if (f_num1 < 0.01 || f_num1 > 1) {
      cout << "\n### It is NOT in [0.01,1] ###\n";
      cout << "\nInput a floating number in [0.01, 1]: ";
      continue;
    }
    break;
  }
  while (true) {
    cin >> f_num2;
    if (f_num2 < 0.01 || f_num2 > 1) {
      cout << "\n### It is NOT in [0.01,1] ###\n";
      cout << "\nInput a floating number in [0.01, 1]: ";
      continue;
    }
    break;
  }
}

string ReadInput() {
  string input;
  while (true) {
    getline(cin, input);
    SkipSpace(input);
    if (!input.empty()) {
      break;
    }
  }
  return input;
}

void SkipSpace(string &str) {
  for (int i = 0; i < str.size(); i++) {
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
  cout << "Mission 1: External merge sort \n";
  cout << "##################################\n\n";
  cout << "Input the file name: [0]Quit\n";
}

void PrintMission3() {
  cout << "##################################\n";
  cout << "* 3: Range search to build index *\n";
  cout << "##################################\n\n";
  cout << "Input two values in (0,1] for range search.\n\n";
  cout << "Input a floating number in [0.01, 1]: ";
}