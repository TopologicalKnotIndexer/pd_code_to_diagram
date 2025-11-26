#pragma once

#include <cassert>

#include "PDCrossing.h"
#include "PDCode.h"

// 构建一棵节点树形结构
// 只保证树形结构，不保证节点之间没有重叠关系
class PDTree {
private:
    PDCode pd_code;

public:
    void load(PDCode new_pd_code) {
        pd_code = new_pd_code;
        assert(pd_code.getCrossingNumber() != 0);
    }
};
