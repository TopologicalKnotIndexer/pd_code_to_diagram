#pragma once

#include <algorithm>
#include <cassert>
#include <set>
#include <vector>

// 描述一个整数集合
class IntegerSet {
private:
    int sorted = false; // 默认是没有排序过的
    std::set<int> valSet; // 记录所有出现过的数值
    std::vector<int> valVec; // 记录所有出现过的数值，valSet 用于给 valVec 去重

public:

    // 向集合中插入元素
    void addInt(int v) {
        if(valSet.count(v) == 0) {
            valSet.insert(v);
            valVec.push_back(v);
            sorted = false; // 新插入过元素的序列总是无序的
        }
        // 如果这个元素已经存在，则什么都不做
    }

    // 给定一个元素计算这个元素当前的排名
    // 排名从 0 开始，并且大于等于 0, 不同的数的排名一定不同
    int rank(int v) {
        assert(valSet.count(v) >= 1); // 这个元素必须存在
        if(!sorted) {
            sort(valVec.begin(), valVec.end());
            sorted = true; // 已经排序过了
        }
        assert(sorted);
        return std::lower_bound(valVec.begin(), valVec.end(), v) - valVec.begin();
    }
};
