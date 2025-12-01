#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "Coord2dPosition.h"
#include "Direction.h"
#include "PDCrossing.h"
#include "PDCode.h"
#include "Random.h"

// 描述上下左右四个方向的节点编号
// 四个方向与 Direction 类中的方向定义一致
struct TreeNode {
    int next_node[4] {};
    Coord2dPosition pos2d; // 描述布局后节点所处的整数坐标

    // 获得某个指定方向上的下一个节点的编号
    int getIdOnDirection(Direction dir) const {
        return next_node[(int)dir];
    }
};

// 记录树中节点的信息
// 具体而言需要记录一个节点 PDCrossing 和 base 的方向
// base 指的是从下方进入这个 crossing 的 socket
struct TreeMsg {
    PDCrossing pd_crossing;
    Direction base_direction;
};

// 构建一棵节点树形结构
// 只保证树形结构，不保证节点之间没有重叠关系
class PDTree {
private:
    PDCode pd_code;

    // 描述树的结构：
    // 0 号节点表示空节点，不存储任何信息
    std::vector<TreeNode> structure;
    std::vector<TreeMsg> message;

    // 在树上新创建一个节点
    int newTreeNode() {
        structure.push_back(TreeNode());
        message.push_back(TreeMsg());
        return structure.size() - 1;
    }

    // 从一个 vector 中随机删除一个元素，并返回
    PDCrossing popRandomCrossing(std::vector<PDCrossing>& unused_list) {
        assert(unused_list.size() != 0);

        int pos = randomInt(0, unused_list.size() - 1);
        PDCrossing ans = unused_list[pos];            // 先拷贝其中的元素
        unused_list.erase(unused_list.begin() + pos); // 再对拷贝后的元素进行删除
        return ans;
    }

    // 给定一个 socket_id
    // 找到 unused_list 中第一次出现这个 socket_id 的位置
    // 并删除它
    PDCrossing popCrossingBySocketId(std::vector<PDCrossing>& unused_list, int socket_id) {
        assert(unused_list.size() != 0);

        int pos = -1;
        for(int i = 0; i < unused_list.size(); i += 1) {
            if(unused_list[i].hasSocket(socket_id)) { // 找到了这个 socket_id 第一次出现的位置
                pos = i;
                break;
            }
        }

        // 如果没有找到，直接报错（理论上 dfs 对 map 正确去重的前提下一定能找到）
        assert(pos != -1);

        // 先记录这个元素值，再从 list 中删去
        PDCrossing ans = unused_list[pos];
        unused_list.erase(unused_list.begin() + pos);
        return ans;
    }

    // 记录某个叶子节点上的空闲的 socket 位置信息
    struct LeafInfo {
        int node_id;   // 叶子节点 id
        Direction dir; // 当前 socket 所处在的叶子节点的方向
        int socket_id; // 当前 socket 自身的编号

        // 描述某一个 socket 的优先权重
        // 在使用 socket 的时候，总是使用优先权重大的 socket 进行拓展
        double right = 0;
    };

    // 观察当前是否已经有节点占据了 new_pos 这个位置
    // 如果有则返回一个较大的惩罚
    // 如果没有则返回 0
    double calcPositionPunish(Coord2dPosition new_pos) const {
        const auto N = (pd_code.getCrossingNumber() + 1);

        // 这里需要跳过零号节点，因为零号节点并不是真正意义上的节点
        for(int i = 1; i < structure.size(); i += 1) {
            if(Coord2dPosition::same(structure[i].pos2d, new_pos)) {
                return N * N;
            }
        }

        // 没有找到重合的节点
        return 0;
    }

    // 观察当前是否有一个存在的节点到 new_pos 的距离小于等于 1
    // 如果有则返回一个比较大的惩罚
    // 如果没有则返回 0
    // 由于父亲节点到子节点的距离总是 1，因此需要跳过父亲节点
    double calcNearPunish(int fa_id, Coord2dPosition new_pos) const {
        const auto N = (pd_code.getCrossingNumber() + 1);

        // 这里需要跳过零号节点，因为零号节点并不是真正意义上的节点
        for(int i = 1; i < structure.size(); i += 1) {

            // 跳过父亲节点
            if(i == fa_id) {
                continue;
            }
            if(Coord2dPosition::distance(structure[i].pos2d, new_pos) <= 1 + Coord2dPosition::EPS) {
                return 2 * N;
            }
        }

        // 没有找到重合的节点
        return 0;
    }

    // 遍历整颗树
    // leaf_id_map 将记录一些关于树上空闲插头的信息
    // 具体参考 getBestSocket 函数前的注释
    void dfs(int x, int fa, std::map<int, LeafInfo>& leaf_id_map) {
        for(int i = 0; i < 4; i += 1) {
            if(structure[x].next_node[i] == fa) { // 避免对父节点进行递归
                continue;
            }
            if(structure[x].next_node[i] == 0) { // 说明这是一个空闲插头
                int socket_id = message[x].pd_crossing.getSocketIdByDirection(
                    message[x].base_direction, (Direction)i);
                
                if(leaf_id_map.find(socket_id) != leaf_id_map.end()) {
                    
                    // 说明这个 socket_id 过去曾经出现过
                    // 删除过去出现的记录
                    leaf_id_map.erase(leaf_id_map.lower_bound(socket_id));
                }else {

                    // 计算当前 socket 如果拓展
                    // 得到的新节点的位置
                    auto new_pos = Coord2dPosition::add(
                            structure[x].pos2d,
                            Coord2dPosition::getDeltaPositionByDirection((Direction)i));

                    // 说明这个 socket_id 过去没有出现过
                    // 记录当前 socket 的详细信息
                    leaf_id_map[socket_id] = (LeafInfo){
                        .node_id = x,
                        .dir = (Direction)i,
                        .socket_id = socket_id,

                        // 关于权重的解释
                        // 由于权重越大越好，因此惩罚项应该用负系数
                        // 1. 我们希望优先拓展离原点更远的节点
                        // 2. 对于同一个节点的多个 socket 优先拓展方向与节点位置坐标一致的 socket
                        // 3. 如果新生成的节点位置有人占据，则施加较大的惩罚
                        // 4. 如果新生成的节点，距离某个（不是父亲的）节点距离小于等于 1，也要施加惩罚
                        .right = (
                            structure[x].pos2d.len() + 
                            Coord2dPosition::dot(
                                structure[x].pos2d.unit(),
                                Coord2dPosition::getDeltaPositionByDirection((Direction)i)) * 0.5
                            - calcPositionPunish(new_pos)
                            - calcNearPunish(x, new_pos))
                    };
                }

            }else { // 说明这不是一个空闲插头
                dfs(structure[x].next_node[i], x, leaf_id_map);
            }
        }
    }
    
    // 用于为所有 socket 排序
    // 从而确定不同 socket 进行拓展的优劣
    // 按照 right 值从大到小排序
    static bool sortLeafList(LeafInfo li1, LeafInfo li2) {
        return li1.right > li2.right;
    }

    // 选择一个可用的 socket
    // 1. 这个 socket 目前必须是空闲的
    // 2. 这个 socket 对应的编号，目前在树上恰出现一次
    // 3. 如果有多个可用的，优先选择 right 最大的 socket
    LeafInfo getBestSocket(int root) {
        // leaf_id_map 第一次遇到某个 socket_id 时
        // leaf_id_map[socket_id] 赋值为这个 socket 的 LeafInfo
        // 第二次遇到同样的 socket_id 时，在 leaf_id_map 上删除 leaf_id_map[socket_id]
        std::map<int, LeafInfo> leaf_id_map;
        dfs(root, -1, leaf_id_map); // 这里的 fa 如果设为 0 会导致错误跳过一些分支

        // 如果没有空闲的插头，说明树已经建完，不可以再调用 getBestSocket
        assert(leaf_id_map.size() > 0);

        // 对 leaf_id_map 序列化，以便于排序取一个元素出来
        std::vector<LeafInfo> leaf_id_list;
        for(auto pr: leaf_id_map) {
            leaf_id_list.push_back(pr.second);
        }

        // 按 right 从大到小排序后，取第一个元素
        std::sort(leaf_id_list.begin(), leaf_id_list.end(), PDTree::sortLeafList);
        return leaf_id_list[0];
    }

    // 从零开始构建一棵四岔树
    void buildTree() {
        // 预先保存所有交叉点数
        const int n = pd_code.getCrossingNumber();
        assert(n != 0);

        // 记录所有还没有被使用过的交叉点
        std::vector<PDCrossing> unused;
        for(int i = 0; i < n; i += 1) unused.push_back(pd_code.getCrossing(i));

        // 先随机选择一个节点，用于生成根节点
        // 根节点默认 base 方向朝向正东方向，并且坐标放置在原点处
        int root = newTreeNode();
        message[root].pd_crossing = popRandomCrossing(unused);
        message[root].base_direction = Direction::EAST;
        structure[root].pos2d = Coord2dPosition();

        // 如果还有没有放到树上的节点，则运行下面的循环
        while(unused.size() > 0) {

            // 选择一个最优 socket_id 进行拓展
            LeafInfo leaf_info = getBestSocket(root);

            // 在 unused 序列中找到第一次出现这个 socket_id 的 crossing
            // 根据这个 crossing 的信息新建一个节点 
            int new_node = newTreeNode();
            message[new_node].pd_crossing = popCrossingBySocketId(unused, leaf_info.socket_id);

            // 计算对面的方向
            auto oppo_dir = (Direction)((2 + (int)leaf_info.dir)% 4);

            // baseShift 的含义是
            // 要想将当前 crossing 编号为 socket_id 的 socket 移动到 aim_dir 方向
            // 需要让 base 方向朝向哪里
            message[new_node].base_direction = message[new_node].pd_crossing.baseShift(
                leaf_info.socket_id, oppo_dir);
            
            // 让父子节点相认
            // 连接两条单向边，再放置子节点的正确坐标位置
            structure[new_node].next_node[(int)oppo_dir] = leaf_info.node_id;
            structure[leaf_info.node_id].next_node[(int)leaf_info.dir] = new_node;
            structure[new_node].pos2d = Coord2dPosition::add(
                structure[leaf_info.node_id].pos2d, 
                Coord2dPosition::getDeltaPositionByDirection(leaf_info.dir));
        }
    }

public:
    
    // 把对象恢复到初始化之前的状态
    void clear() {
        pd_code.clear();
        structure.clear();
        message.clear();

        // 保证零号节点总是存在的
        // 因为零号节点用于表示空的 socket，而最小节点编号为 1
        int zero = newTreeNode();
        assert(zero == 0);
    }

    // 加载一个 pd_code 并计算生成树
    void load(PDCode new_pd_code) {
        clear(); // 清除历史数据

        pd_code = new_pd_code;
        assert(pd_code.getCrossingNumber() != 0);

        buildTree(); // 构建树
    }
    
    // 输出关于建树的调试信息
    void debugOutput() {
        for(int i = 1; i < message.size();  i += 1) {
            std::cout << "node " << i << ": " << std::endl;

            // 按照从正东开始的逆时针方向依次输出 socket
            // 括号中输出的是 base 的方向
            std::cout << "    sockets(" << (int)message[i].base_direction << "): ";
            for(int d = 0; d < 4; d += 1) {
                std::cout << message[i].pd_crossing.getSocketIdByDirection(
                    message[i].base_direction,
                    (Direction)d
                ) << " ";
            }
            std::cout << std::endl;

            // 输出结构信息：正东开始的逆时针方向依次输出（新编号）
            std::cout << "    tree: ";
            for(int d = 0; d < 4; d += 1) {
                std::cout << structure[i].next_node[d] << " ";
            }
            std::cout << std::endl;
        }
    }
};
