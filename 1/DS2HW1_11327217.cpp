//11327217 蔡易勳   

/// TODO: Double Ended Heap (DEAP) 樹根沒東西, Delete smallest/largest key
//  insert any key (左右 檢查 左邊吸最小, 右邊吸最大) 影片 2-07
 
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <climits>
#include <stack>
#include <queue>
void PrintTitle();
void SkipSpace(std::string &str);
std::string ReadInput();

/// NOTE: 從1號開始的唯一『序號』
struct Node {
  int id;                    // 唯一『序號』（key）
  int lastYearGraduatesCount;      
  Node(int id, int lastYearGraduatesCount) {
    this->id = id;
    this-> lastYearGraduatesCount = lastYearGraduatesCount;
  } 
};
class MinHeap {
 private:
  std::vector<Node> heap; 

  void heapifyUp(int index) {
    while (index > 0) {
      int parentIndex = (index - 1) / 2;
      
      if (heap[index].lastYearGraduatesCount < heap[parentIndex].lastYearGraduatesCount) {
        std::swap(heap[index], heap[parentIndex]); // 直接交換整個 Node
        index = parentIndex;
      } else {
        break;
      }
    }
  }

 public:
  void insert(int id, int count) {
    heap.push_back(Node(id, count)); // 加在最後面
    heapifyUp(heap.size() - 1);      // 向上調整
  }

  void printHeap() {
      if (heap.empty()) {
        std::cout << "Heap is empty." << std::endl;
        return;
      }

      std::cout << "<min heap>" << std::endl;

      // 1. Root 永遠在索引 0
      std::cout << "root: [" << heap[0].id << "] " << heap[0].lastYearGraduatesCount << std::endl;

      // 2. Bottom 永遠在最後一個索引
      int lastIdx = heap.size() - 1;
      std::cout << "bottom: [" << heap[lastIdx].id << "] " << heap[lastIdx].lastYearGraduatesCount << std::endl;

      // 3. Leftmost bottom (最後一層最左邊)
      // 邏輯：找到最後一層的起始索引。
      // 公式：找出不大於 lastIdx 的最大 (2^n - 1)
      int leftmostIdx = 0;
      while ((leftmostIdx * 2 + 1) <= lastIdx) {
          leftmostIdx = leftmostIdx * 2 + 1;
      }
      std::cout << "leftmost bottom: [" << heap[leftmostIdx].id << "] " << heap[leftmostIdx].lastYearGraduatesCount << std::endl;
  }
};

class MaxHeap {
};

class MinMaxHeap {
};

class DEAP{

};



class GraduateInfo{ 
 private:
  int id; // 唯一序號 額外做
  int schoolCode;
  std::string schoolName;
  int deptCode;
  std::string deptName;
  std::string dayNightMode;
  int level;
  int studentCount;
  int teacherCount;
  int lastYearGraduatesCount;
  std::string cityName;
  std::string systemType;

  public:
    void setGraduateInfo(int schoolCode, std::string schoolName, int deptCode, 
        std::string deptName, std::string dayNightMode, int level, 
        int studentCount, int teacherCount, int lastYearGraduatesCount, 
        std::string cityName, std::string systemType) {
      
      this->schoolCode = schoolCode;
      this->schoolName = schoolName;
      this->deptCode = deptCode;
      this->deptName = deptName;
      this->dayNightMode = dayNightMode;
      this->level = level;
      this->studentCount = studentCount;
      this->teacherCount = teacherCount;
      this->lastYearGraduatesCount = lastYearGraduatesCount;
      this->cityName = cityName;
      this->systemType = systemType;
    } 
    void setId(int id) {this->id = id; }

    int getSchoolCode() { return this->schoolCode; }
    std::string getSchoolName() { return this->schoolName; }
    int getDeptCode() { return this->deptCode; }
    std::string getDeptName() { return this->deptName; }
    std::string getDayNightMode() { return this->dayNightMode; }
    int getLevel() { return this->level; }
    int getStudentCount() { return this->studentCount; }
    int getTeacherCount() { return this->teacherCount; }
    int getLastYearGraduatesCount() { return this->lastYearGraduatesCount; }
    std::string getCityName() { return this->cityName; }
    std::string getSystemType() { return this->systemType; }
    

};

class UniversityCatalog {
 private:
  MinHeap minHeap;
  std::vector<GraduateInfo> info;
  int max_schoolCode;
  
 public:
  UniversityCatalog() {
    max_schoolCode = 0;
  }
  void reSet() {
    //minHeap.clear();
    info.clear();
    info.shrink_to_fit();  // 釋放多餘容量
    max_schoolCode = 0;

  }
  bool fetchFile() {
    std::ifstream in;
    while (1) {
        std::cout << "Input a file number [0: quit]: ";
        std::string file_num = ReadInput();
        if (file_num == "0") return false;

        std::string txt_path = "input" + file_num + ".txt";
        in.open(txt_path);

        if (in.fail()) { 
            std::cout << std::endl 
                      << "### " << txt_path 
                      << " does not exist! ###" 
                      << std::endl;
            printf("\n");
            continue;
        }
        break;
    }

    std::string title;
    std::getline(in, title);
    std::getline(in, title);
    std::getline(in, title);
    

    std::string line;
    while (std::getline(in, line)) {
        int count_id = 1;

        std::stringstream ss(line);

        std::string schoolCode_str, schoolName_str, deptCode_str, deptName_str,
                    dayNightMode_str, level_str, studentCount_str,
                    teacherCount_str, lastYearGraduatesCount_str,
                    cityName_str, systemType_str;

        std::getline(ss, schoolCode_str, '\t');
        std::getline(ss, schoolName_str, '\t');
        std::getline(ss, deptCode_str, '\t');
        std::getline(ss, deptName_str, '\t');
        std::getline(ss, dayNightMode_str, '\t');
        std::getline(ss, level_str, '\t');
        std::getline(ss, studentCount_str, '\t');
        std::getline(ss, teacherCount_str, '\t');
        std::getline(ss, lastYearGraduatesCount_str, '\t');
        std::getline(ss, cityName_str, '\t');
        std::getline(ss, systemType_str);

        // 數字轉換
        int schoolCode = std::stoi(schoolCode_str);
        int studentCount = std::stoi(studentCount_str);
        int teacherCount = std::stoi(teacherCount_str);
        int lastYearGraduatesCount = std::stoi(lastYearGraduatesCount_str);
        int level = std::stoi(level_str);
        int deptCode = stoi(deptCode_str);

        if (schoolCode > max_schoolCode) {
            max_schoolCode = schoolCode;
        }

        GraduateInfo g;
        g.setGraduateInfo( schoolCode, schoolName_str, deptCode, deptName_str, dayNightMode_str, level,
                           studentCount, teacherCount, lastYearGraduatesCount, cityName_str, systemType_str);
        g.setId(count_id);

        minHeap.insert(count_id, lastYearGraduatesCount);
        info.push_back(g);
        count_id = count_id + 1;
    }

    in.close();
    return true;
}

int getinfoize() {
  return info.size();
}
int getTreeHeight() {
  
}

void taskOne() {
  minHeap.printHeap();
}

void taskTwo() { 
  
}

void taskThree() {

}

void taskFour() {
  
}
};
int main() {
  UniversityCatalog uc;
  bool deleteMin = true;
  while (true) {
    PrintTitle();
    int cmd;
    std::cin >> cmd;
    if (std::cin.fail()) { // 檢查輸入是否失敗
      return 0;
    } else if (cmd == 0 ){
      return 0;
    } else if (cmd == 1) {
      
      printf("\n");
      if (uc.fetchFile()) {
        uc.taskOne();
      }
      deleteMin = true;
    } else if (cmd == 2) {
      if (uc.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      uc.taskTwo();
      
    } else if (cmd == 3) {
      if (uc.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      printf("\n");
      uc.taskThree();
      deleteMin = !deleteMin;
     
    } else if (cmd == 4) {
      if (uc.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      deleteMin = true;
      printf("\n");
      uc.taskFour();
    
    } else {
      printf("\n");
      std::cout << "Command does not exist!\n";
    }
    printf("\n");  
  }
}

std::string ReadInput() {
  std::string input;
  while (1) {
    std::getline(std::cin, input);
    SkipSpace(input);
    if (input.empty()) continue;
    else break;
  }
  return input;
}

void SkipSpace(std::string &str) {
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
  std::cout << "* Data Structures and Algorithms *\n";
  std::cout << "*** Heap Construction and Use ****\n";
  std::cout << "* 0. QUIT                        *\n";
  std::cout << "* 1. Build a min heap            *\n";
  std::cout << "* 2. Build a min-max heap        *\n";
  //std::cout << "* 3. Delete the min on schoolCode field  *\n";    
  //std::cout << "* 4. Rebuild the balanced schoolCode BST *\n";
  std::cout << "**********************************\n";
  std::cout << "Input a choice(0, 1, 2): ";
} 
