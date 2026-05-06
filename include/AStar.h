#pragma once
#include <vector>
#include <queue>
#include <limits>
#include "Environment.h" // 引入环境接口

class AStar
{
private:
    struct SearchNode
    {
        int id;
        int f; // f = g + h，总估计代价
        int g; // 从起点到当前节点的实际代价

        // 构造函数：参数顺序 (节点ID, f值, g值)
        SearchNode(int nodeId, int fCost, int gCost) : id(nodeId), f(fCost), g(gCost) {}

        // 定义比较运算符，用于优先队列排序（f 较小优先）
        bool operator>(const SearchNode& other) const {
            return f > other.f;
        }
    };

    // 内部状态结构体，存储每个节点的g值、父节点ID、迭代标记和是否关闭
    struct NodeInternalState
    {
        int g_;
        int parent_;
        int iter_;
        bool closed_;
    };
    int iter_=0; // 迭代计数器，用于快速重置节点状态
    int maxNodes_; // 最大节点数，用于预分配内存
    std::vector<NodeInternalState> node_states_; // 存储每个节点的内部状态

    std::priority_queue<SearchNode, std::vector<SearchNode>
                        , std::greater<SearchNode>> open_list_; // A*的开放列表
    
    void resetState(); // 重置状态，准备新的一轮搜索
    void checkNode(int id); // 检查并初始化节点状态
    // 从目标节点回溯到起点，构建路径
    void reconstructPath(int startId, int goalId, std::vector<int>& outPath); 
    
public:
    // 构造函数：传入最大节点数,提前分配好内存，避免运行时频繁分配
    explicit AStar(int maxNodes);

    // 核心寻路函数：传入环境、起点ID、目标ID，以及输出路径的容器
    bool findPath(const ISearchEnvironment& env, int startId, int goalId, 
                    std::vector<int>& outPath);
};

