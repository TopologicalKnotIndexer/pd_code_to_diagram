// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
// #define NO_MAIN 

// 在编译时引入 -DDEBUG 可以在 main 函数中输出调试信息
#ifndef DEBUG
    #define DEBUG (0)
#endif

#include <iostream>
#include <string>
#include <vector>

#include "LinkAlgo.h"
#include "NodeSet3D/GenNodeSetAlgo.h"
#include "PDTreeAlgo/PDCode.h"
#include "PDTreeAlgo/PDTree.h"
#include "PDTreeAlgo/SocketInfo.h"

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main(int argc, char** argv) {

    // 获取所有参数
    std::vector<std::string> args;
    for(int i = 1; i < argc; i += 1) {
        args.push_back(std::string(argv[i]));
    }

    // 是否要输出一个图
    bool show_diagram = false;
    bool with_zero = false;
    for(int i = 0; i < args.size(); i += 1) {
        if(args[i] == "--diagram" || args[i] == "-d") {
            show_diagram = true;
        }
        if(args[i] == "--with_zero" || args[i] == "-z") {
            with_zero = true;
        }
    }

    // 设置随机种子
    unsigned int seed = 42;
    myrandom::setSeed(seed);

    if(DEBUG) std::cerr << "input pd_code ..." << std::endl;
    PDCode pd_code;
    pd_code.InputPdCode(std::cin); // 输入一个 pd_code

    if(DEBUG) std::cerr << "generating pd_tree ..." << std::endl;
    PDTree pd_tree;

    // 生成树形图直到没有交叉点重叠
    while(true) {
        pd_tree.clear();
        pd_tree.load(pd_code); // 生成树形图
        if(pd_tree.checkNoOverlay()) {
            break;
        }
    }

    if(DEBUG) std::cerr << "generating and checking socket_info ..." << std::endl;
    SocketInfo s_info = pd_tree.getSocketInfo(); // 生成完全的插头信息
    int component_cnt = pd_tree.getComponentCnt();
    s_info.check(pd_code.getCrossingNumber(), component_cnt);   // 检查信息合法性

    if(DEBUG) std::cerr << "running link algo ..." << std::endl;
    LinkAlgo link_algo(pd_code.getCrossingNumber(), s_info, component_cnt);
    if(show_diagram) {
        link_algo.getFinalGraph().debugOutput(std::cout, with_zero); // 输出二维布局图
    }

    if(DEBUG) std::cerr << "output ans ..." << std::endl;
    GenNodeSetAlgo gen_node_set_algo(link_algo.getFinalGraph(), link_algo.getAllEdges());
    if(!show_diagram) {
        gen_node_set_algo.outputGraph(std::cout); // 输出三维点坐标情况
    }
    return 0;
}
#endif
