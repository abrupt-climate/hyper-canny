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

/*! \file base/exceptions.hh
 *  \brief Common exception classes.
 */

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
