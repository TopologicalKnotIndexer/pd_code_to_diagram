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

// 安全获取stringstream全部内容且不改变其状态的函数
std::string getStreamContentWithoutChange(std::stringstream& ss) {
    // 1. 记录当前流的读写位置（核心：保存状态）
    std::ios::pos_type originalPos = ss.tellg();
    
    // 2. 将流的读取位置重置到起始处
    ss.seekg(0, std::ios::beg);
    
    // 3. 读取全部内容
    std::string content((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
    
    // 4. 恢复流的原始读写位置（核心：还原状态）
    ss.seekg(originalPos);
    
    return content;
}

// 将 cin 中的所有内容读取到 stringstream 中
// 返回值：填充了 cin 内容的 stringstream 对象
// 异常：读取失败时抛出 runtime_error 异常
std::stringstream readCinToStringStream() {
    std::stringstream ss;
    std::string line;

    try {
        // 逐行读取 cin 所有内容，直到 EOF（结束符）
        // Windows 下按 Ctrl+Z 回车触发 EOF，Linux/Mac 下按 Ctrl+D 触发 EOF
        while (std::getline(std::cin, line)) {
            // 将读取到的行写入 stringstream，保留换行符（还原原始输入格式）
            ss << line << '\n';
        }

        // 检查读取过程是否出现错误（非 EOF 导致的失败）
        if (std::cin.bad()) {
            throw std::runtime_error("cin 读取过程中发生严重错误");
        }

        // 重置 stringstream 的读取位置到开头（方便后续直接使用）
        ss.clear(); // 清除可能的 eofbit
        ss.seekg(0, std::ios::beg);
    }
    catch (const std::exception& e) {
        // 捕获并包装异常，让调用方更清晰地知道错误来源
        throw std::runtime_error("cin error：" + std::string(e.what()));
    }

    return ss;
}

void try_once(unsigned int seed, std::stringstream& ss, bool show_diagram, bool show_serial, bool with_zero, bool show_border) {
    myrandom::setSeed(seed);

    if(DEBUG) std::cerr << "input pd_code ..." << std::endl;
    PDCode pd_code;
    pd_code.InputPdCode(ss); // 输入一个 pd_code

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
    auto im = link_algo.getFinalGraph().exportToIntMatrix();

    // 检查最大编号所在的连通分支是否在最外圈
    auto detector = BorderDetect();
    ASSERT(detector.checkBorderMaxCC(im.toIntMatrix2()));

    if(DEBUG) std::cerr << "output ans ..." << std::endl;
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
    for(int i = 0; i < args.size(); i += 1) {
        if(args[i] == "--diagram" || args[i] == "-d") {
            show_diagram = true;
        }
        else if(args[i] == "--with_zero" || args[i] == "-z") {
            with_zero = true;
        }
        else if(args[i] == "--serial" || args[i] == "-s") {
            show_serial = true;
        }
        else if(args[i] == "--border" || args[i] == "-b") {
            show_border = true;
        }else {
            std::cerr << "warning: undefined command line argument: " + args[i] << std::endl;
        }
    }

    // read in all content
    auto ss = readCinToStringStream();
    int max_try = 100;

    bool fail = true;
    bool suc = false;
    for(unsigned int seed = 42; seed <= 42 + max_try; seed += 1) {
        try{
            ss.clear(); // 清除可能的 eofbit
            ss.seekg(0, std::ios::beg);
            
            if(DEBUG) {
                auto ss_content = getStreamContentWithoutChange(ss);
                std::cerr << "INPUT_BEGIN{" << ss_content << "}INPUT_END" << std::endl;
            }

            try_once(seed, ss, show_diagram, show_serial, with_zero, show_border);
            fail = false; // 没有失败
            suc = true;   // 成功了
        }catch(const std::runtime_error& re){ // 截获异常，失败
            fail = true;
            if(DEBUG) {
                std::cerr << "seed: " << seed << ", try_once failed" << std::endl;
                std::cerr << re.what() << std::endl;
            }
        }
        if(!fail) { //  如果没失败就退出
            break;
        }
    }

    // 如果没有成功，则抛出异常
    if(!suc) { 
        if(DEBUG) {
            std::cerr << "failed after " << max_try << " try." << std::endl;
        }
    }
    ASSERT(suc);
    return 0;
}
#endif
