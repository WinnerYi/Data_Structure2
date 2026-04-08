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

// NOTE: 從1號開始的唯一『序號』!

struct AVLNode {
    string deptName;   // 科系名稱 (Key)
    vector<int> ids;     // 儲存相同科系的所有序號
    int height;
    AVLNode* left;
    AVLNode* right;

    AVLNode(string deptName, int id) {
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
    
    // 計算平衡因子 (左子樹高度 - 右子樹高度)
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
    AVLNode* insertNode(AVLNode* node, string dept, int id) {
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
            node->ids.push_back(id);
            return node; 
        }

        updateHeight(node);
        int balance = getBalance(node);

        if (balance > 1 && dept < node->left->deptName) return rotateLL(node);
        if (balance < -1 && dept > node->right->deptName) return rotateRR(node);
        if (balance > 1 && dept > node->left->deptName) return rotateLR(node);
        if (balance < -1 && dept < node->right->deptName) return rotateRL(node);

        return node;
    }

    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    // 內部遞迴 Delete
    AVLNode* deleteNode(AVLNode* node, std::string dept) {
        if (node == nullptr) return node;

        if (dept < node->deptName) {
            node->left = deleteNode(node->left, dept);
        } else if (dept > node->deptName) {
            node->right = deleteNode(node->right, dept);
        } else {
            if (node->left == nullptr) {
                AVLNode* temp = node->right;
                delete node;
                node = temp;
            } else if (node->right == nullptr) {
                AVLNode* temp = node->left;
                delete node;
                node = temp;
            } else {
                AVLNode* temp = minValueNode(node->right);
                node->deptName = temp->deptName;
                node->ids = temp->ids;
                node->right = deleteNode(node->right, temp->deptName);
            }
        }

        if (node == nullptr) return node;

        updateHeight(node);
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateLL(node);
        if (balance > 1 && getBalance(node->left) < 0)
            return rotateLR(node);
        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateRR(node);
        if (balance < -1 && getBalance(node->right) > 0)
            return rotateRL(node);

        return node;
    }

    int countNodes(AVLNode* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    void clearTree(AVLNode* node) {
        if (node != nullptr) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

    /// MARK: 內部遞迴：尋找特定科系的節點
    AVLNode* searchNode(AVLNode* node, string dept) {
        if (node == nullptr || node->deptName == dept) {
            return node;
        }
        if (dept < node->deptName) {
            return searchNode(node->left, dept);
        } else {
            return searchNode(node->right, dept);
        }
    }

public:
    AVLTree() { root = nullptr; }
    ~AVLTree() { clearTree(root); }

    void insert(string dept, int id) {
        root = insertNode(root, dept, id);
    }

    void remove(string dept) {
        root = deleteNode(root, dept);
    }

    /// MARK: 取得特定科系的所有 ids (供任務四使用)
    vector<int> getIds(string dept) {
        AVLNode* result = searchNode(root, dept);
        if (result == nullptr) {
            return {};
        }
        return result->ids;
    }

    // --- 題目要求的三個最終輸出 ---

    int getTreeHeight() {
        return getHeight(root);
    }

    int getTotalNodes() {
        return countNodes(root);
    }

    void printRootData(vector<GraduateInfo> temp_info) {
        if (root == nullptr) {
            cout << "Tree is empty." << endl;
            return;
        }

        cout << "Tree height = " << getTreeHeight() << endl;
        cout << "Number of nodes = " << getTotalNodes() << endl;
       
        for (int i = 0; i < root->ids.size(); i++) {
          cout << (i + 1) << ": [" << root->ids[i] << "] " 
                    << temp_info[root->ids[i] - 1].getSchoolName() << ", " 
                    << temp_info[root->ids[i] - 1].getDeptName() << ", " 
                    << temp_info[root->ids[i] - 1].getEducationDivision() << ", " 
                    << temp_info[root->ids[i] - 1].getLevel() << ", " 
                    << temp_info[root->ids[i] - 1].getStudentCount() << ", " 
                    << temp_info[root->ids[i] - 1].getLastYearGraduatesCount() << endl;
        }
        cout << endl;
    }

    void clear() {
        clearTree(root);
        root = nullptr;
    }
};

// 資料項目 (2-3 樹節點內儲存的資料單元)
struct DataEntry {
    int studentCount;  // 學生數量作為 Key
    vector<int> ids;   // 學生數相同時記錄多個 id
    DataEntry(int count, int id) {
        studentCount = count;
        ids.push_back(id);
    }
};

// 2-3 樹節點 (可容納 1~2 個 Entry，3 個時觸發分裂)
struct TreeNode {  
    vector<DataEntry> entries;  // 內部儲存的資料項目
    vector<TreeNode*> children; // 子節點指標
    bool isLeaf;
    TreeNode() {
        isLeaf = true;
    }
};
    
// 分裂結果，向上回傳升級的項目和右子樹
struct SplitResult {
    DataEntry* promotedEntry = nullptr; // 向上提升的元素
    TreeNode* rightNode = nullptr;      // 分裂出來的右節點
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

        for (int i = 0; i < node->entries.size(); i++) {
            if (node->entries[i].studentCount == studentCount) {
                node->entries[i].ids.push_back(id);
                return result;
            }
        }

        if (node->isLeaf) {
            insertIntoNode(node, DataEntry(studentCount, id));
        } else {
            int childIdx = 0;
            while (childIdx < node->entries.size() && studentCount > node->entries[childIdx].studentCount) {
                childIdx++;
            }

            SplitResult childResult = insertRecursive(node->children[childIdx], studentCount, id);

            if (childResult.promotedEntry != nullptr) {
                insertIntoNode(node, *childResult.promotedEntry, childResult.rightNode);
                delete childResult.promotedEntry;
            }
        }

        if (node->entries.size() == 3) {
            TreeNode* rightSibling = new TreeNode();
            rightSibling->isLeaf = node->isLeaf;
            nodeCount++; 

            rightSibling->entries.push_back(node->entries[2]);

            if (!node->isLeaf) {
                rightSibling->children.push_back(node->children[2]);
                rightSibling->children.push_back(node->children[3]);
                node->children.resize(2); 
            }

            result.promotedEntry = new DataEntry(node->entries[1]);
            result.rightNode = rightSibling; 

            node->entries.pop_back(); 
            node->entries.pop_back(); 
        }

        return result;
    }


    void topKMaxHelper(TreeNode* node, int& k, vector<DataEntry>& result) {
        if (node == nullptr || k <= 0) return;

        if (node->isLeaf) {
            for (int i = node->entries.size() - 1; i >= 0 && k > 0; i--) {
                result.push_back(node->entries[i]);
                k--; 
            }
        } else {
            for (int i = node->entries.size(); i >= 0; i--) {
                topKMaxHelper(node->children[i], k, result);
                if (k <= 0) return; 

                if (i > 0) {
                    result.push_back(node->entries[i - 1]);
                    k--;
                    if (k <= 0) return;
                }
            }
        }
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
    
    void printTopKMax(int k, vector<GraduateInfo>& temp_info) {
        if (root == nullptr) {
            return;
        }

        std::vector<DataEntry> topKEntries;
        
        topKMaxHelper(root, k, topKEntries);

        int serialNum = 1;
        for (const auto& entry : topKEntries) {
            for (int id : entry.ids) {
                int index = id - 1; 
                std::cout << serialNum++ << ": [" << id << "] " 
                          << temp_info[index].getSchoolName() << ", " 
                          << temp_info[index].getDeptName() << ", " 
                          << temp_info[index].getEducationDivision() << ", " 
                          << temp_info[index].getLevel() << ", " 
                          << temp_info[index].getStudentCount() << ", " 
                          << temp_info[index].getLastYearGraduatesCount() << std::endl;
            }
        }
    }

    void insert(int studentCount, int id) {
        if (root == nullptr) {
            root = new TreeNode();
            root->entries.push_back(DataEntry(studentCount, id));
            nodeCount++;
            return;
        }

        SplitResult res = insertRecursive(root, studentCount, id);
        
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
  string last_txt_path; // 紀錄上一次讀的資料檔
  string cur_txt_path;
  
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
        
        studentCount = to_string(safeStoi(studentCount));
        lastYearGraduatesCount = to_string(safeStoi(lastYearGraduatesCount));
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
    last_txt_path = "";
    cout << "Tree height = " << tree23.getHeight() << endl;
    cout << "Number of nodes = " << tree23.getNodeCount() << endl;
    
    TreeNode* rootNode = tree23.getRoot();
    if (rootNode != nullptr) {
        int serialNum = 1;
        for (int i = 0; i < rootNode->entries.size(); i++) {
            for (int j = 0; j < rootNode->entries[i].ids.size(); j++) {
                int targetId = rootNode->entries[i].ids[j];
                GraduateInfo targetData = info[targetId - 1]; 

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
  } else if (cmd == "3") { 
    bool has_print = false;
    cout << "\nEnter K in [1," << info.size() << "]: ";
    int k;
    std::cin >> k;
    
    if (k >= 1 && k <= info.size()) {
        tree23.printTopKMax(k, info);
        has_print = true;
    }
    if (has_print)  printf ("\n");
   
  } else if (cmd == "4") {
    // 任務四：AVL 樹字串精確搜尋
        bool has_print = false;
        cout << "Enter a department name to search: ";
        string dept = ReadInput();
        

        // 從 AVL Tree 撈出該科系對應的所有 ids
        vector<int> matched_ids = avl_tree.getIds(dept);

        if (matched_ids.empty()) {
            printf("\n");
            cout << dept << " is not found!" << endl;
            return;
        }
        printf("\n");
        cout << "Enter K in [1," << matched_ids.size() << "]: ";
        
        int k;
        std::cin >> k;

        if (k >= 1 && k <= matched_ids.size()) {
            // 用來輔助排序的自訂結構
            struct SortItem {
                int id;
                int studentCount;
            };

            vector<SortItem> items;
            for (int id : matched_ids) {
                items.push_back({id, safeStoi(info[id - 1].getStudentCount())});
            }

            // 手刻 Bubble Sort (氣泡排序) 來取代 std::sort
            int n = items.size();
            for (int i = 0; i < n - 1; i++) {
                for (int j = 0; j < n - i - 1; j++) {
                    bool swap_needed = false;
                    
                    // 條件一：學生數遞減排序
                    if (items[j].studentCount < items[j + 1].studentCount) {
                        swap_needed = true;
                    } 
                    // 條件二：若學生數相同，則依序號遞增排序
                    else if (items[j].studentCount == items[j + 1].studentCount) {
                        if (items[j].id > items[j + 1].id) {
                            swap_needed = true;
                        }
                    }

                    if (swap_needed) {
                        SortItem temp = items[j];
                        items[j] = items[j + 1];
                        items[j + 1] = temp;
                    }
                }
            }

            // 處理「同值超額輸出」的邊界條件
            int printCount = k;
            int boundaryValue = items[k - 1].studentCount;
            while (printCount < items.size() && items[printCount].studentCount == boundaryValue) {
                printCount++;
            }

            // 格式化輸出
            
            for (int i = 0; i < printCount; i++) {
                int id = items[i].id;
                int index = id - 1;
                cout << (i + 1) << ": [" << id << "] " 
                     << info[index].getSchoolName() << ", " 
                     << info[index].getDeptName() << ", " 
                     << info[index].getEducationDivision() << ", " 
                     << info[index].getLevel() << ", " 
                     << info[index].getStudentCount() << ", " 
                     << info[index].getLastYearGraduatesCount() << endl;
                has_print = true;
            }

        }
        if (has_print) printf ("\n");
    
  }
}

};

int main() {
  UniversityCatalog uc;
  bool has_cmd2 = false;
  
  while (true) {
    PrintTitle();
    string cmd = ReadInput(); 
    if (cmd == "0") { 
      return 0;
    } else if (cmd == "1"){
      cout << endl;
      if (uc.fetchFile()) {
        has_cmd2 = false;
        uc.doTask(cmd);
        printf("\n");
      }
    }  else if (cmd == "2"){ 
      if (uc.getInfoCount() == 0) {
        cout << "### Choose 1 first. ###\n";
      } else {
        has_cmd2 = true;
        uc.doTask(cmd);
      }

    } else if (cmd == "3") {
        if (uc.getInfoCount() == 0) {
          cout << "### Choose 1 first. ###\n";
        } else {
          uc.doTask(cmd); 
        }
    
    } else if (cmd == "4") {
        if (uc.getInfoCount() == 0) {
          cout << "### Choose 1 first. ###\n";
        } else if (!has_cmd2) {
          cout << "### Choose 2 first. ###\n"; // 任務四的防呆機制：若尚未執行任務二建立 AVL 樹，暫不執行只輸出提示訊息
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

void PrintTitle () {
  cout << "* Data Structures and Algorithms *\n";
  cout << "****** Balanced Search Tree ******\n";
  cout << "* 0. QUIT                        *\n";
  cout << "* 1. Build 23 tree               *\n";
  cout << "* 2. Build AVL tree              *\n";
  cout << "* 3. Top-K max search on 23 tree *\n";
  cout << "* 4. Exact search on AVL tree    *\n";
  cout << "**********************************\n";
  cout << "Input a choice(0, 1, 2, 3, 4): ";
}