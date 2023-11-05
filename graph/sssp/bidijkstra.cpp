#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>

struct Edge
{
	std::size_t head;
	long long weight;
	bool operator()(const Edge& lhs, const Edge& rhs)
	{	// this function is intended for min-heap data structure
		return lhs.weight > rhs.weight;
	}
};

class Graph
{	
	public:
	using representation_type = std::vector<std::vector<Edge>>;
	using priority_queue_type = std::priority_queue<Edge, std::vector<Edge>, Edge>;
	Graph(std::size_t V) : adj_list(V, std::vector<Edge>{}), rev_adj_list(V, std::vector<Edge>{}) {}
	void add_edge(std::size_t tail, std::size_t head, long long weight)
	{
		adj_list[tail].push_back({head, weight});
		rev_adj_list[head].push_back({tail, weight});
	}
	double bidijkstra(const std::size_t, const std::size_t) const;
	private:
	// a disadvantage is using 2x space
	// different from kosaraju algorithm bidirectional dijkstra can be called very frequently,
	// that is why reverse adjacency list is a member variable instead of automatic variable in the function 
	representation_type adj_list;
	representation_type rev_adj_list;
	void explore(const std::size_t, const std::vector<Edge>&, std::vector<double>&, priority_queue_type&) const;
	double shortest_distance(const std::vector<bool>&, const std::vector<bool>&, const std::vector<double>&, const std::vector<double>&) const;
};

void Graph::explore(const std::size_t curr, const std::vector<Edge>& neighbours, std::vector<double>& cost, priority_queue_type& pq) const
{	// explore neighbours of current 
	for(const auto& neigbour : neighbours)
	{	// compute cost to go from initial vertex to neighbour 
		long long prospective_cost = cost[curr] + neigbour.weight;
		if(prospective_cost < cost[neigbour.head])
		{	// if found a better path
			cost[neigbour.head] = prospective_cost;
			pq.push({neigbour.head, prospective_cost});
		}
	}
}

double Graph::shortest_distance(const std::vector<bool>& fwd_visited, const std::vector<bool>& bwd_visited, const std::vector<double>& fwd_cost, const std::vector<double>& bwd_cost) const
{
	const std::size_t V = adj_list.size();
	double distance = std::numeric_limits<double>::infinity();
	for(std::size_t v=0; v<V; ++v)
	{
		if((fwd_visited[v] == true) || (bwd_visited[v] == true))
		{
			distance = std::min(distance, fwd_cost[v] + bwd_cost[v]);
		}
	}
	return distance;
}

double Graph::bidijkstra(const std::size_t s, const std::size_t t) const
{	// solves shortest path of road networks between two nodes in half time of dijkstra's algorithm in O(m lgn)
	const std::size_t V = adj_list.size();
	double inf = std::numeric_limits<double>::infinity();
	// create forward and backward visit logs
	std::vector<bool> fwd_visited(V, false);
	std::vector<bool> bwd_visited(V, false);
	// create forward and backward cost vectors
	std::vector<double> fwd_cost(V, inf);
	std::vector<double> bwd_cost(V, inf);
	fwd_cost[s] = 0.;
	bwd_cost[t] = 0.;
	// create forward and backward priority queues (min heap)
	priority_queue_type fwd_pq;
	priority_queue_type bwd_pq;
	fwd_pq.push({s, 0});
	bwd_pq.push({t, 0});
	// find s - v and v - t shortest paths
	while((fwd_pq.empty() == false) || (bwd_pq.empty() == false))
	{	// iterate until both of the queues are empty
		if(fwd_pq.empty() == false)
		{	// forward step
			auto fwd = fwd_pq.top().head;
			fwd_pq.pop();
			explore(fwd, adj_list[fwd], fwd_cost, fwd_pq);
			fwd_visited[fwd] = true;
			if(bwd_visited[fwd] == true)					return shortest_distance(fwd_visited, bwd_visited, fwd_cost, bwd_cost);
		}
		if(bwd_pq.empty() == false)
		{	// backward step
			auto bwd = bwd_pq.top().head;
			bwd_pq.pop();
			explore(bwd, rev_adj_list[bwd], bwd_cost, bwd_pq);
			bwd_visited[bwd] = true;
			if(fwd_visited[bwd] == true)					return shortest_distance(fwd_visited, bwd_visited, fwd_cost, bwd_cost);
		}
	}	// when while loop is broken due to condition, there is no s - t path
	return inf;
}

int main()
{	
	// file is 1-based
	std::ifstream file("dijkstraData.txt");							
	std::size_t V;
	file >> V;
	Graph graph(V);
	std::string line;
	while(file.eof() == false)
	{	// each row starts with the tail vertex
		// then continues with edges head,weight separated by space
		getline(file, line);
		std::istringstream buffer(line);
		std::size_t u, v;
		long long w;
		buffer >> u;
		char comma = ',';
		while(buffer >> v >> comma >> w)
		{	// adj list is 0-based				
			graph.add_edge(u - 1, v - 1, w);
		}
	}
	file.close();
	// compute shortest path distance from s to t (0-based)
	std::size_t s = 0, t = 6;
	std::cout << graph.bidijkstra(s, t) << "\n";
}
