#pragma once

#include <algorithm> // 用于std::remove_if
#include <cctype>    // 用于std::isspace
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>  // 用于std::pair
#include <vector>

#include "AbstractDataInput.h"
#include "../../Utils/MyAssert.h"

class FileDataInput: public AbstractDataInput{
private:

    // 辅助函数：去除字符串首尾的空白字符（空格、制表符、换行等）
    std::string trim(const std::string& str) const {
        // 找到第一个非空白字符的位置
        auto start = str.find_first_not_of(" \t\n\r\f\v");
        if (start == std::string::npos) {
            return ""; // 全是空白字符，返回空字符串
        }
        // 找到最后一个非空白字符的位置
        auto end = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(start, end - start + 1);
    }

    // 核心函数：统计非空行数量，并存储非空行到vector
    // 返回值：pair<非空行数量, 存储非空行的vector>
    std::pair<int, std::vector<std::string>> count_non_empty_lines(std::istream& is) const {
        std::vector<std::string> non_empty_lines;
        std::string line;
        int count = 0;

        // 逐行读取输入流，直到流结束
        while (std::getline(is, line)) {
            // 去除首尾空白后判断是否为非空行
            std::string trimmed_line = trim(line);
            if (!trimmed_line.empty()) {
                non_empty_lines.push_back(line); // 存储原始行（而非去除空白后的行）
                count++;
            }
        }

        // 检查流是否正常结束（处理读取过程中的错误）
        if (is.bad()) {
            throw std::runtime_error("读取输入流时发生严重错误！");
        }

        return {count, non_empty_lines};
    }

    /**
     * @brief 统计一行字符串中按空白字符切分的非空白段数量
     * @param line 输入的一行字符串（std::string）
     * @return 非空白段的数量（int）
     * @note 空白字符包括：空格、制表符(\t)、换行(\n)、回车(\r)等，连续空白只算一个分隔
     */
    int count_non_blank_segments(const std::string& line) const {
        int count = 0;          // 非空白段计数
        bool in_segment = false; // 标记是否处于非空白段中

        // 遍历字符串的每个字符
        for (char c : line) {
            // 判断当前字符是否为空白字符
            if (std::isspace(static_cast<unsigned char>(c))) {
                // 空白字符：退出非空白段
                in_segment = false;
            } else {
                // 非空白字符：如果未处于段中，计数+1并标记进入段
                if (!in_segment) {
                    count++;
                    in_segment = true;
                }
                // 已处于段中，无需处理（连续非空白字符属于同一段）
            }
        }

        return count;
    }

public:
    virtual ~FileDataInput() {}
    virtual IntMatrix2 loadMatrix(std::istream& in) const override {
        auto [row_cnt, vec] = count_non_empty_lines(in);
        ASSERT(row_cnt > 0);

        auto col_cnt = count_non_blank_segments(vec[0]);
        auto int_matrix = IntMatrix2(row_cnt, col_cnt);

        for(int i = 0; i < row_cnt; i += 1) {
            std::stringstream ss;
            ss << vec[i];
            for(int j = 0; j < col_cnt; j += 1) {
                int v; ss >> v;
                int_matrix.setPos(i, j, v);
            }
        }

        return int_matrix;
    }
};
