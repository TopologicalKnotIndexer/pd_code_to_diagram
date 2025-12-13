// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
//#define NO_MAIN 

#include <iostream>

#include "PathEngine/SpfaPathEngine.h"
#include "PathEngine/PixelGraphEngine.h"

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main() {
    PixelGraphEngine pge;

    int n, m;
    std::cin >> n >> m;
    for(int i = 0; i < n; i += 1) {
        for(int j = 0; j < m; j += 1) {
            int x; std::cin >> x;
            if(x != 0) pge.setPos(i, j, x);
        }
    }

    SpfaPathEngine spe;
    auto ans = spe.runAlgo(pge, 0, n-1, 0, m-1, 0, 0, n-1, m-1);

    std::cout << "minDis: " << std::get<0>(ans) << std::endl;

    for(auto data: std::get<1>(ans)) {
        std::cout << "(" << data.getXf() <<  ", " << data.getYf() << ")" << " -> "
            << "(" << data.getXt() <<  ", " << data.getYt() << ")" << std::endl;
    }
    return 0;
}
#endif
