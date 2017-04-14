#pragma once

#include "base/optional.hh"
#include "base/iterating.hh"
#include "base/string_utils.hh"
#include "base/console.hh"
#include "base/timer.hh"

namespace HyperCanny
{
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
}

