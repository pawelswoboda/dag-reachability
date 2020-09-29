#pragma once

#include "query.h"
#include "adjacency_list.h"
#include "bloom_filter_label.h"
#include "interval_label.h"

namespace TR {

    class bloom_filter_plus {
        public:
            bloom_filter_plus(const adjacency_list& _adj, const adjacency_list& _inv_adj);

            template<typename ITERATOR>
                bloom_filter_plus(ITERATOR arc_begin, ITERATOR arc_end);

            reach query(const size_t v, const size_t w) const;

            const adjacency_list& adjacency() const { return adj; }
            const adjacency_list& inverse_adjacency() const { return inv_adj; }

        private:
            const adjacency_list adj;
            const adjacency_list inv_adj;
            const bloom_filter_label b;
            const interval_label i; 
            mutable std::vector<char> visited;
            std::vector<size_t> visited_vertices;
    };

    template<typename ITERATOR>
        bloom_filter_plus::bloom_filter_plus(ITERATOR arc_begin, ITERATOR arc_end)
        : adj(arc_begin, arc_end),
        inv_adj(adj.inverse_adjacency_list()),
        b(adj, inv_adj),
        i(adj, inv_adj),
        visited(adj.nr_nodes(), false)
    {}
}
