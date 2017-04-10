#pragma once

#include <memory>
#include <iostream>
#include <sstream>
#include <exception>
#include <chrono>
#include <utility>
#include <list>
#include <vector>

#include "optional.hh"

namespace HyperCanny
{
    /* generic string utilities ========================================== */
    /*! \brief get the head of a range
     * \param r a ranged object (having begin and end methods)
     * \return reference to first element of the range.
     */
    template <typename R>
    auto head(R const &r) -> decltype(*r.begin())
    {
        return *r.begin();
    }

    template <typename T>
    class Tail
    {
        public:
            typedef typename T::value_type value_type;
            typedef typename T::const_iterator const_iterator;

            Tail(T const &a):
                b(a.begin()), e(a.end())
            {
                ++b;
            }

            const_iterator begin() const { return b; }
            const_iterator end() const { return e; }
            const_iterator cbegin() const { return b; }
            const_iterator cend() const { return e; }

        private:
            const_iterator b, e;
    };

    /*! \brief get the tail of a range
     * \param r a range object (having begin and end methods).
     * \return a Tail object combining begin()++ and end().
     */
    template <typename T>
    Tail<T> tail(T const &a) { return Tail<T>(a); }

    template <typename T>
    std::optional<T> from_string(std::string const &s)
    {
        T value;
        std::istringstream iss(s);

        try {
            iss >> value;
            if (iss.fail()) {
                return std::nullopt;
            }
        }
        catch (std::istream::failure const &e) {
            return std::nullopt;
        }

        return value;
    }

    template <typename T>
    std::string to_string(T const &value)
    {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    template <typename R>
    std::string string_join(R const &r, std::string const &d)
    {
        std::string a = head(r);
        for (auto b : tail(r))
            a += d + b;

        return a;
    }

    inline void format_to(std::ostream &out) {}

    template <typename First, typename ...Rest>
    void format_to(std::ostream &out, First a, Rest &&...rest)
    {
        out << a;
        format_to(out, std::forward<Rest>(rest)...);
    }

    template <typename ...Args>
    std::string format(Args &&...args)
    {
        std::ostringstream ss;
        format_to(ss, std::forward<Args>(args)...);
        return ss.str();
    }

    class Exception: public std::exception
    {
        std::string msg;

        public:
            Exception(std::string const &msg):
                msg(msg) {}

            virtual char const *what() const throw()
            {
                return msg.c_str();
            }

            virtual ~Exception() throw() {}
    };

    template <typename I>
    void split(std::string const &s, char d, I inserter)
    {
        size_t p, q = 0;
        while (q < s.length())
        {
            p = s.find_first_not_of(d, q);
            if (p == std::string::npos) return;

            q = s.find_first_of(d, p);
            if (q == std::string::npos)
            {
                *inserter = s.substr(p, s.length() - p);
                return;
            }
            else
            {
                *inserter = s.substr(p, q-p);
                ++inserter;
            }
        }
    }

    class Console
    {
        std::list<std::string> m_indent;
        static std::unique_ptr<Console> s_instance;

        public:
            class Log
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
                        split(m, '\n', std::back_inserter(lines));
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
                message();
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

