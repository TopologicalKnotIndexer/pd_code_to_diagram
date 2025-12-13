#pragma once

#include <vector>

#include "AbstractGraphEngine.h"
#include "LineData.h"
#include "IntegerSet.h"
#include "PixelGraphEngine.h"

class VectorGraphEngine: public AbstractGraphEngine {
private:
    std::vector<LineData> lineDataSet;
    PixelGraphEngine pge;

public:
    virtual ~VectorGraphEngine(){}

    virtual int getPos(int x, int y) const override {
        return pge.getPos(x, y);
    }

    void setLine(const LineData& lineData) {
        lineDataSet.push_back(lineData);
        pge.setLine(lineData);
    }

    virtual std::tuple<int, int, int, int> getBorderCoord() const override {
        return pge.getBorderCoord();
    }

    // 对坐标集合进行稀疏化
    // k 为稀疏化系数，即让最小的坐标差距为 (k-1)，所以 k 应当大于等于 1
    // 返回值是 MapX 以及 MapY 描述旧的坐标如何被映射成新的坐标
    // 这里不会真的进行稀疏化，而是返回一个稀疏化方案，实际的稀疏化需要调用 commitCoordMap
    std::tuple<std::map<int, int>, std::map<int, int>> 
    parsifyDryRun(int k) const {
        assert(k >= 1);
        IntegerSet intSetX; // 把所有坐标加入到一个集合中
        IntegerSet intSetY;
        for(auto lineData: lineDataSet) {
            int xf, xt, yf, yt;
            xf = lineData.getXf();
            xt = lineData.getXt();
            yf = lineData.getYf();
            yt = lineData.getYt();
            intSetX.addInt(xf);
            intSetX.addInt(xt);
            intSetY.addInt(yf);
            intSetY.addInt(yt);
        }

        // 描述原来的坐标如何被映射成新的坐标
        std::map<int, int> mapFuncX; 
        std::map<int, int> mapFuncY;

        for(auto lineData: lineDataSet) {
            int xf, xt, yf, yt;
            xf = lineData.getXf();
            xt = lineData.getXt();
            yf = lineData.getYf();
            yt = lineData.getYt();

            // 记录所有坐标是如何变化的
            mapFuncX[xf] = intSetX.rank(xf) * k;
            mapFuncX[xt] = intSetX.rank(xt) * k;
            mapFuncY[yf] = intSetX.rank(yf) * k;
            mapFuncY[yt] = intSetX.rank(yt) * k;
        }

        // 描述坐标映射
        return std::make_tuple(mapFuncX, mapFuncY);
    }

    // 对所有坐标值进行映射
    void commitCoordMap(const std::map<int, int>& mapFuncX, const std::map<int, int>& mapFuncY) {
        // 构建新的 std::vector<LineData> 和 PixelGraphEngine
        std::vector<LineData> new_lineDataSet;
        PixelGraphEngine      new_pge;
        
        for(auto lineData: lineDataSet) {
            int xf, xt, yf, yt;
            xf = lineData.getXf();
            xt = lineData.getXt();
            yf = lineData.getYf();
            yt = lineData.getYt();

            // 检验被映射的值的合法性
            // 如果有一些值没有指定怎么映射，那么这个映射方案本身不合法
            assert(mapFuncX.find(xf) != mapFuncX.end());
            assert(mapFuncX.find(xt) != mapFuncX.end());
            assert(mapFuncY.find(yf) != mapFuncY.end());
            assert(mapFuncY.find(yt) != mapFuncY.end());

            auto new_lineData = LineData(
                mapFuncX.find(xf) -> second,
                mapFuncX.find(xt) -> second,
                mapFuncY.find(yf) -> second,
                mapFuncY.find(yt) -> second,
                lineData.getV()
            );

            new_lineDataSet.push_back(new_lineData);
            new_pge.setLine(new_lineData);
        }

        // 把新的数据拷贝给内部变量
        lineDataSet = new_lineDataSet;
        pge = new_pge;
    }
};
