//Задача о рюкзаке
//g++ combo.cpp -o combo.exe
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// bool nextOccurence(vector<size_t>& occurrence){}

int main(int argc, char const *argv[])
{
    int size = 5;
    int combinations[size + 1]{0};
    int weightes[size]{2,7,2,9,8};//28
    int prices[size]{5,3,1,9,6};//24
    int maxW = 16;
    int currW = 0;
    int currP = 0;
    int bestP = 0;
    int bestW = 0;
    string combo = "";
    for (int el : weightes) bestW += el;
    ++bestW;

    // string res = "012345";
    // res.resize(2);
    // cout << res << endl;

    for (int i = 0; i < size; ++i)
    {
        combinations[i] = 0;
        while (combinations[size] != 1)
        {
            // for (int n : combinations) cout << n << ' ';
            int j = 0;
            while (combinations[j] == 1)
            {
                combinations[j] = 0;
                ++j;
            }
            combinations[j] = 1;
        
            // делаю расчет
            for (int k = 0; k < size; ++k)
            {
                if (combinations[k] == 1)
                {
                    currW += weightes[k];
                    if (weightes[k] > maxW || currW > maxW)
                    {
                        cout << "Weight overflow\n";
                        currP = 0;

                        break;
                    }
                    currP += prices[k];
                }
            }
            if (bestP == currP) bestW = min(currW, bestW);
            if (bestP < currP)
            {
                combo = "";
                for (int n : combinations) combo += to_string(n);
                bestP = currP;
                bestW = currW;
            }

            // bestP = max(currP, bestP);
            currP = 0;
            currW = 0;
        }
    }
    combo.resize(size);
    cout << "BEST PRICE: " << bestP << ", BEST WEIGHT: " << bestW << ", COMBO: " << combo << endl;
    return 0;
}
