// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
// #define NO_MAIN 

// 在编译时引入 -DDEBUG 可以输出调试信息
#ifdef DEBUG
    #if DEBUG
        #undef DEBUG
        #define DEBUG (1)
    #else
        #undef DEBUG
        #define DEBUG (0)
    #endif
#else
    #define DEBUG (0)
#endif

#include <iostream>
#include <string>
#include <vector>

#include "BorderDetect/BorderDetect.h"
#include "LinkAlgo.h"
#include "NodeSet3D/GenNodeSetAlgo.h"
#include "PDTreeAlgo/PDCode.h"
#include "PDTreeAlgo/PDTree.h"
#include "PDTreeAlgo/SocketInfo.h"
#include "PdToDiagram2d.h"
#include "PathEngine/Common/GetBorderSet.h"
#include "Utils/StringStream.h"

// 从 stringstream 读入一个 pd_code
// 然后试图构建二维布局或者三维布局
// 如果失败会抛出异常
void try_many_times(unsigned int min_seed, int last_socket_id, std::stringstream& ss, 
    int max_try,
    bool show_diagram,   // 是否显示二维布局图
    bool show_serial,    // 是否显示三位空间信息序列化表示
    bool with_zero,      // 输出二维布局图时是否使用零作为空位占位符
    bool show_border,    // 仅仅输出在边界上的所有 socket_id
    bool components,     // 输出所有联通分支相关信息
    bool test_all_border // 测试所有构型
) {

    // 先计算二维布局
    auto pdToDiagram2d = PdToDiagram2d();
    auto detector = BorderDetect();

    // 计算连通分支时候不需要构建二维构型图
    // 而且如果开启了计算连通分支开关，则不再需要计算其他输出
    if(components) {
        auto all_cc = pdToDiagram2d.getAllCc(ss);
        REWIND_STRING_STREAM(ss); // 用后复原
        std::cout << detector.jsonifyAllCc(all_cc);
        return;
    }

    // 需要检查的所有
    std::vector<int> last_socket_set;
    if(!test_all_border) {
        last_socket_set.push_back(last_socket_id);
    }else {
        auto all_cc = pdToDiagram2d.getAllCc(ss);
        for(auto cc: all_cc) {
            if(cc.size()) {
                last_socket_set.push_back(*cc.begin());
            }
        }
    }

    // 计算中间结果
    std::vector<std::tuple<IntMatrix, GenNodeSetAlgo, GetBorderSet>> calc_ans;

    // 计算所有可能外围设定对应的
    int suc_cnt   = 0;
    int cur_idx   = 0;
    int total_cnt = last_socket_set.size();
    for(auto last_socket_id_now: last_socket_set) {
        cur_idx += 1;
        
        // 输出正在测试第几组样例
        if(total_cnt > 1) {
            SHOW_DEBUG_MESSAGE(
                std::string("testing ") 
                + std::to_string(cur_idx)
                + " / " + std::to_string(total_cnt));
        }
        try {
            auto [link_algo, im] = pdToDiagram2d.convert(min_seed, last_socket_id_now, ss, max_try);

            GenNodeSetAlgo gen_node_set_algo(link_algo.getFinalGraph(), link_algo.getAllEdges());
            GetBorderSet gbs(im);

            // 记录中间答案
            calc_ans.push_back(std::make_tuple(im, gen_node_set_algo, gbs));
            suc_cnt += 1;

        }
        PROCESS_EXCEPTION(MaxTryExceeded, ;) // 处理超过最大尝试次数导致的异常
    }

    // 输出所有测试的测试结果
    if(test_all_border) {
        std::cout << suc_cnt << " / " << total_cnt << std::endl;
    }
    
    // 针对非测试状态编写的代码
    {
        SHOW_DEBUG_MESSAGE("output ans ...");
        auto [im, gen_node_set_algo, gbs] = calc_ans[0];

        if(show_diagram) {
            im.debugOutput(std::cout, with_zero); // 输出二维布局图
            return;
        }
        if(show_serial) {
            gen_node_set_algo.outputGraph(std::cout); // 输出三维点坐标情况
            return;
        }
        if(show_border) { // 仅仅输出边界信息
            gbs.debugOutput(std::cout); // 仅仅输出边界信息
            return;
        }
    }
}

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main(int argc, char** argv) {

    // 获取所有参数
    std::vector<std::string> args;
    for(int i = 1; i < argc; i += 1) {
        args.push_back(std::string(argv[i]));
    }
    
    int  last_socket_id  = -1;    // 默认最后一个 socket 所在的连通分支需要在最外侧
    bool show_diagram    = false; // 是否要输出一个图
    bool show_serial     = false; // 输出一个 3D 序列化
    bool with_zero       = false; // 输出图的时候是否要
    bool show_border     = false; // 是否要输出边界信息（输出边界信息的话，就不会输出图或者序列化表示）
    bool components      = false; // 是否需要输出所有的联通分支
    bool test_all_border = false; // 测试所有构型

// 用于定义所有参数信息
#define DECLARE_ARGUMENT(LONG_NAME, SHORT_NAME, VAR_NAME) if(( \
    args[i] == (LONG_NAME) || args[i] == (SHORT_NAME)) \
) { \
    (VAR_NAME) = true; \
}else

    // 处理命令行参数
    for(int i = 0; i < args.size(); i += 1) {
        DECLARE_ARGUMENT(    "--diagram", "-d",    show_diagram)
        DECLARE_ARGUMENT(  "--with_zero", "-z",       with_zero)
        DECLARE_ARGUMENT(     "--serial", "-s",     show_serial)
        DECLARE_ARGUMENT(     "--border", "-b",     show_border)
        DECLARE_ARGUMENT( "--components", "-c",      components)
        DECLARE_ARGUMENT(       "--test", "-t", test_all_border)

        // 数字的情况可以用于设置 last_socket_id
        if(args[i].size() > 2 && args[i].substr(0, 2) == "--" && isAllDigits(args[i].substr(2))) {
            last_socket_id = std::stoi(args[i].substr(2));
        }else
        { // 没有匹配时的默认处理方式
            std::cerr << "warning: undefined command line argument: " + args[i] << std::endl;
        }
    }

#undef DECLARE_ARGUMENT

    // read in all content in stdin
    auto pd_code_ss = readCinToStringStream();
    int max_try = 100;
    unsigned int min_seed = 42;

    // 尝试给出答案
    try_many_times(
        min_seed, 
        last_socket_id, 
        pd_code_ss, 
        max_try, 
        show_diagram, show_serial, with_zero, show_border, components, test_all_border);
    return 0;
}
#endif
