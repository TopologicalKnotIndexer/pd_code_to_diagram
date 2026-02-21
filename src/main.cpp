// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
// #define NO_MAIN 

// 在编译时引入 -DDEBUG 可以在 main 函数中输出调试信息
#ifndef DEBUG
    #define DEBUG (0)
#else
    #undef DEBUG
    #define DEBUG (1)
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
#include "PathEngine/Common/GetBorderSet.h"
#include "Utils/StringStream.h"

void try_once(unsigned int seed, int last_socket_id, std::stringstream& ss, 
    bool show_diagram, // 是否显示二维布局图
    bool show_serial,  // 是否显示三位空间信息序列化表示
    bool with_zero,    // 输出二维布局图时是否使用零作为空位占位符
    bool show_border,  // 仅仅输出在边界上的所有 socket_id
    bool components    // 输出所有联通分支相关信息
) {
    
    // 重置随机种子
    myrandom::setSeed(seed);

    SHOW_DEBUG_MESSAGE("input pd_code ...");
    PDCode pd_code;
    pd_code.InputPdCode(ss); // 输入一个 pd_code

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
    auto im2 = im.toIntMatrix2();
    auto detector = BorderDetect();
    auto detector_flag = detector.checkBorderMaxCC(last_socket_id, im2);

    // 检查布局算法是否成功
    if(!detector_flag) {
        THROW_EXCEPTION(BadBorderException, "");
    }

    // 显示所有连通分支
    if(components) {
        detector.showAllCc(im2);
        return;
    }

    SHOW_DEBUG_MESSAGE("output ans ...");
    GenNodeSetAlgo gen_node_set_algo(link_algo.getFinalGraph(), link_algo.getAllEdges());
    
    if(show_diagram) {
        im.debugOutput(std::cout, with_zero); // 输出二维布局图
    }
    
    if(show_serial) {
        gen_node_set_algo.outputGraph(std::cout); // 输出三维点坐标情况
    }

    if(show_border) { // 仅仅输出边界信息
        GetBorderSet gbs(im);
        gbs.debugOutput(std::cout); // 仅仅输出边界信息
    }
}

// 这条宏用来处理异常
// 他的功能是生成一个 catch 语句并在 catch 语句中打印错误相关的信息 (DEBUG 模式下才打印)
// 打印完信息后，执行 OTHER_CMD 中给出的语句
#define PROCESS_EXCEPTION(EXCEPTION_TYPE, OTHER_CMD) \
catch(const EXCEPTION_TYPE& re){ \
    SHOW_DEBUG_MESSAGE(re.what()); \
    OTHER_CMD; \
}

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main(int argc, char** argv) {

    // 获取所有参数
    std::vector<std::string> args;
    for(int i = 1; i < argc; i += 1) {
        args.push_back(std::string(argv[i]));
    }
    
    bool show_diagram = false; // 是否要输出一个图
    bool show_serial  = false; // 输出一个 3D 序列化
    bool with_zero    = false; // 输出图的时候是否要
    bool show_border  = false; // 是否要输出边界信息（输出边界信息的话，就不会输出图或者序列化表示）
    bool components   = false; // 是否需要输出所有的联通分支

// 用于定义所有参数信息
#define DECLARE_ARGUMENT(LONG_NAME, SHORT_NAME, VAR_NAME) if(( \
    args[i] == (LONG_NAME) || args[i] == (SHORT_NAME)) \
) { \
    (VAR_NAME) = true; \
}else

    // 处理命令行参数
    for(int i = 0; i < args.size(); i += 1) {
        DECLARE_ARGUMENT(    "--diagram", "-d", show_diagram)
        DECLARE_ARGUMENT(  "--with_zero", "-z",    with_zero)
        DECLARE_ARGUMENT(     "--serial", "-s",  show_serial)
        DECLARE_ARGUMENT(     "--border", "-b",  show_border)
        DECLARE_ARGUMENT( "--components", "-c",   components)
        { // 没有匹配时的默认处理方式
            std::cerr << "warning: undefined command line argument: " + args[i] << std::endl;
        }
    }

    // read in all content in stdin
    auto ss = readCinToStringStream();
    int max_try = 100;

    bool fail = true;
    bool suc = false;
    for(unsigned int seed = 42; seed <= 42 + max_try; seed += 1) {
        try{
            REWIND_STRING_STREAM(ss);

            // last_socket_id = -1 表示让最大编号元素在最外圈
            try_once(seed, 1, ss, show_diagram, show_serial, with_zero, show_border, components);
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
            + std::to_string(max_try) 
            + std::string(" try."));
    }
    ASSERT(suc);
    return 0;
}
#endif
