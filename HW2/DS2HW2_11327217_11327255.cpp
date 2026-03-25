// 11327217 蔡易勳   11327255許頌恩
// demo link: https://dsds.lab214b.uk:5001/DS/DS2HW2DEMO/DEMOb
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

void PrintTitle();
void SkipSpace(string &str);
string ReadInput();
int safeStoi(string s);

struct DataEntry {
    int studentCount;
    vector<int> ids;
    DataEntry(int count, int id) {
        studentCount = count;
        ids.push_back(id);
    }
};

struct TreeNode {  
    vector<DataEntry> entries;
    vector<TreeNode*> children; 
    bool isLeaf;
  TreeNode() {
        isLeaf = true;
    }
};

class GraduateInfo{ 
 private:
  int id;
  string schoolCode, schoolName, deptCode, deptName;
  string educationDivision, level, studentCount;
  string teacherCount, lastYearGraduatesCount, cityName, systemType;

 public:
    void setGraduateInfo(string schoolCode, string schoolName, string deptCode, 
        string deptName, string educationDivision, string level,
        string studentCount, string teacherCount, string lastYearGraduatesCount, 
        string cityName, string systemType) {
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
    void setId(int id) { this->id = id; }

    int getId() { return id; }
    string getSchoolName() { return schoolName; }
    string getDeptName() { return deptName; }
    string getEducationDivision() { return educationDivision; }
    string getLevel() { return level; }
    string getStudentCount() { return studentCount; }
    string getLastYearGraduatesCount() { return lastYearGraduatesCount; }
};

/// NOTE: 從1號開始的唯一『序號』!

struct AVLNode {
    std::string deptName;   // 科系名稱 (Key)
    std::vector<int> ids;     // 儲存相同科系的所有序號
    int height;
    AVLNode* left;
    AVLNode* right;

    AVLNode(std::string deptName, int id) {
        this->deptName = deptName;
        this->ids.push_back(id); // 建立節點時存入第一筆序號
        this->height = 1;
        this->left = nullptr;
        this->right = nullptr;
    }
};

class AVLTree {
 private:
    AVLNode* root;

    int getMax(int a, int b) {
        return (a > b) ? a : b;
    }

    int getHeight(AVLNode* n) {
        if (n == nullptr) return 0;
        return n->height;
    }

    void updateHeight(AVLNode* n) {
        if (n != nullptr) {
            n->height = 1 + getMax(getHeight(n->left), getHeight(n->right));
        }
    }

    int getBalance(AVLNode* n) {
        if (n == nullptr) return 0;
        return getHeight(n->left) - getHeight(n->right);
    }

    
    AVLNode* rotateRR(AVLNode* x) {
        AVLNode* y = x->right;
        x->right = y->left;
        y->left = x;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode* rotateLL(AVLNode* x) {
        AVLNode* y = x->left;
        x->left = y->right;
        y->right = x;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode* rotateLR(AVLNode* x) {
        x->left = rotateRR(x->left);
        return rotateLL(x);
    }

    AVLNode* rotateRL(AVLNode* x) {
        x->right = rotateLL(x->right);
        return rotateRR(x);
    }

    // 內部遞迴 Insert
    AVLNode* insertNode(AVLNode* node, std::string dept, int id) {
        // 1. 找到空位，建立新節點
        if (node == nullptr) {
            return new AVLNode(dept, id);
        }

        // 2. 比較字串大小決定走向
        if (dept < node->deptName) {
            node->left = insertNode(node->left, dept, id);
        } else if (dept > node->deptName) {
            node->right = insertNode(node->right, dept, id);
        } else {
            // 3. 科系名稱相同！不新增節點，直接把序號加入原節點的 vector 中
            // 因為是「依序號由小到大一筆一筆新增」，所以 push_back 就會是排好序的
            node->ids.push_back(id);
            return node; // 沒有新增節點，樹高不變，直接 return
        }

        // 更新高度與檢查平衡 (只有新增節點才會走到這裡)
        updateHeight(node);
        int balance = getBalance(node);

        // 判斷並執行旋轉 (字串比較)
        if (balance > 1 && dept < node->left->deptName) return rotateLL(node);
        if (balance < -1 && dept > node->right->deptName) return rotateRR(node);
        if (balance > 1 && dept > node->left->deptName) return rotateLR(node);
        if (balance < -1 && dept < node->right->deptName) return rotateRL(node);

        return node;
    }

    // 遞迴計算節點總數
    int countNodes(AVLNode* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    // 遞迴清空樹
    void clearTree(AVLNode* node) {
        if (node != nullptr) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

public:
    AVLTree() { root = nullptr; }
    ~AVLTree() { clearTree(root); }

    void insert(std::string dept, int id) {
        root = insertNode(root, dept, id);
    }

    // --- 題目要求的三個最終輸出 ---

    // 1. 找出整棵樹的樹高
    int getTreeHeight() {
        return getHeight(root);
    }

    // 2. 找出整棵樹的節點數
    int getTotalNodes() {
        return countNodes(root);
    }

    // 3. 找出樹根內的所有資料
    void printRootData(std::vector<GraduateInfo> temp_info) {
        cout << "Tree height = " << getTreeHeight() << endl;
        cout << "Number of nodes = " << getTotalNodes() << endl;
       
        for (int i = 0; i < root->ids.size(); i++) {
          std::cout << (i + 1) << ": [" << root->ids[i] << "] " 
                    << temp_info[root->ids[i] - 1].getSchoolName() << ", " 
                    << temp_info[root->ids[i] - 1].getDeptName() << ", " 
                    << temp_info[root->ids[i] - 1].getEducationDivision() << ", " 
                    << temp_info[root->ids[i] - 1].getLevel() << ", " 
                    << temp_info[root->ids[i] - 1].getStudentCount() << ", " 
                    << temp_info[root->ids[i] - 1].getLastYearGraduatesCount() << std::endl;
        }
        std::cout << std::endl;
    }

    void clear() {
        clearTree(root);
        root = nullptr;
    }
};

    

struct SplitResult {
    DataEntry* promotedEntry = nullptr;
    TreeNode* rightNode = nullptr;
};

class Two_Three_Tree {
 private:
    TreeNode* root;
    int nodeCount;

    void insertIntoNode(TreeNode* node, const DataEntry& entry, TreeNode* rightChild = nullptr) {
        int i = 0;
        while (i < node->entries.size() && entry.studentCount > node->entries[i].studentCount) {
            i++;
        }
        node->entries.insert(node->entries.begin() + i, entry);
        if (rightChild != nullptr) {
            node->children.insert(node->children.begin() + i + 1, rightChild);
        }
    }

    SplitResult insertRecursive(TreeNode* node, int studentCount, int id) {
        SplitResult result;

        //檢查是否已經有相同的學生數
        for (int i = 0; i < node->entries.size(); i++) {
            if (node->entries[i].studentCount == studentCount) {
                node->entries[i].ids.push_back(id);
                return result;
            }
        }

        // 如果是葉子節點，直接插入
        if (node->isLeaf) {
            insertIntoNode(node, DataEntry(studentCount, id));
        } else {
            // 尋找該往哪個子節點走
            int childIdx = 0;
            while (childIdx < node->entries.size() && studentCount > node->entries[childIdx].studentCount) {
                childIdx++;
            }

            // 遞迴往下
            SplitResult childResult = insertRecursive(node->children[childIdx], studentCount, id);

            // 如果下方子節點有分裂，接收往上推的資料
            if (childResult.promotedEntry != nullptr) {
                insertIntoNode(node, *childResult.promotedEntry, childResult.rightNode);
                delete childResult.promotedEntry;
            }
        }

        // 檢查當前節點是否滿
        if (node->entries.size() == 3) {
            TreeNode* rightSibling = new TreeNode();
            rightSibling->isLeaf = node->isLeaf;
            nodeCount++; // 產生新節點

            // 將最大的 Entry[2] 搬到右邊的新節點
            rightSibling->entries.push_back(node->entries[2]);

            // 如果不是葉子，要把右半邊的小孩也搬過去
            if (!node->isLeaf) {
                rightSibling->children.push_back(node->children[2]);
                rightSibling->children.push_back(node->children[3]);
                node->children.resize(2); // 原節點只保留前兩個小孩
            }

            // 準備把中間的 Entry[1] 往上推給爸爸
            result.promotedEntry = new DataEntry(node->entries[1]);
            result.rightNode = rightSibling;

            // 原節點縮編，只保留最小的 Entry[0]
            node->entries.pop_back(); 
            node->entries.pop_back(); 
        }

        return result;
    }

public:
    Two_Three_Tree() {
        root = nullptr;
        nodeCount = 0;
    }

    void clear() {
        root = nullptr;
        nodeCount = 0;
    }

    void insert(int studentCount, int id) {
        if (root == nullptr) {
            root = new TreeNode();
            root->entries.push_back(DataEntry(studentCount, id));
            nodeCount++;
            return;
        }

        SplitResult res = insertRecursive(root, studentCount, id);
        
        // 如果 Root 也分裂了，整棵樹長高，產生新的 Root
        if (res.promotedEntry != nullptr) {
            TreeNode* newRoot = new TreeNode();
            newRoot->isLeaf = false;
            newRoot->entries.push_back(*res.promotedEntry);
            newRoot->children.push_back(root);
            newRoot->children.push_back(res.rightNode);
            
            root = newRoot;
            nodeCount++;
            delete res.promotedEntry;
        }
    }

    int getHeight() {
        if (root == nullptr) return 0;
        int height = 1;
        TreeNode* current = root;
        while (!current->isLeaf) {
            height++;
            current = current->children[0];
        }
        return height;
    }

    int getNodeCount() { return nodeCount; }
    TreeNode* getRoot() { return root; }
};





class UniversityCatalog {
 private:
  vector<GraduateInfo> info;
  Two_Three_Tree tree23;
  AVLTree avl_tree;
  std::string last_txt_path; // 紀錄上一次讀的資料檔
  std::string cur_txt_path;
  
 public:
  void reSet() {
    info.clear();
    tree23.clear();
  }

  int getInfoCount() {
    return info.size();
  }

  bool fetchFile() {
    ifstream in;
    while (1) {
        cout << "Input a file number ([0] Quit): ";
        string file_num = ReadInput();
        if (file_num == "0") return false;

        string txt_path = "input" + file_num + ".txt";
        in.open(txt_path);

        if (in.fail()) { 
            cout << "\n### " << txt_path << " does not exist! ###\n\n";
            continue;
        }
        cur_txt_path = txt_path;
        break;
    }
    
    reSet(); // 讀新檔案前清空舊資料

    int count_id = 1;
    string title;
    getline(in, title);
    getline(in, title);
    getline(in, title);

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string schoolCode, schoolName, deptCode, deptName, educationDivision, level;
        string studentCount, teacherCount, lastYearGraduatesCount, cityName, systemType;

        getline(ss, schoolCode, '\t');
        getline(ss, schoolName, '\t');
        getline(ss, deptCode, '\t');
        getline(ss, deptName, '\t');
        getline(ss, educationDivision, '\t');
        getline(ss, level, '\t');
        getline(ss, studentCount, '\t');
        getline(ss, teacherCount, '\t');
        getline(ss, lastYearGraduatesCount, '\t');
        getline(ss, cityName, '\t');
        getline(ss, systemType);
        
        GraduateInfo g;
        g.setGraduateInfo(schoolCode, schoolName, deptCode, deptName, educationDivision, 
                          level, studentCount, teacherCount, lastYearGraduatesCount, 
                          cityName, systemType);
        g.setId(count_id);
        info.push_back(g); 
        int sCount = safeStoi(studentCount);
     
        
        tree23.insert(sCount, count_id); 
        
        count_id++;
    }
    in.close();
    return true;
}


void doTask(string cmd) { 
  if (cmd == "1") {
    cout << "Tree height = " << tree23.getHeight() << endl;
    cout << "Number of nodes = " << tree23.getNodeCount() << endl;
    
    TreeNode* rootNode = tree23.getRoot();
    if (rootNode != nullptr) {
        int serialNum = 1;
        // 走訪 Root 裡所有的 Entry
        for (int i = 0; i < rootNode->entries.size(); i++) {
            // 走訪該 Entry 裡所有的 id (序號)
            for (int j = 0; j < rootNode->entries[i].ids.size(); j++) {
                int targetId = rootNode->entries[i].ids[j];
                GraduateInfo targetData = info[targetId - 1]; // 陣列 index 從 0 開始

                cout << serialNum++ << ": [" 
                     << targetData.getId() << "] "
                     << targetData.getSchoolName() << ", "
                     << targetData.getDeptName() << ", "
                     << targetData.getEducationDivision() << ", "
                     << targetData.getLevel() << ", "
                     << targetData.getStudentCount() << ", "
                     << targetData.getLastYearGraduatesCount() 
                     << endl;
            }
        }
    }
  } else if (cmd == "2") {

    if (cur_txt_path == last_txt_path) {
      cout << "### AVL tree has been built. ###\n";
    } else {
      avl_tree.clear();
      for (int i = 0; i < info.size(); i++) {
        avl_tree.insert(info[i].getDeptName(), info[i].getId());
      }
      last_txt_path = cur_txt_path;
    }

   
    avl_tree.printRootData(info);
    

  } 
}

};

int main() {
  UniversityCatalog uc;
  
  while (true) {
    PrintTitle();
    string cmd = ReadInput(); // demo防呆
    if (cmd == "0") { 
      return 0;
    } else if (cmd == "1"){
      cout << endl;
      if (uc.fetchFile()) {
        uc.doTask(cmd);
      }
    }  else if (cmd == "2"){ 
      if (uc.getInfoCount() == 0) {
        cout << "### Choose 1 first. ###\n";
      } else {
        uc.doTask(cmd);
      }

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
    for (char c : s) { // 只保留數字字符
        if (c >= '0' && c <= '9') {
            cleanStr += c;
        }
    }
    if (cleanStr.empty()) {
        return 0;
    }
    return stoi(cleanStr);
}

void PrintTitle () {
  cout << "* Data Structures and Algorithms *\n";
  cout << "****** Balanced Search Tree ******\n";
  cout << "* 0. QUIT                        *\n";
  cout << "* 1. Build 23 tree               *\n";
  cout << "* 2. Build AVL tree              *\n";
  cout << "**********************************\n";
  cout << "Input a choice(0, 1, 2): ";
}