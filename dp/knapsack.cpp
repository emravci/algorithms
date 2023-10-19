#include <iostream>
#include <vector>
#include <fstream>
#include <functional>
#include <chrono>

struct Item
{
	std::size_t value, weight;
};

std::size_t knapsack(const std::size_t W, const std::vector<Item>& items)
{
	std::size_t n = items.size();
	// use of pointer to arrays halves the time it takes 
	// since it gets rid of extensive copy ops at the end of outer loop
	// call operator initializes arrays to zero
	auto Aprev = new std::size_t[W + 1]();
	auto A     = new std::size_t[W + 1]();
	for(std::size_t i=1; i<=n; ++i)
	{
		for(std::size_t x=0; x<=W; ++x)
		{
			const Item& item = items[i - 1];
			std::size_t case_where_item_included = (x < item.weight) ? (0) : (Aprev[x - item.weight] + item.value);
			A[x] = std::max(Aprev[x], case_where_item_included);
		}
		// swap pointers instead of time consuming Aprev = A operation with std::vector
		std::swap(Aprev, A);
	}
	// Aprev used to extract the answer instead of A due to unnecessary swap at the end of outer loop
	std::size_t AW = Aprev[W]; 
	delete[] Aprev, A;
	return AW;
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	std::size_t W = 0;		// knapsack capacity
	std::size_t n = 0;		// item count
	std::ifstream file("knapsack_big.txt");
	file >> W >> n;
	std::vector<Item> items(n);
	Item item;
	for(std::size_t i=0; i<n; ++i)
	{
		file >> item.value >> item.weight;
		items[i] = item;
	}
	std::cout << knapsack(W, items) << "\n";
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Computed in " << duration.count() << " seconds\n";
	return 0;
}
