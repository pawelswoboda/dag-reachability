#pragma once
#include <vector>
#include <cassert>

namespace TR {

    class adjacency_list {
        public:
            template<typename ARC_ITERATOR>
                adjacency_list(ARC_ITERATOR arc_begin, ARC_ITERATOR arc_end);

            size_t nr_nodes() const { return node_offsets.size()-1; }
            size_t nr_arcs() const { return arcs.size(); }
            adjacency_list inverse_adjacency_list() const;

            auto begin(const size_t i) { assert(i < nr_nodes()); return arcs.begin() + node_offsets[i]; }
            auto end(const size_t i) { assert(i < nr_nodes()); return arcs.begin() + node_offsets[i+1]; }

            auto begin(const size_t i) const { assert(i < nr_nodes()); return arcs.begin() + node_offsets[i]; }
            auto end(const size_t i) const { assert(i < nr_nodes()); return arcs.begin() + node_offsets[i+1]; }

            size_t degree(const size_t i) const { assert(i < nr_nodes()); return node_offsets[i+1] - node_offsets[i]; }

            size_t operator()(const size_t i, const size_t idx) const;

            std::vector<size_t> topological_sorting() const;
            void sort_arcs_topologically();

        private:
            std::vector<size_t> arcs;
            std::vector<size_t> node_offsets;
    };

    template<typename ARC_ITERATOR>
        adjacency_list::adjacency_list(ARC_ITERATOR arc_begin, ARC_ITERATOR arc_end)
        {
            std::vector<size_t> node_arc_count;
            for(auto arc_it=arc_begin; arc_it!=arc_end; ++arc_it)
            {
                const size_t i = (*arc_it)[0];
                const size_t j = (*arc_it)[1];
                if(node_arc_count.size() <= std::max(i,j))
                    node_arc_count.resize(std::max(i,j)+1, 0);
                node_arc_count[i]++;
            }

            arcs.resize(std::distance(arc_begin, arc_end));
            node_offsets.reserve(node_arc_count.size()+1);
            node_offsets.push_back(0);
            for(const size_t i : node_arc_count)
                node_offsets.push_back(node_offsets.back() + i);
            std::fill(node_arc_count.begin(), node_arc_count.end(), 0);

            for(auto arc_it=arc_begin; arc_it!=arc_end; ++arc_it)
            {
                const size_t i = (*arc_it)[0];
                const size_t j = (*arc_it)[1];
                arcs[node_offsets[i] + node_arc_count[i]++] = j;
            } 
        }
}
