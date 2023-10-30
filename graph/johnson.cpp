#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <queue>

struct Edge
{
	std::size_t head;
	long long weight;
	bool operator()(const Edge& lhs, const Edge& rhs)
	{	// this function is intended for min-heap data structure
		return lhs.weight > rhs.weight;
	}
};

struct SingleSourceShortestPaths : std::pair<bool, std::vector<double>>
{
	using vector_type = std::vector<double>;
	using super_type = std::pair<bool, vector_type>;
	SingleSourceShortestPaths(bool _has_negative_cycle, const vector_type& _cost)	:	super_type{_has_negative_cycle, _cost}	{}
	bool& has_negative_cycle()
	{	
		return this->first;		
	}
	const bool& has_negative_cycle() const
	{	
		return this->first;		
	}
	vector_type& cost()
	{	
		return this->second;
	}
	const vector_type& cost() const
	{	
		return this->second;
	}
};

struct AllPairsShortestPaths : std::pair<bool, std::vector<std::vector<double>>>
{
	using vector_type = std::vector<double>;
	using nested_vector_type = std::vector<vector_type>;
	using super_type = std::pair<bool, nested_vector_type>;
	AllPairsShortestPaths(bool _has_negative_cycle, const nested_vector_type& _cost)	:	super_type{_has_negative_cycle, _cost}	{}
	bool& has_negative_cycle()
	{	
		return this->first;		
	}
	const bool& has_negative_cycle() const
	{	
		return this->first;		
	}
	nested_vector_type& cost()
	{	
		return this->second;
	}
	const nested_vector_type& cost() const
	{	
		return this->second;
	}
};

class Graph
{
	public:
	Graph(const std::size_t V)	:	adj_list(V + 1, std::vector<Edge>())	
	{	// the very first step of johnson's algorithm running in O(n) time 
		// there is an artificial vertex in the graph, that is why size of adj list is V + 1
		for(std::size_t v=0; v<V; ++v)
		{	// for each vertex add an edge from source 0 to any other vertex v with weight 0
			this->add_edge(0, v + 1, 0);
		}
	}
	void add_edge(std::size_t u, std::size_t v, long long w)
	{	// actual vertices are 1-based
		adj_list[u].push_back({v, w});
	}
	SingleSourceShortestPaths bellman_ford(std::size_t s) const;
	SingleSourceShortestPaths::vector_type dijkstra(std::size_t s) const;
	AllPairsShortestPaths johnson();
	private:
	std::vector<std::vector<Edge>> adj_list;
};

SingleSourceShortestPaths Graph::bellman_ford(std::size_t s) const
{	// bellman-ford algorithm runs in O(nm) time 
	// also works with negative weight edges
	const std::size_t V = adj_list.size();
	std::vector<double> C(V, std::numeric_limits<double>::infinity());
	C[s] = 0;
	// compute cost from s to any node at most V - 1 edges
	for(std::size_t i=1; i<V; ++i)
	{	// edge count that might be used in any s - tail path
		for(std::size_t tail=0; tail<V; ++tail)
		{	// this and inner loop below runs in O(m) combined, traversing all edges 
			for(auto& edge : adj_list[tail])
			{	// visit every edge from tail
				C[edge.head] = std::min(C[edge.head], C[tail] + edge.weight);
			}			
		}
	}
	// check whether there is a negative weight cycle
	// run inner loops one more time to see if we get a better path
	// if so, then there is a negative weight cycle because any path cannot exceed |V - 1| edges without cycles
	for(std::size_t tail=0; tail<V; ++tail)
	{
		for(auto& edge : adj_list[tail])
		{
			if(C[edge.head] > C[tail] + edge.weight)
			{
				return {true, C};
			}
		}
	}
	return {false, C};
}

SingleSourceShortestPaths::vector_type Graph::dijkstra(std::size_t s) const
{	// runs in O(m lgn) time due to min heap data structure
	// different from other functions of the class, this one uses 0-based indices
	// on its cost vector since 0th index is created for johnson's algorithm
	// in other words, 0th index of this->adj_list does not exists here
	std::priority_queue<Edge, std::vector<Edge>, Edge> q;
	const std::size_t V = adj_list.size();
	double inf = std::numeric_limits<double>::infinity();
	std::vector<double> cost(V - 1, inf);
	// initialize for starting vertex
	q.push({s, 0});
	cost[s - 1] = 0;
	while(q.empty() == false)
	{
		std::size_t curr = q.top().head;
		q.pop();
		const auto& neighbours = adj_list[curr];
		for(const auto& neighbour : neighbours)
		{
			long long prospective_cost = cost[curr - 1] + neighbour.weight;
			if(prospective_cost < cost[neighbour.head - 1])
			{
				cost[neighbour.head - 1] = prospective_cost;
				q.push({neighbour.head, prospective_cost});
			}
		}
	}
	return cost;
}

AllPairsShortestPaths Graph::johnson()
{	// jonhson's algorithm runs in O(nm lgn) time 
	// which is much betten then floyd-warshall running in O(n^3) for sparse graphs
	// first step is handled in construction phase by creating an edge between s-v pairs with weight 0 running in O(n)
	// second step is running bellman-ford algorithm to detected negative weight cycles running in O(nm)
	// during construction, index 0 is assigned to source vertex
	auto [has_negative_cycle, vertex_weight] = this->bellman_ford(0);
	if(has_negative_cycle == true)
	{	// return empty nested vector when there is a negative weight cycle
		return {has_negative_cycle, AllPairsShortestPaths::nested_vector_type()};
	}
	// third step is converting weights to non-negative values running in O(m)
	const std::size_t V = adj_list.size();
	for(std::size_t tail=1; tail<V; ++tail)
	{
		for(auto& edge : adj_list[tail])
		{	// ce' = ce + pu - pv for an edge u -> v
			edge.weight += (vertex_weight[tail] - vertex_weight[edge.head]);
		}
	}
	// forth step is running dijkstra's algorithm on each vertex except the artificial vertex 0
	// running in O(nm lgn) due to n calls to dijkstra's algorithm running in O(m lgn)
	AllPairsShortestPaths::nested_vector_type cost(V - 1, AllPairsShortestPaths::vector_type());
	for(std::size_t s=1; s<V; ++s)
	{	
		cost[s - 1] = this->dijkstra(s);
	}
	// fifth step is correcting the adj list returning back to original weights running in O(m)
	// sixth step is correcting the path lengths running in O(n^2)
	for(std::size_t tail=1; tail<V; ++tail)
	{	// the following two loop differs because in the first loop it may skip some tail - head pairs
		// when there is no edge between tail and head, which corrupts the computation
		for(auto& edge : adj_list[tail])
		{	// fifth step running in O(m)
			// ce' = ce + pu - pv for an edge u -> v
			edge.weight += vertex_weight[edge.head] - vertex_weight[tail];
		}
		for(std::size_t head=1; head<V; ++head)
		{	// sixth step running in O(n^2)
			// as in dijkstra's algorithm, cost matrix is 0-based
			// different than rest of the class 
			cost[tail - 1][head - 1] += vertex_weight[head] - vertex_weight[tail];
		}
	}
	return {false, cost};
}

void print(const std::vector<std::vector<double>>& matrix)
{
	for(auto& vector : matrix)
	{
		for(auto& elem : vector)
		{
			std::cout << elem << " ";
		}
		std::cout << "\n";
	}	
}

int main()
{
	// file is 1-based
	std::ifstream file("johnson_data5.txt");
	std::size_t V, E;
	file >> V >> E;
	Graph graph(V);
	std::size_t u, v;
	long long w;
	for(std::size_t i=0; i<E; ++i)
	{	// graph works with 1-based vertices as well 
		// because of artificial vertex created for johnson's algorithm
		file >> u >> v >> w;
		graph.add_edge(u, v, w);
	}
	file.close();
	auto [has_negative_cycle, cost] = graph.johnson();
	std::cout << "has negative cycle? " << std::boolalpha << has_negative_cycle << "\n";
	print(cost);
	return 0;
}
