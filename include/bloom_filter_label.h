#pragma once

#include "query.h"
#include "adjacency_list.h"
#include <bitset>
#include <vector>
#include <tuple>
#include <random>

namespace TR {

    class bloom_filter_label {
        public:
            bloom_filter_label(const adjacency_list& adj, const adjacency_list& inv_adj);

            template<typename ITERATOR>
                bloom_filter_label(ITERATOR arc_begin, ITERATOR arc_end);

            reach query(const size_t i, const size_t j) const;

        private:
            void create_index(const adjacency_list& adj, const adjacency_list& inv_adj);
            // return (i) vector with partition nr for each vertex and (ii) vector with partition representative for each partition nr
            std::tuple<std::vector<size_t>, std::vector<size_t>> vertex_partition(const adjacency_list& adj, const adjacency_list& inv_adj, const size_t nr_partitions) const;

            constexpr static size_t bloom_filter_width = 1024;
            using bloom_hash = std::bitset<bloom_filter_width>;
            std::vector<bloom_hash> L_in;
            std::vector<bloom_hash> L_out;
            static bloom_hash init_node_hash_value(const size_t nr_nodes);

            static std::vector<bloom_hash> compute_hashes(const adjacency_list& adj, const adjacency_list& inv_adj);

    };

    template<typename ITERATOR>
        bloom_filter_label::bloom_filter_label(ITERATOR arc_begin, ITERATOR arc_end)
    {
        adjacency_list adj(arc_begin, arc_end);
        adjacency_list inv_adj = adj.inverse_adjacency_list();
        create_index(adj, inv_adj);

    }

}
