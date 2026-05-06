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
    int H = grid.size();      // 地图高度（行数）
    int W = grid[0].size();   // 地图宽度（列数）

    path.clear();

    // 起点就是终点？直接返回
    if (start_x == goal_x && start_y == goal_y) {
        path.push_back({start_x, start_y});
        return true;
    }

    // ---------- 步骤1：创建并重置所有格子的档案 ----------
    vector<vector<Cell>> cells(H, vector<Cell>(W));
    // vector 创建时自动调用 Cell 的默认值：g=INF, f=INF, parent=-1, closed=false
    // 所以不需要额外重置了

    // ---------- 步骤2：初始化起点 ----------
    cells[start_y][start_x].g = 0;                       // 起点到自己的距离 = 0
    cells[start_y][start_x].f = heuristic(start_x, start_y, goal_x, goal_y); // f = 0 + h = h

    priority_queue<OpenNode> open_list;                   // 排队区
    open_list.push({start_x, start_y, cells[start_y][start_x].f}); // 把起点放进去

    // ========== 步骤3：主循环 ==========

    // 8个方向：右 左 下 上 右下 左下 右上 左上
    const int dx[8] = {1, -1, 0,  0, 1,  1, -1, -1};
    const int dy[8] = {0,  0, 1, -1, 1, -1,  1, -1};

    while (!open_list.empty()) {
        // ----- 3a. 从排队区取出 f 值最小的号码牌 -----
        OpenNode cur = open_list.top();
        open_list.pop();

        int cx = cur.x;
        int cy = cur.y;
        Cell& cur_cell = cells[cy][cx];  // 拿出当前格子的档案

        // ----- 3b. 这个格子已经处理过了？跳过（过期号码牌）-----
        if (cur_cell.closed) continue;

        // ----- 3c. 标记为"已处理" -----
        cur_cell.closed = true;

        // ----- 3d. 到达终点了！溯回路径 -----
        if (cx == goal_x && cy == goal_y) {
            // 从终点沿着 parent 一路找回起点
            int x = goal_x;
            int y = goal_y;
            while (x != -1 && y != -1) {
                path.push_back({x, y});
                Cell& c = cells[y][x];
                int px = c.parent_x;
                int py = c.parent_y;
                x = px;
                y = py;
            }
            reverse(path.begin(), path.end());  // 倒过来变成起点→终点
            return true;
        }

        // ----- 3e. 还没到终点，检查周围8个邻居 -----
        for (int i = 0; i < 8; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            // 检查1：越界了？
            if (!inBounds(nx, ny, W, H)) continue;

            // 检查2：是墙？
            if (grid[ny][nx] == 1) continue;

            // 检查3：斜着走的时候，两个相邻的直走格子不能是墙（防止穿墙角）
            bool is_diag = (dx[i] != 0 && dy[i] != 0);
            if (is_diag) {
                // 例如往右下走：(cx+1, cy) 和 (cx, cy+1) 两个格子必须是路
                if (grid[cy][cx + dx[i]] == 1 || grid[cy + dy[i]][cx] == 1) continue;
            }

            Cell& neighbor_cell = cells[ny][nx];

            // 检查4：邻居已经处理过了？
            if (neighbor_cell.closed) continue;

            // ----- 计算新路径的 g 值 -----
            int move_cost = is_diag ? 14 : 10;           // 直走10，斜走14
            int new_g = cur_cell.g + move_cost;          // 经过当前格子走到邻居的代价

            // ----- 如果新路径比之前记录的更短，更新 -----
            if (new_g < neighbor_cell.g) {
                neighbor_cell.g = new_g;                 // 更新最短距离
                neighbor_cell.parent_x = cx;             // 记下"爸爸"是当前格子
                neighbor_cell.parent_y = cy;

                int h = heuristic(nx, ny, goal_x, goal_y);
                neighbor_cell.f = new_g + h;             // 更新总分

                open_list.push({nx, ny, neighbor_cell.f}); // 放进排队区
            }
        }
    }

    // open_list 空了还没找到终点 → 没有路
    return false;
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
