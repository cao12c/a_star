#pragma once
#include <vector>

// 邻居
struct Neighbor {
    int id; // 邻居的ID
    int cost; // 权重
};

// 定义接口来解耦。算法只需要知道环境提供的接口，而不需要关心环境的具体实现细节。
class ISearchEnvironment {public:
    virtual ~ISearchEnvironment() = default;
    
    // 获取总节点数（用于 A* 预分配内存）
    virtual int getNodeCount() const = 0;
    
    // 获取某个节点的所有合法邻居及代价
    virtual void getNeighbors(int nodeId, std::vector<Neighbor>& outNeighbors) const = 0;
    
    // 计算启发式代价 (Heuristic)
    virtual int getHeuristic(int fromId, int toId) const = 0;
};


