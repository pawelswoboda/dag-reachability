#include "test.h"
#include "pruned_path_labeling.h"

void test_pruned_path_labeling(const size_t nr_nodes, const size_t avg_out_degree)
{
    const auto edges = construct_random_dag(nr_nodes, avg_out_degree);
    TR::pruned_path_labeling r(edges.begin(), edges.end());

    for(const auto e : edges)
    {
        test(r.query(e[0], e[1]) == true, "false negative in reachability of random graph");
        test(r.query(e[1], e[0]) == false, "false positive in reachability of random graph");
    }
}

int main(int argc, char** argv)
{
    std::vector<std::array<size_t,2>> edges = {
        {0,1},
        {1,2},
        {2,3}
    };

    TR::pruned_path_labeling r(edges.begin(), edges.end());

    test(r.query(0,1), "edge 0->1 reachability not detected");
    test(r.query(0,2), "edge 0->2 reachability not detected");
    test(r.query(0,3), "edge 0->3 reachability not detected");
    test(r.query(1,2), "edge 1->2 reachability not detected");
    test(r.query(1,3), "edge 1->3 reachability not detected");
    test(r.query(2,3), "edge 2->3 reachability not detected");

    test(!r.query(1,0), "edge 1->0 reachability not detected");
    test(!r.query(2,0), "edge 2->0 reachability not detected");
    test(!r.query(3,0), "edge 3->0 reachability not detected");
    test(!r.query(2,1), "edge 2->1 reachability not detected");
    test(!r.query(3,1), "edge 3->1 reachability not detected");
    test(!r.query(3,2), "edge 3->2 reachability not detected"); 

    test_pruned_path_labeling(10, 2);
    test_pruned_path_labeling(100, 2);
    test_pruned_path_labeling(1000, 2);
    test_pruned_path_labeling(10000, 2);
    test_pruned_path_labeling(100000, 2);
    test_pruned_path_labeling(1000000, 2);

    test_pruned_path_labeling(10, 200);
    test_pruned_path_labeling(100, 200);
    test_pruned_path_labeling(1000, 200);
    test_pruned_path_labeling(10000, 200);
    test_pruned_path_labeling(100000, 200);
    test_pruned_path_labeling(1000000, 200);
}
