// author: EMRE AVCI
// date: Nov 09, 2023
// references: 
//				[1] https://jlazarsfeld.github.io/ch.150.project/contents/
// 				[2] university of california san diego algorithms and data structures specialisation algorithms on graphs course on coursera
//				[3] university of freiburg - efficient route planning course

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath> 
#include <algorithm>
#include <chrono>
#include <type_traits>
#include <cassert>

// these two classes are created to increase readability and maintainability of Graph::shortcut function
struct CreateAndCountShortcuts {};
struct OnlyCountShortcuts {};

struct Edge
{
	std::size_t head;
	long long weight;
	bool operator()(const Edge& lhs, const Edge& rhs)
	{	// this overload is intended for min heap
		return lhs.weight > rhs.weight;
	}
};

class ContractedGraph
{	// this class has two member variables (adjacency lists)
	// one of them consists of upward edges only
	// the other consists of downward edges. 
	// downward graph is created in transpose form.
	friend class Graph;
	using representation_type = std::vector<std::vector<Edge>>;
	template<class Type> using priority_queue_type = std::priority_queue<Type, std::vector<Type>, Type>;
	public:
	ContractedGraph(std::size_t V = 0) : upward_adj_list(V, std::vector<Edge>()), downward_rev_adj_list(V, std::vector<Edge>()) {}
	long long search(const std::size_t, const std::size_t) const;
	private:
	representation_type upward_adj_list;
	representation_type downward_rev_adj_list;
	static std::vector<double> dijkstra(const representation_type&, const std::size_t, const std::size_t);
	void add_upward_edge(std::size_t tail, std::size_t head, long long weight)
	{
		upward_adj_list[tail].push_back({head, weight});
	}
	void add_downward_edge(std::size_t tail, std::size_t head, long long weight)
	{	// downward edges are created in transpose form to be used by modified bidirectional dijkstra
		downward_rev_adj_list[head].push_back({tail, weight});
	}
};

std::vector<double> ContractedGraph::dijkstra(const ContractedGraph::representation_type& adj_list, const std::size_t s, const std::size_t t)
{	// runs in O(m lgn) time due to min heap data structure
	priority_queue_type<Edge> q;
	std::size_t V = adj_list.size();
	std::vector<double> cost(V, std::numeric_limits<double>::infinity());
	// initialize for starting vertex
	q.push({s, 0});
	cost[s] = 0;
	while(q.empty() == false)
	{
		std::size_t curr = q.top().head;
		q.pop();
		// no need to explore further than target
		if(curr == t)			break;
		const auto& neighbours = adj_list[curr];
		for(const auto& neighbour : neighbours)
		{
			long long prospective_cost = cost[curr] + neighbour.weight;
			if(prospective_cost < cost[neighbour.head])
			{
				cost[neighbour.head] = prospective_cost;
				q.push({neighbour.head, prospective_cost});
			}
		}
	}
	return cost;
}

long long ContractedGraph::search(const std::size_t u, const std::size_t v) const
{	// modified bidirectional dijkstra algorithm
	assert(upward_adj_list.size() == downward_rev_adj_list.size() && "vertices count should be the same");
	const std::size_t V = upward_adj_list.size();
	auto cost_from_u = ContractedGraph::dijkstra(upward_adj_list, u, v);
	auto cost_from_v = ContractedGraph::dijkstra(downward_rev_adj_list, v, u);
	double cost = std::numeric_limits<double>::infinity();
	for(std::size_t w = 0; w < V; ++w)
	{	// find the vertex that causes minimum cost overall
		// forward and backward paths are connected at this vertex w
		cost = std::min(cost, cost_from_u[w] + cost_from_v[w]);
	}
	return static_cast<long long>(std::isinf(cost) ? (-1) : (cost));
}

class Graph
{	// in preprocess step graph is modified
	// in case, the graph is needed unmodified, by keeping track of added shortcut counts
	// and using pop_back original graph can be restored
	using representation_type = std::vector<std::vector<Edge>>;
	template<class Type> using priority_queue_type = std::priority_queue<Type, std::vector<Type>, Type>;
	public:
	Graph(std::size_t V) : adj_list(V, std::vector<Edge>()), rev_adj_list(V, std::vector<Edge>()) {}
	void add_edge(std::size_t tail, std::size_t head, long long weight)
	{
		adj_list[tail].push_back({head, weight});
		rev_adj_list[head].push_back({tail, weight});
	}
	ContractedGraph preprocess();
	private:
	struct Node
	{	// this class is for node ordering
		std::size_t id;
		long long importance;
		bool operator()(const Node& lhs, const Node& rhs)
		{	// this overload is intended for min heap
			return lhs.importance > rhs.importance;
		}
	};
	representation_type adj_list;
	representation_type rev_adj_list;
	// the following can be used to return to original graph
	// std::vector<std::size_t> shortcut_count;
	// std::vector<std::size_t> rev_shortcut_count;
	std::vector<long long> local_dijkstra(std::size_t, std::size_t, long long, const std::vector<bool>&) const;
	template<class Type> long long shortcut(const std::size_t, const std::vector<bool>&);
	long long contracted_neighbours(std::size_t, const std::vector<bool>&) const;
	inline long long edge_difference(const std::size_t v, const std::vector<bool>& contracted)
	{	// return edge_difference of uncontracted node v
		return shortcut<OnlyCountShortcuts>(v, contracted) - static_cast<long long>(adj_list[v].size() + rev_adj_list[v].size());
	}
};

std::vector<long long> Graph::local_dijkstra(std::size_t u, std::size_t v, long long Pmax, const std::vector<bool>& contracted) const
{	// search shortest paths from u excluding v until cost is greater than Pmax
	// v is the node to be contracted
	// u is the source vertex
	const std::size_t V = adj_list.size();
	std::vector<long long> cost(V, std::numeric_limits<long long>::max());
	priority_queue_type<Edge> q;
	q.push({u, 0});
	cost[u] = 0;
	while(q.empty() == false)
	{	
		if(q.top().weight > Pmax)		break;
		std::size_t curr = q.top().head;
		q.pop();
		for(const auto& neighbour : adj_list[curr])
		{	// explore in original graph excluding contracted nodes and vertex v
			// when a vertex is contracted, edges are not deleted instead a cheaper version (bool flag is used)
			if(contracted[neighbour.head] == false && neighbour.head != v)
			{
				long long prospective_cost = cost[curr] + neighbour.weight;
				if(prospective_cost < cost[neighbour.head])
				{
					cost[neighbour.head] = prospective_cost;
					q.push({neighbour.head, prospective_cost});
				}
			}
		}
	}
	return cost;
}

template<class Type> long long Graph::shortcut(const std::size_t v, const std::vector<bool>& contracted)
{	// two different instantiation is possible due to static_assert
	// those are CreateAndCountShortcuts and OnlyCountShortcuts
	// if contracted[vertex] is true, then that vertex and the edges adjacent to it do not exist
	long long count = 0;
	if(adj_list[v].empty() == true)		return count;
	long long w_max_weight = std::max_element(adj_list[v].begin(), adj_list[v].end(), [](const Edge& lhs, const Edge& rhs)
	{	// find the outgoing edge with the largest weight
		return lhs.weight < rhs.weight;
	})->weight;
	for(auto& u : rev_adj_list[v])
	{
		if(contracted[u.head] == false)
		{
			long long Pmax = u.weight + w_max_weight;
			// run local_dijkstra algorithm which returns an std::vector
			auto dist_from_u_to = local_dijkstra(u.head, v, Pmax, contracted);
			for(auto& w : adj_list[v])
			{	
				if(contracted[w.head] == false)
				{	// check if u - v - w path is the shortest
					long long Pw = u.weight + w.weight;
					if(dist_from_u_to[w.head] > Pw)
					{	// if so, add a shortcut edge from u to w with weight Pw
						if constexpr (std::is_same<Type, CreateAndCountShortcuts>::value)
						{	// with c++17 std::is_same_v can be used
							add_edge(u.head, w.head, Pw);
							// ++shortcut_count[u.head];
							// ++rev_shortcut_count[w.head];
						}
						else
						{
							static_assert(std::is_same<Type, OnlyCountShortcuts>::value, 
							"only CreateAndCountShortcuts and OnlyCountShortcuts types are allowed");
						}
						++count;
					}
				}				
			}
		}			
	}
	return count;
}

long long Graph::contracted_neighbours(std::size_t v, const std::vector<bool>& contracted) const
{
	long long count = 0;
	for(const auto& neighbour : adj_list[v])
	{
		if(contracted[neighbour.head] == true)		++count;
	}
	for(const auto& neighbour : rev_adj_list[v])
	{
		if(contracted[neighbour.head] == true)		++count;
	}
	return count;
}

ContractedGraph Graph::preprocess()
{
	const std::size_t V = adj_list.size();
	// find node ordering 
	priority_queue_type<Node> nodes;
	// instead of removing edges the following bool vector is used to differentiate contracted and uncontracted vertices
	std::vector<bool> contracted(V, false);
	for(std::size_t v = 0; v < V; ++v)
	{
		nodes.push({v, edge_difference(v, contracted)});
	}
	// contract nodes
	std::size_t order = 0;
	std::vector<std::size_t> contraction_order(V, 0);
	while(nodes.empty() == false)
	{	// contract in the order of increasing importance
		auto node = nodes.top();
		nodes.pop();
		// recompute importance of the node
		// edge difference and contracted_neighbours change in each iteration 
		node.importance = edge_difference(node.id, contracted) + contracted_neighbours(node.id, contracted);		
		if(nodes.empty() == false && node.importance > nodes.top().importance)
		{	// if it is not the smallest anymore, push it back then continue (lazy update)
			nodes.push(node);
			continue;
		}	// contract the node
		std::size_t v = node.id;
		shortcut<CreateAndCountShortcuts>(v, contracted);
		contracted[v] = true;
		contraction_order[v] = ++order;
	}
	// create upward and downward graphs
	ContractedGraph preprocessed(V);
	for(std::size_t u = 0; u < V; ++u)
	{
		for(auto& v : adj_list[u])
		{
			if(contraction_order[u] < contraction_order[v.head])
			{
				preprocessed.add_upward_edge(u, v.head, v.weight);
			} 
			else
			{
				preprocessed.add_downward_edge(u, v.head, v.weight);
			}
		}
	}
	return preprocessed;
}

int main()
{
	std::size_t V, E;
	std::cin >> V >> E;
	ContractedGraph preprocessed;
	{	// create roadmap inside a block so that after ContractedGraph is computed, memory is released
		Graph roadmap(V);
		std::size_t u, v;
		long long w;
		for(std::size_t e = 0; e < E; ++e)
		{
			std::cin >> u >> v >> w;
			roadmap.add_edge(u - 1, v - 1, w);
		}
		preprocessed = roadmap.preprocess();
	}
	std::size_t Q, u, v;
	std::cin >> Q;
	for(std::size_t q = 0; q < Q; ++q)
	{
		std::cin >> u >> v;
		std::cout << preprocessed.search(u - 1, v - 1) << "\n";
	}
	return 0;
}
