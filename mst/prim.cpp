#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>

struct Edge
{	// v is the head of directed edge
	// w is the weight of that edge
	long long v, w;	
	bool operator()(const Edge& lhs, const Edge& rhs)	
	{	// this function is intended for min-heap data structure
		return lhs.w > rhs.w;
	}
};

long long prim(const std::vector<std::vector<Edge>>& adj_list)
{	// prim's minimum spanning tree algorithm with O(m lgn) due to heap
	std::priority_queue<Edge, std::vector<Edge>, Edge> q;
	// push 0th vertex with 0 cost
	q.push({0, 0});
	std::size_t V = adj_list.size();
	std::vector<bool> visited(V, false);
	long long cost = 0;
	while(q.empty() == false)
	{
		Edge current = q.top();
		q.pop();
		if(visited[current.v] == false)
		{
			visited[current.v] = true;
			cost += current.w;
			for(auto& neighbour : adj_list[current.v])	
			{
				if(visited[neighbour.v] == false)	
				{
					q.push(neighbour);
				}
			}
		}
	}
	return cost;
}


int main()
{
	std::ifstream file("edges.txt");
	std::size_t V = 0, E = 0;
	file >> V >> E;
	
	std::vector<std::vector<Edge>> adj_list(V, std::vector<Edge>());
	long long u, v, w;
	for(std::size_t i=0; i<E; ++i)		
	{	// adjacency list is 0-based
		file >> u >> v >> w;
		adj_list[u - 1].push_back({v - 1, w});
		adj_list[v - 1].push_back({u - 1, w});
	}
	std::cout << prim(adj_list) << "\n";
	return 0;
}
