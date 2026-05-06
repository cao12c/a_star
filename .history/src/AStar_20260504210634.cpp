#include "include\AStar.h"
#include <algorithm>

const int INF = std::numeric_limits<int>::max();

// 初始化：一次性分配好所有内存
AStar::AStar(int maxNodes) : maxNodes_(maxNodes) {
    node_states_.assign(maxNodes_, { INF, -1, 0, false });
}

void AStar::resetState() {
    if (iter_ == std::numeric_limits<int>::max()) {
        iter_ = 1;
        for (auto& state : node_states_) state.iter_ = 0;
    } else {
        iter_++;
    }
    // 快速清空标准库队列的写法
    open_list_ = std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>>();
}

void AStar::checkNode(int id) {
    auto& state = node_states_[id];
    if (state.iter_ != iter_) {
        state.g_ = INF;
        state.parent_ = -1;
        state.closed_ = false;
        state.iter_ = iter_;
    }
}

void AStar::reconstructPath(int startId, int goalId, std::vector<int>& outPath) {
    outPath.clear();
    int cur = goalId;
    while (cur != -1) {
        outPath.push_back(cur);
        if (cur == startId) break;
        cur = node_states_[cur].parent_;
    }
    std::reverse(outPath.begin(), outPath.end());
}

// 核心主循环
bool AStar::findPath(const ISearchEnvironment& env, int startId, int goalId, std::vector<int>& outPath) {
    outPath.clear();
    if (startId == goalId) { outPath.push_back(startId); return true; }

    resetState();
    checkNode(startId); 
    node_states_[startId].g_ = 0;
    
    // 向环境要从起点到终点的预估代价 (h)
    int h0 = env.getHeuristic(startId, goalId); 
    open_list_.push({startId, h0, 0});

    std::vector<Neighbor> neighbors; 
    neighbors.reserve(26); // 预留大一点的空间，以后做3D无人机也够用

    while (!open_list_.empty()) {
        SearchNode cur = open_list_.top();
        open_list_.pop();

        int cid = cur.id;
        auto& curState = node_states_[cid];

        if (curState.closed_ || cur.g != curState.g_) continue; 
        if (cid == goalId) {
            reconstructPath(startId, goalId, outPath);
            return true;
        }

        curState.closed_ = true; 
        
        // ---------------------------------------------------------
        // 最核心的解耦点：清空袋子，把当前ID给环境，让环境把能走的邻居装进来
        // ---------------------------------------------------------
        neighbors.clear();
        env.getNeighbors(cid, neighbors); 

        for (const auto& n : neighbors) {
            int nid = n.id;
            checkNode(nid); 
            auto& nState = node_states_[nid];

            if (nState.closed_) continue;

            int ng = curState.g_ + n.cost; // cost 是卡带算好传进来的
            if (ng < nState.g_) {
                nState.g_ = ng;
                nState.parent_ = cid;
                // 再次向环境要预估代价
                int h = env.getHeuristic(nid, goalId);
                open_list_.push({ nid, ng + h, ng });
            }
        }
    }
    return false;
}