#pragma once
#include "console.hh"
#include <memory>
#include <chrono>

namespace HyperCanny
{
    class Timer
    {
        using clock = std::chrono::high_resolution_clock;
        using unit_of_time  = std::chrono::milliseconds;

        clock::time_point t1;
        std::string msg;

        std::unique_ptr<Console::Log> log;

        public:
            Timer() = default;

            void start(std::string const &msg_)
            {
                msg = msg_;
                log = std::make_unique<Console::Log>(
                    format("\033[34;1m⏲ \033[m start timer [", msg, "]"),
                    "\033[34m│\033[m   ");
                t1 = clock::now();
            }

            void stop() const
            {
                clock::time_point t2 = clock::now();
                double duration = std::chrono::duration<double, std::milli>(t2 - t1).count();
                log->finish("\033[34m┕\033[m stop timer [", msg, "]: ", duration, " ms");
            }
    };
}
