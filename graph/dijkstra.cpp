#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>      // std::ifstream
#include <string>
#include <sstream>
#include <limits>

struct Edge
{	// v is the head of directed edge
	// w is the weight of that edge
	long long v, w;
	bool operator()(const Edge& lhs, const Edge& rhs)
	{	// this function is intended for min-heap data structure
		return lhs.w > rhs.w;
	}
};


long long dijkstra(const std::vector<std::vector<Edge>>& adj_list, const long long s, const long long t)
{	// runs in O(m lgn) time due to min heap data structure
	std::priority_queue<Edge, std::vector<Edge>, Edge> q;
	std::size_t V = adj_list.size();
	double inf = std::numeric_limits<double>::infinity();
	std::vector<double> cost(V, inf);
	// std::vector<bool> visited(V, false);
  	// std::vector<int> parent(V, -1);
	// initialize for starting vertex
	q.push({s, 0});
	cost[s] = 0;
	while(q.empty() == false)
	{
		long long curr = q.top().v;
		q.pop();
		const auto& neighbours = adj_list[curr];
		for(const auto& neighbour : neighbours)
		{
			long long prospective_cost = cost[curr] + neighbour.w;
			if(prospective_cost < cost[neighbour.v])
			{
				cost[neighbour.v] = prospective_cost;
				// parent[neighbour.v] = curr;
				q.push({neighbour.v, prospective_cost});
			}
		}
	}
	return static_cast<long long>(cost[t]);
}

int main() 
{
	// the vertex count is given
	int V = 200;
  	// file is 1-based
	std::ifstream file("dijkstraData.txt");
	std::string line;
  	// adjacency list is 0-based
	std::vector<std::vector<Edge>> adj_list(V, std::vector<Edge>());
	while(file.eof() == false)
	{	// each row starts with the tail vertex
		// then continues with edges head,weight separated by space
		getline(file, line);
		std::istringstream buffer(line);
		long long u;
		Edge edge;
		buffer >> u;
		char comma = ',';
		while(buffer >> edge.v >> comma >> edge.w)
		{
			--edge.v;											
			adj_list[u - 1].push_back(edge);				
		}
	}
	file.close();
	// compute shortest path distance from s to t 
	long long s = 0, t = 6;										
	std::cout << dijkstra(adj_list, s, t) << "\n";			
	return 0;
}
