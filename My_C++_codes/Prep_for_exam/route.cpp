#ifndef __PROGTEST__
 
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <limits>
 
#endif /* __PROGTEST__ */

 
class CMap {
public:
 
    CMap(const std::vector<std::vector<int>> &map) : board(map){
    }
    ~CMap() = default;
    void print() const {
        for (auto i : board) {
            for (auto j : i) {
                std::cout << j << " ";
            }
            std::cout << "\n";
        }
        std::cout << board[0].size() << " " << board.size() << "\n";
    }
    int findDistance(size_t x1, size_t y1, size_t x2, size_t y2) {
        std::queue<std::pair<size_t, size_t>> togo;
        std::set<std::pair<size_t, size_t>> visited;
        if (x1 == x2 && y1 == y2) return 0;
        int moves = 1;
        int x3 = x2;
        int y3 = y2;
        
        togo.push({x1,y1});
        visited.insert({x1,y1});

        int dx[] = {0,1,0,-1};
        int dy[] = {1,0,-1,0};

        while(!togo.empty()) {
            size_t size = togo.size();
            for (size_t k = 0; k < size; k++) {

                for (int i = 0; i < 4; i++) {
                    int move_x = dx[i];
                    int move_y = dy[i];

                    int col_size = board[0].size();
                    int row_size = board.size();

                    int base_x = togo.front().first;
                    int base_y = togo.front().second;

                    int new_x = base_x + move_x;
                    int new_y = base_y + move_y;

                    if (new_x < 0 || new_x >= col_size || new_y < 0 || new_y >= row_size ) continue;
                    if ((board[new_y][new_x] - board[base_y][base_x]) > 6 || (board[base_y][base_x] - board[new_y][new_x]) > 3 ) continue;
                    if (visited.find({new_x, new_y}) != visited.end()) continue;
                    togo.push({new_x,new_y});
                    visited.insert({new_x,new_y});

                    if (new_x == x3 && new_y == y3) {
                        std::cout << "moves count: " << moves << "\n";
                        return moves;
                    }

                }
                togo.pop();
            }        
            moves++;       
        }
        // std::cout << "konec pico\n";
        return -1;
    }
 
private:
    std::vector<std::vector<int>> board;

};
 
 
#ifndef __PROGTEST__
 
int main() {
    CMap m0({
                    {5,  24, 21},
                    {11, 27, 18},
                    {17, 30, 22},
                    {21, 27, 19}});
    // m0.print();
// m0.findDistance(2, 0, 1, 3);
   assert(m0.findDistance(0, 0, 2, 3) == 11);
    assert(m0.findDistance(0, 1, 0, 0) == -1);
    assert(m0.findDistance(2, 0, 1, 3) == 4);
 
 
    CMap m1({
                    {1,  2,  3,  4,  5},
                    {6,  7,  8,  9,  10},
                    {11, 12, 13, 14, 15},
                    {16, 17, 18, 19, 20},
                    {21, 22, 23, 24, 25}
            });
            m1.print();
 m1.findDistance(0, 0, 4, 4);
    assert(m1.findDistance(0, 0, 4, 4) == 8);
    assert(m1.findDistance(4, 4, 0, 0) == -1);
    assert(m1.findDistance(0, 0, 0, 0) == 0);
    assert(m1.findDistance(4, 4, 4, 4) == 0);
 
    CMap m2({
                    {14, 7,  1,   1, 5},
                    {17, 13, 100, 1, 5},
                    {14, 1,  4,   1, 4},
                    {11, 2,  100, 1, 4},
                    {8,  5,  100, 1, 4}
            });
 
    assert(m2.findDistance(2, 0, 4, 4) == 6);
    assert(m2.findDistance(3, 1, 1, 4) == 5);
    assert(m2.findDistance(2, 1, 4, 0) == -1);
    assert(m2.findDistance(2, 0, 1, 4) == 7);
 
    CMap m3({
                    {1, 2, 3, 4, 5},
                    {1, 1, 1, 1, 1},
                    {1, 2, 3, 4, 5}
            });
 
    assert(m3.findDistance(0, 0, 4, 2) == 6);
    assert(m3.findDistance(4, 2, 0, 0) == 6);
    assert(m3.findDistance(0, 1, 4, 1) == 4);
    assert(m3.findDistance(4, 1, 0, 1) == 4);
 
    CMap m4({
                    {12, 15, 18},
                    {15, 12, 15},
                    {18, 15, 12}
            });
 
    assert(m4.findDistance(0, 0, 2, 2) == 4);
    assert(m4.findDistance(2, 2, 0, 0) == 4);
    assert(m4.findDistance(1, 1, 0, 0) == 2);
    assert(m4.findDistance(0, 0, 1, 1) == 2);
 
    return EXIT_SUCCESS;
}
 
#endif /* __PROGTEST__ */
