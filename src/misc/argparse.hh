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

#include <string>
#include <iostream>
#include <sstream>
#include <experimental/optional>
#include <stdexcept>
#include "base.hh"

namespace HyperCanny {
namespace argparse
{
    class Exception: public std::exception
    {
        std::string msg;

        public:
            Exception(Exception const &other): msg(other.msg) {};
            Exception(Exception &&other) = default;

            template <typename ...Args>
            explicit Exception(Args &&...args):
                msg(format(std::forward<Args>(args)...)) {}

            char const *what() const throw () { return msg.c_str(); }

            ~Exception() throw () {}
    };

    class Args;

    class Option
    {
        friend class Args;

        std::string tag_;
        std::string description_;
        std::string default_value_;

        bool optional_, flag_;

        public:
            Option() = default;

            Option(std::string const &tag, std::string const &description):
                tag_(tag),
                description_(description),
                default_value_("0"),
                optional_(true),
                flag_(true) {}

            Option(std::string const &tag, std::string const &description,
                   std::string const &default_value, bool optional = true):
                tag_(tag),
                description_(description),
                default_value_(default_value),
                optional_(optional),
                flag_(false) {}
    };

    class Args
    {
        std::map<std::string, Option> option_;
        std::map<std::string, std::string> value_;

        public:
            explicit Args(std::initializer_list<Option> const &options)
            {
                for (Option const &o : options)
                    option_[o.tag_] = o;
            }

            template <typename T>
            std::optional<T> get(std::string const &tag) const
            {
                return from_string<T>(value_.at(tag));
            }

            template <typename T>
            T get(std::string const &tag, T fallback) const
            {
                std::optional<T> a = get<T>(tag);
                if (a)
                    return *a;
                else
                    return fallback;
            }

            template <typename Rng>
            void parse(Rng const &args)
            {
                auto begin = args.begin(), end = args.end();

                while (begin != end)
                {
                    std::string tag = *begin;
                    ++begin;

                    if (option_.count(tag) == 0)
                        throw Exception("Unknown option: ", tag);

                    if (option_[tag].flag_)
                    {
                        value_[tag] = "1";
                    }
                    else
                    {
                        value_[tag] = *begin;
                        ++begin;
                    }
                }

                for (auto const &kv : option_)
                {
                    if (value_.count(kv.first) == 0)
                    {
                        if (!kv.second.optional_)
                            throw Exception("Missing option: ", kv.first);
                        else
                            value_[kv.first] = kv.second.default_value_;
                    }
                }
            }
    };
}}
