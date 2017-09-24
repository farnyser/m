#ifndef M_EXECUTION_HPP
#define M_EXECUTION_HPP

#include <vector>

namespace M
{
    struct Execution
    {
        std::vector<Quantity> quantity;
        std::vector<Price> price;
    };
}

#endif //M_EXECUTION_HPP
