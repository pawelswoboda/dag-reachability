#include "test.h"
#include "adjacency_list.h"

using namespace TR;

void test_topological_sorting(const std::vector<std::array<size_t,2>>& edges)
{
    adjacency_list adj(edges.begin(), edges.end());
    const auto ts = adj.topological_sorting();
    test(ts.size() == adj.nr_nodes(), "topological sorting must have exactly nr nodes entries.");
    std::vector<size_t> inv_ts(adj.nr_nodes());
    for(size_t i=0; i<adj.nr_nodes(); ++i)
        inv_ts[ts[i]] = i;

    for(const auto e : edges)
        test(inv_ts[e[0]] < inv_ts[e[1]], "topological sorting not valid.");
}

int main(int argc, char** argv)
{
    test_topological_sorting(construct_random_dag(10,2));
    test_topological_sorting(construct_random_dag(100,2));
    test_topological_sorting(construct_random_dag(1000,2));
    test_topological_sorting(construct_random_dag(10000,2));
    test_topological_sorting(construct_random_dag(100000,2));

    test_topological_sorting(construct_random_dag(10,20)); 
    test_topological_sorting(construct_random_dag(100,20));
    test_topological_sorting(construct_random_dag(1000,20));
    test_topological_sorting(construct_random_dag(10000,20));
    test_topological_sorting(construct_random_dag(100000,20));
}
