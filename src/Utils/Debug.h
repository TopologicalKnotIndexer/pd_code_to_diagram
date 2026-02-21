#pragma once

#include <algorithm>  // 用于 std::all_of
#include <cctype>     // 用于 isdigit (注意：需处理符号位问题)
#include <iostream>
#include <map>
#include <string>

template<typename _K, typename _V>
void debugMap(std::string map_name, const std::map<_K, _V>& kv_map) {
    std::cout << map_name << ": " << std::endl;
    for(const auto& k_v: kv_map) {
        std::cout << k_v.first << ": " << k_v.second << std::endl;
    }
}

// 显示针对特定 DEBUG_FLAG 的调试信息
#define SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG_FLAG, MSG) { \
    if(DEBUG_FLAG) { \
        std::cerr << MSG << std::endl; \
    } \
}

// 显示指定的调试信息
#define SHOW_DEBUG_MESSAGE(MSG) { \
    SHOW_CERTAIN_DEBUG_MESSAGE(DEBUG, MSG) \
}

// 判断一个字符串是否完全由数字构成
bool isAllDigits(const std::string& str) {
    if (str.empty()) { // 空字符串直接返回false
        return false;
    }

    // std::all_of 检查区间内所有元素是否满足条件
    // isdigit需要强转unsigned char，避免处理负数ASCII值时的未定义行为
    return std::all_of(str.begin(), str.end(), 
        [](unsigned char c) { return std::isdigit(c); });
}
