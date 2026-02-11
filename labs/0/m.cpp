#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>

int main()
{
  // std::vector<std::vector<size_t>> v = {
  //   {1, 2}, 
  //   {2, 3}, 
  //   {3, 4}
  // };
  std::vector<size_t> v = { 1, 2, 3, 4, 5, 6, 7, 8};

    auto start = std::chrono::high_resolution_clock::now();
    std::sort(v.begin(), v.end());
    do
    {
        // copy(v.begin()[0], v.end()[0], std::ostream_iterator<size_t>(std::cout, " "));
        copy(v.begin(), v.end(), std::ostream_iterator<size_t>(std::cout, " "));
        std::cout << '!' << std::endl;
    } while (std::next_permutation(v.begin(), v.end()));
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Duration: " << duration.count() / 1000.f << " s\n";
}
//g++ m.cpp -o m.exe
