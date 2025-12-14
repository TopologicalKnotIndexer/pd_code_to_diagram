#pragma once

#include <algorithm>
#include <cassert>
#include <tuple>

#include "PathEngine/VectorGraphEngine.h"
#include "PathEngine/ErasePointGraphEngineWrap.h"
#include "PathEngine/MergeGraphEngineWrap.h"
#include "PDTreeAlgo/SocketInfo.h"
#include "PathEngine/SpanGraphEngineWrap.h"
#include "PathEngine/SpfaPathEngine.h"
#include "Utils/Coord2dPosition.h"

// 给定一组 socket 信息，计算节点之间的连接关系
class LinkAlgo {
private:
    SocketInfo socket_info;
    VectorGraphEngine treeEdgeVGE;
    VectorGraphEngine crossingVGE;
    const int crossing_cnt;

    void rawParsify(int k) {
        const auto& pr = treeEdgeVGE.parsifyDryRun(k);
        socket_info.commitCoordMap(std::get<0>(pr), std::get<1>(pr));
        treeEdgeVGE.commitCoordMap(std::get<0>(pr), std::get<1>(pr));
        crossingVGE.commitCoordMap(std::get<0>(pr), std::get<1>(pr));
    }

    // 保持两个节点之间距离
    void parseArrange() {
        rawParsify(6);
    }

    void saveOne(const std::vector<int>& unused_sokcet_id_list) {
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
        assert(vec.size() == 2);
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

        auto pr = SpfaPathEngine().runAlgo(epgew, xmin, xmax, ymin, ymax, x1, y1, x2, y2);
        assert(std::get<0>(pr) != -1.0);     // 如果这里条件不成立，说明原来的图不是平面图
        assert(std::get<1>(pr).size() != 0); // 如果这里条件不成立，说明原来的图不是平面图

        auto dis  = std::get<0>(pr);
        auto path = std::get<1>(pr);

        // 把这条路线直接放入地图中并将当前节点标记为已经使用过的
        for(const LineData& ld: path) {
            auto line_data_now = ld.setV(socket_id); // 编号必须写成当前 socket_id
            treeEdgeVGE.setLine(line_data_now);
        }
        socket_info.setUsed(socket_id, true); // 设为已经使用过了
    }

    // 稠密化
    void compactArrange() {
        rawParsify(2);
    }

    // 试图将最近的一组边放置到图上
    void buildOne() {
        auto unused_sokcet_id_list = socket_info.getAllUnusedId(crossing_cnt);
        assert(unused_sokcet_id_list.size() > 0); // 至少有一个没有使用过的编号

        parseArrange();                  // 先把图像稀疏化，使得一定有边可以相连
        saveOne(unused_sokcet_id_list);  // 把一组 sokcet_id 连接起来
        compactArrange();                // 再稠密化
    }

    // 试图最终把所有边都放到图上
    void buildAll() {
        while(socket_info.getUsedCnt() < 2 * crossing_cnt) {
            buildOne();
        }
    }

public:
    ~LinkAlgo(){}
    LinkAlgo(int _crossing_cnt, const SocketInfo& _socket_info): 
        socket_info(_socket_info), crossing_cnt(_crossing_cnt) {
        socket_info.check(_crossing_cnt);           // 保证数据合法
        treeEdgeVGE = socket_info.getTreeEdgeVGE(); // 所有的树边
        crossingVGE = socket_info.getCrossingVGE(); // 所有的交叉点节点
        buildAll();
    }

    // 这里的做法就是，直接把交叉点叠加到所有树边构成的图上就行
    // 这里获得的图，在交叉点位置是负数，其他点位置是正数
    MergeGraphEngineWrap getFinalGraph() {
        return MergeGraphEngineWrap(
            crossingVGE,
            treeEdgeVGE
        );
    }

    // 拷贝当前所有树边的信息
    // 其中只包含所有的边
    std::vector<LineData> getAllEdges() const {
        return treeEdgeVGE.getAllEdges();
    }
};
