#include "test.h"
#include "bloom_filter_label.h"

using namespace TR;

void test_bloom_filter_label(const size_t nr_nodes, const size_t out_degree)
{
    const auto edges = construct_random_dag(nr_nodes, out_degree);
    bloom_filter_label b(edges.begin(), edges.end());

    size_t nr_wrong_negatives = 0;
    for(const auto e : edges)
    {
        //test(b.query(e[0], e[1]) == reach::reachable, "false negative in reachability of random graph");
    }

    const auto test_edges = construct_random_dag(nr_nodes, out_degree);
    const adjacency_list adj(edges.begin(), edges.end());
    size_t tp = 0, fp = 0, fn = 0, tn = 0;;
    for(const auto e : test_edges)
    {
        const bool gt = reachable(adj, e[0], e[1]);
        const reach prediction = b.query(e[0], e[1]);

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
    //std::cout << "on random test edges:\n";
    //std::cout << "#errors = " << nr_errors << ", #unreachable = " << nr_unreachable << "\n";
    //std::cout << "error % = " << 100.0*double(nr_errors)/double(nr_unreachable) << "\n";
}

int main(int argc, char** argv)
{
    test_bloom_filter_label(10, 2);
    test_bloom_filter_label(100, 2);
    test_bloom_filter_label(1000, 2);
    test_bloom_filter_label(10000, 2);
    test_bloom_filter_label(100000, 2);
    test_bloom_filter_label(1000000, 2);

    test_bloom_filter_label(10, 200);
    test_bloom_filter_label(100, 200);
    test_bloom_filter_label(1000, 200);
    test_bloom_filter_label(10000, 200);
    test_bloom_filter_label(20000, 200);
    test_bloom_filter_label(30000, 200);
    test_bloom_filter_label(40000, 200);

}

