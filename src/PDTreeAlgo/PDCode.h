#pragma once

#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "NumInput.h"
#include "PDCrossing.h" // 用于描述 PdCode 中的一个交叉点的信息
#include "../Utils/MyAssert.h"

// 描述一个扭结的 PD_CODE
// 其中关键为：有 n 个 crossing 描述符
// 每个 crossing 描述符由 4 个 socket 编号构成一个四元组
// 四元组的第一个符号表示 “从下方进入交叉点的 socket 编号”
// 四元组中编号的顺序为 socket 编号按照逆时针顺序排列的结果
class PDCode {
private:

    // n 为扭结交叉点的个数，
    int n;

    // 每个交叉点有四个 “插头”, pd_code[i][0 ~ 3] 分别描述第 i 个 crossing 的四个插头的编号
    std::vector<std::vector<int>> pd_code;

    // 用于计算联通分支
    void __dfs(
        const std::map<int, std::set<int>>& graph_next, 
        int socket_id, 
        std::set<int>& visit) const {
        
        // 跳过已经访问过的节点
        if(visit.find(socket_id) != visit.end()) {
            return;
        }

        // 访问这个节点
        visit.insert(socket_id);

        // 访问与这个节点相邻的节点
        for(const auto& item: (graph_next.find(socket_id)->second)) {
            __dfs(graph_next, item, visit);
        }
    }

public:

    // 获取一个图
    // 用于描述 socket_id 之间的联通情况
    std::map<int, std::set<int>> getGraphNext() const {
        auto graph_next = std::map<int, std::set<int>>();
        for(int i = 0; i < getCrossingNumber(); i += 1) {
            for(int d = 0; d <= 1; d += 1) {
                auto frm = pd_code[i][d + 0];
                auto eto = pd_code[i][d + 2];
                graph_next[frm].insert(eto); // 在图上连接两个点
                graph_next[eto].insert(frm);
            }
        }
        return graph_next;
    }

    // 获得 socket_id 所在连通分支的所有对象
    std::set<int> getComponent(int socket_id) const {
        auto graph_next = getGraphNext();
        auto visit = std::set<int>(); // 记录所有访问过的的点
        __dfs(graph_next, socket_id, visit);
        return visit;
    }

    // 把对象恢复到初始化之前的状态
    void clear() {
        n = 0;
        pd_code.clear();
    }

    // 一个默认的 pd_code 有零个交叉点
    // 这样的 pd_code 不可以参与各种运算
    PDCode(): n(0), pd_code({}) {
    }

    // 获取 pd_code 的交叉点个数
    // 如果返回值为 0 说明这个 pd_code 对象还没有初始化
    int getCrossingNumber() const {
        return n;
    }

    // 检查当前对象是否正常，如果当前对象异常则 assert 报错
    void sanityCheck() const {
        
        // 零个交叉点的 PDCode 说明未初始化，不可以获取交叉点信息
        ASSERT(getCrossingNumber() != 0);
    }

    // 使用交叉点编号获取一个交叉点信息
    PDCrossing getCrossing(int idx) const {
        sanityCheck();

        // 合法的交叉点编号范围是 0 ~ n-1 闭区间
        ASSERT(0 <= idx && idx < getCrossingNumber());
        
        // 直接构建一个 PDCrossing 对象返回即可
        PDCrossing pd_crossing;
        pd_crossing.load(pd_code[idx]);
        return pd_crossing;
    }

    // 序列化，我们不建议修改下面的参数，但是用户可以根据需要修改它
    std::string toString(
        std::string before_code="PD",
        std::string before_item="X",
        std::string begin_item="[", 
        std::string sep=", ", 
        std::string end_item="]") const {
        
        sanityCheck();
        std::string ans = before_code + begin_item; // 加载起始符号
        
        for(int i = 0; i < n; i += 1) {
            ans += getCrossing(i).toString(before_item, begin_item, sep, end_item);
            if(i != n-1) {
                ans += sep;
            }
        }
        return ans + end_item; // 加载结束符号
    }

    // 从指定输入流输入一个 pd_code 并判断这个扭结是否合法，如果不合法返回 false
    bool InputPdCode(std::istream& input_stream) {

        // 清空原来记录的扭结信息
        pd_code.clear();
        
        // 输入所有的字符，从中读取一个 pd_code
        auto int_vec = extractIntegersFromStream(input_stream);
        ASSERT(int_vec.size() != 0);
        ASSERT(int_vec.size() % 4 == 0);
        n = int_vec.size() / 4;
        
        // cnt[i] 用于统计编号为 i 的插头出现了多少次
        // 每个插头应当恰好出现两次扭结才合法
        std::map<int, int> cnt;

        for(int i = 0; i < n; i += 1) {
            std::vector<int> pd_crossing;
            for(int j = 0; j < 4; j += 1) {
                int pos = i * 4 + j;
                pd_crossing.push_back(int_vec[pos]);
                cnt[int_vec[pos]] += 1;
            }
            pd_code.push_back(pd_crossing);
        }

        // 首先这个扭结中总共会出现 4n 个插头
        // 这 4n 个插头一定是 1~2n 这 2n 个数值，每种出现两次
        for(int i = 1; i <= 2*n; i += 1) {
            if(cnt[i] != 2) {
                return false;
            }
        }
        return true;
    }
};
