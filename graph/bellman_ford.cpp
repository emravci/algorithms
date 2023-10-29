#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <string>
#include <sstream>

struct Edge
{
	std::size_t head;
	long long weight;
};

struct SingleSourceShortestPaths : std::pair<bool, std::vector<double>>
{	// stands for single-source shortest paths
	using super_type = std::pair<bool, std::vector<double>>;
	SingleSourceShortestPaths(bool _has_negative_cycle, const std::vector<double>& _cost)	:	super_type{_has_negative_cycle, _cost}	{}
	// in this simple case, instead of creating inline functions, 
	// references to variables could be used as public member variables (bool& has_negative_cycle etc)
	// however, in general this is a bad approach, since references cannot be changed after construction
	bool& has_negative_cycle()
	{	
		return this->first;		
	}
	const bool& has_negative_cycle() const
	{	
		return this->first;		
	}
	std::vector<double>& cost()
	{	
		return this->second;
	}
	const std::vector<double>& cost() const
	{	
		return this->second;
	}
};

SingleSourceShortestPaths bellman_ford(const std::vector<std::vector<Edge>> adj_list, const std::size_t s)
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

int main()
{
	// file is 1-based
	std::ifstream file("dijkstraData.txt");		
	std::size_t V;
	file >> V;
	std::vector<std::vector<Edge>> adj_list(V, std::vector<Edge>());
	std::string line;
	std::size_t tail;
	while(file.eof() == false)
	{	// each row starts with the tail vertex
		// then continues with edges head,weight separated by space
		getline(file, line);
		std::istringstream buffer(line);
		Edge edge;
		buffer >> tail;
		char comma = ',';
		while(buffer >> edge.head >> comma >> edge.weight)
		{	// adj list is 0-based
			--edge.head;									
			adj_list[tail - 1].push_back(edge);					
		}
	}
	file.close();
	// compute shortest path distance from s to t (0-based)
	std::size_t s = 0, t = 6;					
	// auto [has_negative_cycle, cost] = bellman_ford(adj_list, s);
	// std::cout << cost[t] << "\n";
	std::cout << bellman_ford(adj_list, s).cost()[t] << "\n";
	return 0;
}
