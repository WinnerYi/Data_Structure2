// 11327217 蔡易勳   11327255許頌恩

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

void PrintTitle();
void SkipSpace(std::string &str);
std::string ReadInput();

/// NOTE: 從1號開始的唯一『序號』!

struct Node {
    int id;               // 唯一『序號』（key）
    int studentCount;      
    int height;           // 紀錄節點高度
    Node* left;
    Node* right;

    Node(int id, int studentCount) {
        this->id = id;
        this->studentCount = studentCount;
        this->height = 1; // 新節點預設為 leaf，高度為 1
        this->left = nullptr;
        this->right = nullptr;
    } 
};

class AVLTree {
private:
    Node* root;

    // --- 新增：自訂的比較函式，取代 std::max ---
    int getMax(int a, int b) {
        return (a > b) ? a : b;
    }

    // 取得節點高度
    int getHeight(Node* n) {
        if (n == nullptr) return 0;
        return n->height;
    }

    // 更新節點高度 (這裡改用我們自訂的 getMax)
    void updateHeight(Node* n) {
        if (n != nullptr) {
            n->height = 1 + getMax(getHeight(n->left), getHeight(n->right));
        }
    }

    // 計算平衡因子 (左子樹高度 - 右子樹高度)
    int getBalance(Node* n) {
        if (n == nullptr) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }
    
    // 處理 RR Imbalance (向左旋轉)
    Node* rotateRR(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        
        // 旋轉後更新高度
        updateHeight(x);
        updateHeight(y);
        
        return y; 
    }

    // 處理 LL Imbalance (向右旋轉)
    Node* rotateLL(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        y->right = x;
        
        // 旋轉後更新高度
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }

    // 處理 LR Imbalance
    Node* rotateLR(Node* x) {
        x->left = rotateRR(x->left);
        return rotateLL(x);
    }

    // 處理 RL Imbalance
    Node* rotateRL(Node* x) {
        x->right = rotateLL(x->right);
        return rotateRR(x);
    }

    // 內部遞迴 Insert 函式
    Node* insertNode(Node* node, int id, int studentCount) {
        if (node == nullptr) {
            return new Node(id, studentCount);
        }

        if (id < node->id) {
            node->left = insertNode(node->left, id, studentCount);
        } else if (id > node->id) {
            node->right = insertNode(node->right, id, studentCount);
        } else {
            return node;
        }

        // 更新當前節點高度
        updateHeight(node);

        // 檢查平衡因子
        int balance = getBalance(node);

        // LL Case
        if (balance > 1 && id < node->left->id) {
            return rotateLL(node);
        }
        
        // RR Case
        if (balance < -1 && id > node->right->id) {
            return rotateRR(node);
        }
        
        // LR Case
        if (balance > 1 && id > node->left->id) {
            return rotateLR(node);
        }
        
        // RL Case
        if (balance < -1 && id < node->right->id) {
            return rotateRL(node);
        }

        return node;
    }

public:
    AVLTree() {
        root = nullptr;
    }

    void insert(int id, int studentCount) {
        root = insertNode(root, id, studentCount);
    }
};

void printHeap( std::vector <Node> heap, int cmd) {
 
}


class Two_Three_Tree {
  ///TODO:
};




class GraduateInfo{ 
 private:
  int id; // 唯一序號 額外做

  std::string schoolCode;
  std::string schoolName;
  std::string deptCode;
  std::string deptName;
  std::string educationDivision; // 進修別
  std::string level;
  std::string studentCount;
  std::string teacherCount;
  std::string lastYearGraduatesCount;
  std::string cityName;
  std::string systemType;

  public:
    void setGraduateInfo(std::string schoolCode, std::string schoolName, std::string deptCode, 
        std::string deptName, std::string educationDivision,
        std::string level,
        std::string studentCount, std::string teacherCount, std::string lastYearGraduatesCount, 
        std::string cityName, std::string systemType) {
      
      this->schoolCode = schoolCode;
      this->schoolName = schoolName;
      this->deptCode = deptCode;
      this->deptName = deptName;
      
      this->educationDivision = educationDivision;
      this->level = level;
      this->studentCount = studentCount;
      this->teacherCount = teacherCount;
      this->lastYearGraduatesCount = lastYearGraduatesCount;
      
      this->cityName = cityName;
      this->systemType = systemType;
    } 
    void setId(int id) {this->id = id; }

    // not yet done
    std::string getSchoolCode() { return this->schoolCode; }
    std::string getSchoolName() { return this->schoolName; }
    std::string getDeptCode() { return this->deptCode; }
    std::string getDeptName() { return this->deptName; }

    std::string getEducationDivision() { return this-> educationDivision;}
    std::string getLevel() {return this->level; }
    std::string getLastYearGraduatesCount;
    std::string getCityName() { return this->cityName; }
    std::string getSystemType() { return this->systemType; }
    int getId() {return this->id; }
    

};

class UniversityCatalog {
 private:
 
  std::vector<GraduateInfo> info;
  std::vector<GraduateInfo> store_deap_info;
  
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
    info.clear();
    while (std::getline(in, line)) {
        

        std::stringstream ss(line);

        std::string schoolCode, schoolName, deptCode, deptName,
                    educationDivision, level, studentCount,
                    teacherCount, lastYearGraduatesCount, 
                    cityName, systemType;

        std::getline(ss, schoolCode, '\t');
        std::getline(ss, schoolName, '\t');
        std::getline(ss, deptCode, '\t');
        std::getline(ss, deptName, '\t');
        std::getline(ss, educationDivision, '\t');
        std::getline(ss, level, '\t');
        std::getline(ss, studentCount, '\t');
        std::getline(ss, teacherCount, '\t');
        std::getline(ss, lastYearGraduatesCount, '\t');
        std::getline(ss, cityName, '\t');
        std::getline(ss, systemType);

        
        GraduateInfo g;
        g.setGraduateInfo( schoolCode, schoolName, deptCode, deptName,
                             educationDivision, level, studentCount,
                            teacherCount, lastYearGraduatesCount, 
                            cityName, systemType);
        g.setId(count_id);
     
        if (cmd == 1) {
          // 2-3 tree insert
        } else if (cmd == 2) {
          // AVL tree insert
        } else if (cmd == 3) {
          
        } 
        
        info.push_back(g); 
       
        count_id = count_id + 1;
    }
     

    in.close();
    
    return true;
}


void doTask(int cmd) { // this is for task 1 ~ 3
  if (cmd == 1) {
    
    
  } else if (cmd == 2) {
    

  } else if (cmd == 3) {
   
  }
}


};
int main() {

  UniversityCatalog uc;
  
  while (true) {
    PrintTitle();
    int cmd;
    std::cin >> cmd;
    if (std::cin.fail()) { // 檢查輸入是否失敗
      return 0;
    } else if (cmd == 0 ){
      return 0;
    } else if (cmd != 1 && cmd != 2 && cmd != 3 && cmd != 4){  
      std::cout << "\nCommand does not exist!\n";
    } else {
      printf("\n");
      if (cmd == 4) {
       
      }
      else if (uc.fetchFile(cmd)) {
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
  std::cout << "* 3. Build a DEAP                *\n";    
  std::cout << "* 4: Top-K maximum from DEAP     *\n";
  std::cout << "**********************************\n";
  std::cout << "Input a choice(0, 1, 2, 3, 4): ";
} 
