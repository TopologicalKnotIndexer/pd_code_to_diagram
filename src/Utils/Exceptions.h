#pragma once

#include <stdexcept>   // 异常基类

// 用于在抛出异常时自动化构建异常信息（文件名行号）
#define THROW_EXCEPTION(EXCEPTION_TYPE, MSG) \
    throw EXCEPTION_TYPE(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + std::string(#EXCEPTION_TYPE) + ":" + std::string(MSG))

// 定义自定义异常
#define DEFINE_EXCEPTION(EXCEPTION_TYPE) \
class EXCEPTION_TYPE: public std::exception { \
private: \
    std::string error_msg; \
public: \
    explicit EXCEPTION_TYPE(std::string msg) : error_msg(std::move(msg)) {} \
    const char* what() const noexcept override { \
        return error_msg.c_str(); \
    } \
}

// 这条宏用来处理异常
// 他的功能是生成一个 catch 语句并在 catch 语句中打印错误相关的信息 (DEBUG 模式下才打印)
// 打印完信息后，执行 OTHER_CMD 中给出的语句
#define PROCESS_EXCEPTION(EXCEPTION_TYPE, OTHER_CMD) \
catch(const EXCEPTION_TYPE& re){ \
    SHOW_DEBUG_MESSAGE(re.what()); \
    OTHER_CMD; \
}

// 边界暴露异常
// 没有把指定的边界暴露出来
DEFINE_EXCEPTION(BadBorderException);

// 扭结 crossing 重合带来的异常
// 这个异常在随机过程中经常不满足，需要多次尝试
DEFINE_EXCEPTION(CrossingMeetException);
