#pragma once

// 记录四个方向，并且保证这四个方向按照逆时针顺序排列
// 不可以修改这四个方向的编号
enum class Direction {
    EAST = 0,
    NORTH = 1,
    WEST = 2,
    SOUTH = 3
};

// 为 Direction 类型提供哈希函数
namespace std {
    template<> struct hash<Direction> {
        size_t operator()(const Direction& d) const {
            return hash<int>()(static_cast<int>(d));
        }
    };
}