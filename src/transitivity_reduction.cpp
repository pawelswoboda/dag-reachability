#include "transitivity_reduction.h"
#include "bloom_filter_plus.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

namespace TR {

    struct arc_hash {
        size_t operator()(const std::array<size_t,2>& a) const
        {
            const size_t h1 = std::hash<size_t>()(a[0]);
            const size_t h2 = std::hash<size_t>()(a[1]);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    // assume adj and inv_adj have arcs incident to each node sorted topologically
    std::vector<std::array<size_t,2>> sort_arcs(const adjacency_list& adj, const adjacency_list& inv_adj)
    {
        struct up_down_node {
            up_down_node(const size_t node_nr, const bool down)
                : v(node_nr), up_down_flag(down) {} 
            size_t v : 63;
            size_t up_down_flag : 1;
            size_t node_nr() const { return v; }
            bool up_node() const { return up_down_flag == 0; }
            bool down_node() const { return up_down_flag == 1; }
        };
        auto up_node = [&](const size_t node_nr) {
            return up_down_node(node_nr, false);
        };

        auto down_node = [&](const size_t node_nr) {
            return up_down_node(node_nr, true);
        };

        auto up_down_node_degree = [&](const up_down_node& v) {
            if(v.up_node())
                return adj.degree(v.node_nr());
            else
                return inv_adj.degree(v.node_nr());
        }; 
        auto up_down_node_order = [&](const up_down_node& v, const up_down_node& w) {
            return up_down_node_degree(v) < up_down_node_degree(w);
        };

        std::vector<up_down_node> node_sorting;
        node_sorting.reserve(2*adj.nr_nodes());
        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            node_sorting.push_back(up_node(i));
            node_sorting.push_back(down_node(i));
        }
        std::sort(node_sorting.begin(), node_sorting.end(), up_down_node_order);

        std::vector<std::array<size_t,2>> arc_order;
        std::unordered_set<std::array<size_t,2>, arc_hash> arc_inserted;
        arc_inserted.reserve(adj.nr_arcs());
        arc_order.reserve(adj.nr_arcs());

        for(const auto node : node_sorting)
        {
            const size_t v = node.node_nr();
            if(node.up_node())
            {
                for(auto arc_it=inv_adj.begin(v); arc_it!=inv_adj.end(v); ++arc_it)
                    if(arc_inserted.count({*arc_it, v}) == 0)
                    {
                        arc_order.push_back({*arc_it, v});
                        arc_inserted.insert({*arc_it, v});
                    } 
            }
            else
            {
                for(auto arc_it=adj.begin(v); arc_it!=adj.end(v); ++arc_it)
                    if(arc_inserted.count({v, *arc_it}) == 0)
                    {
                        arc_order.push_back({v, *arc_it});
                        arc_inserted.insert({v, *arc_it});
                    }
            }
        }

        assert(arc_order.size() == adj.nr_arcs());
        return arc_order; 
    }

    std::vector<std::array<size_t,2>> transitive_reduction(adjacency_list& adj)
    {
        adj.sort_arcs_topologically();
        adjacency_list inv_adj = adj.inverse_adjacency_list();
        inv_adj.sort_arcs_topologically();

        std::unordered_map<std::array<size_t,2>, std::array<size_t,2>, arc_hash> arc_iterator_indices;
        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            size_t j_idx = 0;
            for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it, ++j_idx)
            {
                assert(arc_iterator_indices.count({i, *arc_it}) == 0);
                arc_iterator_indices.insert(std::make_pair(std::array<size_t,2>{i,*arc_it}, std::array<size_t,2>{j_idx, std::numeric_limits<size_t>::max()}));
            }
        }

        for(size_t j=0; j<inv_adj.nr_nodes(); ++j)
        {
            size_t i_idx = 0;
            for(auto arc_it=inv_adj.begin(j); arc_it!=inv_adj.end(j); ++arc_it, ++i_idx)
            {
                assert(arc_iterator_indices.count({*arc_it, j}) == 1);
                auto elem = arc_iterator_indices.find({*arc_it, j});
                assert(elem->second[1] == std::numeric_limits<size_t>::max());
                elem->second[1] = i_idx;
            }
        }

        two_dimensional_array<char> adj_mask;
        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            adj_mask.add_row();
            for(size_t j=0; j<adj.degree(i); ++j)
                adj_mask.push_back(true);
        }

        two_dimensional_array<char> inv_adj_mask;
        for(size_t i=0; i<inv_adj.nr_nodes(); ++i)
        {
            inv_adj_mask.add_row();
            for(size_t j=0; j<inv_adj.degree(i); ++j)
                inv_adj_mask.push_back(true);
        }

        bloom_filter_plus b(adj, inv_adj);
        const std::vector<size_t> topo_order = adj.topological_sorting();
        std::vector<size_t> inv_topo_order(topo_order.size());
        for(size_t i=0; i<topo_order.size(); ++i)
            inv_topo_order[topo_order[i]] = i; 

        auto is_redundant = [&](const size_t i, const size_t j) -> bool
        {
            assert(i < adj.nr_nodes());
            assert(j < adj.nr_nodes());
            assert(i != j);
            if(adj.degree(i) > inv_adj.degree(j))
            {
                auto mask_arc_it = inv_adj_mask.begin(j);
                for(auto arc_it=inv_adj.begin(j); arc_it!=inv_adj.end(j); ++arc_it, ++mask_arc_it)
                    if(*arc_it != i && *mask_arc_it == true && inv_topo_order[*arc_it] > inv_topo_order[i])
                        if(*arc_it != i && b.query(i, *arc_it) == reach::reachable)
                            return true;
            }
            else
            {
                auto mask_arc_it = adj_mask.begin(i);
                for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++mask_arc_it, ++arc_it)
                {
                    if(*arc_it != j && *mask_arc_it == true && inv_topo_order[*arc_it] < inv_topo_order[j])
                        if(b.query(*arc_it, j) == reach::reachable)
                            return true;
                }

            }
            return false;
        };

        const std::vector<std::array<size_t,2>> sorted_arcs = sort_arcs(adj, inv_adj);

        std::vector<std::array<size_t,2>> reduced_arcs;
        for(const auto e : sorted_arcs)
        {
            const size_t i = e[0];
            const size_t j = e[1];
            if(is_redundant(i,j))
            {
                const auto [j_idx, i_idx] = arc_iterator_indices.find({i,j})->second;
                adj_mask(i, j_idx) == false;
                inv_adj_mask(j, i_idx) == false; 
            }
            else
                reduced_arcs.push_back({i, j});
        }

        return reduced_arcs;
    }


}
