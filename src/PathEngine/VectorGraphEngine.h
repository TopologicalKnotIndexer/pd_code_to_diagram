#pragma once

#include <cassert>
#include <iostream>
#include <vector>

#include "AbstractGraphEngine.h"
#include "Coord2dSet.h"
#include "LineData.h"
#include "PixelGraphEngine.h"

class VectorGraphEngine: public AbstractGraphEngine {
private:
    std::vector<LineData> lineDataSet;
    PixelGraphEngine pge;

public:
    virtual ~VectorGraphEngine(){}

    bool empty() const {
        return lineDataSet.empty();
    }

    virtual int getPos(int x, int y) const override {
        return pge.getPos(x, y);
    }

    virtual void setLine(const LineData& lineData) override {
        lineDataSet.push_back(lineData);
        pge.setLine(lineData);
    }

    virtual void setPos(int x, int y, int v) override {
        setLine(LineData(x, x, y, y, v));
    }

    virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        return pge.getBorderCoord();
    }

    // 这里不会真的进行稀疏化，而是返回一个稀疏化方案，实际的稀疏化需要调用 commitCoordMap
    // 这里只会统计 x, y 坐标的出现值集合
    Coord2dSet
    getCoord2dSet() const {
        Coord2dSet ans;
        for(auto lineData: lineDataSet) {
            ans.addPos(lineData.getXf(), lineData.getYf());
            ans.addPos(lineData.getXt(), lineData.getYt());
        }
        return ans;
    }

    // 对所有坐标值进行映射
    void commitCoordMap(Coord2dSet& coord2d_set, int k) {
        assert(k >= 1);
        // 构建新的 std::vector<LineData> 和 PixelGraphEngine
        std::vector<LineData> new_lineDataSet;
        PixelGraphEngine      new_pge;
        
        for(auto lineData: lineDataSet) {
            int xf, xt, yf, yt;
            xf = lineData.getXf();
            xt = lineData.getXt();
            yf = lineData.getYf();
            yt = lineData.getYt();

            auto new_lineData = LineData(
                coord2d_set.xkRank(xf, k),
                coord2d_set.xkRank(xt, k),
                coord2d_set.ykRank(yf, k),
                coord2d_set.ykRank(yt, k),
                lineData.getV()
            );

            new_lineDataSet.push_back(new_lineData);
            new_pge.setLine(new_lineData);
        }

        // 把新的数据拷贝给内部变量
        lineDataSet = new_lineDataSet;
        pge = new_pge;
    }

    // 获得当前所有树边信息
    std::vector<LineData> getAllEdges() const {
        return lineDataSet;
    }

    virtual std::vector<std::tuple<int, int>> getAllNegPos() const override {
        return pge.getAllNegPos();
    }
};
