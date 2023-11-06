#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <algorithm>

struct Cartesian
{
	long long x, y;
	static long long euclidian(const Cartesian& lhs, const Cartesian& rhs)
	{
		return std::sqrt((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y)); 
	}
};

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
	Graph(const std::size_t V) : vertices(V), adj_list(V, std::vector<Edge>()), heuristic_cost(V), cost(V)	{}
	void add_vertex(std::size_t v, long long x, long long y)
	{
		vertices[v] = {x, y};
	}
	void add_edge(std::size_t tail, std::size_t head, long long weight)
	{
		adj_list[tail].push_back({head, weight});
	}
	long long a_star(const std::size_t, const std::size_t) const;
	private:
	std::vector<Cartesian> vertices;
	std::vector<std::vector<Edge>> adj_list;
	// the following included as member variables to decrease time when multiple queries demanded on the same graph
	mutable std::vector<long long> heuristic_cost;
	mutable std::vector<long long> cost;
};


long long Graph::a_star(const std::size_t s, const std::size_t t) const
{
	const std::size_t V = adj_list.size();
	for(std::size_t v = 0; v < V; ++v)
	{	// compute euclidian distance from v to t
		heuristic_cost[v] = Cartesian::euclidian(vertices[v], vertices[t]);
	}
	std::priority_queue<Edge, std::vector<Edge>, Edge> q;
	long long inf = std::numeric_limits<long long>::max();
	std::fill(cost.begin(), cost.end(), inf);
	// initialize for starting vertex
	q.push({s, 0});
	cost[s] = 0;
	while(q.empty() == false)
	{
		std::size_t curr = q.top().head;
		q.pop();
		// since this is a directed search algorithm and shortest path between
		// s and t is required, no need to continue search after reaching t 
		if(curr == t)				return cost[t];
		const auto& neighbours = adj_list[curr];
		for(const auto& neighbour : neighbours)
		{
			long long prospective_cost = cost[curr] + neighbour.weight;
			if(prospective_cost < cost[neighbour.head])
			{	// add to queue with tentative = prospective + heuristic
				// tentative cost is giving directions to search
				q.push({neighbour.head, prospective_cost + heuristic_cost[neighbour.head]});
				cost[neighbour.head] = prospective_cost;
			}
		}
	}	// path does not exist
	return -1;
}

int main()
{
	std::size_t V, E;
	std::cin >> V >> E;
	Graph graph(V);
	long long x, y;
	for(std::size_t v = 0; v < V; ++v)
	{
		std::cin >> x >> y;
		graph.add_vertex(v, x, y);
	}
	std::size_t tail, head;
	long long weight;
	for(std::size_t e = 0; e < E; ++e)
	{
		std::cin >> tail >> head >> weight;
		graph.add_edge(tail - 1, head - 1, weight);
	}
	std::size_t Q;
	std::cin >> Q;
	std::size_t s, t;
	for(std::size_t q = 0; q < Q; ++q)
	{
		std::cin >> s >> t;
		std::cout << graph.a_star(s - 1, t - 1) << "\n";
	}
	return 0;
}
