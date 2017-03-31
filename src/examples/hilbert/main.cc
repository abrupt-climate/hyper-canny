#include "../../numeric/hilbert.hh"
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace HyperCanny;

template <typename T>
void write(numeric::NdArrayBase<T, 2> const &image)
{
    auto shape = image.shape();
    auto flat = image.cbegin();
    for (unsigned j = 0; j < shape[1]; ++j)
    {
        for (unsigned i = 0; i < shape[0]; ++i, ++flat)
        {
            std::cout << std::setw(4) << *flat << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{
    std::cout << "# generating hilbert array" << std::endl;
    auto h = numeric::hilbert_array(10);
    write(h);
    return EXIT_SUCCESS;
}
