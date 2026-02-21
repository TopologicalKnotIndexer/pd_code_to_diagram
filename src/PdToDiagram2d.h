#pragma once
#include <algorithm>
#include <sstream>
#include <tuple>

#include "BorderDetect/BorderDetect.h"
#include "BorderDetect/Graph/ConnectedComponents.h"
#include "BorderDetect/Graph/Graph.h"
#include "PathEngine/Common/IntMatrix.h"
#include "PdTreeAlgo/PDCode.h"
#include "PdTreeAlgo/PDTree.h"
#include "Utils/Debug.h"
#include "Utils/Exceptions.h"
#include "Utils/Random.h"
#include "Utils/StringStream.h"
#include "LinkAlgo.h"

class PdToDiagram2d {
public:
    virtual std::tuple<LinkAlgo, IntMatrix> tryConvertOnce(
        unsigned int seed,
        int last_socket_id,
        std::stringstream& pd_code_ss
    ) const {
        
        // 重置随机种子
        myrandom::setSeed(seed);

        SHOW_DEBUG_MESSAGE("input pd_code ...");
        PDCode pd_code;
        pd_code.InputPdCode(pd_code_ss); // 输入一个 pd_code

        SHOW_DEBUG_MESSAGE("generating pd_tree ...");
        PDTree pd_tree;

        // 生成树形图直到没有交叉点重叠
        while(true) {
            pd_tree.clear();
            pd_tree.load(pd_code, last_socket_id); // 生成树形图
            if(pd_tree.checkNoOverlay()) {
                break;
            }
        }

        SHOW_DEBUG_MESSAGE("generating and checking socket_info ...");
        SocketInfo s_info = pd_tree.getSocketInfo(); // 生成完全的插头信息
        int component_cnt = pd_tree.getComponentCnt();
        s_info.check(pd_code.getCrossingNumber(), component_cnt);   // 检查信息合法性

        SHOW_DEBUG_MESSAGE("running link algo ...");
        LinkAlgo link_algo(pd_code.getCrossingNumber(), s_info, component_cnt);
        auto im = link_algo.getFinalGraph().exportToIntMatrix();

        // 检查最大编号所在的连通分支是否在最外圈
        SHOW_DEBUG_MESSAGE("checking border ...");
        auto im2 = im.toIntMatrix2();
        auto detector = BorderDetect();
        auto detector_flag = detector.checkBorderMaxCC(last_socket_id, im2);

        // 检查布局算法是否成功
        if(!detector_flag) {
            THROW_EXCEPTION(BadBorderException, "");
        }

        // 返回计算得到的 matrix
        return std::make_tuple(link_algo, im);
    }

    // last_socket_id 用于给出哪个连通分支应该位于最外侧
    // last_socket_id = -1 表示让最大编号元素在最外圈
    virtual std::tuple<LinkAlgo, IntMatrix> convert(
        unsigned int min_seed, 
        int last_socket_id,
        std::stringstream& pd_code_ss,
        int max_try = 100
    ) const {
        auto ans = std::make_tuple(LinkAlgo(), IntMatrix(1, 1));

        bool fail = true;
        bool suc = false;
        for(unsigned int seed = min_seed; seed <= min_seed + max_try; seed += 1) {
            try{
                REWIND_STRING_STREAM(pd_code_ss);

                // 赋值函数
                ans = tryConvertOnce(seed, last_socket_id, pd_code_ss);

                fail = false; // 没有失败
                suc = true;   // 成功了
            }
            PROCESS_EXCEPTION(CrossingMeetException, fail = true)
            PROCESS_EXCEPTION(BadBorderException, fail = true)

            if(!fail) { //  如果没失败就退出
                break;
            }
        }

        // 如果没有成功，则抛出异常
        if(!suc) { 
            SHOW_DEBUG_MESSAGE(
                std::string("failed after ") 
                + std::to_string(max_try) + std::string(" try."));

            // 抛出最大尝试超过异常
            THROW_EXCEPTION(MaxTryExceeded, "");
        }
        return ans;
    }

    // 给定一个 pd_code，计算其中的所有连通分支
    virtual std::vector<std::set<int>> getAllCc(std::stringstream& pd_code_ss) const {
        PDCode pd_code;
        pd_code.InputPdCode(pd_code_ss); // 输入一个 pd_code

        // 构建一个双向图
        Graph node_graph;
        for(int i = 0; i < pd_code.getCrossingNumber(); i += 1) {
            auto crossing = pd_code.getCrossing(i);
            for(int j = 0; j <= 1; j += 1) {
                int frm = crossing.getRaw(j);
                int eto = crossing.getRaw(j + 2);
                node_graph.addEdge(frm, eto);
            }
        }

        // 计算所有连通分支
        auto cc_alg = ConnectedComponents(node_graph);
        auto all_cc = cc_alg.getConnectedComponents();
        return all_cc;
    }
};
