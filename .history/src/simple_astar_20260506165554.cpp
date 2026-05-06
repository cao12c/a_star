#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <climits>
using namespace std;

const int INF = INT_MAX;

// ============================================================
// 第一部分：数据结构
// ============================================================

// 每个格子的"档案"：记录关于这个格子的所有信息
struct Cell {
    int g = INF;         // 从起点走到这里花了多少
    int parent_x = -1;   // 我是从哪个格子走过来的 (x)
    int parent_y = -1;   // 我是从哪个格子走过来的 (y)
    bool closed = false; // 这个格子是不是已经处理完了
};

// 排队区里的一个元素：{x, y, g, f}
// 这就是你说的 "列表里有一个坐标和两个整数"
struct OpenNode {
    int x, y;
    int g;  // 走到这个格子的实际代价
    int f;  // g + h，总分，谁小谁优先
};

// ============================================================
// 第二部分：工具函数
// ============================================================

// 猜一猜从 (x1,y1) 到 (x2,y2) 还剩多远
int heuristic(int x1, int y1, int x2, int y2) {
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    return 10 * max(dx, dy) + 4 * min(dx, dy);
}

// 判断坐标在不在网格里面
bool inBounds(int x, int y, int W, int H) {
    return x >= 0 && x < W && y >= 0 && y < H;
}

// ============================================================
// 第三部分：A* 寻路
// ============================================================

bool findPath(const vector<vector<int>>& grid,
              int start_x, int start_y,
              int goal_x, int goal_y,
              vector<pair<int,int>>& path)
{
    int H = grid.size();
    int W = grid[0].size();
    path.clear();

    // 起点就是终点
    if (start_x == goal_x && start_y == goal_y) {
        path.push_back({start_x, start_y});
        return true;
    }

    // ---------- 第1步：创建所有格子的档案 ----------
    vector<vector<Cell>> cell(W, vector<Cell>(H));
    // ---------- 第2步：创建排队区 ----------
    // 排队区 = 一个集合（vector），每个坐标最多出现一次
    vector<OpenNode> open_list;

    // in_open[y][x] = 这个坐标是不是已经在排队区里了
    // 用来保证"集合"的特性：每个元素唯一
    vector<vector<bool>> in_open(H, vector<bool>(W, false));

    // ---------- 第3步：把起点放进排队区 ----------
    cells[start_y][start_x].g = 0;
    int start_h = heuristic(start_x, start_y, goal_x, goal_y);
    int start_f = 0 + start_h;

    open_list.push_back({start_x, start_y, 0, start_f});
    in_open[start_y][start_x] = true;

    // 8个移动方向
    const int dx[8] = {1, -1, 0,  0, 1,  1, -1, -1};
    const int dy[8] = {0,  0, 1, -1, 1, -1,  1, -1};

    // ---------- 第4步：主循环 ----------
    while (!open_list.empty()) {

        // --- 4a. 从排队区里找到 f 最小的那个元素 ---
        // 做法：扫一遍整个 vector，记下 f 最小的那个的下标
        int best_idx = 0;
        for (int i = 1; i < (int)open_list.size(); i++) {
            if (open_list[i].f < open_list[best_idx].f) {
                best_idx = i;
            }
        }

        // --- 4b. 把它从排队区里拿出来 ---
        // 做法：用最后一个元素覆盖它，然后删掉最后一个元素
        OpenNode cur = open_list[best_idx];
        open_list[best_idx] = open_list.back();
        open_list.pop_back();
        in_open[cur.y][cur.x] = false;  // 不在排队区里了

        int cx = cur.x;
        int cy = cur.y;
        Cell& cur_cell = cells[cy][cx];

        // --- 4c. 已经处理过了？跳过 ---
        if (cur_cell.closed) continue;

        // --- 4d. 标记为已处理 ---
        cur_cell.closed = true;

        // --- 4e. 是终点吗？是就回溯路径 ---
        if (cx == goal_x && cy == goal_y) {
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
            reverse(path.begin(), path.end());
            return true;
        }

        // --- 4f. 不是终点，检查周围8个邻居 ---
        for (int i = 0; i < 8; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            // 防线1：越界
            if (!inBounds(nx, ny, W, H)) continue;

            // 防线2：撞墙
            if (grid[ny][nx] == 1) continue;

            // 防线3：斜着走时检查墙角
            bool is_diag = (dx[i] != 0 && dy[i] != 0);
            if (is_diag) {
                if (grid[cy][cx + dx[i]] == 1 || grid[cy + dy[i]][cx] == 1) continue;
            }

            Cell& nb = cells[ny][nx];

            // 防线4：已处理
            if (nb.closed) continue;

            // ========== 核心：算 → 比 → 改 → 扔 ==========

            // 算：从起点经过我走到邻居，总共多远
            int move_cost = is_diag ? 14 : 10;
            int new_g = cur.g + move_cost;

            // 比：比邻居之前记录的路更短吗？
            if (new_g < nb.g) {
                // 改：更新邻居的档案
                nb.g = new_g;
                nb.parent_x = cx;
                nb.parent_y = cy;

                int new_h = heuristic(nx, ny, goal_x, goal_y);
                int new_f = new_g + new_h;

                // 扔：放进排队区
                if (in_open[ny][nx]) {
                    // 邻居已经在排队区里了 → 找到它，更新它的 g 和 f
                    for (int k = 0; k < (int)open_list.size(); k++) {
                        if (open_list[k].x == nx && open_list[k].y == ny) {
                            open_list[k].g = new_g;
                            open_list[k].f = new_f;
                            break;
                        }
                    }
                } else {
                    // 邻居不在排队区里 → 加进去
                    open_list.push_back({nx, ny, new_g, new_f});
                    in_open[ny][nx] = true;
                }
            }
        }
    }

    // 排队区空了还没到终点 → 没路
    return false;
}

// ============================================================
// 第四部分：演示
// ============================================================

int main() {
    // 5列 × 7行 的地图，0=路，1=墙
    vector<vector<int>> grid = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 1},
        {0, 0, 1, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    };

    int start_x = 0, start_y = 0;  // 左上角
    int goal_x  = 4, goal_y  = 6;  // 右下角

    vector<pair<int,int>> path;
    bool found = findPath(grid, start_x, start_y, goal_x, goal_y, path);

    if (found) {
        cout << "找到路径！共 " << path.size() << " 步：" << endl;
        for (auto [x, y] : path) {
            cout << "  -> (" << x << ", " << y << ")" << endl;
        }

        // 画地图
        int H = grid.size();
        int W = grid[0].size();
        vector<vector<char>> map(H, vector<char>(W, '.'));
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                if (grid[y][x]) map[y][x] = '#';
        for (auto [x, y] : path) map[y][x] = '@';
        map[start_y][start_x] = 'S';
        map[goal_y][goal_x] = 'G';

        cout << endl << "地图：" << endl;
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
