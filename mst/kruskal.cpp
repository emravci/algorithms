#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <chrono>

class DisjointSet
{
	public:
	DisjointSet(std::size_t n)	:	_parent(n),		_rank(n, 0),		_count{n}
	{	// at the beginning every node points to itself
		std::iota(_parent.begin(), _parent.end(), 0);
	}
	std::size_t find(std::size_t i)
	{	// find the leader of the set containing i
		std::size_t leader = i;
		while(_parent[leader] != leader)
		{
			leader = _parent[leader];
		}
		while(_parent[i] != i)
		{	// path compression
			const std::size_t parent_i = _parent[i];
			_parent[i] = leader;
			i = parent_i;
		}		
		return leader;
	}
	bool unite(const std::size_t i, const std::size_t j)
	{	// union i and j so that leader of the set with lower rank should point the other
		const std::size_t leader_i = find(i);
		const std::size_t leader_j = find(j);
		if(leader_i != leader_j)
		{	// when disjoint sets are united, number of clusters decreases by 1
			--_count;
			if(_rank[leader_i] > _rank[leader_j])
			{
				_parent[leader_j] = leader_i;
			}
			else if(_rank[leader_i] < _rank[leader_j])
			{
				_parent[leader_i] = leader_j;
			}
			else
			{	// when ranks equal rank of the new leader is increased by 1
				_parent[leader_j] = leader_i;
				++_rank[leader_i];
			}
			return true;
		}	// if i and j belong to the same set return false
		return false;
	}
	std::size_t count() const
	{	// number of sets
		return _count;
	}
	private:
	std::vector<std::size_t> _parent;
	std::vector<std::size_t> _rank;
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
	auto start = std::chrono::high_resolution_clock::now();
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
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Computed in " << duration.count() << " milliseconds\n";
	return 0;
}
