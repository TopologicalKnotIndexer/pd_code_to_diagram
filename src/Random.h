#pragma once

#include <chrono>
#include <random>
#include <stdexcept>

// 生成 [L, R] 区间的 int 类型随机数
int randomInt(int L, int R) {
    // 输入合法性检查
    if (L > R) {
        throw std::invalid_argument("L must be <= R");
    }

    // 静态引擎：仅初始化一次（避免频繁创建导致种子重复）
    static std::mt19937 engine([]() {
        // 跨平台种子：优先高质量随机种子， fallback 时间种子
        std::random_device rd;
        return rd.entropy() > 0.0 ? rd() : static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }());

    // 均匀整数分布：严格 [L, R] 闭区间
    static std::uniform_int_distribution<int> dist;
    dist.param(std::uniform_int_distribution<int>::param_type(L, R)); // 动态设置区间

    return dist(engine);
}
