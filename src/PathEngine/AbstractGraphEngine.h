#pragma once

#include <iomanip>
#include <tuple>
#include "LineData.h"

// 抽象地图引擎
class AbstractGraphEngine {
public:
    virtual ~AbstractGraphEngine(){}

    // 可以获得一个位置的值是多少
    // 一般来说认为 0 是空气，其他数值是障碍物
    virtual int getPos(int x, int y) const = 0;

    // 计算 xmin, xmax, ymin, ymax
    virtual std::tuple<int, int, int, int> getBorderCoord() const = 0;

    // 设置一个位置的值
    virtual  void setPos(int x, int y, int v) = 0;
    
    // 设置一条线段上所有元素的值，但是这条线段必须和轴平行
    // 否则这段代码会报错
    virtual void setLine(const LineData& lineData) {
        int xf = lineData.getXf();
        int xt = lineData.getXt();
        int yf = lineData.getYf();
        int yt = lineData.getYt();
        int  v = lineData.getV ();
        assert(xf == xt || yf == yt);
        if(xf == xt) {
            for(int i = std::min(yf, yt); i <= std::max(yf, yt); i += 1) {
                setPos(xf, i, v);
            }
        }else { // yf == yt
            for(int i = std::min(xf, xt); i <= std::max(xf, xt); i += 1) {
                setPos(i, yf, v);
            }
        }
    }

    // 输出一个值域矩阵
    virtual void debugOutput() const {
        int xmin, xmax, ymin, ymax;
        std::tie(xmin, xmax, ymin, ymax) = getBorderCoord();

        for(int i = xmin; i <= xmax; i += 1) {
            for(int j = ymin; j <= ymax; j += 1) {
                if(getPos(i, j) != 0) {
                    std::cout << std::setw(3) << std::setfill(' ') << getPos(i, j) << " ";
                }else {
                    std::cout << "    ";
                }
            }
            std::cout << std::endl;
        }
    }
};
