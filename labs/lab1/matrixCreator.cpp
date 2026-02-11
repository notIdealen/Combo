#include <vector>
#include <cstdlib>
#include <iostream>

using matrix = std::vector<std::vector<int>>;

matrix createMatrix(unsigned int n)
{
    matrix m(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m[i].push_back(i == j ? 0 : rand() % 50 + 1);

    return m;
}

void printMatrix(matrix& m)
{
    std::cout << '[' << std::endl;
    for (std::vector<int> v : m)
    {
        std::cout << "  [ ";
        for (int e : v)
            e < 10 ? std::cout << ' ' << e << ' ' : std::cout << e << ' ';
        std::cout << "]\n";
    }
    std::cout << ']' << std::endl;
}
