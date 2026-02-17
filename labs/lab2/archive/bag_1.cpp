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
    while (occurrence[occurrence.size()] != 1)
    {
        int j = 0;
        while (occurrence[j] == 1)
        {
            occurrence[j] = 0;
            ++j;
        }
        occurrence[j] = 1;
        return true;
    }
    return false;
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

int main(int argc, char const *argv[])
{
    vector<int> w8s   (0);
    vector<int> prices(0);
    readData(w8s, prices, argv[1]);
    size_t size = w8s.size();
    vector<size_t> combinations{size + 1};
    int maxW = stoi(argv[2]);
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

        // if (nextOccurence(combinations))
        // {
            for (int k = 0; k < size; ++k)
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
            bestP = max(currP, bestP);
            currP = 0;
            currW = 0;
        }
        // else
        // {
        //     break;
        // }
    }

    cout << bestP << endl;
    return 0;
}
