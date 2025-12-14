#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <cctype>

// 从输入流读取字符，过滤非数字为空格，提取所有整数到vector<int>
std::vector<int> extractIntegersFromStream(std::istream& is) {
    // 步骤1：读取所有字符直到EOF，非数字替换为空格
    std::string processedStr;
    char ch;
    while (is.get(ch)) { // 逐字符读取直到EOF
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            processedStr.push_back(ch); // 数字保留
        } else {
            processedStr.push_back(' '); // 非数字替换为空格
        }
    }

    // 步骤2：以连续空格为分隔符提取整数
    std::vector<int> result;
    std::istringstream iss(processedStr); // 用处理后的字符串构建字符串流
    int num;
    while (iss >> num) { // 自动以空白符（连续空格/制表符/换行）分隔读取整数
        result.push_back(num);
    }

    return result;
}
