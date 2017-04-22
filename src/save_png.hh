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

/*! \file save_png.hh
 *  \brief Defines some convenience functions to save floating-point data
 *  to a PNG file for quick inspection.
 */
#include <cmath>
#include <png++/png.hpp>
#include <tuple>
#include <algorithm>

namespace HyperCanny
{
    using Colour = std::tuple<int, int, int>;

    namespace colour_map
    {
        Colour rainbow(float x) {
            float r = (0.472-0.567*x+4.05*pow(x, 2))
                      /(1.+8.72*x-19.17*pow(x, 2)+14.1*pow(x, 3)),
                  g = 0.108932-1.22635*x+27.284*pow(x, 2)-98.577*pow(x, 3)
                      +163.3*pow(x, 4)-131.395*pow(x, 5)+40.634*pow(x, 6),
                  b = 1./(1.97+3.54*x-68.5*pow(x, 2)+243*pow(x, 3)
                      -297*pow(x, 4)+125*pow(x, 5));

            return Colour(r*255, g*255, b*255);
        }

        Colour sqrt_rainbow(float x) {
            return rainbow(sqrt(x));
        }
    }

    /*! \brief Saves data to a PNG file.
     *  \param filename Name of output file.
     *  \param width Width of the image.
     *  \param height Height of the image.
     *  \param data `std::vector` containing sequential values.
     *  \param palette Function of data-type to a rgb-tuple of integers.
     */
    template <typename T, typename ColourMap>
    inline void save_png(
            std::string const &filename,
            unsigned width,
            unsigned height,
            std::vector<T> const &data,
            ColourMap palette)
    {
        std::cerr << "<<< writing image to " << filename << " ... ";
        png::image<png::rgb_pixel> image(width, height);
        T lower_bound = *std::min_element(data.begin(), data.end());
        T upper_bound = *std::max_element(data.begin(), data.end());

        for (unsigned j = 0; j < height; ++j)
        {
            for (unsigned i = 0; i < width; ++i)
            {
                // TODO: use std::apply once C++17 is mainstream.
                auto c = palette((data[i + j*width] - lower_bound)/(upper_bound - lower_bound));
                image[j][i] = png::rgb_pixel(std::get<0>(c), std::get<1>(c), std::get<2>(c));
            }
        }

        image.write(filename);
        std::cerr << ">>> (ok)\n";
    }
}
