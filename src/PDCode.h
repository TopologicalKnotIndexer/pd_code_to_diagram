#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>

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

public:

    // 从指定输入流输入一个 pd_code 并判断这个扭结是否合法，如果不合法返回 false
    bool input_pd_code(std::istream& input_stream) {

        // 清空原来记录的扭结信息
        pd_code.clear()

        // cnt[i] 用于统计编号为 i 的插头出现了多少次
        // 每个插头应当恰好出现两次扭结才合法
        std::map<int, int> cnt;
        
        // 先输入扭结的交叉点个数
        input_stream >> n;

        // 对于每一个交叉点，依次输入 PD_CODE 中的四个插头
        // 输入的顺序是从下方进入边编号开始逆时针顺序遍历四个插头的编号
        for(int i = 0; i < N; i += 1) {
            
            // 输入一个交叉点处的四个插头
            std::vetor<int> crs;
            for(int j = 0; j < 4; j += 1) {
                int idx;
                input_stream >> idx;
                crs.push_back(idx);

                // 统计每种插头编号出现的次数
                if(cnt.count(idx) == 0) cnt[idx] = 0;
                cnt[idx] += 1;
            }
            pd_code.push_back(crs);
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
