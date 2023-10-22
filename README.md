# algorithms
### huffman.cpp
## dynamic programming as dp
### knapsack.cpp
The algorithm runs in O(nW) time, where n is the number of items and W is the knapsack capacity. Space complexity of the naive algorithm is O(nW). However, this may not fit into memory for large capacities, say one million. In order to optimize the space used by the algorithm, instead of creating a matrix of size n x W, two arrays of size W are created and at the end of each iteration newly computed array is copied to previosly computed array, which decreases space complexity to O(W). However, this creates a bottleneck for the algorithm, since it takes a lot of time to copy elements of an array with O(W) time complexity when array size, knapsack capacity in this case, is large. To overcome this problem, pointers to arrays are used instead of std::vector arrays. With pointers to arrays, std::swap needs to be used, since straightforward copy operation would lead to both pointers pointing the same array, therefore, corrupting the computation and also memory leak, since pointer to one of the arrays is lost after copiying operation. Use of pointers instead of std::vector decreases running time by nearly half.
## graph
### dijkstra.cpp
### floyd_warshall.cpp
## minimum spanning tree as mst
### prim.cpp
