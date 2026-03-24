// 11327217 蔡易勳   11327255許頌恩

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

class AVL_Tree {
  // 任務二待實作
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


class UniversityCatalog {
 private:
  vector<GraduateInfo> info;
  Two_Three_Tree tree23;
  
 public:
  void reSet() {
    info.clear();
    tree23.clear();
  }

  bool fetchFile(int cmd) {
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
     
        if (cmd == 1) {
            tree23.insert(sCount, count_id); 
        } 
        else if (cmd == 2) {
          // AVL tree insert (任務二)
        } 
        
        count_id++;
    }
    in.close();
    return true;
}


void doTask(int cmd) { 
  if (cmd == 1) {
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
  } else if (cmd == 2) {
    // 任務二
  } 
}

};

int main() {
  UniversityCatalog uc;
  
  while (true) {
    PrintTitle();
    int cmd;
    cin >> cmd;
    if (cin.fail()) { 
      return 0;
    } else if (cmd == 0 ){
      return 0;
    } else if (cmd != 1 && cmd != 2){  
      cout << "\nCommand does not exist!\n";
    } else {
      cout << endl;
      if (uc.fetchFile(cmd)) {
        uc.doTask(cmd);
      }
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