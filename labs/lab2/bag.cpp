//Задача о рюкзаке
//g++ bag.cpp -o bag.exe
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// bool nextOccurence(vector<size_t>& occurrence){}

void readData(vector<int>& w, vector<int>& p)
{
    string line = "";
    ifstream file("data/wp.txt");
    bool flag = true;
    int a,b;
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
    vector<int> weightes(0);//28
    vector<int> prices  (0);//24
    readData(weightes, prices);
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
