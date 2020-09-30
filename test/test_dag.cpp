#include "test.h"
#include "adjacency_list.h"

using namespace TR;

void test_dag(const std::vector<std::array<size_t,2>> edges, const bool result)
{
    adjacency_list adj(edges.begin(), edges.end());
    if(result == true)
        test(adj.is_dag(), "graph is not a DAG.");
    else
        test(!adj.is_dag(), "graph is a DAG.");
}

int main(int argc, char** argv)
{
    test_dag(construct_random_dag(10,2), true);
    test_dag(construct_random_dag(100,2), true);
    test_dag(construct_random_dag(1000,2), true);
    test_dag(construct_random_dag(10000,2), true);
    test_dag(construct_random_dag(100000,2), true);

    test_dag(construct_random_dag(10,20), true); 
    test_dag(construct_random_dag(100,20), true);
    test_dag(construct_random_dag(1000,20), true);
    test_dag(construct_random_dag(10000,20), true);
    test_dag(construct_random_dag(100000,20), true);

    std::vector<std::array<size_t,2>> edges = {
        {0,1},
        {1,2},
        {2,0}
    };

    test_dag(edges, false);
}
