//g++ main.cpp -o qap.exe
// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <limits>
// #include <chrono>

// using namespace std;

// using matrix = vector<vector<int>>;

// matrix dist = {
//     { 0, 23, 45, 18, 37, 29, 51, 42, 33, 26},
//     {23,  0, 31, 40, 19, 38, 27, 53, 44, 35},
//     {45, 31,  0, 52, 41, 17, 39, 28, 50, 46},
//     {18, 40, 52,  0, 25, 47, 36, 21, 12, 30},
//     {37, 19, 41, 25,  0, 33, 22, 48, 39, 24},
//     {29, 38, 17, 47, 33,  0, 44, 32, 43, 38},
//     {51, 27, 39, 36, 22, 44,  0, 19, 28, 41},
//     {42, 53, 28, 21, 48, 32, 19,  0, 15, 27},
//     {33, 44, 50, 12, 39, 43, 28, 15,  0, 18},
//     {26, 35, 46, 30, 24, 38, 41, 27, 18,  0},
// };

// matrix w8 = {
//     { 0, 12,  8, 21,  5, 14,  3,  9, 17,  6},
//     {12,  0, 19,  7, 23,  4, 16, 28, 11, 20},
//     { 8, 19,  0, 13,  9, 25,  7, 14, 22, 10},
//     {21,  7, 13,  0, 18, 11, 24,  6, 15, 27},
//     { 5, 23,  9, 18,  0, 31, 12, 26,  8, 19},
//     {14,  4, 25, 11, 31,  0, 18, 13, 29,  7},
//     { 3, 16,  7, 24, 12, 18,  0, 22, 10, 25},
//     { 9, 28, 14,  6, 26, 13, 22,  0, 33, 16},
//     {17, 11, 22, 15,  8, 29, 10, 33,  0, 14},
//     { 6, 20, 10, 27, 19,  7, 25, 16, 14,  0}
// };

// // matrix dist = {
// //     { 0, 23, 45, 18, 37, 29, 51},// 42, 33, 26},
// //     {23,  0, 31, 40, 19, 38, 27},// 53, 44, 35},
// //     {45, 31,  0, 52, 41, 17, 39},// 28, 50, 46},
// //     {18, 40, 52,  0, 25, 47, 36},// 21, 12, 30},
// //     {37, 19, 41, 25,  0, 33, 22},// 48, 39, 24},
// //     {29, 38, 17, 47, 33,  0, 44},// 32, 43, 38},
// //     {51, 27, 39, 36, 22, 44,  0},// 19, 28, 41},
// //     // {42, 53, 28, 21, 48, 32, 19,  0, 15, 27},
// //     // {33, 44, 50, 12, 39, 43, 28, 15,  0, 18},
// //     // {26, 35, 46, 30, 24, 38, 41, 27, 18,  0},
// // };

// // matrix w8 = {
// //     { 0, 12,  8, 21,  5, 14,  3},//  9, 17,  6},
// //     {12,  0, 19,  7, 23,  4, 16},// 28, 11, 20},
// //     { 8, 19,  0, 13,  9, 25,  7},// 14, 22, 10},
// //     {21,  7, 13,  0, 18, 11, 24},//  6, 15, 27},
// //     { 5, 23,  9, 18,  0, 31, 12},// 26,  8, 19},
// //     {14,  4, 25, 11, 31,  0, 18},// 13, 29,  7},
// //     { 3, 16,  7, 24, 12, 18,  0},// 22, 10, 25},
// //     // { 9, 28, 14,  6, 26, 13, 22,  0, 33, 16},
// //     // {17, 11, 22, 15,  8, 29, 10, 33,  0, 14},
// //     // { 6, 20, 10, 27, 19,  7, 25, 16, 14,  0}
// // };
// // matrix dist = {
// //     { 0, 23, 45, 18, 37},
// //     {23,  0, 31, 40, 19},
// //     {45, 31,  0, 52, 41},
// //     {18, 40, 52,  0, 25},
// //     {37, 19, 41, 25,  0}
// // };

// // matrix w8 = {
// //     { 0, 12,  8, 21, 5},
// //     {12,  0, 19,  7, 23},
// //     { 8, 19,  0, 13, 9},
// //     {21,  7, 13,  0, 18},
// //     { 5, 23,  9, 18,  0}
// // };
// // vector<int> w8 = {1,2,3,4};


// int main(int argc, char const *argv[])
// {
//     const int matrixSize = dist.size();
//     unsigned int minResult = numeric_limits<int>::max();
//     // cout << minResult << endl;
//     unsigned int buf = 0;
//     // int counter = 0;
//     sort(dist.begin(), dist.end());
//     auto start = std::chrono::high_resolution_clock::now();
//     do
//     {
//         for (int i = 0; i < matrixSize; i++)
//         {
//             for (int j = 0; j < matrixSize; j++)
//                 buf += dist[i][j] * w8[i][j];
//         }

//         minResult = min(buf, minResult);
//         buf = 0;
//         // counter++;
//         // cout << counter << ". " << minResult << endl;
//     } while (std::next_permutation(dist.begin(), dist.end()));
//     auto end = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//     cout << minResult << endl;
//     std::cout << "Duration: " << duration.count() / 1000.f << " s\n";
//     return 0;
// }

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>

using namespace std;

using matrix = vector<vector<int>>;

int main() {
    matrix C = {
        { 0, 23, 45, 18, 37, 29, 51, 42, 33, 26},
        {23,  0, 31, 40, 19, 38, 27, 53, 44, 35},
        {45, 31,  0, 52, 41, 17, 39, 28, 50, 46},
        {18, 40, 52,  0, 25, 47, 36, 21, 12, 30},
        {37, 19, 41, 25,  0, 33, 22, 48, 39, 24},
        {29, 38, 17, 47, 33,  0, 44, 32, 43, 38},
        {51, 27, 39, 36, 22, 44,  0, 19, 28, 41},
        {42, 53, 28, 21, 48, 32, 19,  0, 15, 27},
        {33, 44, 50, 12, 39, 43, 28, 15,  0, 18},
        {26, 35, 46, 30, 24, 38, 41, 27, 18,  0},
    };

    matrix Q = {//w8
        { 0, 12,  8, 21,  5, 14,  3,  9, 17,  6},
        {12,  0, 19,  7, 23,  4, 16, 28, 11, 20},
        { 8, 19,  0, 13,  9, 25,  7, 14, 22, 10},
        {21,  7, 13,  0, 18, 11, 24,  6, 15, 27},
        { 5, 23,  9, 18,  0, 31, 12, 26,  8, 19},
        {14,  4, 25, 11, 31,  0, 18, 13, 29,  7},
        { 3, 16,  7, 24, 12, 18,  0, 22, 10, 25},
        { 9, 28, 14,  6, 26, 13, 22,  0, 33, 16},
        {17, 11, 22, 15,  8, 29, 10, 33,  0, 14},
        { 6, 20, 10, 27, 19,  7, 25, 16, 14,  0}
    };

    const int n = C.size();
    vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i;

    unsigned int minCost = numeric_limits<unsigned int>::max();
    vector<int> bestPerm;

    auto start = chrono::high_resolution_clock::now();

    do {
        unsigned int cost = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                cost += Q[i][j] * C[perm[i]][perm[j]];
            }
        }
        if (cost < minCost) {
            minCost = cost;
            bestPerm = perm;
        }
    } while (next_permutation(perm.begin(), perm.end()));

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Min cost: " << minCost << endl;
    cout << "Best perm: ";
    for (int p : bestPerm) cout << p << " ";
    cout << "\nTime: " << duration.count() / 1000.0 << " s" << endl;

    // Проверка: для матриц 10×10 нужно 10! = 3 628 800 перестановок
    // cout << "Перебрано перестановок: " << 3628800 << endl;

    return 0;
}
