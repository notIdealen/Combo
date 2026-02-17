//Задача о рюкзаке
//g++ bag.cpp -o bag.exe
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

bool nextOccurence(vector<size_t>& occurrence)
{
    if (occurrence[occurrence.size()] == 1) return false;
    
    int j = 0;
    while (occurrence[j] == 1)
    {
        occurrence[j] = 0;
        ++j;
    }
    occurrence[j] = 1;

    return true;
}

void readData(vector<int>& w, vector<int>& p, const string& path)
{
    string line = "";
    int a,b;
    ifstream file(path);
    while (getline(file, line))
    {
        istringstream iss(line);
        iss >> a >> b;
        w.push_back(a);
        p.push_back(b);
    }
    file.close();
}

int calculatePrice(vector<size_t>& combinations, vector<int>& w8s, vector<int>& prices, int maxW)
{
    int currP = 0;
    int currW = 0;
    for (int k = 0; k < combinations.size(); ++k)
    {
        if (combinations[k] == 1)
        {
            currW += w8s[k];
            if (w8s[k] > maxW || currW > maxW)
            {
                currP = 0;
                break;
            }
            currP += prices[k];
        }
    }
    // cout << "CUURRR:" << currP << endl;
    return currP;
}

int main(int argc, char const *argv[])
{
    vector<int> w8s   (0);
    vector<int> prices(0);
    readData(w8s, prices, argv[1]);
    size_t size = w8s.size();
    vector<size_t> combinations(size + 1);
    int maxW = stoi(argv[2]);
    int currW = 0;
    int currP = 0;
    int bestP = 0;

    for (int i = 0; i < size; ++i)
    {
        combinations[i] = 0;
        while (nextOccurence(combinations))
        {
            // for (int k = 0; k < size; ++k)
            // {
            //     if (combinations[k] == 1)
            //     {
            //         currW += w8s[k];
            //         if (w8s[k] > maxW || currW > maxW)
            //         {
            //             currP = 0;
            //             break;
            //         }
            //         currP += prices[k];
            //     }
            // }
            currP = calculatePrice(combinations, w8s, prices, maxW);
            // cout << currP << endl;
            bestP = max(currP, bestP);
            currP = 0;
            // currW = 0;
        }
    }

    cout << bestP << endl;
    return 0;
}
