#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <chrono>

class Graph
{
	public:
	Graph(std::size_t V)	:	adj_list(V, std::vector<std::size_t>())		{}
	void add_edge(std::size_t u, std::size_t v)
	{
		adj_list[u].push_back(v);
	}
	Graph transpose() const;
	std::vector<std::size_t> kosaraju() const;
	private:
	void dfs_pass_one(std::size_t, std::vector<bool>&, std::stack<std::size_t>&) const;
	void dfs_pass_two(std::size_t, std::vector<std::size_t>&, const std::size_t) const;
	std::vector<std::vector<std::size_t>> adj_list;
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

void Graph::dfs_pass_two(std::size_t vertex, std::vector<std::size_t>& scc_id, const std::size_t ID) const
{	// iterative depth-first-search algorithm
	std::stack<std::size_t> stack;
	stack.push(vertex);
	while(stack.empty() == false)
	{
		std::size_t current = stack.top();
		stack.pop();
		if(scc_id[current] == 0)
		{	// id = 0 refers to unvisited nodes
			scc_id[current] = ID;
			for(auto& neighbour : adj_list[current])
			{
				if(scc_id[neighbour] == 0)
				{	// id = 0 refers to unvisited nodes
					stack.push(neighbour);
				}
			}
		}		
	}
}

std::vector<std::size_t> Graph::kosaraju() const
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
	// initilize vector with 0
	std::vector<std::size_t> strongly_connected_components_id(V, 0);
	// any node part of any strongly connected components must have a value greater than 0
	// in other words 0 refers to unvisited nodes
	std::size_t ID = 0;
	while(stack.empty() == false) 
	{
		std::size_t v = stack.top();
		stack.pop();
		if(strongly_connected_components_id[v] == 0)
		{
			reversed.dfs_pass_two(v, strongly_connected_components_id, ++ID);
		}
	}
	return strongly_connected_components_id;
}

struct Clause
{	// x v y
	// negative numbers refer to complement of a variable
	long long x, y;
	static std::size_t map(long long z)
	{	// maps variable to index since no index can be negative
		return (z > 0) ? (2 * (z - 1)) : (-2 * z - 1);
	}
};

bool is_2satisfiable(const std::size_t N, const std::vector<Clause>& clauses)
{	// for reference, please see https://cp-algorithms.com/graph/2SAT.html
	// there are N variables and
	// there needs to be two nodes for each variable (one for 'x' and one for 'not x')
	// even entries (0, 2, 4,..) represents 'x'; whereas odd represents 'not x'
	// create a graph of 2SAT problem
	Graph graph(2 * N);	
	for(auto& clause : clauses)
	{	// (a v b) is equivalent to (-a -> b) and (-b -> a)
		graph.add_edge(Clause::map(-clause.x), Clause::map(clause.y));
		graph.add_edge(Clause::map(-clause.y), Clause::map(clause.x));
	}
	// kosaraju function will return SCC ID's of individual nodes
	auto SCC_IDs = graph.kosaraju();
	for(std::size_t i=0; i<2*N; i+=2)
	{	// if two consecutive nodes share the same ID, meaning, same SCC, it is unsat
		if(SCC_IDs[i] == SCC_IDs[i + 1])
		{
			return false;
		}
	}
	return true;
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	for(int i=1; i<7; ++i)
	{
		std::string file_name{"sat" + std::to_string(i) + ".txt"};
		std::ifstream file(file_name);
		// number of variables and number of clauses
		std::size_t N, C;
		file >> N >> C;
		std::vector<Clause> clauses(C);
		Clause clause;
		for(std::size_t i=0; i<C; ++i)
		{
			file >> clause.x >> clause.y;
			clauses[i] = clause;			
		}
		file.close();
		std::cout << is_2satisfiable(N, clauses);
	}
	std::cout << "\n";
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Computed in " << duration.count() << " seconds\n";
	return 0;
}
