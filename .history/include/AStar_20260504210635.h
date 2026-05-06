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
        int id; // 节点ID
        int gCost; // 从起点到当前节点的当前代价
        int fCost; // gCost + hCost 总代价

        // 构造函数
        SearchNode(int nodeId, int g, int f) : id(nodeId), gCost(g), fCost(f) {}

        // 定义比较运算符，用于优先队列排序（fCost较小优先）
        // bool:返回值 operator表示：重载大于号这个符号。
        // const SearchNode& other表示要比的另外一个对象
        bool operator>(const SearchNode& other) const {
            return fCost > other.fCost;
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
    std::vector<NodeInternalState> nodeStates_; // 存储每个节点的内部状态

    std::priority_queue<SearchNode, std::vector<SearchNode>
                        , std::greater<SearchNode>> openList_; // A*的开放列表
    
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

