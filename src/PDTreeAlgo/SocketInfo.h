#pragma once

#include <cassert>
#include <map>
#include <vector>

#include "../Utils/Direction.h"
#include "../PathEngine/VectorGraphEngine.h"

class SocketInfo {
private:
    std::map<int, int> socket_used;

    // 这里的前两个 int 表示中心点坐标，后面一个 Direction 表示 socket 的方向
    // 理论上 vector 中恰好有两个元素
    std::map<int, std::vector<std::tuple<int, int, Direction>>> socket_info;

    // 判断是否已经完成了信息完整性检查
    int checked;

public:
    ~SocketInfo(){}
    SocketInfo() { // 无参构造函数
        checked  = false;
    }

    int getUsedCnt() const { // 统计有多少个边被使用过
        int socket_used_cnt = 0;
        for(auto pr: socket_used) {
            if(pr.second) {
                socket_used_cnt += 1;
            }
        }
        return socket_used_cnt;
    }

    // 获取得到所有的未被使用的 socket_id
    std::vector<int> getAllUnusedId(int crossing_cnt) const {
        std::vector<int> ans;
        for(int i = 1; i <= 2 * crossing_cnt; i += 1) {
            if(!getUsed(i)) {
                ans.push_back(i);
            }
        }
        return ans;
    }
    
    void setUsed(int socket_id, bool new_val) {
        socket_used[socket_id] = new_val;
    }

    // 新增一个信息
    void addInfo(int socket_id, int x, int y, Direction d) {
        assert(checked == false); // 完成检查之后不再允许新增信息
        socket_info[socket_id].push_back(std::make_tuple(x, y, d));
    }

    // 获取一个 socket 是否使用
    bool getUsed(int socket_id) const {
        if(socket_used.find(socket_id) == socket_used.end()) {
            return false;
        }else {
            return socket_used.find(socket_id) -> second;
        }
    }

    // 获取一个 socket 的位置和方向信息
    std::vector<std::tuple<int, int, Direction>> getInfo(int socket_id) {
        assert(socket_info[socket_id].size() == 2);
        return socket_info[socket_id];
    }

    // 检查数据合法性
    void check(int n) { // 这里的 n 是 crossing number
        if(checked) return;

        int used_cnt = 0;
        for(int i = 1; i <= 2 * n; i += 1) {
            if(socket_used[i] != false) {
                used_cnt += 1;
            }
            if(socket_info[i].size() != 2) { // 在运行 check 函数的时候
                assert(false);
            }
        }
        if(used_cnt != n - 1) { // 由于是一个有 n 个节点的树，所以当时恰好使用了 n - 1 条边
            assert(false);
        }
        checked = true;
    }

    // 需要能够重新调整所有坐标
    void commitCoordMap(const std::map<int, int>& mapFuncX, const std::map<int, int>& mapFuncY) {
        assert(checked == true);
        
        SocketInfo new_socket_info;
        new_socket_info.socket_used = socket_used; // 直接拷贝 “使用否” 矩阵
        new_socket_info.checked = false;
        
        for(const auto& vec: socket_info) {
            auto socket_id = vec.first;
            const auto& socket_pos_dir_list = vec.second;
            for(const auto& data: socket_pos_dir_list) { // 移动两个数据
                int xnow;
                int ynow;
                Direction dnow;
                std::tie(xnow, ynow, dnow) = data;

                // 要保证坐标被正确映射了
                assert(mapFuncX.find(xnow) != mapFuncX.end());
                assert(mapFuncY.find(ynow) != mapFuncY.end());

                // 计算新的坐标
                auto new_xnow = mapFuncX.find(xnow) -> second;
                auto new_ynow = mapFuncY.find(ynow) -> second;
                new_socket_info.addInfo(socket_id, new_xnow, new_ynow, dnow);
            }
        }

        // 使用默认拷贝构造，一次性成型
        new_socket_info.checked = true;
        *this = new_socket_info;
        assert(checked == true);
    }

    // 获取得到所有树边对应的 VGE
    VectorGraphEngine getTreeEdgeVGE() const {
        assert(checked == true);
        VectorGraphEngine vge;

        for(auto idx: socket_used) {
            auto socket_id = idx.first;
            auto used = idx.second;
            if(used) { // 说明这个 socket 是树边
                int x1, y1; Direction d1;
                std::tie(x1, y1, d1) = socket_info.find(socket_id) -> second[0];
                int x2, y2; Direction d2;
                std::tie(x2, y2, d2) = socket_info.find(socket_id) -> second[1];

                // 添加一条树边
                vge.setLine(LineData(x1, x2, y1, y2, socket_id));
            }
        }
        return vge;
    }

    // 获取得到所有交叉点坐标对应的 VGE
    VectorGraphEngine getCrossingVGE() const {
        assert(checked == true);
        std::set<std::tuple<int, int>> coord2d_set; // 用于给所有点坐标去重

        for(const auto& socket_info_pair: socket_info) {
            const auto& socket_id = std::get<0>(socket_info_pair);
            const auto& socket_list = std::get<1>(socket_info_pair);
            for(const auto& pos_dir_data: socket_list) {
                int xpos, ypos;
                Direction dir;
                std::tie(xpos, ypos, dir) = pos_dir_data; // 获取一个坐标
                coord2d_set.insert(std::make_tuple(xpos, ypos));
            }
        }

        VectorGraphEngine vge;
        for(const auto& coord2d: coord2d_set) {
            int xnow, ynow;
            std::tie(xnow, ynow) = coord2d;

            // 这里的 -2 表示这个位置是一个交叉点
            vge.setPos(xnow, ynow, -2);
        }
        return vge;
    }
};
