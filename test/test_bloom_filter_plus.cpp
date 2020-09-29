#include "test.h"
#include "bloom_filter_plus.h"

using namespace TR;

void test_bloom_filter_plus(const size_t nr_nodes, const size_t out_degree)
{
    const auto edges = construct_random_dag(nr_nodes, out_degree);
    bloom_filter_plus R(edges.begin(), edges.end());

    for(const auto e : edges)
    {
        test(R.query(e[0], e[1]) == reach::reachable, "false negative in reachability of random graph");
        test(R.query(e[1], e[0]) == reach::unreachable, "false positive in reachability of random graph");
    }

    const auto test_edges = construct_random_dag(nr_nodes, out_degree);
    const adjacency_list adj(edges.begin(), edges.end());
    for(const auto e : test_edges)
    {
        const bool reach_gt = reachable(adj, e[0], e[1]);
        const reach reach_pred = R.query(e[0], e[1]);
        test(reach_pred != reach::undefined, "bloom filter plus must always return defined reachability answer");
        if(reach_pred == reach::reachable)
            test(reach_gt == true, "random edge reachability: reachability error.");
        if(reach_pred == reach::unreachable)
            test(reach_gt == false, "random edge reachability: reachability error.");
    }
}

int main(int argc, char** argv)
{
    std::vector<std::array<size_t,2>> edges = {
        {0,1},
        {1,2},
        {2,3}
    };

    TR::bloom_filter_plus r(edges.begin(), edges.end());

    test(r.query(0,1) == reach::reachable, "edge 0->1 reachability not detected");
    test(r.query(0,2) == reach::reachable, "edge 0->2 reachability not detected");
    test(r.query(0,3) == reach::reachable, "edge 0->3 reachability not detected");
    test(r.query(1,2) == reach::reachable, "edge 1->2 reachability not detected");
    test(r.query(1,3) == reach::reachable, "edge 1->3 reachability not detected");
    test(r.query(2,3) == reach::reachable, "edge 2->3 reachability not detected");

    test(r.query(1,0) == reach::unreachable, "edge 1->0 reachability not detected");
    test(r.query(2,0) == reach::unreachable, "edge 2->0 reachability not detected");
    test(r.query(3,0) == reach::unreachable, "edge 3->0 reachability not detected");
    test(r.query(2,1) == reach::unreachable, "edge 2->1 reachability not detected");
    test(r.query(3,1) == reach::unreachable, "edge 3->1 reachability not detected");
    test(r.query(3,2) == reach::unreachable, "edge 3->2 reachability not detected"); 

    test_bloom_filter_plus(10, 2);
    test_bloom_filter_plus(100, 2);
    test_bloom_filter_plus(1000, 2);
    test_bloom_filter_plus(10000, 2);
    test_bloom_filter_plus(100000, 2);
    test_bloom_filter_plus(1000000, 2);

    test_bloom_filter_plus(10, 200);
    test_bloom_filter_plus(100, 200);
    test_bloom_filter_plus(1000, 200);
    test_bloom_filter_plus(10000, 200);
    test_bloom_filter_plus(100000, 200);
    test_bloom_filter_plus(1000000, 200);
}
