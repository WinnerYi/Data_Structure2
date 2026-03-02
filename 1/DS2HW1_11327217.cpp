//11327217 蔡易勳     
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

struct Node {
  int hp;                    // 生命值（BST key）
  std::vector<int> ids;      // 相同 hp 的所有編號
  Node* left;
  Node* right;

  Node(int hp, int id) {
    this->hp = hp;
    left = nullptr;
    right = nullptr;
    ids.push_back(id); // 預設編號
  } 
};



class GraduateInfo{ 
 private:
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

class Pokemon {
 private:
  BinarySearchTreeIterative bst;
  std::vector<Raichu> raichus;
  int max_hp;
  
 public:
  Pokemon() {
    max_hp = 0;
  }
  void reSet() {
    bst.clear();
    raichus.clear();
    raichus.shrink_to_fit();  // 釋放多餘容量
    max_hp = 0;

  }
  bool fetchFile() {
    std::ifstream in;
    while (1) {
      std:: cout << "Input a file number [0: quit]: ";
      std::string file_num = ReadInput();
      if (file_num == "0") return false;
      std::string txt_path = "input" + file_num + ".txt";
      in.open(txt_path);
      if(in.fail()){ 
        std::cout << std::endl << "### " << txt_path + " does not exist! ###" << std::endl;
        printf("\n");
        continue;
      }
      break;
    }
    std::string title;
    std::getline(in, title);

    std::string line;
    while (std::getline(in, line)) {
      std::stringstream ss(line);
      int id, total, hp, attack, defense, sp_atk, sp_def, speed, generation;
      std::string name, type1, type2, legendary;

      std::string id_str, total_str, hp_str, attack_str,
                  defense_str, sp_atk_str, sp_def_str,
                  speed_str, generation_str;
      std::getline(ss, id_str, '\t');
      std::getline(ss, name, '\t');
      std::getline(ss, type1, '\t');
      std::getline(ss, type2, '\t');
      std::getline(ss, total_str, '\t');
      std::getline(ss, hp_str, '\t');
      std::getline(ss, attack_str, '\t');
      std::getline(ss, defense_str, '\t');
      std::getline(ss, sp_atk_str, '\t');
      std::getline(ss, sp_def_str, '\t');
      std::getline(ss, speed_str, '\t');
      std::getline(ss, generation_str, '\t');
      std::getline(ss, legendary); // 最後一欄到行尾
      id        = std::stoi(id_str);
      total      = std::stoi(total_str);
      hp         = std::stoi(hp_str);
      attack     = std::stoi(attack_str);
      defense    = std::stoi(defense_str);
      sp_atk     = std::stoi(sp_atk_str);
      sp_def     = std::stoi(sp_def_str);
      speed      = std::stoi(speed_str);
      generation = std::stoi(generation_str);
      if (hp > max_hp) {
        max_hp = hp;
      }
      Raichu r;
      r.setRaichu(id, name, type1, type2,
                  total, hp, attack, defense,
                  sp_atk, sp_def, speed,
                  generation, legendary);
      bst.insert(hp, id);
      raichus.push_back(r);
  }   
    in.close();
    return true;
}

int getRaichuSize() {
  return raichus.size();
}
int getTreeHeight() {
  return bst.height();
}

void taskOne() {
  std::cout << '\t' << "#" << '\t' << std::setw(19) << std::left << "Name" << '\t'<< std::setw(10) << std::left << "Type 1"  << '\t' << "HP";
  
  std::cout << std::endl;
  for (int i = 0; i < raichus.size(); i++) {
    std::cout << "[" << std::right << std::setw(3) << i + 1 << "]";
    std::cout << '\t' << raichus[i].getId();
    std::cout << '\t' << std::left<< std::setw(20) << raichus[i].getName() << "\t";
    std::cout << std::left<< std::setw(10) << raichus[i].getType1() << "\t";
    std::cout << std::left<< std::setw(6) << raichus[i].getHp();
    std::cout << "\n";
  }
  std::cout << "HP tree height = " << bst.height() << std::endl;
}

void taskTwo() { // done
  int low, high;
  while (1) {
    std::cout << "\nInput a non-negative integer: ";
    std::cin >> low;
    if (std::cin.fail()) { // 檢查輸入是否失敗
        std::cin.clear();              
        std::cin.ignore(10000, '\n'); 
        std::cout << "\n### It is NOT a non-negative integer. ###\nTry again: "; 
        continue;;
    }
    if (low > (max_hp * 2)) {
      std::cout << "\n### It is NOT in [0," << max_hp * 2 << "]. ###\nTry again: ";
      continue;
    }
    if (low >= 0) break;
    if (low < 0) {
      std::cout << "\n### It is NOT a non-negative integer. ###\nTry again: ";
    }
  }
  while (1) {
    std::cout << "\nInput a non-negative integer: ";
    std::cin >> high;
    if (std::cin.fail()) { // 檢查輸入是否失敗
        std::cin.clear();              
        std::cin.ignore(10000, '\n'); 
        std::cout << "\n### It is NOT a non-negative integer. ###\nTry again: "; 
        continue;;
    }
    if (high > (max_hp * 2)) {
      std::cout << "\n### It is NOT in [0," << max_hp * 2 << "]. ###\nTry again: ";
      continue;
    }
    if (high >= 0) break;
    if (high < 0) {
      std::cout << "\n### It is NOT a non-negative integer. ###\nTry again: ";
    }
  }
  if (low > high) {
    int temp = low;
    low = high;
    high = temp;
  }

  std::vector<Node*> result;
  int visitedCount = 0;
  bst.rangeSearchIterative(low, high, result, visitedCount);
  
  
  if (result.empty()) {
    std::cout << "No record was found in the specified range." << std::endl;
  } else {
    for (int i = 0; i < result.size() - 1; i++) {
      for (int j = 0; j < result.size() - 1 - i; j++) {
        if (result[j]->hp < result[j+1]->hp) {
            // 交換
            Node* temp = result[j];
            result[j] = result[j+1];
            result[j+1] = temp;
        }
     }
   }
    std::cout << "\t#\t" << std::setw(19) << std::left << "Name" << "\t" << std::setw(10) << std::left
              <<  "Type 1" << "\t" << "Total" << "\t" << "HP" << "\t" << "Attack" << "\t" << "Defense"; 
    std::cout << std::endl;

    int idx = 1;
    for (int i = 0; i < result.size(); i++) {  
      Node* node = result[i];
      for (int j = 0; j < node->ids.size(); j++) { // 走這個節點的每個 id
        int id = node->ids[j];
        for (int k = 0; k < raichus.size(); k++) { // 從頭找 raichus
            if (raichus[k].getId() == id) {
                std::cout << "[" << std::right << std::setw(3) << idx << "]";
                idx++;
                std::cout << '\t' <<raichus[k].getId();
                std::cout << '\t' << std::setw(20) << std::left << raichus[k].getName() << '\t'; 
                std::cout << std::setw(10) << raichus[k].getType1() << '\t';
                std::cout << std::setw(6) << std::left << raichus[k].getTotal() << '\t';
                std::cout << raichus[k].getHp() << '\t';
                std::cout << raichus[k].getAttack() << '\t';
                std::cout << raichus[k].getDefense() << '\n';
                break; 
            }
        }
      }

    }
  }
  std::cout << "Number of visited nodes = " << visitedCount << std::endl;
}

void taskThree(bool deleteMin) {
  Node* deleted = bst.deleteExtreme(deleteMin);
  std::cout << "\t#\t" << std::setw(19) << std::left << "Name" << "\t" << std::setw(10) << std::left
              <<  "Type 1" << "\t" << "Total" << "\t" << "HP" << "\t" << "Attack" << "\t" << "Defense" << '\t' << "Sp. Atk" << '\t' << "Sp. Def"; 
   std::cout << std::endl;
  int idx = 1;
  for (int i = 0; i < deleted->ids.size(); i++) {
      int id = deleted->ids[i];
      for (int j = 0; j < raichus.size(); j++) {
        if (raichus[j].getId() == id) {
          std::cout << "[" << std::right << std::setw(3) << idx << "]";
          std::cout << '\t' << raichus[j].getId();
          std::cout << '\t' << std::setw(20) << std::left << raichus[j].getName() << '\t';
          std::cout <<  std::setw(10) << raichus[j].getType1() << '\t';
          std::cout <<  std::setw(6)<< std::left << raichus[j].getTotal() << '\t';
          std::cout << raichus[j].getHp() << '\t';
          std::cout << raichus[j].getAttack() << '\t';
          std::cout << raichus[j].getDefense() << '\t';
          std::cout << std::setw(6) << std::left << raichus[j].getSpAttack() << "\t";
          std::cout << raichus[j].getSpDefense() << std::endl;
          idx++;
          break;
        }
      }
    }
  delete deleted;
  std::cout << "HP tree height = " << bst.height() << std::endl;
}

void taskFour() {
  bst.rebuildMinHeight();
  bst.printHPtree();
}
};
int main() {
 
  Pokemon pokemon;
   bool deleteMin = true;
  while (true) {
    PrintTitle();
    int cmd;
    std::cin >> cmd;
    if (std::cin.fail()) { // 檢查輸入是否失敗
      std::cin.clear();              
      std::cin.ignore(10000, '\n');  
      std::cout << "\nCommand does not exist!\n\n";
      continue;
    } else if (cmd == 0 ){
      return 0;
    } else if (cmd == 1) {
      pokemon.reSet();
      printf("\n");
      if (pokemon.fetchFile()) {
        pokemon.taskOne();
      }
      deleteMin = true;
    } else if (cmd == 2) {
      if (pokemon.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      pokemon.taskTwo();
      
    } else if (cmd == 3) {
      if (pokemon.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      printf("\n");
      pokemon.taskThree(deleteMin);
      deleteMin = !deleteMin;
     
    } else if (cmd == 4) {
      if (pokemon.getTreeHeight() == 0) {
        std::cout << "\n----- Execute Mission 1 first! -----\n\n";
        continue;
      }
      deleteMin = true;
      printf("\n");
      pokemon.taskFour();
    
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
  std::cout << "*** (^_^) Data Structure (^o^) ***\n";
  std::cout << "** Binary Search Tree on Pokemon *\n";
  std::cout << "* 0. QUIT                        *\n";
  std::cout << "* 1. Read a file to build HP BST *\n";
  std::cout << "* 2. Range search on HP field    *\n";
  std::cout << "* 3. Delete the min on HP field  *\n";    
  std::cout << "* 4. Rebuild the balanced HP BST *\n";
  std::cout << "**********************************\n";
  std::cout << "Input a choice(0, 1, 2, 3, 4): ";
} 
