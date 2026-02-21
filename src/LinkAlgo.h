#pragma once

#include <algorithm>
#include <tuple>

#include "PathEngine/GraphEngine/VectorGraphEngine.h"
#include "PathEngine/GraphEngine/PixelGraphEngine.h"
#include "PathEngine/GraphEngineWrap/ErasePointGraphEngineWrap.h"
#include "PathEngine/GraphEngineWrap/MergeGraphEngineWrap.h"
#include "PathEngine/GraphEngineWrap/SpanGraphEngineWrap.h"
#include "PathEngine/PathAlgorithm/SpfaPathEngine.h"
#include "PDTreeAlgo/SocketInfo.h"
#include "Utils/Coord2dPosition.h"
#include "Utils/MyAssert.h"

template<typename T>
void vecPushFront(std::vector<T>& vec, T&& value) {
    vec.insert(vec.begin(), std::forward<T>(value));
}

// 给定一组 socket 信息，计算节点之间的连接关系
class LinkAlgo {
private:
    SocketInfo socket_info;
    VectorGraphEngine treeEdgeVGE;
    VectorGraphEngine crossingVGE;
    int crossing_cnt;

    void rawParsify(int k) {
        ASSERT(crossing_cnt > 0);
        auto c2ds1 = treeEdgeVGE.getCoord2dSet();
        auto c2ds2 = crossingVGE.getCoord2dSet();
        auto c2dsm = Coord2dSet::merge(c2ds1, c2ds2);

        socket_info.commitCoordMap(c2dsm, k);
        treeEdgeVGE.commitCoordMap(c2dsm, k);
        crossingVGE.commitCoordMap(c2dsm, k);
    }

    // 保持两个节点之间距离
    void parseArrange() {
        ASSERT(crossing_cnt > 0);
        rawParsify(6);
    }

    void saveOne(const std::vector<int>& unused_sokcet_id_list) {
        ASSERT(crossing_cnt > 0);
        auto socket_id = unused_sokcet_id_list[0];

        // 构建去掉四个点的图
        SpanGraphEngineWrap sgew(crossingVGE);
        MergeGraphEngineWrap megw(
            sgew,
            treeEdgeVGE
        );
        ErasePointGraphEngineWrap epgew(megw);

        // 确定起点终点，并将其设置为可行走的
        auto vec = socket_info.getInfo(socket_id);
        ASSERT(vec.size() == 2);
        for(auto pos: vec) { // 设置 epgew 的四个禁用障碍点 (否则会被 SpanGraphEngineWrap 堵死)
            int xpos, ypos;
            Direction dir;
            std::tie(xpos, ypos, dir) = pos;

            int dx = (int)round(Coord2dPosition::getDeltaPositionByDirection(dir).getX());
            int dy = (int)round(Coord2dPosition::getDeltaPositionByDirection(dir).getY());
            epgew.addEmptyPos(xpos, ypos);           // 交叉点本身是可以行走的
            epgew.addEmptyPos(xpos + dx, ypos + dy); // 交叉点旁边的一个点是可以行走的
        }

        // 获取起点以及终点
        int x1, y1; Direction d1; std::tie(x1, y1, d1) = vec[0];
        int x2, y2; Direction d2; std::tie(x2, y2, d2) = vec[1];

        // 获取可以行走的坐标范围（预留外界的合法边界）
        int xmin, xmax, ymin, ymax;
        std::tie(xmin, xmax, ymin, ymax) = epgew.getBorderCoord(); // 这里曾经遇到过调用纯虚函数的报错
        xmin -= 5;
        xmax += 5;
        ymin -= 5;
        ymax += 5;

        // 保存路径结果
        std::vector<LineData> path;
        if(x1 != x2 || y1 != y2) {
            auto pr = SpfaPathEngine().runAlgo(epgew, xmin, xmax, ymin, ymax, x1, y1, x2, y2);
            ASSERT(std::get<0>(pr) != -1.0);     // 如果这里条件不成立，说明原来的图不是平面图
            ASSERT(std::get<1>(pr).size() != 0); // 如果这里条件不成立，说明原来的图不是平面图

            auto dis  = std::get<0>(pr);
            path = std::get<1>(pr);
        }else {
            // 说明起点和终点是同一个点
            // 那么我们需要把中心点堵住，然后构建两个 socket 端点的路径
            // 最后再把路径修改回中心点
            PixelGraphEngine single_point_graph;
            single_point_graph.setPos(x1, y1, -3); // 把中心点设置为障碍物
            MergeGraphEngineWrap nmgew(single_point_graph, epgew);
            ASSERT(nmgew.getPos(x1, y1) != 0);

            // 获取得到相应的起始点坐标
            int new_x1, new_y1; Direction new_d1; std::tie(new_x1, new_y1, new_d1) = vec[0];
            int new_x2, new_y2; Direction new_d2; std::tie(new_x2, new_y2, new_d2) = vec[1];
            ASSERT(new_x1 == x1 && new_y1 == y1);
            ASSERT(new_x2 == x2 && new_y2 == y2);

            // 获取位置偏移
            int dx1 = (int)round(Coord2dPosition::getDeltaPositionByDirection(new_d1).getX());
            int dy1 = (int)round(Coord2dPosition::getDeltaPositionByDirection(new_d1).getY());
            int dx2 = (int)round(Coord2dPosition::getDeltaPositionByDirection(new_d2).getX());
            int dy2 = (int)round(Coord2dPosition::getDeltaPositionByDirection(new_d2).getY());

            // 构建新坐标（距离中心点的距离总是 1）
            new_x1 += dx1;
            new_y1 += dy1;
            new_x2 += dx2;
            new_y2 += dy2;

            // 计算最短路
            auto pr = SpfaPathEngine().runAlgo(nmgew, xmin, xmax, ymin, ymax, new_x1, new_y1, new_x2, new_y2);
            ASSERT(std::get<0>(pr) != -1.0);     // 如果这里条件不成立，说明原来的图不是平面图
            ASSERT(std::get<1>(pr).size() != 0); // 如果这里条件不成立，说明原来的图不是平面图

            auto dis  = std::get<0>(pr);
            path = std::get<1>(pr);

            // 重新设置起点和终点
            ASSERT(x1 == x2 && y1 == y2);
            vecPushFront(path, LineData(x1, path[0].getXf(), y1, path[0].getYf(), 0));
            path.push_back(LineData(path[path.size() - 1].getXt(), x1, path[path.size() - 1].getYt(), y1, 0));
        }

        // 把这条路线直接放入地图中并将当前节点标记为已经使用过的
        for(const LineData& ld: path) {
            auto line_data_now = ld.setV(socket_id); // 编号必须写成当前 socket_id
            treeEdgeVGE.setLine(line_data_now);
        }
        socket_info.setUsed(socket_id, true); // 设为已经使用过了
    }

    // 稠密化
    void compactArrange() {
        ASSERT(crossing_cnt > 0);
        rawParsify(2);
    }

    // 试图将最近的一组边放置到图上
    void buildOne() {
        ASSERT(crossing_cnt > 0);
        auto unused_sokcet_id_list = socket_info.getAllUnusedId(crossing_cnt);
        ASSERT(unused_sokcet_id_list.size() > 0); // 至少有一个没有使用过的编号

        parseArrange();                  // 先把图像稀疏化，使得一定有边可以相连
        saveOne(unused_sokcet_id_list);  // 把一组 sokcet_id 连接起来
        compactArrange();                // 再稠密化
    }

    // 试图最终把所有边都放到图上
    void buildAll() {
        ASSERT(crossing_cnt > 0);
        while(socket_info.getUsedCnt() < 2 * crossing_cnt) {
            buildOne();
        }
    }

public:
    ~LinkAlgo(){}

    // component_cnt 是底图连通分支数目
    LinkAlgo(int _crossing_cnt, const SocketInfo& _socket_info, int component_cnt): 
        socket_info(_socket_info), crossing_cnt(_crossing_cnt) {
        socket_info.check(_crossing_cnt, component_cnt); // 保证数据合法
        treeEdgeVGE = socket_info.getTreeEdgeVGE();      // 所有的树边
        crossingVGE = socket_info.getCrossingVGE();      // 所有的交叉点节点
        buildAll();
    }

    // 试图构造一个无参数版本
    LinkAlgo() {
        crossing_cnt = -1;
    }

    // 这里的做法就是，直接把交叉点叠加到所有树边构成的图上就行
    // 这里获得的图，在交叉点位置是负数，其他点位置是正数
    MergeGraphEngineWrap getFinalGraph() {
        ASSERT(crossing_cnt > 0);
        return MergeGraphEngineWrap(
            crossingVGE,
            treeEdgeVGE
        );
    }

    // 拷贝当前所有树边的信息
    // 其中只包含所有的边
    std::vector<LineData> getAllEdges() const {
        ASSERT(crossing_cnt > 0);
        return treeEdgeVGE.getAllEdges();
    }
};
