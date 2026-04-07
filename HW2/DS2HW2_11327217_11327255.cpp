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

    // 尋找樹中的最小值節點 (用於刪除時尋找右子樹的 Inorder Successor)
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    // 內部遞迴 Delete
    AVLNode* deleteNode(AVLNode* node, std::string dept) {
        // 1. 標準 BST 刪除
        if (node == nullptr) return node;

        if (dept < node->deptName) {
            node->left = deleteNode(node->left, dept);
        } else if (dept > node->deptName) {
            node->right = deleteNode(node->right, dept);
        } else {
            // 找到要刪除的節點
            if (node->left == nullptr) {
                AVLNode* temp = node->right;
                delete node;
                node = temp;
            } else if (node->right == nullptr) {
                AVLNode* temp = node->left;
                delete node;
                node = temp;
            } else {
                // 有兩個子節點：尋找右子樹的最小值節點做為替代
                AVLNode* temp = minValueNode(node->right);

                // 複製替代節點的資料過來
                node->deptName = temp->deptName;
                node->ids = temp->ids;

                // 刪除該替代節點
                node->right = deleteNode(node->right, temp->deptName);
            }
        }

        // 如果刪除後樹為空，直接回傳
        if (node == nullptr) return node;

        // 2. 沿著路徑往上更新高度
        updateHeight(node);

        // 3. 檢查平衡因子 (Balance Factor)
        int balance = getBalance(node);

        // 4. 如果不平衡，進行旋轉修復
        // Left Left Case
        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateLL(node);

        // Left Right Case
        if (balance > 1 && getBalance(node->left) < 0)
            return rotateLR(node);

        // Right Right Case
        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateRR(node);

        // Right Left Case
        if (balance < -1 && getBalance(node->right) > 0)
            return rotateRL(node);

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

    void insert(string dept, int id) {
        root = insertNode(root, dept, id);
    }

    // 新增的對外開放刪除函式
    void remove(string dept) {
        root = deleteNode(root, dept);
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

class RedBlackTree {
private:
    struct RBNode {
        string key;          // 科系名稱
        vector<int> ids;     // 儲存相同科系的所有序號
        int color;                // 0: Black, 1: Red
        RBNode* parent;
        RBNode* left;
        RBNode* right;
    };

    RBNode* root;
    RBNode* TNULL; // 哨兵節點 (Sentinel node)，用於簡化邊界條件處理

    // 初始化哨兵節點
    void initTNULL() {
        TNULL = new RBNode;
        TNULL->color = 0; // TNULL 永遠是黑色
        TNULL->left = nullptr;
        TNULL->right = nullptr;
        TNULL->parent = nullptr;
    }

    void leftRotate(RBNode* x) {
        RBNode* y = x->right;
        x->right = y->left;
        if (y->left != TNULL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBNode* x) {
        RBNode* y = x->left;
        x->left = y->right;
        if (y->right != TNULL) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    void insertFixup(RBNode* k) {
        RBNode* u;
        while (k->parent != nullptr && k->parent->color == 1) {
            if (k->parent == k->parent->parent->left) {
                u = k->parent->parent->right;
                if (u->color == 1) {
                    u->color = 0;
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    rightRotate(k->parent->parent);
                }
            } else {
                u = k->parent->parent->left;
                if (u->color == 1) {
                    u->color = 0;
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    leftRotate(k->parent->parent);
                }
            }
            if (k == root) {
                break;
            }
        }
        root->color = 0;
    }

    void deleteFixup(RBNode* x) {
        RBNode* s;
        while (x != root && x->color == 0) {
            if (x == x->parent->left) {
                s = x->parent->right;
                if (s->color == 1) {
                    s->color = 0;
                    x->parent->color = 1;
                    leftRotate(x->parent);
                    s = x->parent->right;
                }
                if (s->left->color == 0 && s->right->color == 0) {
                    s->color = 1;
                    x = x->parent;
                } else {
                    if (s->right->color == 0) {
                        s->left->color = 0;
                        s->color = 1;
                        rightRotate(s);
                        s = x->parent->right;
                    }
                    s->color = x->parent->color;
                    x->parent->color = 0;
                    s->right->color = 0;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                s = x->parent->left;
                if (s->color == 1) {
                    s->color = 0;
                    x->parent->color = 1;
                    rightRotate(x->parent);
                    s = x->parent->left;
                }
                if (s->right->color == 0 && s->left->color == 0) {
                    s->color = 1;
                    x = x->parent;
                } else {
                    if (s->left->color == 0) {
                        s->right->color = 0;
                        s->color = 1;
                        leftRotate(s);
                        s = x->parent->left;
                    }
                    s->color = x->parent->color;
                    x->parent->color = 0;
                    s->left->color = 0;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = 0;
    }

    void rbTransplant(RBNode* u, RBNode* v) {
        if (u->parent == nullptr) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    RBNode* minimum(RBNode* node) {
        while (node->left != TNULL) {
            node = node->left;
        }
        return node;
    }

    void clearTree(RBNode* node) {
        if (node != TNULL) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

    int countNodes(RBNode* node) {
        if (node == TNULL) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    int getHeight(RBNode* node) {
        if (node == TNULL) return 0;
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }

public:
    RedBlackTree() {
        initTNULL();
        root = TNULL;
    }

    ~RedBlackTree() {
        clearTree(root);
        delete TNULL;
    }

    // 1. Insert: 插入科系名稱與對應序號
    void insert(string key, int id) {
        RBNode* node = new RBNode;
        node->parent = nullptr;
        node->key = key;
        node->ids.push_back(id); // 建立節點時存入第一筆序號
        node->left = TNULL;
        node->right = TNULL;
        node->color = 1; // 新節點預設為紅色

        RBNode* y = nullptr;
        RBNode* x = this->root;

        while (x != TNULL) {
            y = x;
            if (node->key < x->key) {
                x = x->left;
            } else if (node->key > x->key) {
                x = x->right;
            } else {
                // 如果科系名稱相同，不新增節點，直接把序號加入原節點的 vector 中
                x->ids.push_back(id);
                delete node; // 刪除剛才多配置的節點
                return;
            }
        }

        node->parent = y;
        if (y == nullptr) {
            root = node;
        } else if (node->key < y->key) {
            y->left = node;
        } else {
            y->right = node;
        }

        if (node->parent == nullptr) {
            node->color = 0;
            return;
        }

        if (node->parent->parent == nullptr) {
            return;
        }

        insertFixup(node);
    }

    // 2. Delete: 刪除指定科系名稱的節點
    void remove(string key) {
        RBNode* node = root;
        RBNode* z = TNULL;
        
        while (node != TNULL) {
            if (node->key == key) {
                z = node;
                break;
            }
            if (key < node->key) {
                node = node->left;
            } else {
                node = node->right;
            }
        }

        if (z == TNULL) {
            return; // 找不到該 Key
        }

        RBNode* y = z;
        RBNode* x;
        int y_original_color = y->color;
        
        if (z->left == TNULL) {
            x = z->right;
            rbTransplant(z, z->right);
        } else if (z->right == TNULL) {
            x = z->left;
            rbTransplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                rbTransplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            rbTransplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        
        if (y_original_color == 0) {
            deleteFixup(x);
        }
    }

    // 清空整棵樹
    void clear() {
        clearTree(root);
        root = TNULL;
    }

    // 取得樹的節點數
    int getTotalNodes() {
        return countNodes(root);
    }

    // 取得樹的高度
    int getTreeHeight() {
        return getHeight(root);
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
        // 尋找適合插入的位置
        while (i < node->entries.size() && entry.studentCount > node->entries[i].studentCount) {
            i++;
        }
        // 將資料插入到正確的 index
        node->entries.insert(node->entries.begin() + i, entry);

        // 如果有伴隨分裂產生的右子節點，也要插入到對應的 children 陣列中
        if (rightChild != nullptr) {
            node->children.insert(node->children.begin() + i + 1, rightChild);
        }
    }

    // 處理 2-3 Tree 的插入與分裂邏輯
    SplitResult insertRecursive(TreeNode* node, int studentCount, int id) {
        SplitResult result;

        // 1. 檢查目前節點內是否已經存在相同的 studentCount
        for (int i = 0; i < node->entries.size(); i++) {
            if (node->entries[i].studentCount == studentCount) {
                node->entries[i].ids.push_back(id);
                return result;
            }
        }

        // 2. 判斷目前是否走到葉節點 (Leaf)
        if (node->isLeaf) {
            // 到達葉節點：直接將新資料插入
            insertIntoNode(node, DataEntry(studentCount, id));
        } else {
            // 是內部節點 (Internal Node)：決定要往哪一個子樹繼續走
            int childIdx = 0;
            while (childIdx < node->entries.size() && studentCount > node->entries[childIdx].studentCount) {
                childIdx++;
            }

            // 往下遞迴呼叫子節點進行插入
            SplitResult childResult = insertRecursive(node->children[childIdx], studentCount, id);

            // 檢查子節點插入後是否有發生分裂(有元素被推擠上來)
            if (childResult.promotedEntry != nullptr) {
                // 將下方推擠上來的元素與新產生的右子樹，合併回目前的節點中
                insertIntoNode(node, *childResult.promotedEntry, childResult.rightNode);
                delete childResult.promotedEntry;
            }
        }

        // 3. 檢查目前節點是否發生 Overflow
        if (node->entries.size() == 3) {
            // 建立一個新的右兄弟節點來接收分裂出去的資料
            TreeNode* rightSibling = new TreeNode();
            rightSibling->isLeaf = node->isLeaf;
            nodeCount++; 

            // 將原本節點最右邊 (第3個) 的元素搬給新節點
            rightSibling->entries.push_back(node->entries[2]);

            // 如果目前節點不是葉節點，連同對應的子樹指標也要搬過去
            if (!node->isLeaf) {
                rightSibling->children.push_back(node->children[2]);
                rightSibling->children.push_back(node->children[3]);
                node->children.resize(2); // 原本的節點只保留前 2 個子樹
            }

            // 將中間的元素Promote)給上一層處理
            result.promotedEntry = new DataEntry(node->entries[1]);
            result.rightNode = rightSibling; // 綁定剛剛分裂出來的右節點

            // 只保留最左邊的元素
            node->entries.pop_back(); 
            node->entries.pop_back(); 
        }

        // 回傳處理結果給上一層遞迴
        return result;
    }

    // --- 以下為新增的 Delete 相關邏輯 ---

    // 取得子樹中的最小值節點 (用於尋找 In-order Successor)
    TreeNode* getMinNode(TreeNode* node) {
        while (!node->isLeaf) {
            node = node->children[0];
        }
        return node;
    }

    // 處理節點 Underflow (項目數變成 0)
    void fixUnderflow(TreeNode* parent, int childIndex) {
        TreeNode* child = parent->children[childIndex];

        // 1. 若左兄弟有多餘的項目 (3-node)，向左兄弟借 (Redistribute)
        if (childIndex > 0 && parent->children[childIndex - 1]->entries.size() > 1) {
            TreeNode* leftSib = parent->children[childIndex - 1];
            child->entries.insert(child->entries.begin(), parent->entries[childIndex - 1]);
            parent->entries[childIndex - 1] = leftSib->entries.back();
            leftSib->entries.pop_back();

            if (!leftSib->isLeaf) {
                child->children.insert(child->children.begin(), leftSib->children.back());
                leftSib->children.pop_back();
            }
        }
        // 2. 若右兄弟有多餘的項目 (3-node)，向右兄弟借 (Redistribute)
        else if (childIndex < parent->children.size() - 1 && parent->children[childIndex + 1]->entries.size() > 1) {
            TreeNode* rightSib = parent->children[childIndex + 1];
            child->entries.push_back(parent->entries[childIndex]);
            parent->entries[childIndex] = rightSib->entries.front();
            rightSib->entries.erase(rightSib->entries.begin());

            if (!rightSib->isLeaf) {
                child->children.push_back(rightSib->children.front());
                rightSib->children.erase(rightSib->children.begin());
            }
        }
        // 3. 只能與左兄弟合併 (Merge)
        else if (childIndex > 0) {
            TreeNode* leftSib = parent->children[childIndex - 1];
            leftSib->entries.push_back(parent->entries[childIndex - 1]); // 父節點項目降下

            for (auto& e : child->entries) leftSib->entries.push_back(e);
            for (auto& c : child->children) leftSib->children.push_back(c);

            parent->entries.erase(parent->entries.begin() + (childIndex - 1));
            parent->children.erase(parent->children.begin() + childIndex);

            delete child;
            nodeCount--;
        }
        // 4. 只能與右兄弟合併 (Merge)
        else {
            TreeNode* rightSib = parent->children[childIndex + 1];
            child->entries.push_back(parent->entries[childIndex]); // 父節點項目降下

            for (auto& e : rightSib->entries) child->entries.push_back(e);
            for (auto& c : rightSib->children) child->children.push_back(c);

            parent->entries.erase(parent->entries.begin() + childIndex);
            parent->children.erase(parent->children.begin() + (childIndex + 1));

            delete rightSib;
            nodeCount--;
        }
    }

    // 內部遞迴刪除
    bool removeRecursive(TreeNode* node, int studentCount) {
        int i = 0;
        while (i < node->entries.size() && studentCount > node->entries[i].studentCount) {
            i++;
        }

        // 找到目標 studentCount
        if (i < node->entries.size() && node->entries[i].studentCount == studentCount) {
            if (node->isLeaf) {
                // Leaf 節點直接刪除項目
                node->entries.erase(node->entries.begin() + i);
                return true;
            } else {
                // Internal 節點: 與 In-order Successor 交換 (投影片 d)
                TreeNode* minNode = getMinNode(node->children[i + 1]);
                DataEntry successorData = minNode->entries[0];
                node->entries[i] = successorData; 

                // 去 Successor 所在的葉節點把它刪掉
                removeRecursive(node->children[i + 1], successorData.studentCount);

                // 檢查刪除後右子樹是否 Underflow
                if (node->children[i + 1]->entries.empty()) {
                    fixUnderflow(node, i + 1);
                }
                return true;
            }
        }
        // 沒找到，繼續往下遍歷
        else {
            if (node->isLeaf) return false; 

            bool deleted = removeRecursive(node->children[i], studentCount);
            
            // 遞迴回來後檢查剛剛走過的子節點是否 Underflow (投影片 recursion)
            if (deleted && node->children[i]->entries.empty()) {
                fixUnderflow(node, i);
            }
            return deleted;
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

    // 對外開放的 Delete 方法，依據 studentCount 刪除整個節點資料
    void remove(int studentCount) {
        if (root == nullptr) return;

        removeRecursive(root, studentCount);

        // 特殊情況處理：Delete the root (投影片 e)
        // 當根節點的唯一項目被移走，導致根節點變空時
        if (root->entries.empty()) {
            if (root->isLeaf) {
                delete root;
                root = nullptr;
                nodeCount--;
            } else {
                TreeNode* oldRoot = root;
                root = root->children[0];
                delete oldRoot;
                nodeCount--; // 兩個節點 merge 成一個，整體節點數-1
            }
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

// ======================= 新增 2-3-4 Tree 實作 =======================

// 2-3-4 樹節點 (可容納 1~3 個 Entry，4 個時觸發分裂)
struct T234_TreeNode {  
    vector<DataEntry> entries; 
    vector<T234_TreeNode*> children; 
    bool isLeaf;
    T234_TreeNode() {
        isLeaf = true;
    }
};
    
// 分裂結果 (2-3-4 Tree 專用)
struct T234_SplitResult {
    DataEntry* promotedEntry = nullptr; 
    T234_TreeNode* rightNode = nullptr;      
};

class Two_Three_Four_Tree {
 private:
    T234_TreeNode* root;
    int nodeCount;

    void insertIntoNode(T234_TreeNode* node, const DataEntry& entry, T234_TreeNode* rightChild = nullptr) {
        int i = 0;
        // 尋找適合插入的位置
        while (i < node->entries.size() && entry.studentCount > node->entries[i].studentCount) {
            i++;
        }
        // 將資料插入到正確的 index
        node->entries.insert(node->entries.begin() + i, entry);

        // 如果有伴隨分裂產生的右子節點，也要插入到對應的 children 陣列中
        if (rightChild != nullptr) {
            node->children.insert(node->children.begin() + i + 1, rightChild);
        }
    }

    // 處理 2-3-4 Tree 的插入與分裂邏輯
    T234_SplitResult insertRecursive(T234_TreeNode* node, int studentCount, int id) {
        T234_SplitResult result;

        // 1. 檢查目前節點內是否已經存在相同的 studentCount
        for (int i = 0; i < node->entries.size(); i++) {
            if (node->entries[i].studentCount == studentCount) {
                node->entries[i].ids.push_back(id);
                return result;
            }
        }

        // 2. 判斷目前是否走到葉節點 (Leaf)
        if (node->isLeaf) {
            insertIntoNode(node, DataEntry(studentCount, id));
        } else {
            int childIdx = 0;
            while (childIdx < node->entries.size() && studentCount > node->entries[childIdx].studentCount) {
                childIdx++;
            }

            // 往下遞迴
            T234_SplitResult childResult = insertRecursive(node->children[childIdx], studentCount, id);

            if (childResult.promotedEntry != nullptr) {
                insertIntoNode(node, *childResult.promotedEntry, childResult.rightNode);
                delete childResult.promotedEntry;
            }
        }

        // 3. 檢查 Overflow：2-3-4 樹最大容量為 3，等於 4 時需要分裂
        if (node->entries.size() == 4) {
            T234_TreeNode* rightSibling = new T234_TreeNode();
            rightSibling->isLeaf = node->isLeaf;
            nodeCount++; 

            // 將右半部 (第 3, 4 個) 元素搬給新節點 (index 2, 3)
            rightSibling->entries.push_back(node->entries[2]);
            rightSibling->entries.push_back(node->entries[3]);

            if (!node->isLeaf) {
                rightSibling->children.push_back(node->children[2]);
                rightSibling->children.push_back(node->children[3]);
                rightSibling->children.push_back(node->children[4]);
                node->children.resize(2); // 原本的節點保留前 2 個子樹
            }

            // 將中間偏左的元素 (index 1) Promote 給上一層
            result.promotedEntry = new DataEntry(node->entries[1]);
            result.rightNode = rightSibling; 

            // 移除被分裂與被推擠出去的元素，左節點只保留 index 0
            node->entries.pop_back(); 
            node->entries.pop_back(); 
            node->entries.pop_back(); 
        }

        return result;
    }

    // 取得子樹中的最小值節點
    T234_TreeNode* getMinNode(T234_TreeNode* node) {
        while (!node->isLeaf) {
            node = node->children[0];
        }
        return node;
    }

    // 處理節點 Underflow (在 B-Tree 機制中，2-3 與 2-3-4 樹的 Underflow 同樣發生在元素數量降至 0)
    void fixUnderflow(T234_TreeNode* parent, int childIndex) {
        T234_TreeNode* child = parent->children[childIndex];

        // 1. 若左兄弟有多餘的項目 (>1 即至少為 2 階)，向左兄弟借
        if (childIndex > 0 && parent->children[childIndex - 1]->entries.size() > 1) {
            T234_TreeNode* leftSib = parent->children[childIndex - 1];
            child->entries.insert(child->entries.begin(), parent->entries[childIndex - 1]);
            parent->entries[childIndex - 1] = leftSib->entries.back();
            leftSib->entries.pop_back();

            if (!leftSib->isLeaf) {
                child->children.insert(child->children.begin(), leftSib->children.back());
                leftSib->children.pop_back();
            }
        }
        // 2. 若右兄弟有多餘的項目 (>1)，向右兄弟借
        else if (childIndex < parent->children.size() - 1 && parent->children[childIndex + 1]->entries.size() > 1) {
            T234_TreeNode* rightSib = parent->children[childIndex + 1];
            child->entries.push_back(parent->entries[childIndex]);
            parent->entries[childIndex] = rightSib->entries.front();
            rightSib->entries.erase(rightSib->entries.begin());

            if (!rightSib->isLeaf) {
                child->children.push_back(rightSib->children.front());
                rightSib->children.erase(rightSib->children.begin());
            }
        }
        // 3. 只能與左兄弟合併
        else if (childIndex > 0) {
            T234_TreeNode* leftSib = parent->children[childIndex - 1];
            leftSib->entries.push_back(parent->entries[childIndex - 1]); 

            for (auto& e : child->entries) leftSib->entries.push_back(e);
            for (auto& c : child->children) leftSib->children.push_back(c);

            parent->entries.erase(parent->entries.begin() + (childIndex - 1));
            parent->children.erase(parent->children.begin() + childIndex);

            delete child;
            nodeCount--;
        }
        // 4. 只能與右兄弟合併
        else {
            T234_TreeNode* rightSib = parent->children[childIndex + 1];
            child->entries.push_back(parent->entries[childIndex]); 

            for (auto& e : rightSib->entries) child->entries.push_back(e);
            for (auto& c : rightSib->children) child->children.push_back(c);

            parent->entries.erase(parent->entries.begin() + childIndex);
            parent->children.erase(parent->children.begin() + (childIndex + 1));

            delete rightSib;
            nodeCount--;
        }
    }

    bool removeRecursive(T234_TreeNode* node, int studentCount) {
        int i = 0;
        while (i < node->entries.size() && studentCount > node->entries[i].studentCount) {
            i++;
        }

        if (i < node->entries.size() && node->entries[i].studentCount == studentCount) {
            if (node->isLeaf) {
                node->entries.erase(node->entries.begin() + i);
                return true;
            } else {
                T234_TreeNode* minNode = getMinNode(node->children[i + 1]);
                DataEntry successorData = minNode->entries[0];
                node->entries[i] = successorData; 

                removeRecursive(node->children[i + 1], successorData.studentCount);

                if (node->children[i + 1]->entries.empty()) {
                    fixUnderflow(node, i + 1);
                }
                return true;
            }
        }
        else {
            if (node->isLeaf) return false; 

            bool deleted = removeRecursive(node->children[i], studentCount);
            
            if (deleted && node->children[i]->entries.empty()) {
                fixUnderflow(node, i);
            }
            return deleted;
        }
    }

public:
    Two_Three_Four_Tree() {
        root = nullptr;
        nodeCount = 0;
    }

    void clear() {
        root = nullptr;
        nodeCount = 0;
    }

    void insert(int studentCount, int id) {
        if (root == nullptr) {
            root = new T234_TreeNode();
            root->entries.push_back(DataEntry(studentCount, id));
            nodeCount++;
            return;
        }

        T234_SplitResult res = insertRecursive(root, studentCount, id);
        
        if (res.promotedEntry != nullptr) {
            T234_TreeNode* newRoot = new T234_TreeNode();
            newRoot->isLeaf = false;
            newRoot->entries.push_back(*res.promotedEntry);
            newRoot->children.push_back(root);
            newRoot->children.push_back(res.rightNode);
            
            root = newRoot;
            nodeCount++;
            delete res.promotedEntry;
        }
    }

    void remove(int studentCount) {
        if (root == nullptr) return;
        removeRecursive(root, studentCount);

        if (root->entries.empty()) {
            if (root->isLeaf) {
                delete root;
                root = nullptr;
                nodeCount--;
            } else {
                T234_TreeNode* oldRoot = root;
                root = root->children[0];
                delete oldRoot;
                nodeCount--; 
            }
        }
    }

    int getHeight() {
        if (root == nullptr) return 0;
        int height = 1;
        T234_TreeNode* current = root;
        while (!current->isLeaf) {
            height++;
            current = current->children[0];
        }
        return height;
    }

    int getNodeCount() { return nodeCount; }
    T234_TreeNode* getRoot() { return root; }
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
  bool has_cmd2 = false;
  
  while (true) {
    PrintTitle();
    string cmd = ReadInput(); // demo防呆
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
          uc.doTask(cmd); // to do:
        }
    
    } else if (cmd == "4") {
        if (uc.getInfoCount() == 0) {
          cout << "### Choose 1 first. ###\n";
        } else if (!has_cmd2) {
          cout << "### Choose 2 first. ###\n";
        } else {
          uc.doTask(cmd); // todo:
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