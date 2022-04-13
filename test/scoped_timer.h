#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace usr_defined_timer {
// 用于统计程序运行时间的计数器。
class ScopedTimer {
public:
    explicit ScopedTimer(const std::string& prog_nm = "ScopedTimer")
        : prog_nm_(prog_nm)
        , start_time_(std::chrono::high_resolution_clock::now())
    {
    }
    ~ScopedTimer()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto nano_seconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_).count();
        std::cout << "Runtime of " << prog_nm_ << " is " << 1e-6 * nano_seconds << "ms." << std::endl;
    }

    // 以纳秒的形式返回程序从开始运行到现在一共经历了多长时间。
    int64_t countInNS() const
    {
        auto cur_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(cur_time - start_time_).count();
    }

private:
    // 程序名。
    std::string prog_nm_;
    // 程序开始运行时间。
    std::chrono::high_resolution_clock::time_point start_time_;
};
}
