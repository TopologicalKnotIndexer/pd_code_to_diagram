#pragma once

#include <iostream>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include "AbstractGraph.h"

// 给定一个图计算所有联通分量
class ConnectedComponents {
protected:
    int* fa; // union find set
    int maxNodeId = 0;

    int find(int x) {
        if(x == fa[x]) return x;
        return fa[x] = find(fa[x]);
    }

    void link(int x, int y) {
        int rx = find(x);
        int ry = find(y);
        if(rx != ry) {
            if(rand() & 1) std::swap(rx, ry);
            fa[rx] = ry;
        }
    }

    // 计算整个图的所有连通分支
    void construct_all(const AbstractGraph& ag) {
        for(int i = 1; i <= ag.getMaxNodeId(); i += 1) {
            for(auto nxt: ag.getNextNode(i)) {
                link(i, nxt);
            }
        }
    }

public:
    virtual ~ConnectedComponents() {
        delete[] fa;
    }
    ConnectedComponents(const AbstractGraph& ag): maxNodeId(ag.getMaxNodeId()) {
        fa = new int[ag.getMaxNodeId() + 1];
        for(int i = 1; i <= ag.getMaxNodeId(); i += 1) {
            fa[i] = i;
        }
        construct_all(ag);
    }

    virtual std::vector<std::set<int>> getConnectedComponents() {

        // 初始化集合
        auto ans = std::vector<std::set<int>>();
        while((int)ans.size() - 1 < maxNodeId) {
            ans.push_back(std::set<int>());
        }

        // 枚举所有对象，加入对应的等价类
        for(int i = 1; i <= maxNodeId; i += 1) {
            int root_i = find(i);
            ans[root_i].insert(i);
        }

        // 将所有非空对象整理出来
        auto non_empty = std::vector<std::set<int>>();
        for(int i = 1; i <= maxNodeId; i += 1) {
            if(ans[i].size() > 0) {
                non_empty.push_back(ans[i]);
            }
        }
        return non_empty;
    }
};
