#pragma once

#include "AbstractGraph.h"
#include "../../Utils/MyAssert.h"

class Graph: public AbstractGraph {
protected:
    int maxNodeId;
    std::vector<std::vector<int>> nextNode; // 邻接表

public:
    virtual ~Graph() {}

    Graph(): maxNodeId(0) {
        nextNode.push_back(std::vector<int>());
    }
    Graph(int _maxNodeCnt): maxNodeId(0) {
        nextNode.push_back(std::vector<int>());
        setMaxNodeId(_maxNodeCnt);
    }
    
    // 获取最大节点编号
    // 节点编号大于等于 1
    virtual int getMaxNodeId() const override {
        return maxNodeId;
    }

    // 判断某个节点编号是否存在
    virtual bool checkHasNode(int nodeId) const override {
        return 1 <= nodeId && nodeId <= maxNodeId;
    }

    // 提高最大节点编号
    virtual void setMaxNodeId(int newMaxNodeId) {
        while(maxNodeId < newMaxNodeId) {
            maxNodeId += 1;
            nextNode.push_back({}); // 空序列
        }
    }

    // 新增一个双向边
    virtual void addEdge(int f, int t) {
        setMaxNodeId(f);
        setMaxNodeId(t);
        ASSERT(checkHasNode(f)); // 排除小于零的情况
        ASSERT(checkHasNode(t));
        nextNode[f].push_back(t); // 双向加边
        nextNode[t].push_back(f);
    }

    // 找到某个节点下一步能走到的所有节点
    virtual std::vector<int> getNextNode(int nodeId) const override {
        ASSERT(checkHasNode(nodeId));
        return nextNode[nodeId];
    }
};
