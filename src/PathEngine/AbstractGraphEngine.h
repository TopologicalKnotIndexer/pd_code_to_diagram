#pragma once

// 抽象地图引擎
class AbstractGraphEngine {
public:

    // 可以获得一个位置的值是多少
    // 一般来说认为 0 是空气，其他数值是障碍物
    virtual int getPos(int x, int y) const = 0;
};
