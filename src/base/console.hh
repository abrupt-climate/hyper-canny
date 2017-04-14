#pragma once
#include "string_utils.hh"
#include <vector>
#include <memory>
#include <list>

namespace HyperCanny
{
    class Console
    {
        std::list<std::string> m_indent;
        static std::unique_ptr<Console> s_instance;

        public:
            class Log;

            static Console &get()
            {
                if (not s_instance)
                {
                    s_instance.reset(new Console);
                }

                return *s_instance;
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
                    std::cerr << i;
                std::cerr << format(std::forward<Args>(args)...) << std::endl;
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
                if (not m_popped)
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
