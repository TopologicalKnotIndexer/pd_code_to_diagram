#pragma once

#include <iostream>
#include <sstream>
#include <string>

// 这条语句用于将 stringstream 回卷到开头位置
#define REWIND_STRING_STREAM(ss) { \
    ss.clear(); \
    ss.seekg(0, std::ios::beg); \
}

// 安全获取stringstream全部内容且不改变其状态的函数
static std::string getStreamContentWithoutChange(std::stringstream& ss) {
    // 1. 记录当前流的读写位置（核心：保存状态）
    std::ios::pos_type originalPos = ss.tellg();
    
    // 2. 将流的读取位置重置到起始处
    REWIND_STRING_STREAM(ss);
    
    // 3. 读取全部内容
    std::string content((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());
    
    // 4. 恢复流的原始读写位置（核心：还原状态）
    ss.seekg(originalPos);
    return content;
}

// 将 cin 中的所有内容读取到 stringstream 中
// 返回值：填充了 cin 内容的 stringstream 对象
// 异常：读取失败时抛出 runtime_error 异常
static std::stringstream readCinToStringStream() {
    std::stringstream ss;
    std::string line;

    try {
        // 逐行读取 cin 所有内容，直到 EOF（结束符）
        // Windows 下按 Ctrl+Z 回车触发 EOF，Linux/Mac 下按 Ctrl+D 触发 EOF
        while (std::getline(std::cin, line)) {
            // 将读取到的行写入 stringstream，保留换行符（还原原始输入格式）
            ss << line << '\n';
        }

        // 检查读取过程是否出现错误（非 EOF 导致的失败）
        if (std::cin.bad()) {
            throw std::runtime_error("cin 读取过程中发生严重错误");
        }

        // 重置 stringstream 的读取位置到开头（方便后续直接使用）
        ss.clear(); // 清除可能的 eofbit
        ss.seekg(0, std::ios::beg);
    }
    catch (const std::exception& e) {
        // 捕获并包装异常，让调用方更清晰地知道错误来源
        throw std::runtime_error("cin error：" + std::string(e.what()));
    }

    return ss;
}
