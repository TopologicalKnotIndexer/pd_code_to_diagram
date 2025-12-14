#pragma once

#include <iostream>

#include "AbstractGraphEngine.h"

// MarginGraphEngineWrap 的用途是为一个地图提供边界
// 边界外的地方都被视为同一个值
// 这种做法可以避免地图无穷大带来的不收敛问题
class MarginGraphEngineWrap: public AbstractGraphEngine{
private:
    int xmin, xmax, ymin, ymax;
    int xf, yf, xt, yt; // 记录其实位置和终止位置
    int rawVal;
    const AbstractGraphEngine& age;

public:
    virtual ~MarginGraphEngineWrap(){}
    MarginGraphEngineWrap(const AbstractGraphEngine& _age, 
        int _xmin, int _xmax, int _ymin, int _ymax, int _rawVal,
        int _xf, int _yf, int _xt, int _yt
    ): age(_age) {

        assert(_xmin <= _xmax && _ymin <= _ymax);
        assert(_rawVal != 0); // 空气不能作为外界的值
        xmin = _xmin;
        xmax = _xmax;
        ymin = _ymin;
        ymax = _ymax;
        rawVal = _rawVal;

        // 记录起始位置和终止位置
        xf = _xf;
        yf = _yf;
        xt = _xt;
        yt = _yt;
    }

    virtual std::vector<std::tuple<int, int>> getAllNegPos() const override {
        assert(rawVal >= 0); // 如果 rawVal < 0 则有无穷个负数值，不能这么做
        auto pos_list = age.getAllNegPos();
        std::vector<std::tuple<int, int>> new_pos_list;
        for(auto item: pos_list) {
            int x, y;
            std::tie(x, y) = item;
            if(xmin <= x && x <= xmax && ymin <= y && y <= ymax) {
                new_pos_list.push_back(item);
            }
        }
        return new_pos_list;
    }

    // 可以获得一个位置的值是多少
    // 一般来说认为 0 是空气，其他数值是障碍物
    virtual int getPos(int x, int y) const override {
        if(x == xf && y == yf) { // 起始位置永远视为空气
            return 0;
        }else
        if(x == xt && y == yt) { // 终止位置永远视为空气
            return 0;
        }else
        if(x < xmin || x > xmax || y < ymin || y > ymax) {
            return rawVal;
        }
        return age.getPos(x, y);
    }

    virtual void setPos(int x, int y, int v) override {
        std::cerr << "error: can not setPos for GraphEngineWrap" << std::endl;
        assert(false);
    }

    virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        return age.getBorderCoord();
    }
};
