#include "test.h"
#include "transitivity_reduction.h"
#include <set>
#include <chrono>
#include <iostream>

using namespace TR;

void test_random_dag_connectivity(const size_t nr_nodes, const size_t out_degree)
{
    const auto edges = construct_random_dag(nr_nodes, out_degree);
    const auto begin_time =  std::chrono::steady_clock::now();
    const auto reduced_edges = transitive_reduction(edges.begin(), edges.end());
    const auto end_time =  std::chrono::steady_clock::now();
    std::cout << "Time for transitivity reduction = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count() << "ms" << std::endl;
    
    std::cout << "#original edges = " << edges.size() << ", #reduced edges = " << reduced_edges.size() << ", fraction of edges left = " << 100.0*double(reduced_edges.size())/double(edges.size()) << "%\n";
    return;

    std::set<std::array<size_t,2>> reduced_edge_set;
    for(const auto e : reduced_edges)
        reduced_edge_set.insert(e);

    adjacency_list reduced_adj(reduced_edges.begin(), reduced_edges.end());

    for(const auto e : edges)
    {
        test(reachable(reduced_adj, e[0], e[1]) == true, "every original edge must be reachable in reduced graph.");
    }
}

int main(int argc, char** argv)
{
    std::vector<std::array<size_t,2>> edges = {
        {0,1},
        {1,2},
        {2,3},
        {0,2},
        {0,3},
        {1,3}
    };

    const auto reduced_edges = transitive_reduction(edges.begin(), edges.end());

    test(reduced_edges.size() == 3, "transitivity reduction not returning correct number of edges.");

    test_random_dag_connectivity(10,2);
    test_random_dag_connectivity(100,2);
    test_random_dag_connectivity(1000,2);
    test_random_dag_connectivity(10000,2);
    test_random_dag_connectivity(100000,2);
    test_random_dag_connectivity(1000000,2);

    test_random_dag_connectivity(10,200);
    test_random_dag_connectivity(100,200);
    test_random_dag_connectivity(1000,200);
    test_random_dag_connectivity(10000,200);
    test_random_dag_connectivity(100000,200);
    test_random_dag_connectivity(1000000,200);
}

