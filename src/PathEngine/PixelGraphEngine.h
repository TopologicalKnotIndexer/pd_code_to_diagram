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
    virtual int getPos(int x, int y) const override {
        auto posNow = std::make_tuple(x, y);
        if(pixelValue.find(posNow) != pixelValue.end()) {
            return pixelValue.find(posNow) -> second;
        }
        return 0; // 零表示这个位置是空气
    };

    // 设置一个位置的值
    void setPos(int x, int y, int v) {
        pixelValue[std::make_tuple(x, y)] = v;
    }

    // 设置一条线段上所有元素的值，但是这条线段必须和轴平行
    // 否则这段代码会报错
    void setLine(const LineData& lineData) {
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
};
