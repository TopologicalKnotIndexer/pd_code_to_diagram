#pragma once

#include <vector>
#include "AbstractGraphEngine.h"
#include "LineData.h"

class AbstractPathAlgorithm {
public:
    virtual ~AbstractPathAlgorithm(){}

    // 算法会忽略起始位置以及终止位置处的障碍物
    // 如果没有路径返回一个空的 vector
    // 如果有路径则至少返回带有一个元素的 vector
    // 我们要求返回值的 LineData.v 都是零
    // 返回值中的 double 表示当前找到最短路的距离：(直行 1.0, 原地拐弯 0.1)
    virtual 
    std::tuple<double, std::vector<LineData>>
    runAlgo(const AbstractGraphEngine& age,
        int xmin, int xmax, int ymin, int ymax, // 限制地图的范围，超出范围的地方全视为障碍物
        int xf, int yf, int xt, int yt) = 0;        // 标记起始位置和终止位置
};
