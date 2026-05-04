#pragma once
#include "Environment.h" 
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

struct GridView {
    int W, H;// 地图宽高
    const uint8_t* blocked; 
};

class Grid2DEnvironment : public ISearchEnvironment {
private:
    GridView grid_; // 地图数据

    // 将二维坐标转换为一维数组
    inline int id(int x, int y) const {
        return y * grid_.W + x;
    }
    // 将一维数组索引转换为二维坐标
    inline void xy(int id, int& x, int& y) const {
        x = id % grid_.W;
        y = id / grid_.W;
    }
public:
    // 构造函数，接受一个GridView对象。
    // 将外部传入的grid数据保存到grid_中
    Grid2DEnvironment(const GridView& grid) : grid_(grid) {}

    // 返回格子数(override 纯虚函数,表示这是对接口的实现)
    int getNodeCount() const override {
        return grid_.W * grid_.H;
    }

    // 获取格子可访问的邻居
    void getNeighbors(int nodeId, std::vector<Neighbor>& outNeighbors) const override {
        int cx,cy;
        
        // 引用传递，直接修改cx和cy的值
        xy(nodeId, cx, cy);// 将一维索引转换为二维坐标。

        const int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1}; // 右、左、下、上、右上、左上、右下、左下
        const int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1}; // 右、左、下、上、右上、左上、右下、左下

        for (int i = 0; i < 8; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            // 检查边界和障碍物
            if (nx < 0 || nx >= grid_.W || ny < 0 || ny >= grid_.H) continue;

            int nid = id(nx, ny);//转成一维索引

            // 2. 障碍物检查 (撞墙了)
            if (grid_.blocked[nid] != 0) continue;

            // 3. 检查斜着走的时候旁边有没有墙阻挡
            bool diagonal = (dx[i] != 0 && dy[i] != 0);
            if (diagonal) {
                int adj1 = id(cx + dx[i], cy);
                int adj2 = id(cx, cy + dy[i]);
                if (grid_.blocked[adj1] != 0 || grid_.blocked[adj2] != 0) continue;
            }

            // 4. 如果以上都通过了，说明这个邻居合法。执行下一步操作
            int cost = diagonal ? 14 : 10;
            Neighbor n;  
            n.id = nid;      
            n.cost = cost;   
            outNeighbors.push_back(n); 
        }
    }
    // 启发式函数，计算从fromId到toId的估计代价。用欧几里得
    int getHeuristic(int fromId, int toId) const override {
        int fx, fy, tx, ty;
        xy(fromId, fx, fy);
        xy(toId, tx, ty);

        int dx = std::abs(fx - tx);
        int dy = std::abs(fy - ty);
        int max_val = std::max(dx, dy);
        int min_val = std::min(dx, dy);
        return 10*max_val + 4*(min_val); // 斜着走的部分按14算，剩下的按10算    
    }   
};

