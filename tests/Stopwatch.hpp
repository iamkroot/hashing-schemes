#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>

/**
 * @brief Stopwatch to measure algorithm runtimes
 */
template<typename TimeT = std::chrono::microseconds,
        typename ClockT = std::chrono::high_resolution_clock,
        typename DurationT = uint64_t>
class Stopwatch {
private:
    std::chrono::time_point<ClockT> _start, _end;
public:
    /**
     * @brief Initialize Stopwatch
     */
    Stopwatch() {
        start();
    }

    /**
     * @brief Start Timer
     */
    void start() {
        _start = _end = ClockT::now();
    }

    /**
     * @brief Stop Timer
     * @return Stop Time
     */
    DurationT stop() {
        _end = ClockT::now();
        return elapsed();
    }

    /**
     * @brief Count Elapsed Time
     * @return Elapsed duration
     */
    DurationT elapsed() {
        auto delta = std::chrono::duration_cast<TimeT>(_end - _start);
        return delta.count();
    }

};

#endif //STOPWATCH_HPP
