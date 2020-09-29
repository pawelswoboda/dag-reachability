#pragma once

#include "query.h"
#include "adjacency_list.h"
#include <array>
#include <vector>
#include <random>

namespace TR {

    class interval_label {
        constexpr static size_t nr_interval_labels = 32;

        public: 
            template<typename ITERATOR>
                interval_label(ITERATOR arc_begin, ITERATOR arc_end);
            interval_label(const adjacency_list& adj, const adjacency_list& inv_adj);

            reach query(const size_t s, const size_t t) const;

        private:
            void compute_discovery_and_finish_time(const adjacency_list& adj, const adjacency_list& inv_adj);
            void compute_discovery_and_finish_time(const adjacency_list& adj, const adjacency_list& inv_adj, const std::vector<size_t>& roots, const size_t interval_nr);

            struct time_elem {
                // for reachability queries
                std::array<size_t, nr_interval_labels> discovery;
                std::array<size_t, nr_interval_labels> finish; 
                // GRAIL for unreachability queries
                std::array<size_t, nr_interval_labels> L_lower;
                std::array<size_t, nr_interval_labels> L_upper;

            };
            std::vector<time_elem> times;
            //std::vector<std::array<size_t,nr_interval_labels>> discovery_time;
            //std::vector<std::array<size_t,nr_interval_labels>> finish_time;

            std::random_device rd;
            std::mt19937 g;
                     
    };

    template<typename ITERATOR>
        interval_label::interval_label(ITERATOR arc_begin, ITERATOR arc_end)
        : g(rd())
        {
            adjacency_list adj(arc_begin, arc_end);
            adjacency_list inv_adj = adj.inverse_adjacency_list(); 

            compute_discovery_and_finish_time(adj, inv_adj); 
        }

}
