#include "timing.hh"

using namespace HyperCanny;

std::tuple<unsigned long, unsigned long> HyperCanny::get_runtime(
    cl::Event const &event)
{
    int err_code;
    unsigned long start_time, end_time;
    err_code = event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start_time);
    if (err_code == CL_PROFILING_INFO_NOT_AVAILABLE)
        throw Exception("No profiling info available.");
    checkErr(err_code, "cl::Event::getProfilingInfo START");

    err_code = event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end_time);
    if (err_code == CL_PROFILING_INFO_NOT_AVAILABLE)
        throw Exception("No profiling info available.");
    checkErr(err_code, "cl::Event::getProfilingInfo END");

    return std::make_tuple(start_time, end_time);
}

void HyperCanny::print_runtime_msg(
    std::tuple<unsigned long, unsigned long> const &t,
    std::string const &msg)
{
    unsigned long s, e;
    std::tie(s, e) = t;

    std::cerr << msg << "\n"
              << "start:    " << s << "\n"
              << "end:      " << e << "\n"
              << "duration: " << double(e - s)/1e6 << " ms\n\n";
}

