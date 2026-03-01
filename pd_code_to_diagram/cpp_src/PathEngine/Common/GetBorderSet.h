#pragma once

#include <iostream>
#include <queue>
#include <set>
#include <tuple>

#include "IntMatrix.h"
#include "AbstractIntMatrix.h"
#include "../../Utils/MyAssert.h"

class GetBorderSet {
private:
    std::set<int> border_set; // 记录所有在边界上的整数集合

public:
    std::set<int> getAns() const { // 返回答案
        return border_set;
    }

    void debugOutput(std::ostream& out) const { // 输出所有边界元素
        for(auto v: border_set) {
            out << v << " ";
        }
        out << std::endl;
    }

    GetBorderSet(const AbstractIntMatrix& aim) {
        IntMatrix vis(aim.getRowCnt(), aim.getColCnt());

        std::queue<std::tuple<int, int> > que; // BFS 算法

        vis.setPos(0, 0, 1); // 记录已经访问过了
        que.push(std::make_tuple(0, 0));
        ASSERT(aim.getPos(0, 0) == 0); // 左上角的位置必须是零，因为我们要求有一圈零边界

        // 位移矩阵
        static const int dx[] = {0, 0, 1,-1};
        static const int dy[] = {1,-1, 0, 0};

        // BFS 直到队列为空
        while(!que.empty()) {
            int posx, posy;
            std::tie(posx, posy) = que.front(); que.pop(); // 弹出第一个位置

            for(int d = 0; d < 4; d += 1) {
                int newx = posx + dx[d];
                int newy = posy + dy[d];
                if(0 <= newx && newx < aim.getRowCnt() && 0 <= newy && newy < aim.getColCnt()) { // 只遍历地图内的
                    if(aim.getPos(newx, newy) != 0) {
                        continue; // 跳过障碍物
                    }
                    if(vis.getPos(newx, newy) != 0) continue; // 跳过已经访问过的位置

                    // 访问这个位置
                    vis.setPos(newx, newy, 1);
                    que.push(std::make_tuple(newx, newy));
                }
            }
        }

        // 记录边界元素（四联通边界）
        for(int i = 0; i < aim.getRowCnt(); i += 1) {
            for(int j = 0; j < aim.getColCnt(); j += 1) {
                if(aim.getPos(i, j) == 0) continue; // 跳过非零位置

                int vcnt = 0; // 统计自己周围有哪些 vis 过的元素
                for(int d = 0; d < 4; d += 1) {
                    int x = i + dx[d];
                    int y = j + dy[d];
                    if(vis.getPos(x, y) != 0) { // 周围找到了被访问的元素
                        vcnt += 1;
                    }
                }
                if(vcnt > 0) {
                    border_set.insert(aim.getPos(i, j)); // 记录答案
                }
            }
        }
    }

};