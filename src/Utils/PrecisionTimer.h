#pragma once

#include <chrono>
#include <stdexcept>

// 跨平台高精度计时器（C++17+，毫秒级精度）
class PrecisionTimer {
public:
    // 时钟类型：steady_clock 避免系统时间修改影响，保证计时准确
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using MsDuration = std::chrono::duration<double, std::milli>;

    PrecisionTimer() : is_running_(false), start_time_(), elapsed_ms_(0.0) {}

    // 启动计时（重复启动会抛出异常）
    void start() {
        if (is_running_) {
            throw std::runtime_error("Timer is already running!");
        }
        start_time_ = Clock::now();
        is_running_ = true;
    }

    // 停止计时（未启动时抛出异常）
    void stop() {
        if (!is_running_) {
            throw std::runtime_error("Timer is not running!");
        }
        TimePoint end_time = Clock::now();
        elapsed_ms_ += MsDuration(end_time - start_time_).count();
        is_running_ = false;
    }

    // 重置计时器
    void reset() {
        is_running_ = false;
        elapsed_ms_ = 0.0;
    }

    // 获取累计耗时（毫秒，保留3位小数）
    // 若计时中，返回当前已耗时；若已停止，返回总耗时
    double get_elapsed_ms() const {
        if (is_running_) {
            TimePoint current_time = Clock::now();
            return elapsed_ms_ + MsDuration(current_time - start_time_).count();
        }
        return elapsed_ms_;
    }

    // 便捷接口：立即获取一段代码的耗时（毫秒）
    template <typename Func>
    static double measure(Func&& func) {
        PrecisionTimer timer;
        timer.start();
        std::forward<Func>(func)(); // 执行待计时函数
        timer.stop();
        return timer.get_elapsed_ms();
    }

private:
    bool is_running_;    // 计时状态
    TimePoint start_time_; // 启动时间点
    double elapsed_ms_;   // 累计耗时（毫秒）
};
