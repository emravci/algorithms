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
	std::size_t v;
	long long w;
	bool operator()(const Edge& lhs, const Edge& rhs)
	{	// this function is intended for min-heap data structure
		return lhs.w > rhs.w;
	}
};


std::vector<double> dijkstra(const std::vector<std::vector<Edge>>& adj_list, const std::size_t s)
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
	return cost;
}

int main() 
{
	std::ifstream file("dijkstraData.txt");							// 1 based
	std::size_t V;
	file >> V;
	std::string line;
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
			--edge.v;												// 0 based
			adj_list[u - 1].push_back(edge);						// 0 based
		}
	}
	file.close();
	// compute shortest path distance from s to t 
	long long s = 0, t = 6;											// 0 based
	std::cout << dijkstra(adj_list, s)[t] << "\n";					// 0 based	
	return 0;
}
