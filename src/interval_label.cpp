#include "interval_label.h"
#include <stack>
#include <algorithm>
#include <cassert>
#include <iostream> // TODO: remove

namespace TR {

    interval_label::interval_label(const adjacency_list& adj, const adjacency_list& inv_adj)
        : g(rd())
    {
        compute_discovery_and_finish_time(adj, inv_adj);
    } 

    void interval_label::compute_discovery_and_finish_time(const adjacency_list& adj, const adjacency_list& inv_adj)
    {
        assert(adj.is_dag());
        assert(inv_adj.is_dag());
        assert(adj.nr_nodes() == inv_adj.nr_nodes());

        times.clear();
        times.resize(adj.nr_nodes());
        std::vector<size_t> roots;

        for(size_t i=0; i<adj.nr_nodes(); ++i)
            if(inv_adj.degree(i) == 0)
                roots.push_back(i);

        for(size_t interval_nr=0; interval_nr<nr_interval_labels; ++interval_nr)
        {
            std::shuffle(roots.begin(), roots.end(), g);
            compute_discovery_and_finish_time(adj, inv_adj, roots, interval_nr);
        }
    }

    void interval_label::compute_discovery_and_finish_time(const adjacency_list& adj, const adjacency_list& inv_adj, const std::vector<size_t>& roots, const size_t interval_nr)
    {
        assert(interval_nr < nr_interval_labels);
        std::vector<char> visited(adj.nr_nodes(), false);
        assert(adj.nr_nodes() == inv_adj.nr_nodes());
        size_t current_reach = 1;
        size_t current_grail = 1;

        struct dfs_elem {
            size_t node : 63;
            size_t push_nr : 1;
        };
        std::stack<dfs_elem> s;

        std::vector<size_t> random_adjacency_list;
        for(const size_t r : roots)
        {
            s.push({r,0});
            while(!s.empty())
            {
                const size_t i = s.top().node;
                const size_t push_nr = s.top().push_nr;
                s.pop();
                //std::cout << "visiting " << i << "," << push_nr << "\n";
                if(push_nr == 0)
                {
                    if(visited[i])
                        continue;
                    visited[i] = true;

                    times[i].discovery[interval_nr] = current_reach++;

                    s.push({i,1});

                    random_adjacency_list.clear();
                    for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it)
                    {
                        if(!visited[*arc_it])
                        {
                            random_adjacency_list.push_back(*arc_it);
                        }
                    }
                    std::shuffle(random_adjacency_list.begin(), random_adjacency_list.end(), g);
                    for(const size_t j : random_adjacency_list)
                    {
                        //std::cout << "pushing " << j << "\n";
                        s.push({j,0});
                    }
                }
                else
                {
                    assert(times[i].L_lower[interval_nr] == 0);
                    assert(times[i].L_upper[interval_nr] == 0);
                    size_t min_children_L_lower = std::numeric_limits<size_t>::max();
                    for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it)
                    {
                        assert(visited[*arc_it] == true);
                        assert(times[*arc_it].L_lower[interval_nr] < std::numeric_limits<size_t>::max());
                        assert(times[*arc_it].L_lower[interval_nr] != 0);
                        min_children_L_lower = std::min(times[*arc_it].L_lower[interval_nr], min_children_L_lower);
                    }
                    times[i].L_lower[interval_nr] = std::min(current_grail, min_children_L_lower);
                    assert(times[i].L_lower[interval_nr] != 0);


                    times[i].finish[interval_nr] = current_reach++;
                    times[i].L_upper[interval_nr] = current_grail++;
                    //std::cout << i << ": [" << discovery_time[i][interval_nr] << "," << finish_time[i][interval_nr] << "]\n";
                } 
            } 
        }

        assert(std::count(visited.begin(), visited.end(), true) == adj.nr_nodes());
    }

    reach interval_label::query(const size_t i, const size_t j) const
    {
        assert(i < times.size());
        assert(j < times.size());

        //std::cout << "nr interval labels = " << nr_interval_labels << "\n";
        //std::cout << "query " << i << "->" << j << "\n";
        for(size_t interval_nr=0; interval_nr<nr_interval_labels; ++interval_nr)
        {
            //std::cout << "[" << discovery_time[i][interval_nr] << "," << finish_time[i][interval_nr] << "], ";
        }
        //std::cout << "\n";
        for(size_t interval_nr=0; interval_nr<nr_interval_labels; ++interval_nr)
        {
            //std::cout << "[" << discovery_time[j][interval_nr] << "," << finish_time[j][interval_nr] << "], ";
        }
        //std::cout << "\n";

        for(size_t interval_nr=0; interval_nr<nr_interval_labels; ++interval_nr)
        {
            if(!(times[i].L_lower[interval_nr] <= times[j].L_lower[interval_nr] && times[i].L_upper[interval_nr] >= times[j].L_upper[interval_nr]))
                return reach::unreachable;
        }


        for(size_t interval_nr=0; interval_nr<nr_interval_labels; ++interval_nr)
        {
            assert(times[i].discovery[interval_nr] != std::numeric_limits<size_t>::max());
            assert(times[j].discovery[interval_nr] != std::numeric_limits<size_t>::max());
            assert(times[i].finish[interval_nr] != std::numeric_limits<size_t>::max());
            assert(times[j].finish[interval_nr] != std::numeric_limits<size_t>::max());

            if((times[i].discovery[interval_nr] <= times[j].discovery[interval_nr] && times[i].finish[interval_nr] >= times[j].finish[interval_nr]))
                return reach::reachable;
        }


        return reach::undefined; 
    }
}
