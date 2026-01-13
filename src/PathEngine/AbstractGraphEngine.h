#pragma once

#include <iomanip>
#include <tuple>
#include <vector>
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
    virtual void setPos(int x, int y, int v) = 0;

    // 获取所有小于零的位置的坐标
    virtual std::vector<std::tuple<int, int>> getAllNegPos() const = 0;
    
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

    virtual void outputValue(int val) const {
        std::cout << std::setw(3) << std::setfill(' ') << val << " ";
    }

    // 输出一个值域矩阵
    virtual void debugOutput(std::ostream& out, bool with_zero) const {
        int xmin, xmax, ymin, ymax;
        std::tie(xmin, xmax, ymin, ymax) = getBorderCoord();
        xmin -= 1; // 拓宽边界
        xmax += 1;
        ymin -= 1;
        ymax += 1;

        for(int i = xmin; i <= xmax; i += 1) {
            for(int j = ymin; j <= ymax; j += 1) {
                if(getPos(i, j) != 0) {
                    outputValue(getPos(i, j));
                }else {
                    if(with_zero) {
                        outputValue(0); // with_zero 模式下会输出边界的 0
                    }else {
                        out << "    ";
                    }
                }
            }
            out << std::endl;
        }
    }
};
