#pragma once

#include <cmath>
#include <cassert>
#include "Direction.h"

// 描述坐标所使用的数据类型
using Coord2dType = double;

// 描述一个二位坐标位置
class Coord2dPosition {
private:
    Coord2dType x, y;

public:
    // 浮点误差
    static constexpr Coord2dType EPS = 1e-7;

    // 读取变量中的坐标信息
    Coord2dType getX() const {return x;}
    Coord2dType getY() const {return y;}

    // 无参构造函数，默认构造一个全零坐标
    Coord2dPosition() {
        x = y = 0;
    }

    // 有参构造函数，指定坐标即可
    Coord2dPosition(Coord2dType nx, Coord2dType ny): x(nx), y(ny) {}

    // 计算一个坐标点到原点的距离
    double len() const {
        return sqrt(x * x + y * y);
    }

    // 获得一个和当前向量方向相同的单位向量
    // 零向量的单位化是自身
    Coord2dPosition unit() const {

        // 零向量无法单位化，默认自己就是自己的单位化
        if(x == 0 && y == 0) return Coord2dPosition();

        // 计算向量长度，并单位化
        auto l = len();
        return Coord2dPosition(
            getX() / l,
            getY() / l
        );
    }

    // 给定一个方向
    // 返回向这个方向移动所发生的位置的变化
    static Coord2dPosition getDeltaPositionByDirection(Direction dir) {
        if(dir == Direction::EAST) {
            return Coord2dPosition(1, 0);

        }else if(dir == Direction::NORTH) {
            return Coord2dPosition(0, 1);

        }else if(dir == Direction::WEST) {
            return Coord2dPosition(-1, 0);
        
        }else if(dir == Direction::SOUTH) {
            return Coord2dPosition(0, -1);
        
        // 遇到了未定义的方向则报错
        }else {
            assert(false);
        }
    }

    // 计算坐标位置之间的加法（向量加法）
    static Coord2dPosition add(Coord2dPosition pos1, Coord2dPosition pos2) {
        return Coord2dPosition(pos1.x + pos2.x, pos1.y + pos2.y);
    }

    // 计算坐标位置之间的减法（向量减法）
    static Coord2dPosition sub(Coord2dPosition pos1, Coord2dPosition pos2) {
        return Coord2dPosition(pos1.x - pos2.x, pos1.y - pos2.y);
    }

    // 计算坐标位置之间的点乘积
    static Coord2dType dot(Coord2dPosition pos1, Coord2dPosition pos2) {
        return pos1.x * pos2.x, pos1.y * pos2.y;
    }

    // 计算坐标位置是否相等
    static bool same(Coord2dPosition pos1, Coord2dPosition pos2) {
        return std::abs(pos1.x - pos2.x) < EPS && std::abs(pos1.y - pos2.y) < EPS ;
    }

    // 计算两个节点之间的距离
    static Coord2dType distance(Coord2dPosition pos1, Coord2dPosition pos2) {
        return sub(pos1, pos2).len();
    }
};
