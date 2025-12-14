#pragma once

#include <iostream>
#include <set>
#include "AbstractGraphEngine.h"

// 在基本遵循原图的前提下
// 强制删除几个点，保证这几个点必须返回零，其他点保持不变
class ErasePointGraphEngineWrap : public AbstractGraphEngine {
private:
    const AbstractGraphEngine& raw_age;
    std::set<std::tuple<int, int>> force_empty_pos;

public:
    virtual ~ErasePointGraphEngineWrap(){}
    ErasePointGraphEngineWrap(const AbstractGraphEngine& _raw_age): raw_age(_raw_age) {}

    void clear() {
        force_empty_pos.clear(); // 清除所有指定点
    }
    void addEmptyPos(int x, int y) {
        force_empty_pos.insert(std::make_tuple(x, y)); // 强制要求某个点是空的
    }

    // 获取所有小于零的位置的坐标
    virtual std::vector<std::tuple<int, int>> getAllNegPos() const override {
        auto pos_list = raw_age.getAllNegPos();
        std::vector<std::tuple<int, int>> new_pos_list; // 删除一些点
        for(auto item: pos_list) {
            if(force_empty_pos.count(item) == 0) {
                new_pos_list.push_back(item);
            }
        }
        return new_pos_list;
    }

    virtual int getPos(int x, int y) const override {
        if(force_empty_pos.size() != 3 && force_empty_pos.size() != 4) {
            std::cerr << "warning: in ErasePointGraphEngineWrap, force_empty_pos.size() != 3 or 4" << std::endl;
            assert(false);
        }

        // 强制清零
        if(force_empty_pos.count(std::make_tuple(x, y)) > 0) {
            return 0;
        }else {
            // 没有强制清零，所以可以访问原来的数据值
            return raw_age.getPos(x, y);
        }
    }

    // 这里我们并不考虑清零对数据范围的影响，直接返回原来的尺寸
    virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        return raw_age.getBorderCoord();
    }

    // 不允许设置一个位置的值
    virtual void setPos(int x, int y, int v) override {
        std::cerr << "error: can not setPos for ErasePointGraphEngineWrap" << std::endl;
        assert(false);
    }
};
