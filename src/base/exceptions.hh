#pragma once

#include <exception>
#include <string>

namespace HyperCanny
{
    /*! \brief Base exception for this project.
     */
    class Exception: public std::exception
    {
        std::string msg;

        public:
            explicit Exception(std::string const &msg):
                msg(msg) {}

            virtual char const *what() const throw()
            {
                return msg.c_str();
            }

            virtual ~Exception() throw() {}
    };

    class NotImplementedError: public Exception
    {
        public:
            NotImplementedError()
                : Exception("A function was called that is not implemented.")
            {}
    };
}
