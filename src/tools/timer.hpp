#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

namespace samsar {
    namespace tools {
        class Timer {
        public:
            void start() { start_time_ = std::chrono::high_resolution_clock::now(); }

            template <typename T = std::chrono::milliseconds> double stop()
            {
                stop_time_ = std::chrono::high_resolution_clock::now();
                return std::chrono::duration_cast<T>(stop_time_ - start_time_).count();
            }

        private:
            std::chrono::high_resolution_clock::time_point start_time_;
            std::chrono::high_resolution_clock::time_point stop_time_;
        };
    } // namespace tools
} // namespace samsar

#endif
