#include "adjacency_list.h"
#include <stack>
#include <algorithm>
#include <array>

namespace TR {

    adjacency_list adjacency_list::inverse_adjacency_list() const
    {
        std::vector<size_t> inverse_arc_count(nr_nodes(),0);
        for(const size_t head : arcs)
            ++inverse_arc_count[head];

        std::vector<size_t> inverse_node_offsets;
        inverse_node_offsets.reserve(nr_nodes()+1);
        inverse_node_offsets.push_back(0);
        for(const size_t i : inverse_arc_count)
            inverse_node_offsets.push_back(inverse_node_offsets.back() + i);

        std::fill(inverse_arc_count.begin(), inverse_arc_count.end(), 0);
        std::vector<size_t> inverse_arcs(arcs.size());
        for(size_t i=0; i<nr_nodes(); ++i)
            for(size_t j_ctr=node_offsets[i]; j_ctr<node_offsets[i+1]; ++j_ctr)
            {
                const size_t j = arcs[j_ctr];
                inverse_arcs[inverse_node_offsets[j] +  inverse_arc_count[j]++] = i;
            }


        std::vector<std::array<size_t,2>> tmp;
        adjacency_list inverse(tmp.begin(), tmp.end());
        std::swap(inverse.arcs, inverse_arcs);
        std::swap(inverse.node_offsets, inverse_node_offsets);
        return inverse;
    }

    size_t adjacency_list::operator()(const size_t i, const size_t idx) const
    {
        assert(i < nr_nodes());
        assert(idx < degree(i));
        return arcs[node_offsets[i] + idx];
    }

    std::vector<size_t> adjacency_list::topological_sorting() const
    {
        struct topo_node {
            size_t v : 63;
            size_t push_no : 1;
        };
        std::vector<char> visited(nr_nodes(), false);
        std::stack<topo_node> s;
        std::vector<size_t> order;
        order.reserve(nr_nodes());

        for(size_t i=0; i<nr_nodes(); ++i)
        {
            if(visited[i] == false)
            {
                s.push({i,0});

                while(!s.empty())
                {
                    const size_t i = s.top().v;
                    const bool push_no = s.top().push_no;
                    s.pop();
                    if(push_no == 0)
                    {
                        s.push({i,1});

                        visited[i] = true;
                        for(auto arc_it=begin(i); arc_it!=end(i); ++arc_it)
                            if(visited[*arc_it] == false)
                                s.push({*arc_it,0});
                    }
                    else
                    {
                        order.push_back(i); 
                    }
                }
            }
        }

        std::reverse(order.begin(), order.end());
        return order;
    }

    void adjacency_list::sort_arcs_topologically()
    {
        const auto topo = topological_sorting();
        assert(topo.size() == nr_nodes());
        std::vector<size_t> inv_topo(topo.size());
        for(size_t i=0; i<topo.size(); ++i)
        {
            assert(topo[i] < nr_nodes());
            inv_topo[topo[i]] = i;
        }

        for(size_t i=0; i<nr_nodes(); ++i)
        {
            std::sort(arcs.begin() + node_offsets[i], arcs.begin() + node_offsets[i+1],
                    [&](const size_t j, const size_t k) { return inv_topo[j] < inv_topo[k]; });

        }
    }
}
