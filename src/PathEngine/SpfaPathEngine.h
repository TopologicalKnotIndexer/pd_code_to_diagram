#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <tuple>
#include <queue>

#include "../Utils/Direction.h"
#include "../Utils/Coord2dPosition.h" // 这里有方向和坐标位移的对应关系

#include "AbstractGraphEngine.h"
#include "AbstractPathAlgorithm.h"
#include "MarginGraphEngineWrap.h"

class SpfaPathEngine: public AbstractPathAlgorithm {
private:
    using PosType = std::tuple<int, int, Direction>; 
    std::map<PosType,  double> dis; // 描述初始位置出发后走了多少距离
    std::map<PosType, PosType> pre; // 描述最优前驱
    std::set<PosType> vis;          // 描述一个元素在不在队列里

    using NxtInfo = std::tuple<int, int, Direction, double>;
    inline std::vector<NxtInfo> getNextPos(PosType pos_at) const {
        std::vector<NxtInfo> ans;

        auto xnow = std::get<0>(pos_at);
        auto ynow = std::get<1>(pos_at);
        auto dnow = std::get<2>(pos_at);

        auto coord2d = Coord2dPosition::getDeltaPositionByDirection(dnow);
        auto dx = (int)std::round(coord2d.getX()); // coord2d.getX() 得到的是 double 类型
        auto dy = (int)std::round(coord2d.getY());

        // 前进一个单位距离
        // 前进一格需要花费 1.0 的代价
        ans.push_back(std::make_tuple(xnow + dx, ynow + dy, dnow, 1.0));

        // 考虑转向
        // 转向需要花费 0.1 的代价
        for(auto dnxt: {Direction::EAST, Direction::NORTH, Direction::WEST, Direction::SOUTH}) {
            if(dnxt == dnow) continue;
            ans.push_back(std::make_tuple(xnow, ynow, dnxt, 0.1));
        }
        return ans;
    }

    // 根据路径上经过的点信息合并出折线段
    inline std::vector<LineData> getVecLineData(std::vector<PosType> path) const {
        std::vector<LineData> ans;
        for(int i = 0; i < path.size(); i += 1) {
            auto xnow = std::get<0>(path[i]);
            auto ynow = std::get<1>(path[i]);
            if(i == 0 || std::get<2>(path[i-1]) != std::get<2>(path[i])) { // 方向出现变化
                ans.push_back(LineData(xnow, xnow, ynow, ynow, 0));        // 设置新元素
            }else { // 方向没有变化
                ans[ans.size() - 1] = ans[ans.size() - 1].setAimPos(xnow, ynow);
            }
        }
        return ans;
    }
public:
    inline virtual ~SpfaPathEngine(){}

    inline virtual 
    std::tuple<double, std::vector<LineData>>
    runAlgo(const AbstractGraphEngine& age,
        int xmin, int xmax, int ymin, int ymax,    // 限制地图的范围，超出范围的地方全视为障碍物
        int xf, int yf, int xt, int yt) override { // 标记起始位置和终止位置

        // 清空距离数据结构
        dis.clear();
        vis.clear();

        // 起始位置和终止位置重合，直接就能走到，返回即可
        if(xf == xt && yf == yt) {
            return std::make_tuple(
                0.0,
                std::vector<LineData>({LineData(xf, xt, yf, yt, 0)})
            );
        }
        
        // 构建一个新的 AbstractGraphEngine 用于限制活动范围
        // 同时将起点和终点的位置的值，强制归零
        auto gew = MarginGraphEngineWrap(age, xmin, xmax, ymin, ymax, -1, xf, yf, xt, yt);
        
        // q 记录所有已经在 dis 中出现但还没有进行拓展的节点
        std::queue<PosType> q;
        for(auto dir: {Direction::EAST, Direction::NORTH, Direction::WEST, Direction::SOUTH}) {
            auto pos_at = std::make_tuple(xf, yf, dir);
            dis[pos_at] = 0;
            vis.insert(pos_at); // vis 记录在队列中存在的元素
            q.push(pos_at);
        }

        while(!q.empty()) { // 使用 SPFA 跑遍全图
            auto pos_at = q.front(); q.pop();
            vis.erase(pos_at);

            // 获取当前状态信息
            auto xnow = std::get<0>(pos_at);
            auto ynow = std::get<1>(pos_at);
            auto dnow = std::get<2>(pos_at);
            auto distance_now = dis.find(pos_at) -> second;

            // x_y_d_v 是一个四元组，分别表示：x, y, 新的朝向, 与当前节点的距离
            for(auto x_y_d_v: getNextPos(pos_at))
            {
                auto xnxt = std::get<0>(x_y_d_v);
                auto ynxt = std::get<1>(x_y_d_v);
                auto dnxt = std::get<2>(x_y_d_v);
                auto vnxt = std::get<3>(x_y_d_v); // 表示与当前状态之间的惩罚
                if(gew.getPos(xnxt, ynxt) != 0) { // 被障碍物阻拦了
                    continue;
                }
                auto pos_nxt = std::make_tuple(xnxt, ynxt, dnxt);
                if(dis.find(pos_nxt) == dis.end() || dis[pos_nxt] > distance_now + vnxt) {
                    // 可以更新距离
                    dis[pos_nxt] = distance_now + vnxt;
                    pre[pos_nxt] = pos_at;
                    if(vis.count(pos_nxt) == 0) { // 如果不在队列里
                        vis.insert(pos_nxt);      // 把他放到队列里
                        q.push(pos_nxt);
                    }
                }
            }
        }

        double dis_now = std::numeric_limits<double>::infinity(); // 正无穷
        PosType pos_now = std::make_tuple(xt, yt, Direction::EAST);
        for(auto dir: {Direction::EAST, Direction::NORTH, Direction::WEST, Direction::SOUTH}) {
            PosType pos = std::make_tuple(xt, yt, dir);
            if(dis.find(pos) != dis.end()) {            // 位置可达
                if(dis.find(pos) -> second < dis_now) { // 更新最优位置
                    dis_now = dis.find(pos) -> second;
                    pos_now = pos;
                }
            }
        }

        if(std::isinf(dis_now)) { // 说明没有可行解
            return {};            // 没有可行解应该返回空 list
        }

        // 得到路径上经过的所有点
        auto old_pos_now = pos_now; // 在算法执行前备份 pos_now
        std::vector<PosType> arr;
        while(true) {
            arr.push_back(pos_now);
            if(pre.find(pos_now) == pre.end()) { // 没有前驱了
                break;
            }else {
                pos_now = pre[pos_now]; // 获得前驱节点
            }
        }
        std::reverse(arr.begin(), arr.end()); // 反转这个序列
        assert(arr.size() >= 1);
        return std::make_tuple(
            dis[old_pos_now], getVecLineData(arr)); // 从途径点上的信息合并得到最终的路径
    }
};
