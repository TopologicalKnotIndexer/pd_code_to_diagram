#pragma once

#include <algorithm>
#include <functional>  // 包含 std::hash
#include <iostream>
#include <unordered_map>
#include <set>
#include <tuple>

#include "../Utils/MyAssert.h"

// 为 std::tuple<int, int, int> 特化 std::hash
namespace std {
template <>
struct hash<std::tuple<int, int, int>> {
    using argument_type = std::tuple<int, int, int>;
    using result_type = size_t;

    result_type operator()(const argument_type& t) const noexcept {
        // 1. 提取 tuple 中的三个 int 值
        int a = std::get<0>(t);
        int b = std::get<1>(t);
        int c = std::get<2>(t);

        // 2. 获取基础类型的哈希值
        hash<int> hasher;
        size_t h1 = hasher(a);
        size_t h2 = hasher(b);
        size_t h3 = hasher(c);

        // 3. 组合哈希值（经典哈希组合算法，保证均匀性）
        // 步骤：旋转 + 异或 + 乘质数，降低碰撞概率
        h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        h1 ^= h3 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        
        return h1;
    }
};

}  // namespace std
template<typename _T>
class NodeSet3D {
private:
    int node_cnt;
    std::unordered_map<int, std::tuple<_T, _T, _T>> id_to_coord;
    std::unordered_map<std::tuple<_T, _T, _T>, int> coord_to_id;

    // 记录节点之间的所有边
    std::set<std::tuple<int, int>> link_set;

public:
    NodeSet3D() {
        node_cnt = 0;
    }

    // 节点编号从 1 开始
    // 如果出现过旧的编号，则返回旧的编号
    // 如果没出现过这个坐标则返回新的编号
    int addOrGetNodeId(_T x, _T y, _T z) {
        if(coord_to_id.count(std::make_tuple(x, y, z)) >= 1) {
            return getNodeId(x, y, z);
        }
        int new_id = ++ node_cnt;                      // 创建新坐标
        id_to_coord[new_id] = std::make_tuple(x, y, z); // 记录双向映射
        coord_to_id[std::make_tuple(x, y, z)] = new_id;
        return new_id;
    }

    // 根据空间坐标信息获得节点编号
    int getNodeId(_T x, _T y, _T z) const {
        ASSERT(coord_to_id.count(std::make_tuple(x, y, z)) >= 1); // 必须出现过
        return coord_to_id.find(std::make_tuple(x, y, z)) -> second;
    }

    // 向集合中插入一条边
    void link(int id1, int id2) {
        ASSERT(id1 != id2);
        ASSERT(1 <= id1 && id1 <= node_cnt);
        ASSERT(1 <= id2 && id2 <= node_cnt);
        if(id1 > id2) std::swap(id1, id2);

        link_set.insert(std::make_tuple(id1, id2));
    }

    // 输出一个描述性的字符串
    void outputGraph(std::ostream& out) const {
        // 输出总信息
        out << "node_cnt " << node_cnt << "\n";
        out << "edge_cnt " << link_set.size() << "\n";
        // 先输出所有节点信息
        for(int id = 1; id <= node_cnt; id += 1) {
            int x, y, z;
            std::tie(x, y, z) = id_to_coord.find(id) -> second;
            out << "node " << id << " pos " << x << " " << y << " " << z << "\n";
        }
        // 输出所有连接信息
        for(auto item: link_set) {
            out << "link " << std::get<0>(item) << " " << std::get<1>(item) << "\n";
        }
        out << std::endl;
    }
};
