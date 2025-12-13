#pragma once

#include <algorithm>
#include <cassert>
#include <map>
#include <tuple>

#include "AbstractGraphEngine.h"
#include "LineData.h"

class PixelGraphEngine: public AbstractGraphEngine {
private:
    std::map<std::tuple<int, int>, int> pixelValue;

public:
    inline virtual ~PixelGraphEngine(){}

    static constexpr int INT_INF = 0x7fffffff;

    inline virtual int getPos(int x, int y) const override {
        auto posNow = std::make_tuple(x, y);
        if(pixelValue.find(posNow) != pixelValue.end()) {
            return pixelValue.find(posNow) -> second;
        }
        return 0; // 零表示这个位置是空气
    };

    // 计算 xmin, xmax, ymin, ymax
    // 当前这个函数的计算是暴力计算的，所以会比较慢，尽量少用
    // 所有统计都只统计非零元素的坐标，零元素的坐标不考虑
    inline virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        int xmin = +INT_INF;
        int xmax = -INT_INF;
        int ymin = +INT_INF;
        int ymax = -INT_INF;

        for(const auto& pr: pixelValue) { 
            int xnow = std::get<0>(pr.first);
            int ynow = std::get<1>(pr.first);
            int vnow = pr.second;

            if(vnow != 0) {
                xmin = std::min(xnow, xmin);
                xmax = std::max(xnow, xmax);
                ymin = std::min(ynow, ymin);
                ymax = std::max(ynow, ymax);
            }
        }
        return std::make_tuple(xmin, xmax, ymin, ymax);
    }

    // 设置一个位置的值
    virtual void setPos(int x, int y, int v) override {
        if(v != 0) {
            pixelValue[std::make_tuple(x, y)] = v;
        }else {

            // v = 0 表示清空一个位置，如果这个位置原来有值，就清空它
            if(pixelValue.find(std::make_tuple(x, y)) != pixelValue.end()) {
                pixelValue.erase(std::make_tuple(x, y));
            }
        }
    }

};
