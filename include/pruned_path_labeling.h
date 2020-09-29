#pragma once

#include "adjacency_list.h"
#include "two_dimensional_array.hxx"
#include <vector>
#include <tuple>

namespace TR {

    class pruned_path_labeling {
        public:
            template<typename ARC_ITERATOR>
            pruned_path_labeling(ARC_ITERATOR arc_begin, ARC_ITERATOR arc_end, const size_t _minimum_path_length = 10, const size_t _optimal_path_nr = 50);
            bool query(const size_t i, const size_t j) const;

        private:
            void create_index();
            void compute_optimal_paths();
            std::vector<size_t> multiplied_degree_value() const;
            std::vector<size_t> multiplied_degree_order() const;

            // graphs
            adjacency_list adj;
            adjacency_list inv_adj;

            // path info
            size_t add_new_path();
            size_t add_new_path_node(const size_t v);
            std::vector<size_t> path_vertices;
            std::vector<size_t> path_offsets;

            bool query_tmp(const size_t s, const size_t j) const;

            //std::vector<size_t> optimal_path(std::vector<char>& mask) const;
            std::tuple<two_dimensional_array<size_t>, std::vector<size_t>> optimal_paths(const size_t nr_paths, const size_t min_path_length);
            // path reachability label sets
            struct reach_index {
                unsigned short int path_nr;
                size_t path_node_nr;
            };
            two_dimensional_array<reach_index> reach_from_path;
            two_dimensional_array<reach_index> reach_to_path;

            // temporary reachability structures used while constructing reachability queries
            std::vector<std::vector<reach_index>> reach_from_path_tmp;
            std::vector<std::vector<reach_index>> reach_to_path_tmp;

            // node reachability label sets
            two_dimensional_array<size_t> reach_from_nodes;
            two_dimensional_array<size_t> reach_to_nodes;

            // temporary reachability structures used while constructing reachability queries
            std::vector<std::vector<size_t>> reach_from_nodes_tmp;
            std::vector<std::vector<size_t>> reach_to_nodes_tmp;

            void convert_reachability_structs();

            const int minimum_path_length = 10;
            const int optimal_path_nr = 50; 
    };

    template<typename ARC_ITERATOR>
        pruned_path_labeling::pruned_path_labeling(ARC_ITERATOR arc_begin, ARC_ITERATOR arc_end, const size_t _minimum_path_length, const size_t _optimal_path_nr)
        : minimum_path_length(_minimum_path_length),
        optimal_path_nr(_optimal_path_nr),
        adj(arc_begin, arc_end),
        inv_adj(adj.inverse_adjacency_list())
    {
        create_index();
    }
}
