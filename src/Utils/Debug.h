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
