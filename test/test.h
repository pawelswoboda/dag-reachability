#pragma once

#include <string>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <vector>
#include <array>
#include <stack>
#include <queue>
#include "adjacency_list.h"
#include <iostream>
#include <iomanip>

inline void test(const bool predicate, const std::string msg)
{
    if(!predicate)
        throw std::runtime_error(msg);
}

std::vector<std::array<size_t,2>> construct_random_dag(const size_t nr_nodes, const size_t avg_out_degree)
{
    std::cout << "constructing graph with " << nr_nodes << " nodes and out degree of " << avg_out_degree << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::array<size_t,2>> edges;
    for(size_t i=0; i+1<nr_nodes; ++i)
    {
        std::uniform_int_distribution<size_t> distrib(i+1, nr_nodes-1);
        for(size_t j=0; j<avg_out_degree; ++j)
        {
            edges.push_back({i, distrib(gen)});
        }
    }
    std::cout << "#edges = " << edges.size() << "\n";

    auto edge_sort = [](const auto a, const auto b) {
        if(a[0] == b[0])
            return a[1] < b[1];
        return a[0] < b[0];
    };
    std::sort(edges.begin(), edges.end(), edge_sort);
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

    std::cout << "#unique edges = " << edges.size() << "\n";

    return edges;
}

bool reachable(const TR::adjacency_list& adj, const size_t i, const size_t j)
{
    assert(i < adj.nr_nodes());
    assert(j < adj.nr_nodes());

    std::vector<char> visited(adj.nr_nodes(), false);
    std::stack<size_t> s;
    s.push(i);

    while(!s.empty())
    {
        const size_t k = s.top();
        s.pop();
        visited[k] = true;
        if(k == j)
            return true;
        for(auto arc_it=adj.begin(k); arc_it!=adj.end(k); ++arc_it)
            if(!visited[*arc_it])
                s.push(*arc_it); 
    }

    return false; 
}

std::vector<size_t> shortest_path(const TR::adjacency_list& adj, const size_t i, const size_t j)
{
    assert(i < adj.nr_nodes());
    assert(j < adj.nr_nodes());
    assert(i != j);
    assert(reachable(adj, i, j) == true);


    std::vector<char> visited(adj.nr_nodes(), false);
    std::vector<size_t> parent(adj.nr_nodes(), std::numeric_limits<size_t>::max());
    parent[i] = i;
    std::queue<size_t> q;
    q.push(i);

    while(!q.empty())
    {
        const size_t k = q.front();
        q.pop();
        visited[k] = true;
        if(k == j)
        {
            std::vector<size_t> path;
            size_t l = k;
            while(parent[l] != l)
            {
                path.push_back(l);
                l = parent[l];
            }
            path.push_back(i);

            std::reverse(path.begin(), path.end());
            return path;
        }
        for(auto arc_it=adj.begin(k); arc_it!=adj.end(k); ++arc_it)
            if(!visited[*arc_it])
            {
                visited[*arc_it] = true;
                parent[*arc_it] = k;
                q.push(*arc_it); 
            }
    }

    throw std::runtime_error("not found any path");
    return {}; 
}

void print_confusion_matrix(const size_t tp, const size_t fp, const size_t fn, const size_t tn)
{
    const size_t width = std::log10(std::max({tp, fp, fn, tn}))+1;

    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+\n";

    std::cout << "| " << std::setw(width) << tp << " | " << std::setw(width) << fp << " |\n";

    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+\n";

    std::cout << "| " << std::setw(width) << fn << " | " << std::setw(width) << tn << " |\n";

    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+";
    for(size_t i=0; i<width + 2; ++i)
        std::cout << "-";
    std::cout << "+\n";
}
