#pragma once

#include <iostream>
#include <vector>

#include "NodeSet3D.h"
#include "../PathEngine/AbstractGraphEngine.h"

class GenNodeSetAlgo {
private:
    const AbstractGraphEngine& age;
    std::vector<LineData> line_data_list;
    NodeSet3D<int> node_set_3d;

    // 处理一个 LineData
    // 并将路径上的所有需要连接的点连起来
    void linkEdge(LineData line_data) {
        int xf = line_data.getXf();
        int xt = line_data.getXt();
        int yf = line_data.getYf();
        int yt = line_data.getYt();
        int v  = line_data.getV ();
        assert(xf == xt || yf == yt); // 只能是一条线段
        if(v <= 0) return;            // 只处理正边权
        if(xf == xt) {
            int xpos = xf;
            for(int ypos = std::min(yf, yt); ypos < std::max(yf, yt); ypos += 1) {
                if(age.getPos(xpos, ypos) == v && age.getPos(xpos, ypos + 1) == v) { // 说明构成链接
                    int node1 = node_set_3d.addOrGetNodeId(xpos, ypos, 0);
                    int node2 = node_set_3d.addOrGetNodeId(xpos, ypos + 1, 0);
                    node_set_3d.link(node1, node2);
                }
            }
        }else { // yf == yt
            int ypos = yf;
            for(int xpos = std::min(xf, xt); xpos < std::max(xf, xt); xpos += 1) {
                if(age.getPos(xpos, ypos) == v && age.getPos(xpos + 1, ypos) == v) {
                    int node1 = node_set_3d.addOrGetNodeId(xpos, ypos, 0);
                    int node2 = node_set_3d.addOrGetNodeId(xpos + 1, ypos, 0);
                    node_set_3d.link(node1, node2);
                }
            }
        }
    }

    // 把这个位置，和自己 z 减一的位置连接
    void linkDown(std::tuple<int, int, int> pos) {
        int x, y, z;
        std::tie(x, y, z) = pos;
        auto node1 = node_set_3d.addOrGetNodeId(x, y, z);
        auto node2 = node_set_3d.addOrGetNodeId(x, y, z - 1);
        node_set_3d.link(node1, node2);
    }

    void link(std::tuple<int, int, int> pos1, std::tuple<int, int, int> pos2) {
        int x1, y1, z1; std::tie(x1, y1, z1) = pos1;
        int x2, y2, z2; std::tie(x2, y2, z2) = pos2;
        auto node1 = node_set_3d.addOrGetNodeId(x1, y1, z1);
        auto node2 = node_set_3d.addOrGetNodeId(x2, y2, z2);
        node_set_3d.link(node1, node2);
    }

    // 构建 node_set_3d
    void buildNodeSet3D() {
        // 获取到每一条边的位置信息
        // 然后进行连接
        for(auto line_data: line_data_list) { 
            linkEdge(line_data);
        }
        
        // 获取所有的交叉点
        // 即所有小于零的位置的值
        for(auto crossing_coord2d: age.getAllNegPos()) {
            int xmid, ymid;
            std::tie(xmid, ymid) = crossing_coord2d;

            auto val = age.getPos(xmid, ymid);
            assert(val == -1 || val == -2);

            const int dx = (val == -1) ? 1 : 0; // 用来表明延伸方向
            const int dy = 1 - dx;
            std::tuple<int, int, int> pos1 = std::make_tuple(xmid - dx, ymid - dy, 1);
            std::tuple<int, int, int> pos2 = std::make_tuple(xmid +  0, ymid +  0, 1);
            std::tuple<int, int, int> pos3 = std::make_tuple(xmid + dx, ymid + dy, 1);

            linkDown(pos1);
            linkDown(pos3);
            link(pos1, pos2);
            link(pos2, pos3);
        }
    }
public:
    GenNodeSetAlgo(const AbstractGraphEngine& _age, std::vector<LineData> _line_data_list): 
        age(_age), line_data_list(_line_data_list) {
        buildNodeSet3D();
    }

    // 拷贝构造一个点集合
    NodeSet3D<int> getNodeSet3D() const {
        return node_set_3d;
    }

    // 输出一个描述性的字符串
    void outputGraph(std::ostream& out) const {
        node_set_3d.outputGraph(out);
    }
};
