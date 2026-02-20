#pragma once

#include <set>
#include <vector>

// 一个只读的抽象图
class AbstractGraph {
public:
    virtual ~AbstractGraph() {}

    // 获取最大节点编号
    // 节点编号大于等于 1
    virtual int getMaxNodeId() const = 0;

    // 判断某个节点编号是否存在
    virtual bool checkHasNode(int nodeId) const = 0;

    // 找到某个节点下一步能走到的所有节点
    virtual std::vector<int> getNextNode(int nodeId) const = 0;
};
