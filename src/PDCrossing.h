#pragma once

#include <cassert>
#include <vector>

#include "Direction.h"

// 在 PDCode 中
// 用于描述一个交叉点的信息
class PDCrossing {
private:
    std::vector<int> crs;

public:
    // 从一个指定的 vector<int> 中加载一个四元组，作为 socket 编号序列
    void load(std::vector<int> crs) {
        this -> crs = crs;

        // 如果加载得到的 vector 不是四元组，则报错
        assert(crs.size() == 4);
    }

    // 检查当前对象是否正常，如果当前对象异常则 assert 报错
    void sanityCheck() const {

        // 如果 crs.size() 为零，说明这个对象尚未初始化
        assert(crs.size() != 0);

        // 如果 crs.size() 不为零，但是也不等于 4，说明当前 crossing 的信息出现了错误
        assert(crs.size() == 4);
    }

    // 检查当前交叉点是否含有某个指定的 socket 编号
    bool hasSocket(int socket_id) const {
        sanityCheck();

        // 如果四个 socket 编号中存在指定的 socket_id 则返回 true
        for(int i = 0; i < 4; i += 1) {
            if(crs[i] == socket_id) {
                return true;
            }
        }
        return false;
    }

    // 假设我们将 “下方进入 socket” 旋转到 base 方向
    // 计算当前 Crossing 位于 aim 方向上的 socket 编号
    int getSocketIdByDirection(Direction base, Direction aim) const {

        // 没写完
        assert(false); 
    }
};
