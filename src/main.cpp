// 本程序的功能：
// 输入一个扭结 pd_code，输出平面布局后的扭结图

// 解除注释（或者在编译时引入 -DNO_MAIN） 以避免 main 函数参加编译
// 这个功能用于编译 .so 或者 .dll 文件
//#define NO_MAIN 

#include <cassert>
#include <cmath>
#include <iostream>

#include "PDCode.h"
#include "Random.h"

int testRandomTime(int n) {
    const int MAXN = 200;
    assert(0 <= n && n <= MAXN);

    int num_exists[MAXN + 1] = {};
    int cnt = 0, run_time = 0;
    while(cnt < n) {
        run_time += 1;
        int new_val = randomInt(1, n);

        if(num_exists[new_val] == 0) { // 出现了一个新的数值
            num_exists[new_val] = 1;
            cnt += 1;
        }
    }
    return run_time;
}

void getMuAndSigma(std::vector<int> vec, double& mu, double& sigma) {
    mu = 0;
    sigma = 0;
    for(auto v: vec) {
        mu += (double)v;
        sigma += (double)v*v;
    }
    mu /= vec.size(); // 平均数
    sigma /= vec.size(); // 平方平均数
    sigma -= mu*mu;
    sigma = sqrt(sigma); // 标准差
}

#ifndef NO_MAIN // 如果 NO_MAIN 标志存在，则不编译 main 函数
int main() {

    // 测试 pd_code 的输入和序列化
    // PDCode pd_code;
    // pd_code.InputPdCode(std::cin);
    // std::cout << pd_code.toString() << std::endl;

    // 测试随机数生成器
    
    for(int n = 1; n <= 200; n += 1) {
        std::vector<int> run_time_list;
        for(int i = 1; i <= 100000; i += 1) {
            run_time_list.push_back(testRandomTime(n));
        }

        double mu, sigma;
        getMuAndSigma(run_time_list, mu, sigma);
        std::cout << "n: " << n << " f: " << mu << " g: " << sigma << std::endl;
    }
    return 0;
}
#endif
