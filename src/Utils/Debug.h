#pragma once

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
