//Задача о рюкзаке
//g++ bag.cpp -o bag.exe
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// bool nextOccurence(vector<size_t>& occurrence)
// {

// }

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
    int size = w8s.size();
    vector<int> combinations{size + 1};
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
    }

    cout << bestP << endl;
    return 0;
}
