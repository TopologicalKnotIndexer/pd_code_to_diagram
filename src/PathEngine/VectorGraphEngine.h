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
    virtual int getPos(int x, int y) const override {
        return pge.getPos(x, y);
    }

    void setLine(const LineData& lineData) {
        lineDataSet.push_back(lineData);
        pge.setLine(lineData);
    }

    // 对坐标集合进行稀疏化
    // k 为稀疏化系数，即让最小的坐标差距为 (k-1)，所以 k 应当大于等于 2
    void parsify(int k) {
        assert(k >= 2);
        IntegerSet intSetX;
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
                intSetX.rank(xf) * k,
                intSetX.rank(xt) * k,
                intSetX.rank(yf) * k,
                intSetX.rank(yt) * k,
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
