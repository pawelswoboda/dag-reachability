#include "pruned_path_labeling.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <iostream> // TODO: for now

namespace TR {

    std::vector<size_t> pruned_path_labeling::multiplied_degree_value() const
    {
        std::vector<size_t> d;
        d.reserve(adj.nr_nodes());
        for(size_t i=0; i<adj.nr_nodes(); ++i)
            d.push_back((adj.degree(i)+1) * (inv_adj.degree(i)+1));
        return d;
    }

    std::vector<size_t> pruned_path_labeling::multiplied_degree_order() const
    {
        const auto deg_val = multiplied_degree_value();

        std::vector<size_t> idx_perm;
        idx_perm.reserve(adj.nr_nodes());
        for(size_t i=0; i<adj.nr_nodes(); ++i)
            idx_perm.push_back(i);

        std::sort(idx_perm.begin(), idx_perm.end(), [&](const size_t i, const size_t j) {
                return deg_val[i] > deg_val[j];
                });

        return idx_perm;
    }

    std::tuple<two_dimensional_array<size_t>, std::vector<size_t>> pruned_path_labeling::optimal_paths(const size_t nr_paths, const size_t min_path_length)
    {
        const std::vector<size_t> order = multiplied_degree_order();
        two_dimensional_array<size_t> paths;
        std::vector<size_t> dp(adj.nr_nodes(), 0);
        std::vector<size_t> value = multiplied_degree_value();

        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            for(auto inv_arc_it=inv_adj.begin(i); inv_arc_it!=inv_adj.end(i); ++inv_arc_it)
                dp[i] = std::max(dp[i], dp[*inv_arc_it]);
            
            dp[i] += value[i];
        }

        std::vector<size_t> val_sorted(adj.nr_nodes());
        std::iota(val_sorted.begin(), val_sorted.end(), 0);
        std::sort(val_sorted.begin(), val_sorted.end(), [&](const size_t i, const size_t j) { return value[i] > value[j]; });
        std::vector<char> used(adj.nr_nodes(), false);
        std::vector<size_t> path;

        // back tracking
        for(const size_t v : val_sorted)
        {
            if(paths.size() >= nr_paths)
                break;
            if(used[v] == true)
                continue;

            assert(path.size() == 0);
            path.push_back(v);
            // iteratively pick maximum arc from incoming ones 
            while(path.size() < std::pow(2,16))
            { 
                size_t max_dp = 0;
                size_t max_incoming_node = std::numeric_limits<size_t>::max();
                for(auto arc_it=inv_adj.begin(path.back()); arc_it!=inv_adj.end(path.back()); ++arc_it)
                {
                    if(dp[*arc_it] >= max_dp && used[*arc_it] == false)
                    {
                        max_dp = dp[*arc_it];
                        max_incoming_node = *arc_it;
                    } 
                }
                if(max_incoming_node != std::numeric_limits<size_t>::max())
                    path.push_back(max_incoming_node);
                else
                    break;
            }

            if(path.size() >= min_path_length)
            {
                for(const size_t w : path)
                    used[w] = true;
                std::reverse(path.begin(), path.end());
                paths.add_row(path.begin(), path.end());
            }
            path.clear();
        }

        // now add remaining nodes in order
        std::vector<size_t> nodes;
        for(size_t i=0; i<order.size(); ++i)
        {
            const size_t v = order[i];
            if(!used[v])
                nodes.push_back(v); 
        }

        std::cout << "nr paths = " << paths.size() << "\n";
        //for(size_t p=0; p<paths.size(); ++p)
        //{
        //    for(size_t p_idx=0; p_idx<paths.size(p); ++p_idx)
        //        std::cout << paths(p,p_idx) << ", ";
        //    std::cout << "\n";
        //}
        return {paths, nodes};
    }

    /*
    std::vector<size_t> pruned_path_labeling::optimal_path(std::vector<char>& mask) const
    {
        std::vector<size_t> path;
        std::vector<size_t> dp(adj.nr_nodes(), 0);
        std::vector<size_t> value(adj.nr_nodes());

        std::ptrdiff_t max_i = -1; // first vertex of path
        std::ptrdiff_t max_val = -std::numeric_limits<std::ptrdiff_t>::max();

        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            for(auto inv_arc_it=inv_adj.begin(i); inv_arc_it!=inv_adj.end(i); ++inv_arc_it)
                dp[i] = std::max(dp[i], dp[*inv_arc_it]);

            if(mask[i])
                value[i] = 0;
            
            dp[i] += value[i];

            if(max_val < dp[i])
            {
                max_i = i;
                max_val = dp[i];
            }
        }

        if(max_i == -1)
            return {};

        // back tracking
        size_t v = max_i;
        path.push_back(v);

        while(dp[v] != value[v])
        {
            std::ptrdiff_t next_val = dp[v] - value[v];
            for(size_t i=0; i<inv_adj.degree(v) && path.size() < std::pow(2,16); ++i) // is second condition really needed?
            {
                if(dp[ inv_adj(v,i) ] == next_val)
                {
                    v = inv_adj(v,i);
                    path.push_back(v);
                    break;
                }
            }
        }

        std::reverse(path.begin(), path.end());
    }
    */

    void pruned_path_labeling::create_index()
    {
        two_dimensional_array<size_t> paths;
        std::vector<size_t> nodes;
        std::tie(paths, nodes) = optimal_paths(optimal_path_nr, minimum_path_length);

        std::vector<char> visited(adj.nr_nodes(), false);
        std::vector<size_t> visited_vertices;
        std::vector<char> used(adj.nr_nodes(), false);

        reach_from_path_tmp.clear();
        reach_from_path_tmp.resize(adj.nr_nodes());
        reach_to_path_tmp.clear();
        reach_to_path_tmp.resize(adj.nr_nodes());

        reach_from_nodes_tmp.clear();
        reach_from_nodes_tmp.resize(adj.nr_nodes());
        reach_to_nodes_tmp.clear();
        reach_to_nodes_tmp.resize(adj.nr_nodes());

        // compute path labelings from paths
        std::queue<size_t> q;

        for(size_t path_nr=0; path_nr<paths.size(); ++path_nr)
        {
            // BFS
            for(std::ptrdiff_t i_idx=paths.size(path_nr)-1; i_idx>=0; --i_idx)
            {
                const size_t i = paths(path_nr,i_idx);
                q.push(i);
                while(!q.empty())
                {
                    const size_t v = q.front();
                    q.pop();
                    if(visited[v])
                        continue;
                    visited[v] = true;
                    visited_vertices.push_back(v);

                    if(used[v]) // prune
                        continue;
                    if(query_tmp(i, v)) // prune
                        continue;
                    assert(path_nr <= std::numeric_limits<unsigned short int>::max());
                    reach_from_path_tmp[v].push_back({(unsigned short int)(path_nr), size_t(i_idx)});

                    for(auto arc_it=adj.begin(v); arc_it!=adj.end(v); ++arc_it)
                        q.push(*arc_it);

                }
            }

            for(const size_t v : visited_vertices)
            {
                assert(visited[v] == true);
                visited[v] = false;
            }
            visited_vertices.clear();

            // reverse BFS
            for(size_t i_idx=0; i_idx<paths.size(path_nr); ++i_idx)
            {
                const size_t i = paths(path_nr, i_idx);
                q.push(i);

                while(!q.empty())
                {
                    const size_t v = q.front();
                    q.pop();
                    if(visited[v])
                        continue;
                    visited[v] = true;
                    visited_vertices.push_back(v);

                    if(used[v]) // prune
                        continue;
                    if(query_tmp(v, i)) // prune
                        continue;
                    assert(path_nr <= std::numeric_limits<unsigned short int>::max());
                    reach_to_path_tmp[v].push_back({(unsigned short int)(path_nr), i_idx});
                    for(auto arc_it=inv_adj.begin(v); arc_it!=inv_adj.end(v); ++arc_it)
                        q.push(*arc_it);
                }
                assert(used[paths(path_nr,i_idx)] == false);
                used[paths(path_nr,i_idx)] = true;
            }

            for(const size_t v : visited_vertices)
            {
                assert(visited[v] == true);
                visited[v] = false;
            }
            visited_vertices.clear();
            //std::cout << "path nr = " << path_nr << "\n";
        } 

        // path labeling for nodes
        size_t node_nr = 0;
        for(const size_t node : nodes)
        {
            // BFS
            q.push(node);
            while(!q.empty())
            {
                const size_t v = q.front();
                q.pop();
                if(visited[v])
                    continue;
                visited[v] = true;
                visited_vertices.push_back(v);

                if(used[v]) // prune
                    continue;
                if(query_tmp(node, v)) // prune
                    continue;
                reach_from_nodes_tmp[v].push_back(node_nr);

                for(auto arc_it=adj.begin(v); arc_it!=adj.end(v); ++arc_it)
                    q.push(*arc_it);

            }
            for(const size_t v : visited_vertices)
            {
                assert(visited[v] == true);
                visited[v] = false;
            }
            visited_vertices.clear();

            // reverse BFS
            q.push(node);

            while(!q.empty())
            {
                const size_t v = q.front();
                q.pop();
                if(visited[v])
                    continue;
                visited[v] = true;
                visited_vertices.push_back(v);

                if(used[v]) // prune
                    continue;
                if(query_tmp(v, node)) // prune
                    continue;
                reach_to_nodes_tmp[v].push_back(node_nr);
                for(auto arc_it=inv_adj.begin(v); arc_it!=inv_adj.end(v); ++arc_it)
                    q.push(*arc_it);
            }
            assert(used[node] == false);
            used[node] = true;

            for(const size_t v : visited_vertices)
            {
                assert(visited[v] == true);
                visited[v] = false;
            }
            visited_vertices.clear();
            ++node_nr;
        }
        

        /*
        std::vector<char> used(adj.nr_nodes(), false);
        const auto order = multiplied_degree_order();
        std::vector<size_t> visit_order(adj.nr_nodes());

        size_t paths_nr = 0;
        std::vector<size_t> path;
        // make faster by storing unused vertices in queue
        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            if(paths_used < optimal_path_nr)
            {
                ++paths_used;
                path = optimal_path(used);
                if(path.size() < minimum_path_length)
                {
                    paths_used = optimal_path_nr;
                    path.clear();
                    for(size_t i=0; i<order.size(); ++i)
                        if(!used[order[i]])
                        {
                            path = {order[i]};
                            break;
                        }
                } 
                if(path.size() == 0)
                    continue;
            } 
            else // get unused vertex of maximal order
            {
                for(size_t i=0; i<order.size(); ++i)
                    if(!used[order[i]])
                    {
                        path = {order[i]};
                        break;
                    } 
            }

            std::queue<size_t> q;

            // BFS
            size_t visited_num = 0;
            for(size_t i_idx=0; i_idx<path.size(); ++i_idx)
            {
                const size_t i = path[i_idx];
                q.push(i);
                while(!q.empty())
                {
                    const size_t v = q.front();
                    q.pop();
                    if(visited[v])
                        continue;
                    visited[v] = true;
                    visit_order[visited_num++] = v;

                    if(used[v]) // prune
                        continue;
                    if(query_tmp(i, v)) // prune
                        continue;
                    if(path.size() > 1)
                        reach_from_path_tmp[v].push_back(std::make_pair(path_nr, i_idx]));
                    else
                        reach_from_node_tmp[v].push_back(path_nr);

                    for(auto arc_it=adj.begin(v); arc_it!=adj.end(v); ++arc_it)
                        q.push(*arc_it);

                }
            }

            for(size_t j=0; j<visited_num; ++j)
                visited[visited_vertex[j]] = false;

            // reverse BFS
            visited_num = 0;
            for(size_t i_idx=0; j<path.size(); ++j)
            {
                const size_t i = path[i_idx];
                q.push(i);

                while(!q.empty())
                {
                    const size_t v = q.front();
                    q.pop();
                    if(visited[v])
                        continue;
                    visited[v] = true;
                    visited_vertex[visited_num++] = v;

                    if(used[v]) // prune
                        continue;
                    if(query_tmp(v, i)) // prune
                        continue;
                    if(path.size() > 1)
                        reach_to_path[v].push_back(std::make_pair(path_nr, i_idx]));
                    else
                        reach_to_node_tmp[v].push_back(path_nr);
                            
                    for(auto arc_it=inv_adj.begin(v); arc_it!=inv_adj.end(v); ++arc_it)
                        q.push(*arc_it);
                }
                used[path[i_idx]] = true;
            }

            ++path_nr;
        }
    */

        convert_reachability_structs();
    }

    /*
    bool pruned_path_labeling::query(const size_t s, const size_t t) const
    {
        assert(s < adj.nr_nodes());
        assert(t < adj.nr_nodes());
        assert(s != t);
        if(s > t)
            return false;

        size_t si = 0;
        size_t ti = 0;
        // path reachability
        while(si < reach_to_path.size(s) && ti < reach_from_path.size(t))
        {
            const size_t sp = reach_to_path(s,si).path_nr;
            const size_t sv = reach_to_path(s,si).path_node_nr;
            const size_t tp = reach_from_path(t,ti).path_nr;
            const size_t tv = reach_from_path(t,ti).path_node_nr;
            if(sp == tp && sv <= tv)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        // node reachability
        si = 0;
        ti = 0;
        while(si < reach_to_nodes.size(s) && ti < reach_from_nodes.size(t))
        {
            const size_t sp = reach_to_nodes(s,si);
            const size_t tp = reach_from_nodes(t,ti);
            if(sp == tp)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        return false;
    }
    */

    bool pruned_path_labeling::query_tmp(const size_t s, const size_t t) const
    {
        assert(s < adj.nr_nodes());
        assert(t < adj.nr_nodes());
        //assert(s != t);
        if(s == t)
            return false;
        // TODO: check if s>t in the topological order!
        //if(s > t)
        //    return false;

        size_t si = 0;
        size_t ti = 0;
        // path reachability
        while(si < reach_to_path_tmp[s].size() && ti < reach_from_path_tmp[t].size())
        {
            const size_t sp = reach_to_path_tmp[s][si].path_nr;
            const size_t sv = reach_to_path_tmp[s][si].path_node_nr;
            const size_t tp = reach_from_path_tmp[t][ti].path_nr;
            const size_t tv = reach_from_path_tmp[t][ti].path_node_nr;
            if(sp == tp && sv <= tv)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        // node reachability
        si = 0;
        ti = 0;
        while(si < reach_to_nodes_tmp[s].size() && ti < reach_from_nodes_tmp[t].size())
        {
            const size_t sp = reach_to_nodes_tmp[s][si];
            const size_t tp = reach_from_nodes_tmp[t][ti];
            if(sp == tp)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        return false;
    }

    void pruned_path_labeling::convert_reachability_structs()
    {
        assert(reach_from_path.size() == 0 && reach_from_nodes.size() == 0);

        reach_from_path = two_dimensional_array<reach_index>(reach_from_path_tmp);
        reach_to_path = two_dimensional_array<reach_index>(reach_to_path_tmp);
        reach_from_nodes = two_dimensional_array<size_t>(reach_from_nodes_tmp);
        reach_to_nodes = two_dimensional_array<size_t>(reach_to_nodes_tmp);

        // TODO: clear memory
        //std::swap(reach_from_path_tmp, {});
        //std::swap(reach_to_path_tmp, {});
        //std::swap(reach_from_nodes_tmp, {});
        //std::swap(reach_to_nodes_tmp, {});
    }

    bool pruned_path_labeling::query(const size_t s, const size_t t) const
    {
        assert(s < adj.nr_nodes());
        assert(t < adj.nr_nodes());
        assert(s != t);
        // TODO: check if s>t in the topological order!
        if(s > t)
            return false;

        size_t si = 0;
        size_t ti = 0;

        // path reachability
        while(si < reach_to_path.size(s) && ti < reach_from_path.size(t))
        {
            const size_t sp = reach_to_path(s,si).path_nr;
            const size_t sv = reach_to_path(s,si).path_node_nr;
            const size_t tp = reach_from_path(t,ti).path_nr;
            const size_t tv = reach_from_path(t,ti).path_node_nr;
            if(sp == tp && sv <= tv)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        // node reachability
        si = 0;
        ti = 0;
        while(si < reach_to_nodes.size(s) && ti < reach_from_nodes.size(t))
        {
            const size_t sp = reach_to_nodes(s,si);
            const size_t tp = reach_from_nodes(t,ti);
            if(sp == tp)
                return true;
            if(sp <= tp)
                si++;
            else
                ti++;
        }

        return false;
    }
}
