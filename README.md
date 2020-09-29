# dag-reachability
Reachability and transitive reduction for DAGs

C++ algorithms for deciding reachability in directed acyclic graphs (DAG) and for computing the [transitive reduction](https://en.wikipedia.org/wiki/Transitive_reduction).
Reachability algorithms are based on [1] and [2].
The transitivity algorithm is based on [3].

## Usage

### Reachability:
```C++
#include "bloom_filter_plus.h"
...
std::vector<std::array<size_t,2>> edges = {{0,1}, {1,2}, {2,3}};
TR::bloom_filter_plus rq(edges.begin(), edges.end());
rq.query(0,2) == reach::reachable; // true
rq.query(2,0) == reach::unreachable; // true
...
```

### Transitive reduction:
```C++
#include "transitivity_reduction.h"
...
std::vector<std::array<size_t,2>> edges = {{0,1}, {1,2}, {2,3}, {0,2}};
const std::vector<std::array<size_t,2>> reduced_edges = TR::transitive_reduction(edges.begin(), edges.end());
reduced_edges = {{0,1}, {1,2}, {2,3}}; // true
...
```

## References
* [1]: [`Hilmi Yıldırım, Vineet Chaoji, Mohammed J. Zaki. GRAIL: a scalable index for reachability queries in very large graphs. In The VLDB Journal 2012.`](https://link.springer.com/article/10.1007/s00778-011-0256-4)
* [3]: [`Jiao Su, Qing Zhu, Hao Wei, Jeffrey Xu Yu. Reachability Querying: Can It Be Even Faster? In IEEE Transactions on Knowledge and Data Engineering 2017.`](https://ieeexplore.ieee.org/document/7750623)
* [3]: [`Xian Tang, Junfeng Zhou, Yaxian Qiu, Xiang Liu, Yunyu Shi, Jingwen Zhao. One Edge at a Time: A Novel Approach Towards Efficient Transitive Reduction Computation on DAGs . In IEEE Access 2020.`](https://ieeexplore.ieee.org/document/9006804)
