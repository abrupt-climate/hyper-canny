#pragma once
#include "base.hh"
#include <tuple>
#include <vector>

namespace HyperCanny
{
    extern std::tuple<std::vector<cl::Device>, cl::Context> get_default_gpu_context();
}

