//Задача о рюкзаке
//g++ combo.cpp -o combo.exe
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// bool nextOccurence(vector<size_t>& occurrence){}

int main(int argc, char const *argv[])
{
    vector<int> weightes{2,7,2,9,8};//28
    vector<int> prices{5,3,1,9,6};//24
    int size = weightes.size();
    vector<int> combinations{size + 1};
    int maxW = 16;
    int currW = 0;
    int currP = 0;
    int bestP = 0;

    for (int i = 0; i < size; ++i)
    {
        combinations[i] = 0;
        while (combinations[size] != 1)
        {
            int j = 0;
            while (combinations[j] == 1)
            {
                combinations[j] = 0;
                ++j;
            }
            combinations[j] = 1;

            for (int k = 0; k < size; ++k)
            {
                if (combinations[k] == 1)
                {
                    currW += weightes[k];
                    if (weightes[k] > maxW || currW > maxW)
                    {
                        // cout << "Weight overflow\n";
                        currP = 0;
                        break;
                    }
                    currP += prices[k];
                }
            }
            bestP = max(currP, bestP);
            currP = 0;
            currW = 0;
        }
    }

    cout << "BEST PRICE: " << bestP << endl;
    return 0;
}
