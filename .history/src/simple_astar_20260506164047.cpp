#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <climits>
using namespace std;

const int INF = INT_MAX;

// ============================================================
// 第一部分：数据结构（两个最简单的结构体）
// ============================================================

// 每个格子的"档案"，记录关于这个格子的所有信息
struct Cell {
    int g = INF;      // 从起点走到这个格子，实际走了多远
    int f = INF;      // f = g + h，用来决定先处理谁
    int parent_x = -1; // 我是从哪个格子走过来的 (x坐标)
    int parent_y = -1; // 我是从哪个格子走过来的 (y坐标)
    bool closed = false; // 这个格子是不是已经处理完了
};

// 排队用的"号码牌"，只放3个数字
struct OpenNode {
    int x, y;  // 格子坐标
    int f;     // 总分（越小越优先）

    // C++ 的 priority_queue 默认取最大，我们要最小，所以反着写
    bool operator<(const OpenNode& other) const {
        return f > other.f;  // 注意：> 表示我们想要小的在上面
    }
};

// ============================================================
// 第二部分：工具函数
// ============================================================

// 估算从 (x1,y1) 到 (x2,y2) 的剩余距离
// 用"八角距离"：直走一步 10，斜走一步 14
int heuristic(int x1, int y1, int x2, int y2) {
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    int max_val = max(dx, dy);
    int min_val = min(dx, dy);
    return 10 * max_val + 4 * min_val;  // 斜着走的部分按14算，剩下直走按10算
}

// 判断坐标是否在网格内
bool inBounds(int x, int y, int W, int H) {
    return x >= 0 && x < W && y >= 0 && y < H;
}

// ============================================================
// 第三部分：A* 寻路算法（主角登场）
// ============================================================

// grid[y][x]：0 = 能走，1 = 墙
// start_x, start_y：起点坐标
// goal_x, goal_y：终点坐标
// path：输出路径，每个元素是 pair{x, y}
bool findPath(const vector<vector<int>>& grid,
              int start_x, int start_y,
              int goal_x, int goal_y,
              vector<pair<int,int>>& path)
{
    int h=grid.size();    // 行数
    int w=grid[0].size(); // 列数

    
    while ()
}

// ============================================================
// 第四部分：演示程序
// ============================================================

int main() {
    // 画一个 5x7 的地图
    // 0 = 路，1 = 墙
    vector<vector<int>> grid = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 1},
        {0, 0, 1, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    };
    int W = grid[0].size();  // 5
    int H = grid.size();     // 7

    int start_x = 0, start_y = 0;  // 左上角
    int goal_x  = 4, goal_y  = 6;  // 右下角

    vector<pair<int,int>> path;
    bool found = findPath(grid, start_x, start_y, goal_x, goal_y, path);

    if (found) {
        cout << "找到路径！共 " << path.size() << " 步：" << endl;
        for (auto [x, y] : path) {
            cout << "  → (" << x << ", " << y << ")" << endl;
        }

        // 在地图上画出来
        cout << endl << "地图可视化（@=路径，S=起点，G=终点，■=墙，□=路）：" << endl;

        // 先建一个字符地图
        vector<vector<char>> map(H, vector<char>(W, '.'));
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                map[y][x] = grid[y][x] ? '#' : '.';
            }
        }
        for (auto [x, y] : path) {
            map[y][x] = '@';
        }
        map[start_y][start_x] = 'S';
        map[goal_y][goal_x] = 'G';

        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                cout << map[y][x] << ' ';
            }
            cout << endl;
        }
    } else {
        cout << "没有找到路径！" << endl;
    }

    return 0;
}
