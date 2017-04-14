/* Copyright 2017 Netherlands eScience Center
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
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
