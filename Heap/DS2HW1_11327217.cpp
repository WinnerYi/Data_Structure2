//11327217 蔡易勳   

/// TODO: Double Ended Heap (DEAP) 樹根沒東西, Delete smallest/largest key
//  insert any key (左右 檢查 左邊吸最小, 右邊吸最大) 影片 2-07
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

void PrintTitle();
void SkipSpace(std::string &str);
std::string ReadInput();

/// NOTE: 從1號開始的唯一『序號』
struct Node {
  int id;                    // 唯一『序號』（key）
  int  lastYearGraduatesCount;      
  Node(int id, int lastYearGraduatesCount) {
    this->id = id;
    this-> lastYearGraduatesCount = lastYearGraduatesCount;
  } 
};

void printHeap( std::vector <Node> heap, int cmd) {
  if (cmd == 1) { std::cout << "<min heap>" << std::endl; }
  else if (cmd == 2) { std::cout << "<min-max heap>" << std::endl; }
  else if (cmd == 3) { std::cout << "<max heap>" << std::endl; }
  else if (cmd == 4) { std::cout << "<DEAP>" << std::endl; }
  
  // 1. Root 永遠在index 0
  if (cmd != 4) { std::cout << "root: [" << heap[0].id << "] " << heap[0].lastYearGraduatesCount << std::endl; }
  
  // Bottom 永遠在最後一個索引
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




class MinHeap {
 private:
  std::vector<Node> heap; 
                                              //           0
  void heapifyUp(int index) {                 //       1      2
    while (index > 0) {                       //     3  4   5   6
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
  void reSet() { heap.clear(); }
  

   std::vector <Node> getHeap() { return heap; }
};

class MaxHeap {
 private:
  std::vector <Node> heap;
  void heapifyUp(int index) {
    while (index > 0) {
      int parentIndex = (index - 1) / 2;
      if (heap[index].lastYearGraduatesCount > heap[parentIndex].lastYearGraduatesCount) {
        std::swap(heap[index], heap[parentIndex]);
        index = parentIndex; 
      } else {
        break;
      }
    }
  }
 public:
  void insert(int id, int count) {
    heap.push_back(Node(id, count));
    heapifyUp(heap.size() - 1);
  }
  void reSet() { heap.clear(); }
  std::vector <Node> getHeap() { return heap; }
};


//                  Insertion of Min-Max heap

// Step 1
// 將欲新增的元素插入 Min-Max Heap 最後一個節點

// Step 2
// 將新增節點與其父節點做比較，若父節點位於 Min (Max) Level，且新增節點小於 (大於) 父節點，則交換二者的位置。

// Step 3
// 若交換後新增節點的位於 Min (Max) Level，則依序往上與各 Min (Max) Level 的節點比較，若新增節點較小(大)，則二者交換。

// Step 4
// 重複 Step 3，直至不能再交換或到 root 為止。
class MinMaxHeap {
 private:
    std::vector<Node> heap;
    // 輔助函式：判斷目前索引位於 Min 層還是 Max 層
    bool isMinLevel(int index) {
        int level = 0;
        int current = index + 1; 
        while (current > 1) {
            current = current / 2;
            level++;
        }
        return (level % 2 == 0);
    }

    // 沿著 Min 層（只跟祖父節點比較）向上調整
    void heapifyUpMin(int index) {
        // index > 2 代表它至少在第 2 層以下，才會有「祖父節點」
        while (index > 2) {
            int parent = (index - 1) / 2;
            int grandparent = (parent - 1) / 2;
            
            // 如果比祖父節點還要小，就交換並繼續往上找
            if (heap[index].lastYearGraduatesCount < heap[grandparent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[grandparent]);
                index = grandparent; 
            } else {
                break;
            }
        }
    }

    // 沿著 Max 層（只跟祖父節點比較）向上調整
    void heapifyUpMax(int index) {
        while (index > 2) {
            int parent = (index - 1) / 2;
            int grandparent = (parent - 1) / 2;
            
            // 如果比祖父節點還要大，就交換並繼續往上找
            if (heap[index].lastYearGraduatesCount > heap[grandparent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[grandparent]);
                index = grandparent; 
            } else {
                break;
            }
        }
    }

    // 主調整邏輯
    void heapifyUp(int index) {
        if (index == 0) return; // 根節點不用動

        int parent = (index - 1) / 2;

        if (isMinLevel(index)) {
            // 目前在 Min 層，但卻比 Max 層的父節點還大，這是不ok的
            if (heap[index].lastYearGraduatesCount > heap[parent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[parent]);
                heapifyUpMax(parent); // 交換後，現在跑到 Max 層了，沿著 Max 往上檢查
            } else {
                heapifyUpMin(index);  // 乖乖待在 Min 層，跟祖父比較
            }
        } else {
            // 目前在 Max 層，但卻比 Min 層的父節點還小，這是不ok的
            if (heap[index].lastYearGraduatesCount < heap[parent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[parent]);
                heapifyUpMin(parent); // 交換後，現在跑到 Min 層了，沿著 Min 往上檢查
            } else {
                heapifyUpMax(index);  // 乖乖待在 Max 層，跟祖父比較
            }
        }
    }

public:
    void insert(int id, int count) {
        Node newNode(id, count);
        heap.push_back(newNode);
        heapifyUp(heap.size() - 1);
    }
    void reSet() { heap.clear(); }

    std::vector<Node> getHeap() { 
        return heap; 
    }
};

class DEAP {
 private:
    std::vector<Node> heap;

    // 判斷是否在 Min Heap (左半邊) 
    bool isInMinHeap(int index) {
        if (index == 1) return true;
        if (index == 2) return false;
        
        int temp = index;
        // 不斷往上找父節點，直到碰到 1 或 2
        while (temp > 2) {
            temp = (temp - 1) / 2;
        }
        return (temp == 1);
    }

    // 向上調整 Min Heap
    void heapifyUpMin(int index) {
        while (index > 1) { // Min Heap 頂點是 1
            int parent = (index - 1) / 2;
            if (heap[index].lastYearGraduatesCount < heap[parent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else break;
        }
    }

    // 向上調整 Max Heap
    void heapifyUpMax(int index) {
        while (index > 2) { // Max Heap 頂點是 2
            int parent = (index - 1) / 2;
            if (heap[index].lastYearGraduatesCount > heap[parent].lastYearGraduatesCount) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else break;
        }
    }

public:
    DEAP() {
        heap.push_back(Node(-1, -1)); // 根節點 沒放東西
    }

    void insert(int id, int count) {
      Node newNode(id, count);
        heap.push_back(newNode);
        int index = heap.size() - 1;
        if (index == 1) return; // 第一筆資料直接放 index 1

        // 核心：尋找 partner 的純迴圈計算
        // 找出 index 所在的層級寬度 H (2 的次方)
        int H = 1;
        while ((H * 2 - 1) <= index) {
            H *= 2;
        }
        // 此時 H 代表該層級的節點總數（例如第 2 層有 4 個節點）

        if (isInMinHeap(index)) {
            int partner = index + (H / 2); // 跨到右半邊
            if (partner >= heap.size()) partner = (partner - 1) / 2; // 如果沒 partner，找其父節點
            
            if (heap[index].lastYearGraduatesCount > heap[partner].lastYearGraduatesCount) {
                std::swap(heap[index], heap[partner]);
                heapifyUpMax(partner);
            } else {
                heapifyUpMin(index);
            }
        } else {
            int partner = index - (H / 2); // 跨到左半邊
            if (heap[index].lastYearGraduatesCount < heap[partner].lastYearGraduatesCount) {
                std::swap(heap[index], heap[partner]);
                heapifyUpMin(partner);
            } else {
                heapifyUpMax(index);
            }
        }
    }
    void reSet() { heap.clear(); }
    std::vector <Node> getHeap() { return heap; }
};



class GraduateInfo{ 
 private:
  int id; // 唯一序號 額外做

  std::string schoolCode;
  std::string schoolName;
  std::string deptCode;
  std::string deptName;
  std::string daytime;
  std::string educationDivision; // 進修別
  std::string level;
  std::string studentCount;
  std::string teacherCount;
  std::string lastYearGraduatesCount;
  std::string cityNum;
  std::string cityName;
  std::string typeNum;
  std::string systemType;

  public:
    void setGraduateInfo(std::string schoolCode, std::string schoolName, std::string deptCode, 
        std::string deptName, std::string daytime, std::string educationDivision,
        std::string level,
        std::string studentCount, std::string teacherCount, std::string lastYearGraduatesCount, std::string cityNum,
        std::string cityName,std::string typeNum, std::string systemType) {
      
      this->schoolCode = schoolCode;
      this->schoolName = schoolName;
      this->deptCode = deptCode;
      this->deptName = deptName;
      this->daytime = daytime;
      this->educationDivision = educationDivision;
      this->level = level;
      this->studentCount = studentCount;
      this->teacherCount = teacherCount;
      this->lastYearGraduatesCount = lastYearGraduatesCount;
      this->cityNum = cityNum;
      this->cityName = cityName;
      this->typeNum = typeNum;
      this->systemType = systemType;
    } 
    void setId(int id) {this->id = id; }

    // not yet done
    std::string getSchoolCode() { return this->schoolCode; }
    std::string getSchoolName() { return this->schoolName; }
    std::string getDeptCode() { return this->deptCode; }
    std::string getDeptName() { return this->deptName; }
    
    std::string getCityName() { return this->cityName; }
    std::string getSystemType() { return this->systemType; }
    

};

class UniversityCatalog {
 private:
  MinHeap minHeap;
  MinMaxHeap minMaxHeap;
  MaxHeap maxHeap;
  DEAP deap;
  std::vector<GraduateInfo> info;
  
 public:
  UniversityCatalog() {
  }
  void reSet() {
    info.clear();

  }
  bool fetchFile(int cmd) {
    std::ifstream in;
    while (1) {
        std::cout << "Input a file number ([0] Quit): ";
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
    int count_id = 1;
    std::string title;
    std::getline(in, title);
    std::getline(in, title);
    std::getline(in, title);

    std::string line;
    while (std::getline(in, line)) {
        

        std::stringstream ss(line);

        std::string schoolCode, schoolName, deptCode, deptName,
                    daytime, educationDivision, level, studentCount,
                    teacherCount, lastYearGraduatesCount, cityNum,
                    cityName,typeNum, systemType;

        std::getline(ss, schoolCode, '\t');
        std::getline(ss, schoolName, '\t');
        std::getline(ss, deptCode, '\t');
        std::getline(ss, deptName, '\t');
        std::getline(ss, daytime, ' ');
        std::getline(ss, educationDivision, '\t');
        std::getline(ss, level, '\t');
        std::getline(ss, studentCount, '\t');
        std::getline(ss, teacherCount, '\t');
        std::getline(ss, lastYearGraduatesCount, '\t');
        std::getline(ss,  cityNum, ' ');
        std::getline(ss, cityName, '\t');
        std::getline(ss, typeNum, ' ');
         std::getline(ss, systemType);

        
        GraduateInfo g;
        g.setGraduateInfo( schoolCode, schoolName, deptCode, deptName,
                            daytime, educationDivision, level, studentCount,
                            teacherCount, lastYearGraduatesCount, cityNum,
                            cityName,typeNum, systemType);
        g.setId(count_id);
        
        if (cmd == 1) {
          minHeap.insert(count_id, stoi(lastYearGraduatesCount));
        } else if (cmd == 2) {
          minMaxHeap.insert(count_id, stoi(lastYearGraduatesCount));
        } else if (cmd == 3) {
          maxHeap.insert(count_id, stoi(lastYearGraduatesCount));
        } else if (cmd == 4) {
          deap.insert(count_id, stoi(lastYearGraduatesCount));
        }
        
        info.push_back(g); 
        count_id = count_id + 1;
    }

    in.close();
    return true;
}


void doTask(int cmd) {
  if (cmd == 1) {
    printHeap(minHeap.getHeap(), 1);
    minHeap.reSet();
  } else if (cmd == 2) {
    printHeap(minMaxHeap.getHeap(), 2);
    minMaxHeap.reSet();

  } else if (cmd == 3) {
    printHeap(maxHeap.getHeap(), 3);
    maxHeap.reSet();
  } else if (cmd == 4) {
    printHeap(deap.getHeap(), 4);
    deap.reSet();
  }
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
    } else if (cmd != 1 && cmd != 2){ // && cmd != 3 && cmd != 4
      std::cout << "\nCommand does not exist!\n";
    } else {
      printf("\n");
      if (uc.fetchFile(cmd)) {
        uc.doTask(cmd);
      }
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
  std::cout << "***************************** ****\n";
  std::cout << "Input a choice(0, 1, 2): ";
} 
