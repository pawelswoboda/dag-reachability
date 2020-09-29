#include "bloom_filter_label.h"
#include <stack>
#include <iostream> // TODO: remove

namespace TR {

    template<typename STREAM, typename BIT_SET>
        void print_bitset(STREAM& s, BIT_SET b)
        {
            for(size_t i=0; i<b.size(); ++i)
                if(b[i])
                    std::cout << "1";
                else 
                    std::cout << "0";
            std::cout << "\n";

        }

    bloom_filter_label::bloom_filter_label(const adjacency_list& adj, const adjacency_list& inv_adj)
    {
        create_index(adj, inv_adj);
    }

    typename bloom_filter_label::bloom_hash bloom_filter_label::init_node_hash_value(const size_t nr_nodes)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<size_t> distrib(0, bloom_filter_width-1);

        // the below construction ensures that nearby nodes have the same initial hash value
        static size_t c = 0;
        static size_t k = distrib(gen);
        if(c > nr_nodes / 1024)
        {
            c = 0;
            k = distrib(gen);
        }
        ++c;

        bloom_hash h;
        //h[distrib(gen)] = 1;
        h[k] = 1;
        return h;
    }

    std::tuple<std::vector<size_t>, std::vector<size_t>> bloom_filter_label::vertex_partition(const adjacency_list& adj, const adjacency_list& inv_adj, const size_t nr_partitions) const
    {
        assert(adj.nr_nodes() > 0);

        // construct post order traversal
        std::vector<size_t> post_order;
        post_order.reserve(adj.nr_nodes());
        std::vector<char> visited(adj.nr_nodes(), false);
        struct dfs_elem {
            size_t node : 63;
            size_t push_nr : 1;
        };
        std::stack<dfs_elem> s;

        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            if(inv_adj.degree(i) == 0)
            {
                s.push({i,0});
                while(!s.empty())
                {
                    const size_t i = s.top().node;
                    const size_t push_nr = s.top().push_nr;
                    s.pop();
                    if(push_nr == 0)
                    {
                        s.push({i,1});
                        visited[i] = true;

                        for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it)
                        {
                            if(!visited[*arc_it])
                                s.push({*arc_it,0});
                        }
                    }
                    else
                    {
                        post_order.push_back(i);
                    } 
                } 
            }
        }

        std::vector<size_t> partition_representative(nr_partitions, std::numeric_limits<size_t>::max());
        std::vector<size_t> partition_nrs(adj.nr_nodes());
        for(size_t i=0; i<adj.nr_nodes(); ++i)
        {
            const size_t partition_nr = (i*nr_partitions)/adj.nr_nodes();
            assert(partition_nr < nr_partitions);
            partition_nrs[post_order[i]] = partition_nr;
            if(partition_representative[partition_nr] != std::numeric_limits<size_t>::max())
                partition_representative[partition_nr] = post_order[i];
        }

        return {partition_nrs, partition_representative};
    }

    std::vector<typename bloom_filter_label::bloom_hash> bloom_filter_label::compute_hashes(const adjacency_list& adj, const adjacency_list& inv_adj)
    {
        std::vector<bloom_hash> L(adj.nr_nodes());
        std::vector<char> visited(adj.nr_nodes(), false);
        struct dfs_elem {
            size_t node : 63;
            size_t push_nr : 1;
        };
        std::stack<dfs_elem> s;

        for(size_t v=0; v<adj.nr_nodes(); ++v)
        {
            if(inv_adj.degree(v) == 0)
            {
                assert(visited[v] == false);
                s.push({v,0});

                while(!s.empty())
                {
                    const size_t i = s.top().node;
                    const size_t push_nr = s.top().push_nr;
                    assert(push_nr == 0 || push_nr == 1);
                    s.pop();
                    if(push_nr == 0)
                    {
                        if(visited[i] == true)
                            continue;
                        visited[i] = true;
                        assert(L[i].count() == 0);
                        L[i] = init_node_hash_value(adj.nr_nodes());

                        s.push({i,1});
                        for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it)
                            s.push({*arc_it,0});
                    }
                    else
                    {
                        for(auto arc_it=adj.begin(i); arc_it!=adj.end(i); ++arc_it)
                        {
                            assert(visited[*arc_it] == true);
                            assert(L[i].count() > 0);
                            assert(L[*arc_it].count() > 0);
                            L[i] = L[i] | L[*arc_it]; 
                            assert((L[i] | L[*arc_it]) == L[i]);
                        }
                    }
                }
            }
        }
        for(auto& l : L)
        {
            assert(l.count() > 0);
        }

        return L;
    }

    void bloom_filter_label::create_index(const adjacency_list& adj, const adjacency_list& inv_adj)
    {
        assert(adj.nr_nodes() == inv_adj.nr_nodes());
        const auto [vp,vr] = vertex_partition(adj, inv_adj, adj.nr_nodes()/20+1);

        L_out = compute_hashes(adj, inv_adj);
        L_in = compute_hashes(inv_adj, adj);
    }

    reach bloom_filter_label::query(const size_t i, const size_t j) const
    {
        assert(i < L_in.size());
        assert(j < L_in.size());
        assert(i != j);

        if((L_in[i] | L_in[j]) == L_in[j] && (L_out[i] | L_out[j]) == L_out[i])
            return reach::undefined;

        return reach::unreachable;
    }
}
