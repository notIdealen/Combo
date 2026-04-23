#include <iostream>
#include <vector>
#include <cmath>

#include "includes/Point.hpp"
#include "includes/SteinerGraph.hpp"
#include "includes/Const.hpp"

using namespace std;

SteinerGraph MelzakRecursive(vector<char>& t)
{
    std::vector<int> idx{};
    for (int i = 0; i < t.size(); ++i)
        idx.push_back(i);

    // std::sort(v.begin(), v.end());
    do
    {
        vector<char> temp{};
        // copy(v.begin()[0], v.end()[0], std::ostream_iterator<size_t>(std::cout, " "));
        // copy(v.begin(), v.end(), std::ostream_iterator<size_t>(std::cout, " "));
        // std::cout << '!' << std::endl;
        for (int i = 0; i < idx.size(); ++i)
        {
            temp.push_back(t[idx[i]]);
        }

        for (int i = 0; i < idx.size(); ++i)
        {
            cout << temp[i] << ' ';
            // temp[i];
        }
        cout << endl;


    } while (std::next_permutation(idx.begin(), idx.end()));
    //     SteinerGraph bestGraph{0};
}

int main() {

    vector<char> chs{'q', 'a', 'r', 't'};
    MelzakRecursive(chs);

    return 0;
}
