#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>

struct Edge
{	// edge between nodes u and v with weight w
	long long u, v, w;
};

class SquareMatrix	
{	
	public:
	SquareMatrix(size_t N)	:	_N{N}
	{	// constructor 
		_elements = new double*[_N];
		for(size_t i=0; i<_N; ++i)
		{
			_elements[i] = new double[_N];
		}
	}
	SquareMatrix(size_t N, double v)	:	_N{N}
	{	// constructor with initial value
		_elements = new double*[_N];
		for(size_t i=0; i<_N; ++i)
		{
			_elements[i] = new double[_N];
			std::fill_n(_elements[i], _N, v);
		}		
	}
	// delete copy constructor and assignment operator
	SquareMatrix(const SquareMatrix&) = delete;
	SquareMatrix& operator=(const SquareMatrix&) = delete;
	double& operator()(size_t i, size_t j)
	{	// for indexing 
		return _elements[i][j];
	}
	const double& operator()(size_t i, size_t j) const
	{	// for indexing
		return _elements[i][j];
	}
	static void swap(SquareMatrix& lhs, SquareMatrix& rhs)
	{	// swaps two SquareMatrix obj in O(1) due pointers
		std::swap(lhs._elements, rhs._elements);
		std::swap(lhs._N, rhs._N);
	}
	double min_nondiagonal() const
	{	// finds shortest shortest path distance
		// runs in O(n^2) time
		double shortest_path_distance = std::numeric_limits<double>::infinity();
		for(long long i=1; i<_N; ++i)
		{
			for(long long j=1; j<_N; ++j)
			{
				if(i != j && shortest_path_distance > _elements[i][j])
				{	// i != j prevents the chance of returing 0
					shortest_path_distance = _elements[i][j];
				}
			}
		}
		return shortest_path_distance;
	}
	~SquareMatrix()
	{	// destructor
		for(size_t i=0; i<_N; ++i)
		{
			delete[] _elements[i];
		}
		delete[] _elements;
	}
	private:
	double** _elements;
	size_t _N;
};

long long floyd_warshall(const long long V, const std::vector<Edge>& edges)
{	// returns "the shortest shortest path"
	// it runs in O(n^3) time
	// construct base case
	const long long E = edges.size();
	const double inf = std::numeric_limits<double>::infinity();
	// SquareMatrix class is used to decrease space requirements
	// from O(V^3) to O(V^2)
	SquareMatrix Aprev(V + 1, inf);
	SquareMatrix A(V + 1);
	for(long long i=1; i<=V; ++i)
	{	// when u = v
		Aprev(i, i) = 0;
	}
	for(long long i=0; i<E; ++i)
	{	// when there is an edge between u and v
		Edge edge = edges[i];
		Aprev(edge.u, edge.v) = edge.w;
	}
	for(long long k=1; k<=V; ++k)
	{	// fw loops
		for(long long i=1; i<=V; ++i)
		{
			for(long long j=1; j<=V; ++j)
			{	
				A(i, j) = std::min(Aprev(i, j), Aprev(i, k) + Aprev(k, j));
			}
		}
		SquareMatrix::swap(Aprev, A);
	}
	// due to unnecessary swap at the end of outer loop, swap again
	SquareMatrix::swap(Aprev, A);
	for(long long i=1; i<=V; ++i)
	{	// checks for negative edge cycles
		if(A(i, i) < 0)
		{
			return 0;
		}
	}
	return static_cast<long long>(A.min_nondiagonal());
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	std::ifstream file("g3.txt");
	long long V, E;
	file >> V >> E;
	std::vector<Edge> edges(E);
	Edge edge;
	for(long long i=0; i<E; ++i)
	{
		file >> edge.u >> edge.v >> edge.w;
		edges[i] = edge;
	}
	file.close();
	std::cout << floyd_warshall(V, edges) << "\n";
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Computed in " << duration.count() << " seconds\n";
	return 0;
}
