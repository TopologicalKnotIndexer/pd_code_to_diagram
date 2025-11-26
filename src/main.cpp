// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
//#define NO_MAIN 

#include <iostream>

#include "PDCode.h"

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main() {

    PDCode pd_code;
    pd_code.InputPdCode(std::cin);

    std::cout << pd_code.toString() << std::endl;
    return 0;
}
#endif
