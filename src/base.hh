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

/*! \file base.hh
 *  \brief Includes some base utilities.
 *
 *  Consider these additions to the standard library for this project.
 */

#include "base/optional.hh"
#include "base/iterating.hh"
#include "base/string_utils.hh"
#include "base/console.hh"
#include "base/timer.hh"
#include "base/generator.hh"
#include "base/exceptions.hh"
#include "base/pointer_range.hh"

namespace HyperCanny
{
    /*! \brief Captures output to specified stream for the life-time duration
     *  of this object.
     */
    class CaptureOutput: public std::ostringstream
    {
        std::ostream &stream;
        std::streambuf *buffer;
        bool restored;

        public:
            explicit CaptureOutput(std::ostream &s)
                : std::ostringstream()
                , stream(s)
                , buffer(s.rdbuf(this->rdbuf()))
                , restored(false)
            {}

            void restore()
            {
                stream.rdbuf(buffer);
                restored = true;
            }

            ~CaptureOutput()
            {
                if (!restored) restore();
            }
    };
}
