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

/*! \file base/console.hh
 *  \brief The `Console` class helps to print tidy and nested status messages
 *  on the console.
 *
 *  `Console` is a singleton. Use it by instantiating a `Console::Log` object.
 *  The `Console::Log` constructor can be used to add indentation to the
 *  messaged printed. This indentation is automatically reverted when the
 *  `Console::Log` object goes out of scope.
 */

#include "string_utils.hh"
#include <vector>
#include <memory>
#include <list>
#include <iostream>

namespace HyperCanny
{
    class Console
    {
        std::list<std::string> m_indent;
        static std::unique_ptr<Console> s_instance;

        Console()
        {
        }

        public:
            class Log;

            static Console &get()
            {
                if (!s_instance)
                {
                    s_instance.reset(new Console);
                }

                return *s_instance;
            }

            static std::streambuf *redirect(std::ostream &out)
            {
                return out.rdbuf(std::clog.rdbuf());
            }

            Console &push(std::string const &i)
            {
                m_indent.push_back(i);
                return *this;
            }

            template <typename ...Args>
            Console &pop(Args &&...args)
            {
                m_indent.pop_back();
                std::string m = format(std::forward<Args>(args)...);
                if (m != "")
                    message(m);
                // message();
                return *this;
            }

            template <typename ...Args>
            Console &message(Args &&...args)
            {
                for (std::string const &i : m_indent)
                    std::clog << i;
                std::clog << format(std::forward<Args>(args)...) << std::endl;
                return *this;
            }
    };

    class Console::Log
    {
        Console &m_console;
        bool m_popped;

        public:
            Log()
                : m_console(Console::get())
                , m_popped(true)
            {}

            Log(std::string const &msg,
                std::string const &indent = "\033[32m│\033[m   ")
                : m_console(Console::get().message(msg).push(indent))
                , m_popped(false)
            {}

            Log(Log const &other) = delete;
            Log &operator=(Log const &other) = delete;

            template <typename ...Args>
            void finish(Args &&...args)
            {
                if (!m_popped)
                {
                    m_console.pop(std::forward<Args>(args)...);
                    m_popped = true;
                }
            }

            template <typename ...Args>
            Log &msg(Args &&...args)
            {
                m_console.message(std::forward<Args>(args)...);
                return *this;
            }

            template <typename ...Args>
            Log &message(Args &&...args)
            {
                m_console.message(std::forward<Args>(args)...);
                return *this;
            }

            template <typename ...Args>
            Log &error(Args &&...args)
            {
                std::string m = format(std::forward<Args>(args)...);
                m_console.push("\033[1;31m[error]\033[m ");

                std::vector<std::string> lines;
                string_split(m, '\n', std::back_inserter(lines));
                for (std::string const &l : lines)
                {
                    message(l);
                }

                m_console.pop();
                return *this;
            }

            template <typename ...Args>
            Log &warning(Args &&...args)
            {
                std::string m = format(std::forward<Args>(args)...);
                m_console.push("\033[33m[warning]\033[m ");

                std::vector<std::string> lines;
                string_split(m, '\n', std::back_inserter(lines));
                for (std::string const &l : lines)
                {
                    message(l);
                }

                m_console.pop();
                return *this;
            }

            Log &endl()
            {
                return message();
            }

            ~Log()
            {
                finish();
            }
    };
}
