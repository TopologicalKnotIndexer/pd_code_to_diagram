#pragma once

#ifndef DEBUG_BFS
    #define DEBUG_BFS (0)
#else
    #undef DEBUG_BFS
    #define DEBUG_BFS (1)
#endif

#include <queue>

#include "../IntMatrix2/ZeroOneMatrix.h"
#include "../IntMatrix2/BorderWrap.h"

#include "../../Utils/MyAssert.h"
#include "../../Utils/Debug.h"

class BfsAlgo {
public:

    // 给定一个起始点，返回一个 0, 1 矩阵描述哪些位置可以走到
    ZeroOneMatrix search(const ZeroOneMatrix& graph, int xpos, int ypos) const {
        ASSERT(0 <= xpos && xpos < graph.getRcnt());
        ASSERT(0 <= ypos && ypos < graph.getCcnt());

        // 用一个 vis 数组记录每一个位置是否被访问过
        auto vis = ZeroOneMatrix(graph.getRcnt(), graph.getCcnt());

        // 1 表示障碍物，0 表示不是障碍物
        // 在外围填充 1
        auto new_graph = BorderWrap(1, 
            std::make_shared<ZeroOneMatrix>(graph)); // 拷贝构造一个
        ASSERT(graph.getPos(xpos, ypos) == 0); // 初始位置不能是障碍物

        // BFS 队列
        std::queue<std::tuple<int, int>> q;
        q.push(std::make_tuple(xpos, ypos));
        vis.setPos(xpos, ypos, 1);

        const int dx[] = {1, -1, 0,  0};
        const int dy[] = {0,  0, 1, -1};

        while(!q.empty()) {
            auto [x, y] = q.front(); q.pop();
            SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BFS, 
                std::string("  - Checking Position (") 
                    + std::to_string(x) + ", " + std::to_string(y) + ")");

            for(int d = 0; d < 4; d += 1) {
                int nx = x + dx[d];
                int ny = y + dy[d];

                // 在调试模式下输出访问到的位置
                SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BFS, 
                    std::string("  - Checking Adj Position (") 
                        + std::to_string(nx) + ", " + std::to_string(ny) + ")");

                // 不需要访问已经访问过或者是障碍物的节点
                // 这里必须先判断 new_graph.getPos(nx, ny) 因为 vis.getPos(nx, ny) 没有边界安全
                if(new_graph.getPos(nx, ny) || vis.getPos(nx, ny)) {
                    continue;
                }

                // 既不是障碍物，也没访问过，那就走过去
                vis.setPos(nx, ny, 1);
                q.push(std::make_tuple(nx, ny));
            }
        }

        // 返回哪些位置能访问
        SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BFS, "BFS Finished");
        return vis;
    }
};
