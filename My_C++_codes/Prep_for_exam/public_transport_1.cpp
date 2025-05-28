#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
 
class CTransport {
public:
  CTransport() = default;
  ~CTransport() = default;

  CTransport& addLine(std::istream& is) {
    std::string current;
    std::string previous;
    bool first = true;
    while (getline(is, current)) {
      if (first) {
        previous = current;
        first = false;
      }
      else {
        s[current].insert(previous);
        s[previous].insert(current);
        previous = current;
      }
    }
    isi = false;
    return *this;
  }

  void calculate() const {
    for (auto i : s) {
      for (auto j : s) {
        s_all[i.first][j.first] = -1;
      }
    }

    for (auto i : s) {
      std::queue<std::string> togo;
      std::set<std::string> vis;
      int dis = 0;

      togo.push(i.first);
      vis.insert(i.first);
      s_all[i.first][i.first] = 0;

      while(!togo.empty()) {
        dis++;
        int togo_size = togo.size();
        for (int k = 0; k < togo_size; k++) {
          std::string current = togo.front();
          togo.pop();
          for (auto j : s[current]) {
            if (vis.find(j) != vis.end()) continue;
            togo.push(j);
            vis.insert(j);
            s_all[i.first][j] = dis;
          }
        }
      }
    }
    isi = true;
  }

  void print_routes() {
    for(auto i : s) {
      std::cout << i.first << "-> ";
      for (auto j : i.second) {
        std::cout << j << ", ";
      }
      std::cout << "\n-------------------------------------------------------------\n";
    }
  }

  void print_all_routes() {
    for(auto i : s_all) {
      std::cout << i.first << "-> ";
      for (auto j : i.second) {
        std::cout << j.first << "[" << j.second << "], ";
      }
      std::cout << "\n";
    }    
  }
 
  std::set<std::string> meetingPoints(
    const std::vector<std::string>& fromList, int& sumCost
  ) const {
    if(!isi) calculate();
    sumCost = __INT_MAX__;
    std::set<std::string> result;

    std::set<std::string> list;
    for (auto i : fromList) list.insert(i);
    if (list.size() == 1) {
      sumCost = 0;
      return {*list.begin()};
    }
    else {
      for (auto i : list) {
        if (s.find(i) == s.end()) {
          sumCost = 0;
          return {};
        }
      }
    }
    

    for (auto i : s_all) {
      bool notcon = false;
      int current_price = 0;
      for (auto j : fromList) {
        if (s_all[i.first][j] == -1) notcon = true;
        current_price += s_all[i.first][j];
      }
      if (notcon) continue;
      if (current_price < sumCost) {
        sumCost = current_price;
        result = {i.first};
      }
      else if (current_price == sumCost) {
        result.insert(i.first);
      }
    }

    return result;
  }
 
private:
  mutable std::map<std::string, std::set<std::string>> s;
  mutable std::map<std::string, std::map<std::string,int>> s_all;
  mutable bool isi;
};
 
int main() {
  CTransport t;
  std::istringstream iss;
  iss.clear();
  iss.str(
    "Newton\n"
    "Black Hill\n"
    "Wood Side\n"
    "Green Hill\n"
    "Lakeside\n"
    "Newton\n"
  );
  t.addLine(iss);
  iss.clear();
  iss.str(
    "Wood Side\n"
    "Green Hill\n"
    "Little Burnside\n"
    "Great Newton\n"
    "Wood Side\n"
  );
  t.addLine(iss);


  iss.clear();
  iss.str(
    "Little Newton\n"
    "Little Burnside\n"
    "Castle Hill\n"
    "Newton Crossroad\n"
    "Lakeside Central\n"
    "Little Newton\n"
  );
  t.addLine(iss);
  iss.clear();
  iss.str(
    "Lakeside Central\n"
    "Little Waterton\n"
  );
  t.addLine(iss);
  iss.clear();
  iss.str(
    "Little Waterton\n"
    "Waterton West\n"
    "Waterton Central\n"
    "Waterton East\n"
    "Waterton Woods\n"
    "Waterton North\n"
    "Waterton East\n"
    "Little Waterton\n"
  );
  t.addLine(iss);
  iss.clear();
  iss.str(
    "Tidmouth\n"
    "Gordon's Hill\n"
    "Suderry\n"
    "Knapford\n"
    "Great Waterton\n"
    "Brendam Docks\n"
    "Tidmouth\n"
  );
  t.addLine(iss);
  iss.clear();
  iss.str(
    "Sodor Ironworks\n"
    "Sodor Steamwork\n"
    "Knapford\n"
    "Maron\n"
    "Gordon's Hill\n"
    "Sodor Ironworks\n"
  );
  t.addLine(iss);
 
  int cost;
  assert(
    (t.meetingPoints({"Newton"}, cost) ==
     std::set<std::string> {"Newton"}) &&
    (cost == 0)
  );
 
  assert(
    (t.meetingPoints({"Newton", "Newton"}, cost) ==
     std::set<std::string> {"Newton"}) &&
    (cost == 0)
  );
 
  assert(
    (t.meetingPoints({"Brno"}, cost) == std::set<std::string> {"Brno"}
    ) &&
    (cost == 0)
  );
 
  assert(
    (t.meetingPoints({"Brno", "Brno"}, cost) ==
     std::set<std::string> {"Brno"}) &&
    (cost == 0)
  );
 
  assert(
    (t.meetingPoints({"Newton", "Brno"}, cost) ==
     std::set<std::string> {})
  );

  assert(
    (t.meetingPoints({"Newton", "Black Hill"}, cost) ==
     std::set<std::string> {"Newton", "Black Hill"}) &&
    cost == 1
  );
 
  assert(
    (t.meetingPoints({"Newton", "Newton", "Black Hill"}, cost) ==
     std::set<std::string> {"Newton"}) &&
    cost == 1
  );
 
  assert(
    (t.meetingPoints({"Newton", "Black Hill", "Wood Side"}, cost) ==
     std::set<std::string> {"Black Hill"}) &&
    cost == 2
  );
 
  assert(
    (t.meetingPoints({"Newton Crossroad", "Little Waterton"}, cost) ==
     std::set<std::string> {
       "Newton Crossroad", "Little Waterton", "Lakeside Central"
     }) &&
    cost == 2
  );
 
  assert(
    (t.meetingPoints({"Suddery", "Little Waterton"}, cost) ==
     std::set<std::string> {})
  );
 
  assert(
    (t.meetingPoints({"Tidmouth", "Newton Crossroad"}, cost) ==
     std::set<std::string> {})
  );
 
  iss.clear();
  iss.str(
    "Newton\n"
    "Newton Crossroad\n"
    "Tidmouth\n"
    "Newton\n"
  );
  t.addLine(iss);
 
  assert(
    (t.meetingPoints({"Tidmouth", "Newton Crossroad"}, cost) ==
     std::set<std::string> {"Tidmouth", "Newton Crossroad"}) &&
    cost == 1
  );
}

