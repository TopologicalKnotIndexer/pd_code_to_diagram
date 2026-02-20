#pragma once

#include <stdexcept>   // 异常基类
#include <string>      // 字符串拼接
#include <sstream>     // 格式化错误信息

// 自定义断言宏：触发时抛出异常而非终止程序
// 参数：condition - 断言条件；msg - 自定义错误信息（可选）
#define ASSERT_THROW(condition, msg)                                          \
    do {                                                                      \
        if (!(condition)) {                                                   \
            std::ostringstream oss;                                           \
            oss << "Assertion failed: [" #condition "] "                      \
                << "File: " << __FILE__ << " Line: " << __LINE__ << " "       \
                << "Message: " << msg;                                        \
            throw std::runtime_error(oss.str());                              \
        }                                                                     \
    } while (false)

// 简化版：仅检查条件，使用默认错误信息
#define ASSERT(condition)                                                     \
    ASSERT_THROW(condition, "Assertion failed for condition: " #condition)
