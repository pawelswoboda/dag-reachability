#include "bloom_filter_plus.h"

namespace TR {

    bloom_filter_plus::bloom_filter_plus(const adjacency_list& _adj, const adjacency_list& _inv_adj)
        : adj(_adj),
        inv_adj(_inv_adj),
        b(adj, inv_adj),
        i(adj, inv_adj),
        visited(adj.nr_nodes(), false) 
    {}

    // function not thread-safe due to usage of visited
    reach bloom_filter_plus::query(const size_t v, const size_t w) const
    {
        assert(visited[v] == false);
        const reach ri = i.query(v, w);
        if(ri != reach::undefined)
            return ri;
        const reach rb = b.query(v, w);
        if(rb != reach::undefined)
            return rb;
        visited[v] = true;

        //if(adj.degree(v) <= inv_adj.degree(w))
        {
            for(auto arc_it=adj.begin(v); arc_it!=adj.end(v); ++arc_it)
                if(visited[*arc_it] == false && query(*arc_it, w) == reach::reachable)
                {
                    visited[v] = false;
                    return reach::reachable; 
                }
        }
        //else
        //{
        //    for(auto arc_it=inv_adj.begin(w); arc_it!=inv_adj.end(w); ++arc_it)
        //        if(query(v, *arc_it) == true)
        //        {
        //            visited[v] = false;
        //            return true; 
        //        }
        //}

        visited[v] = false;
        return reach::unreachable;
    }
}
