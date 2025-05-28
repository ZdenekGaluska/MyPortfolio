#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <cassert>
 
using namespace std;
 
class CTransport
{
public:
    CTransport() = default;
    ~CTransport() = default;
    void AddLine(istream &is);
    map<string, int> FindDest(const string &from, int costMax) const;
    void print_data();

    void print_output(std::map<std::string, int> res) {
        for (auto i : res) {
            std::cout << i.first << " -> " << i.second << "\n";
        }
    }
 
private:
    std::map<std::string, std::set<std::string>> s;
};
void CTransport::AddLine(istream &is)
{
    bool firstwhile = true;
    std::string previous;
    std::string first;
    std::string current;
    while(getline(is,current)) {
        if (firstwhile) {
            first = current;
            firstwhile = false;
        }
        else {
        // s[current].insert(previous);
        s[previous].insert(current);
        }
        previous = current;
    }
    s[previous].insert(first);
    // s[first].insert(previous);
}

void CTransport::print_data() {
    for (auto i : s) {
        std::cout << i.first << " -> ";
        for (auto j : s[i.first]) {
            std::cout << j<< ", ";
        }
        std::cout << "\n";
    }
}
map<string, int> CTransport::FindDest(const string &from, int costMax) const
{
    std::map<std::string, int> result;
    std::queue<std::string> togo;
    std::set<std::string> visisted;
    int current_cost = 0;

    togo.push(from);
    visisted.insert(from);
    result[from] = 0;

    while (!togo.empty() && current_cost < costMax) {
        int togo_size = togo.size();
        current_cost++;
        for (int i = 0; i < togo_size; i++) {
            std::string current = togo.front();
            togo.pop();

            auto it = s.find(current);
            if(it == s.end()) continue;

            for (auto i : it->second) {
                if (visisted.find(i) != visisted.end()) continue;
                togo.push(i);
                visisted.insert(i);
                result[i] = current_cost;
            }
        }

    }
    return result;
}
int main()
{
    istringstream iss;
    CTransport t;
    iss.clear();
    iss.str("Newton\nBlack Hill\nWood Side\nGreen Hill\nLakeside\n");
    t.AddLine(iss);
    iss.clear();
    iss.str("Great Newton\nLittle Burnside\nGreen Hill\nWood Side\n");
    t.AddLine(iss);
    // t.print_data();
    iss.clear();
    iss.str("Little Newton\nLittle Burnside\nCastle Hill\nNewton Crossroad\nLakeside Central\n");
    t.AddLine(iss);
    iss.clear();
    iss.str("Tidmouth\nGordon's Hill\nSuderry\nKnapford\nGreat Waterton\nBrendam Docks\n");
    t.AddLine(iss);
    iss.clear();
    iss.str("Sodor Ironworks\nSodor Steamwork\nKnapford\nMaron\nGordon's Hill\n");
    t.AddLine(iss);
    iss.clear();
    iss.str("Lakeside Central\nLittle Waterton\n");
    t.AddLine(iss);
    iss.clear();
    iss.str("Waterton Central\nWaterton East\nWaterton Woods\nWaterton North\nWaterton East\nLittle Waterton\nWaterton West");
    t.AddLine(iss);
    assert(t.FindDest("Maron", 0) == (map<string, int>{{"Maron", 0}}));
    t.print_output(t.FindDest("Maron", 5));
    assert(t.FindDest("Maron", 1) == (map<string, int>{{"Gordon's Hill", 1},
                                                       {"Maron", 0}}));
    assert(t.FindDest("Maron", 2) == (map<string, int>{{"Gordon's Hill", 1},
                                                       {"Maron", 0},
                                                       {"Sodor Ironworks", 2},
                                                       {"Suderry", 2}}));
    assert(t.FindDest("Maron", 3) == (map<string, int>{{"Gordon's Hill", 1},
                                                       {"Knapford", 3},
                                                       {"Maron", 0},
                                                       {"Sodor Ironworks", 2},
                                                       {"Sodor Steamwork", 3},
                                                       {"Suderry", 2}}));
    assert(t.FindDest("Maron", 4) == (map<string, int>{{"Gordon's Hill", 1},
                                                       {"Great Waterton", 4},
                                                       {"Knapford", 3},
                                                       {"Maron", 0},
                                                       {"Sodor Ironworks", 2},
                                                       {"Sodor Steamwork", 3},
                                                       {"Suderry", 2}}));
    assert(t.FindDest("Maron", 5) == (map<string, int>{{"Brendam Docks", 5},
                                                       {"Gordon's Hill", 1},
                                                       {"Great Waterton", 4},
                                                       {"Knapford", 3},
                                                       {"Maron", 0},
                                                       {"Sodor Ironworks", 2},
                                                       {"Sodor Steamwork", 3},
                                                       {"Suderry", 2}}));
    assert(t.FindDest("Waterton Central", 0) == (map<string, int>{{"Waterton Central", 0}}));
    assert(t.FindDest("Waterton Central", 1) == (map<string, int>{{"Waterton Central", 0},
                                                                  {"Waterton East", 1}}));
    assert(t.FindDest("Waterton Central", 2) == (map<string, int>{{"Little Waterton", 2},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton Woods", 2}}));
    assert(t.FindDest("Waterton Central", 3) == (map<string, int>{{"Lakeside Central", 3},
                                                                  {"Little Waterton", 2},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton North", 3},
                                                                  {"Waterton West", 3},
                                                                  {"Waterton Woods", 2}}));
    assert(t.FindDest("Waterton Central", 4) == (map<string, int>{{"Lakeside Central", 3},
                                                                  {"Little Newton", 4},
                                                                  {"Little Waterton", 2},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton North", 3},
                                                                  {"Waterton West", 3},
                                                                  {"Waterton Woods", 2}}));
    assert(t.FindDest("Waterton Central", 5) == (map<string, int>{{"Lakeside Central", 3},
                                                                  {"Little Burnside", 5},
                                                                  {"Little Newton", 4},
                                                                  {"Little Waterton", 2},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton North", 3},
                                                                  {"Waterton West", 3},
                                                                  {"Waterton Woods", 2}}));
    assert(t.FindDest("Waterton Central", 6) == (map<string, int>{{"Castle Hill", 6},
                                                                  {"Green Hill", 6},
                                                                  {"Lakeside Central", 3},
                                                                  {"Little Burnside", 5},
                                                                  {"Little Newton", 4},
                                                                  {"Little Waterton", 2},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton North", 3},
                                                                  {"Waterton West", 3},
                                                                  {"Waterton Woods", 2}}));
    assert(t.FindDest("Waterton Central", 9) == (map<string, int>{{"Black Hill", 9},
                                                                  {"Castle Hill", 6},
                                                                  {"Great Newton", 8},
                                                                  {"Green Hill", 6},
                                                                  {"Lakeside", 7},
                                                                  {"Lakeside Central", 3},
                                                                  {"Little Burnside", 5},
                                                                  {"Little Newton", 4},
                                                                  {"Little Waterton", 2},
                                                                  {"Newton", 8},
                                                                  {"Newton Crossroad", 7},
                                                                  {"Waterton Central", 0},
                                                                  {"Waterton East", 1},
                                                                  {"Waterton North", 3},
                                                                  {"Waterton West", 3},
                                                                  {"Waterton Woods", 2},
                                                                  {"Wood Side", 7}}));
    assert(t.FindDest("Waterton Central", 10) == (map<string, int>{{"Black Hill", 9},
                                                                   {"Castle Hill", 6},
                                                                   {"Great Newton", 8},
                                                                   {"Green Hill", 6},
                                                                   {"Lakeside", 7},
                                                                   {"Lakeside Central", 3},
                                                                   {"Little Burnside", 5},
                                                                   {"Little Newton", 4},
                                                                   {"Little Waterton", 2},
                                                                   {"Newton", 8},
                                                                   {"Newton Crossroad", 7},
                                                                   {"Waterton Central", 0},
                                                                   {"Waterton East", 1},
                                                                   {"Waterton North", 3},
                                                                   {"Waterton West", 3},
                                                                   {"Waterton Woods", 2},
                                                                   {"Wood Side", 7}}));
    iss.clear();
    iss.str("Newton\nNewton Crossroad\nTidmouth");
    t.AddLine(iss);
    assert(t.FindDest("Waterton East", 0) == (map<string, int>{{"Waterton East", 0}}));
    assert(t.FindDest("Waterton East", 1) == (map<string, int>{{"Little Waterton", 1},
                                                               {"Waterton East", 0},
                                                               {"Waterton Woods", 1}}));
    assert(t.FindDest("Waterton East", 2) == (map<string, int>{{"Lakeside Central", 2},
                                                               {"Little Waterton", 1},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1}}));
    assert(t.FindDest("Waterton East", 3) == (map<string, int>{{"Lakeside Central", 2},
                                                               {"Little Newton", 3},
                                                               {"Little Waterton", 1},
                                                               {"Waterton Central", 3},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1}}));
    assert(t.FindDest("Waterton East", 4) == (map<string, int>{{"Lakeside Central", 2},
                                                               {"Little Burnside", 4},
                                                               {"Little Newton", 3},
                                                               {"Little Waterton", 1},
                                                               {"Waterton Central", 3},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1}}));
    assert(t.FindDest("Waterton East", 5) == (map<string, int>{{"Castle Hill", 5},
                                                               {"Green Hill", 5},
                                                               {"Lakeside Central", 2},
                                                               {"Little Burnside", 4},
                                                               {"Little Newton", 3},
                                                               {"Little Waterton", 1},
                                                               {"Waterton Central", 3},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1}}));
    assert(t.FindDest("Waterton East", 6) == (map<string, int>{{"Castle Hill", 5},
                                                               {"Green Hill", 5},
                                                               {"Lakeside", 6},
                                                               {"Lakeside Central", 2},
                                                               {"Little Burnside", 4},
                                                               {"Little Newton", 3},
                                                               {"Little Waterton", 1},
                                                               {"Newton Crossroad", 6},
                                                               {"Waterton Central", 3},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1},
                                                               {"Wood Side", 6}}));
    assert(t.FindDest("Waterton East", 9) == (map<string, int>{{"Black Hill", 8},
                                                               {"Castle Hill", 5},
                                                               {"Gordon's Hill", 8},
                                                               {"Great Newton", 7},
                                                               {"Green Hill", 5},
                                                               {"Lakeside", 6},
                                                               {"Lakeside Central", 2},
                                                               {"Little Burnside", 4},
                                                               {"Little Newton", 3},
                                                               {"Little Waterton", 1},
                                                               {"Newton", 7},
                                                               {"Newton Crossroad", 6},
                                                               {"Sodor Ironworks", 9},
                                                               {"Suderry", 9},
                                                               {"Tidmouth", 7},
                                                               {"Waterton Central", 3},
                                                               {"Waterton East", 0},
                                                               {"Waterton North", 2},
                                                               {"Waterton West", 2},
                                                               {"Waterton Woods", 1},
                                                               {"Wood Side", 6}}));
    assert(t.FindDest("Waterton East", 10) == (map<string, int>{{"Black Hill", 8},
                                                                {"Castle Hill", 5},
                                                                {"Gordon's Hill", 8},
                                                                {"Great Newton", 7},
                                                                {"Green Hill", 5},
                                                                {"Knapford", 10},
                                                                {"Lakeside", 6},
                                                                {"Lakeside Central", 2},
                                                                {"Little Burnside", 4},
                                                                {"Little Newton", 3},
                                                                {"Little Waterton", 1},
                                                                {"Newton", 7},
                                                                {"Newton Crossroad", 6},
                                                                {"Sodor Ironworks", 9},
                                                                {"Sodor Steamwork", 10},
                                                                {"Suderry", 9},
                                                                {"Tidmouth", 7},
                                                                {"Waterton Central", 3},
                                                                {"Waterton East", 0},
                                                                {"Waterton North", 2},
                                                                {"Waterton West", 2},
                                                                {"Waterton Woods", 1},
                                                                {"Wood Side", 6}}));
    assert(t.FindDest("Waterton East", 11) == (map<string, int>{{"Black Hill", 8},
                                                                {"Castle Hill", 5},
                                                                {"Gordon's Hill", 8},
                                                                {"Great Newton", 7},
                                                                {"Great Waterton", 11},
                                                                {"Green Hill", 5},
                                                                {"Knapford", 10},
                                                                {"Lakeside", 6},
                                                                {"Lakeside Central", 2},
                                                                {"Little Burnside", 4},
                                                                {"Little Newton", 3},
                                                                {"Little Waterton", 1},
                                                                {"Maron", 11},
                                                                {"Newton", 7},
                                                                {"Newton Crossroad", 6},
                                                                {"Sodor Ironworks", 9},
                                                                {"Sodor Steamwork", 10},
                                                                {"Suderry", 9},
                                                                {"Tidmouth", 7},
                                                                {"Waterton Central", 3},
                                                                {"Waterton East", 0},
                                                                {"Waterton North", 2},
                                                                {"Waterton West", 2},
                                                                {"Waterton Woods", 1},
                                                                {"Wood Side", 6}}));
    assert(t.FindDest("Waterton East", 1000000) == (map<string, int>{{"Black Hill", 8},
                                                                     {"Brendam Docks", 12},
                                                                     {"Castle Hill", 5},
                                                                     {"Gordon's Hill", 8},
                                                                     {"Great Newton", 7},
                                                                     {"Great Waterton", 11},
                                                                     {"Green Hill", 5},
                                                                     {"Knapford", 10},
                                                                     {"Lakeside", 6},
                                                                     {"Lakeside Central", 2},
                                                                     {"Little Burnside", 4},
                                                                     {"Little Newton", 3},
                                                                     {"Little Waterton", 1},
                                                                     {"Maron", 11},
                                                                     {"Newton", 7},
                                                                     {"Newton Crossroad", 6},
                                                                     {"Sodor Ironworks", 9},
                                                                     {"Sodor Steamwork", 10},
                                                                     {"Suderry", 9},
                                                                     {"Tidmouth", 7},
                                                                     {"Waterton Central", 3},
                                                                     {"Waterton East", 0},
                                                                     {"Waterton North", 2},
                                                                     {"Waterton West", 2},
                                                                     {"Waterton Woods", 1},
                                                                     {"Wood Side", 6}}));
    assert(t.FindDest("Suddery Woodside", 0) == (map<string, int>{{"Suddery Woodside", 0}}));
    assert(t.FindDest("Suddery Woodside", 666) == (map<string, int>{{"Suddery Woodside", 0}}));
    return 0;
}

