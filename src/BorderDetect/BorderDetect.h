#pragma once

#ifndef DEBUG_BORDER_DETECT
    #define DEBUG_BORDER_DETECT (0)
#else
    #undef DEBUG_BORDER_DETECT
    #define DEBUG_BORDER_DETECT (1)
#endif

#include <set>

#include "BFS/BfsAlgo.h"
#include "DataInput/FileDataInput.h"
#include "Graph/ConnectedComponents.h"
#include "Graph/DiagramGraph.h"
#include "IntMatrix2/BorderMask.h"
#include "IntMatrix2/ZeroOneMatrix.h"

#include "../Utils/MyAssert.h"
#include "../Utils/Debug.h"

class BorderDetect {
private:
    // 计算两个集合的交集
    std::set<int> intersect(std::set<int> a, std::set<int> b) const {
        std::set<int> ans;
        for(auto item: a) {
            if(b.find(item) != b.end()) {
                ans.insert(item);
            }
        }
        return ans;
    }

public:

    virtual void showAllCc(const IntMatrix2& imx) const {
        auto dg = DiagramGraph(imx);
        auto cc_alg = ConnectedComponents(dg);
        auto all_cc = cc_alg.getConnectedComponents();

        bool first_line = true;
        std::cout << "[\n";
        for(const auto& cc: all_cc) {
            if(first_line) {
                first_line = false;
            }else {
                std::cout << ",\n";
            }
            bool first = true;
            for(const auto& item: cc) {
                if(first) {
                    first = false;
                    std::cout << "    [";
                }else {
                    std::cout << ", ";
                }
                std::cout << item;
            }
            std::cout << "]";
        }
        std::cout << "\n]\n";
    }

    // 检查最大编号所在的连通分支是否在边界上
    // 这里的 IntMatrix 是一个二维的 int 矩阵
    // 你需要保证最外围的一圈元素都是零（空地）
    // last_socket_id = -1 则要求最大编号 socket 所在连通分支在最外圈
    // last_socket_id > 0 则要求 last_socket_id 所在连通分支在最外圈
    virtual bool checkBorderMaxCC(int last_socket_id, const IntMatrix2& imx) const {

        // 获取整个矩阵中的最大元素
        auto mxv = imx.getMax(); 
        ASSERT(mxv > 0);

        int lastv = mxv;
        if(last_socket_id > 0) {
            lastv = last_socket_id;
        }

        // 将所有非零位置设置为 1
        auto mmx = ZeroOneMatrix(imx);

        // 从左上角位置出发，遍历所有能走的空白位置
        // 1 是障碍物，0 是可通行位置
        SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BORDER_DETECT, "Solving BFS");
        BfsAlgo bfs_algo;
        auto vis_mx = bfs_algo.search(mmx, 0, 0);

        // 检索 0 区域的边界位置
        SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BORDER_DETECT, "Solving Border");
        auto border_pos_raw = BorderMask(std::make_shared<ZeroOneMatrix>(vis_mx), 0);
        auto border_pos     = ZeroOneMatrix(border_pos_raw);

        // 在原始数据矩阵中进行筛选
        // 筛选出所有在边界上的节点
        auto set_int = border_pos.select(imx);

        // 计算原图中的所有联通分支
        // all_cc 包含了所有的 connected component 对应的 std::set<int>
        SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BORDER_DETECT, "Solving Connected Component");
        auto dg = DiagramGraph(imx);
        auto cc_alg = ConnectedComponents(dg);
        auto all_cc = cc_alg.getConnectedComponents();

        // 找到最后编号对应的连通分量
        auto lastv_cc = std::set<int>();
        for(const auto& cc: all_cc) {
            if(cc.find(lastv) != cc.end()) {
                lastv_cc = cc;
            }
        }

        // 检查最大联通分支是否是独立在外的
        // 如果最大联通分支是独立在外的，那么我们的算法
        // 能够保证一个连通分支在最外侧，因此大概率说明不改变 pdcode 也可以做联通和
        // 但是这一点需要进一步进行验证
        // 具体做法就是对所有 link 考虑将其所有联通分支 swap 成最大编号联通分支并生成扭结
        SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_BORDER_DETECT, "Checking Cover");
        return intersect(lastv_cc, set_int).size() != 0;
    }
};
