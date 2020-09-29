#pragma once

#include "pruned_path_labeling.h"
#include <vector>
#include <array>

namespace TR {

        std::vector<std::array<size_t,2>> transitive_reduction(adjacency_list& adj);

    template<typename ITERATOR>
        std::vector<std::array<size_t,2>> transitive_reduction(ITERATOR arc_begin, ITERATOR arc_end)
        {
            adjacency_list adj(arc_begin, arc_end);
            return transitive_reduction(adj);
        }

}
