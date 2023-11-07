# algorithms
## dynamic programming as dp
### knapsack.cpp
The algorithm runs in O(nW) time, where n is the number of items and W is the knapsack capacity. Space complexity of the naive algorithm is O(nW). However, this may not fit into memory for large capacities, say one million. In order to optimize the space used by the algorithm, instead of creating a matrix of size n x W, two arrays of size W are created and at the end of each iteration newly computed array is copied to previosly computed array, which decreases space complexity to O(W). However, this creates a bottleneck for the algorithm, since it takes a lot of time to copy elements of an array with O(W) time complexity when array size, knapsack capacity in this case, is large. To overcome this problem, pointers to arrays are used instead of std::vector arrays. With pointers to arrays, std::swap needs to be used, since straightforward copy operation would lead to both pointers pointing the same array, therefore, corrupting the computation and also memory leak, since pointer to one of the arrays is lost after copiying operation. Use of pointers instead of std::vector decreases running time by nearly half.
## graph
## all pairs shortest paths as apsp
### floyd_warshall.cpp
### johnson.cpp
## minimum spanning tree as mst
### kruskal.cpp
### prim.cpp
Prim's algorithm runs in O(m lgn) time, when heap data structure is used. The algorithm works only for undirected graphs.
## strongly connected components as scc
### kosaraju.cpp
## single source shortest paths as sssp
### a_star.cpp
A* search algorithm is a directed search algorithm and it performs much better than dijkstra's algorithm. However, it requires heuristic costs from any vertex v to t, which can be euclidian or manhattan distance, when coordinates of vertices are known.
### bellman_ford.cpp
### bidijkstra.cpp
Bidirectional dijkstra's algorithm is very similar to dijkstra's algorithm. Only difference is the instead of single dijkstra, two dijkstra search are used from source and target. The algorithm finds shortest path in half time to solve with dijkstra's algorithm, since instead of searching in a circle of 2r radius, 2 circles of r radius are searched.
### dijkstra.cpp
Dijkstra's algorithm is single-source shortest paths algorithm. That is, it computes the shortest paths from the given source vertex to any vertex in the graph. The algorithm runs on directed graphs with non-negative edge costs. Dijkstra's algorithm always terminates, but in some cases the paths it computes will not be the shortest paths, even when the graph contains negative cycle. Naive implementation runs in O(mn) time, whereas, with the use of heap structure, the running time is O(m lgn) time. Along the computation of shortest path distance, the shortest path can also be computed using a secondary array named parent in the code.
### floyd_warshall.cpp
## greedy
### huffman.cpp
Naive algorithm would work in O(n^2) time, whereas, with the use of heap, running time decreases to O(n lgn) time. The aim of the code is to compute minimum and maximum lengths of huffman coding. Length, here, means the distance from root to leaves, when implemented as tree. In other words, it is the depth of a leaf. In huffman algorithm, when two braches merge, a new node is created consisting of all the symbols below that node. The frequency of the new node is simply the combined frequencies. Thus, in order to compute the lengths, instead of creating the actual tree, a nontrivial class is created with a container and a variable to denote the frequency. Container contains the symbols below the node. Container type is selected as std::list since two std::list objects can be merged in O(1) time. After merging the containers into new one and computing the combined frequency, length of each symbol contained in the container is incremented by 1, since merging causes depth of each symbol to increase by 1. This incrementation operation is actually a hidden loop, so it may affect overall running complexity of the algorithm, when the tree resembles more a chain than a balanced tree. In the worst case, the tree looks like a chain and minimum and maximum lengths are 1 and (n - 1) respectively. In that case, algorithm would perform in O(n^2) time. However, on average, the algorithm performs in O(n lgn) time, and the increment operation is not a bottleneck for the algorithm.

