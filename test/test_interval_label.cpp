#include "test.h"
#include "interval_label.h"

using namespace TR;

void test_interval_label(const size_t nr_nodes, const size_t out_degree)
{
    const auto edges = construct_random_dag(nr_nodes, out_degree);
    interval_label i(edges.begin(), edges.end());

    for(const auto e : edges)
        test(i.query(e[1], e[0]) == reach::unreachable, "false positive in reachability of random graph");

    const auto test_edges = construct_random_dag(nr_nodes, out_degree);
    const adjacency_list adj(edges.begin(), edges.end());
    size_t tp = 0, fp = 0, fn = 0, tn = 0;;
    for(const auto e : test_edges)
    {
        const bool gt = reachable(adj, e[0], e[1]);
        const reach prediction = i.query(e[0], e[1]);

        if(prediction == reach::reachable)
            test(gt, "reachability error on random test arcs");
        if(prediction == reach::unreachable)
            test(!gt, "reachability error on random test arcs");

        if(gt && prediction == reach::reachable)
            tp++;
        if(!gt && prediction == reach::undefined)
            fp++;
        if(gt && prediction == reach::undefined)
            fn++;
        if(!gt && prediction == reach::unreachable)
            tn++;
    }

    std::cout << "confusion matrix on random test edges:\n";
    print_confusion_matrix(tp, fp, fn, tn);
    //test(fn == 0, "random edge reachability: unreachable predictions must always be correct.");
}

int main(int argc, char** argv)
{
    test_interval_label(10, 2);
    test_interval_label(100, 2);
    test_interval_label(1000, 2);
    test_interval_label(10000, 2);
    test_interval_label(100000, 2);
    test_interval_label(1000000, 2);

    test_interval_label(10, 200);
    test_interval_label(100, 200);
    test_interval_label(1000, 200);
    test_interval_label(10000, 200);
    test_interval_label(20000, 200);
    test_interval_label(30000, 200);
    test_interval_label(40000, 200);

}
