#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

class DisjointSet
{
	public:
	DisjointSet(std::size_t n)	:	_leader(n, -1),		_count{n}		{}
	std::size_t find(std::size_t i)
	{	// find the leader of the set containing i
		return (_leader[i] == -1) ? i : find(_leader[i]);
	}
	bool unite(const std::size_t i, const std::size_t j)
	{	// union i and j so that leader of the set containing j should point i
		const std::size_t leader_i = find(i);
		const std::size_t leader_j = find(j);
		if(leader_i != leader_j)
		{	// when disjoint sets are united, number of clusters decreases by 1
			--_count;
			_leader[leader_j] = leader_i;
			return true;
		}
		return false;
	}
	std::size_t count() const
	{	// number of sets
		return _count;
	}
	private:
	std::vector<std::size_t> _leader;
	std::size_t _count;
};

struct Edge
{	// edge is from u to v with a cost of w
	std::size_t u, v;
	long long w;
};

long long kruskal(const std::size_t V, std::vector<Edge>& edges)
{	// computes minimum spanning tree cost in O(m lgm) time
	long long cost = 0;
	DisjointSet ds(V);
	std::sort(edges.begin(), edges.end(), [](const Edge& lhs, const Edge& rhs)
	{	// sort in increasing order
		return lhs.w < rhs.w;
	});
	for(auto& edge : edges)
	{
		if(ds.unite(edge.u, edge.v) == true)
		{	// when they are not in the same set, that is, no cycle
			cost += edge.w;
		}
		if(ds.count() == 1)
		{	// when there is only one set remaining, that is, mst is constructed
			return cost;
		}
	}
	return -1;
}

int main()		
{
	// file is 1-based
	std::ifstream file("edges.txt");
	std::size_t V = 0, E = 0;
	file >> V >> E;
	std::vector<Edge> edges(E);
	Edge edge;
	for(std::size_t i=0; i<E; ++i)		
	{	// Edge vector is 0-based
		file >> edge.u >> edge.v >> edge.w;
		--edge.u;
		--edge.v;
		edges[i] = edge;
	}
	std::cout << kruskal(V, edges) << "\n";
	return 0;
}
