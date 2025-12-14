#pragma once

#include <iostream>
#include "AbstractGraphEngine.h"

// MergeGraphEngineWrap 用于合并两个抽象图引擎
// 但是他合并后得到的抽象图引擎并不允许修改
class MergeGraphEngineWrap: public AbstractGraphEngine {
private:
    const AbstractGraphEngine& age_front;
    const AbstractGraphEngine& age_next;

public:
    virtual ~MergeGraphEngineWrap(){}

    MergeGraphEngineWrap(
        const AbstractGraphEngine& _age_front, const AbstractGraphEngine& _age_next):
        age_front(_age_front), age_next(_age_next) {}
    
    // 优先使用 age_front 中的元素，除非 age_front 中没有指定其中的任何值
    virtual int getPos(int x, int y) const override {
        auto front_val = age_front.getPos(x, y);
        return front_val == 0 ? age_next.getPos(x, y) : front_val;
    }

    // 获取 xmin, xmax, ymin, ymax
    virtual std::tuple<int, int, int, int> getBorderCoord() const override {

        // 先获得 front 的坐标范围
        auto tuple_4int = age_front.getBorderCoord();
        int xmin_front = std::get<0>(tuple_4int);
        int xmax_front = std::get<1>(tuple_4int);
        int ymin_front = std::get<2>(tuple_4int);
        int ymax_front = std::get<3>(tuple_4int);
        
        // 再获得 next 的坐标范围
        auto tuple_4int2 = age_next.getBorderCoord();
        int xmin_next = std::get<0>(tuple_4int2);
        int xmax_next = std::get<1>(tuple_4int2);
        int ymin_next = std::get<2>(tuple_4int2);
        int ymax_next = std::get<3>(tuple_4int2);

        return std::make_tuple(
            std::min(xmin_front, xmin_next),
            std::max(xmax_front, xmax_next),
            std::min(ymin_front, ymin_next),
            std::max(ymax_front, ymax_next));
    }

    // 设置一个位置的值
    virtual void setPos(int x, int y, int v) override {
        std::cerr << "error: can not setPos for MergeGraphEngineWrap" << std::endl;
        assert(false);
    }
};
