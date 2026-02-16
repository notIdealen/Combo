//g++ qap.cpp -o qap.exe
//решить для n=10, n=15 Оценить время для n=20, n=50
// расчитать размер матрицы, если м>15, то запускать функцию расчета времени?
#include <iostream>
#include <vector>
// #include <algorithm>
#include <limits>
#include <chrono>
#include "rmff.cpp"
#include "matrixCreator.cpp"

using namespace std;

using matrix = vector<vector<int>>;

unsigned int calculateR(matrix& Q, matrix& C, vector<int>& idxOrder, int n)
{
    unsigned int R = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            R += Q[i][j] * C[idxOrder[i]][idxOrder[j]];
        }
    }
    return R;
}

void calculateTime(size_t n)
{
    double dt10 = 2.866;
    for (size_t i = 11; i <= n; ++i)
    {
        dt10 *= i;
    }
    cout << "Time: " << (dt10 / (3600 * 24 * 365)) << " years" << endl;
}

int main(int argc, char const *argv[]) 
{
    if (argc == 2) 
    {
        const int n = atoi(argv[1]);
        calculateTime(n);
        return 0;
    }
    if (argc != 3) throw "wrong number of args";

    matrix Q = readMatrixFromFile(argv[1]);
    matrix C = readMatrixFromFile(argv[2]);

    const int n = C.size();

    vector<int> idxOrder(n);
    for (int i = 0; i < n; ++i) idxOrder[i] = i;

    unsigned int Rmin = numeric_limits<unsigned int>::max();

    auto start = chrono::high_resolution_clock::now();

    do {
        unsigned int R = calculateR(Q, C, idxOrder, n);
        Rmin = min(R, Rmin);
    } while (next_permutation(idxOrder.begin(), idxOrder.end()));

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << Rmin << endl;
    cout << "Time: " << duration.count() / 1000.f << " s" << endl;

    return 0;
}
