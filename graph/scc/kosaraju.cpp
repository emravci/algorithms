#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <algorithm>
#include <queue>
#include <cassert>
#include <chrono>

class Graph
{
	public:
	using nested_vector_type = std::vector<std::vector<std::size_t>>;
	Graph(std::size_t V)	:	adj_list(V, std::vector<std::size_t>())		{}
	void add_edge(std::size_t u, std::size_t v)
	{
		adj_list[u].push_back(v);
	}
	Graph transpose() const;
	nested_vector_type kosaraju() const;
	private:
	void dfs_pass_one(std::size_t, std::vector<bool>&, std::stack<std::size_t>&) const;
	void dfs_pass_two(std::size_t, std::vector<bool>&, std::vector<std::size_t>&) const;
	nested_vector_type adj_list;
};

Graph Graph::transpose() const
{	// create graph with reversed edges
	const std::size_t V = adj_list.size();
	Graph reverse_graph(V);
	for(std::size_t u=0; u<V; ++u) 
	{	// read edges u -> v
		for(auto& v : adj_list[u])
		{	// create new edges v -> u on reversed graph
			reverse_graph.add_edge(v, u);
		}
	}
	return reverse_graph;
}

void Graph::dfs_pass_one(std::size_t vertex, std::vector<bool>& visited, std::stack<std::size_t>& stack) const
{	// recursive depth-first-search algorithm
	visited[vertex] = true;
	for(auto& neighbour : adj_list[vertex]) 
	{
		if(visited[neighbour] == false) 
		{
			dfs_pass_one(neighbour, visited, stack);
		}
	}
	stack.push(vertex);
}

void Graph::dfs_pass_two(std::size_t vertex, std::vector<bool>& visited, std::vector<std::size_t>& scc) const
{	// iterative depth-first-search algorithm
	std::stack<std::size_t> stack;
	stack.push(vertex);
	while(stack.empty() == false)
	{
		std::size_t current = stack.top();
		stack.pop();
		if(visited[current] == false)
		{
			visited[current] = true;
			scc.push_back(current);
			for(auto& neighbour : adj_list[current])
			{
				if(visited[neighbour] == false)
				{
					stack.push(neighbour);
				}
			}
		}		
	}
}

Graph::nested_vector_type Graph::kosaraju() const
{	// kosaraju algorithm to compute strongly connected components in O(m + n) time 
	const std::size_t V = this->adj_list.size();
	std::vector<bool> visited(V, false);
	std::stack<std::size_t> stack;
	// pass one : run dfs on the original graph
	for(std::size_t v=0; v<V; ++v) 
	{
		if(visited[v] == false) 
		{
			this->dfs_pass_one(v, visited, stack);
		}
	}
	// pass two : run dfs on the reversed graph using the stack created in pass one
	// stack is used to make sure that second dfs runs in decreasing order 
	// with respect to finishing times of vertices during first dfs
	Graph reversed = this->transpose();
	std::fill(visited.begin(), visited.end(), false);
	Graph::nested_vector_type strongly_connected_components;
	while(stack.empty() == false) 
	{
		std::size_t v = stack.top();
		stack.pop();
		if(visited[v] == false)
		{
			std::vector<std::size_t> scc;
			reversed.dfs_pass_two(v, visited, scc);
			strongly_connected_components.push_back(scc);
		}
	}
	return strongly_connected_components;
}

template<class Type>
std::vector<Type> largest(const std::vector<Type>& array, std::size_t k)
{	// computest the largest k elements in an array in O(n lgk) time
	// check if array size is larger than k
	assert(array.size() >= k && "array size is smaller than k");
	// initialize priority queue (mean heap) with the first k elements
	std::priority_queue<Type, std::vector<Type>, std::greater<Type>> q(array.begin(), array.begin() + k);
	for(std::size_t i=k, N=array.size(); i<N; ++i)
	{   // iterate from k to N
		if(array[i] > q.top())
		{	// extract min and push candidate if candidate is greater than minimum
			q.pop();
			q.push(array[i]);
		}
	}	
	// return answer in decreasing order
	std::vector<Type> _largest(k);
	for(auto it=_largest.rbegin(), end=_largest.rend(); it!=end; ++it)
	{	// since min heap is used, iterate from end to beginning 
		(*it) = q.top();
		q.pop();
	}
	return _largest;
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	// read the file (1-based)
	std::ifstream file("SCC.txt");
	std::size_t V, E;
	file >> V >> E;
	Graph graph(V);
	for(std::size_t i=0; i<E; ++i)	
	{	// 0-based
		std::size_t u, v;
		file >> u >> v;
		graph.add_edge(u - 1, v - 1);
	}
	file.close();
	// compute strongly connected components
	auto SCCs = graph.kosaraju();
	// create an array of sizes of strongly connected components
	std::vector<std::size_t> sizes_of_SCCs(SCCs.size());
	std::transform(SCCs.begin(), SCCs.end(), sizes_of_SCCs.begin(), [](const std::vector<std::size_t>& scc)
	{	
		return scc.size();
	});
	// print sizes of 5 largest SCC
	auto largest5 = largest(sizes_of_SCCs, 5);
	for(auto& elem : largest5)
	{
		std::cout << elem << " ";
	}
	std::cout << "\n";
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Computed in " << duration.count() << " seconds\n";
	return 0;
}
