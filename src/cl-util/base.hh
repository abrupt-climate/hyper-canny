#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include "../base.hh"
#include <CL/cl2.hpp>

namespace HyperCanny
{
    template <typename ...Args>
    inline void checkErr(cl_int err, Args &&...args)
    {
        if (err != CL_SUCCESS)
        {
            std::string msg = format(std::forward<Args>(args)...);
            console.error(err, msg);
            throw Exception(msg);
        }
    }
}
