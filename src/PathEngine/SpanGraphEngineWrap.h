#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include "AbstractGraphEngine.h"

class SpanGraphEngineWrap: public AbstractGraphEngine {
private:
    const AbstractGraphEngine& raw_age;

public:
    virtual ~SpanGraphEngineWrap(){}
    SpanGraphEngineWrap(const AbstractGraphEngine& _raw_age): raw_age(_raw_age) {}

    virtual int getPos(int x, int y) const override {
        const int dx[] = {0, 1, 0,-1};
        const int dy[] = {1, 0,-1, 0};

        // 把所有非零值放到这个 list 里
        std::vector<int> non_zero_value_list;
        if(raw_age.getPos(x, y) != 0) { // 先考虑当前位置
            non_zero_value_list.push_back(raw_age.getPos(x, y));
        }

        for(int d = 0; d < 4; d += 1) { // 枚举所有方向
            int xpos = x + dx[d];
            int ypos = y + dy[d];
            if(raw_age.getPos(xpos, ypos) != 0) {
                non_zero_value_list.push_back(raw_age.getPos(xpos, ypos));
            }
        }

        // 如果附近没有任何非零元素，返回 0
        if(non_zero_value_list.size() == 0) return 0;
        else {

            // 否则返回所有非零元素的最大值
            return *std::max_element(non_zero_value_list.begin(), non_zero_value_list.end());
        }
    }

    virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        int xmin, xmax, ymin, ymax;
        std::tie(xmin, xmax, ymin, ymax) = raw_age.getBorderCoord();

        // 非零元素总是至多向边界拓展 1
        return std::make_tuple(xmin - 1, xmax + 1, ymin - 1, ymax + 1);
    }

    // 不允许设置一个位置的值
    virtual void setPos(int x, int y, int v) override {
        std::cerr << "error: can not setPos for SpanGraphEngineWrap" << std::endl;
        assert(false);
    }
};
