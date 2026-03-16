//
//g++
#include <iostream>

#include "includes/Graph.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    Graph g;
    if (static_cast<string>(argv[1]) == "el")
    {
        g.SetEdgeListFromFile(argv[2]);
        g.CreateAdjacencyListFromEdgeList();
        g.PrintSCC();
    }
    else if (static_cast<string>(argv[1]) == "am")
    {
        g.SetAdjacencyMatrixFromFile(argv[2]);
        g.ConvertAMToEL();
        g.CreateAdjacencyListFromEdgeList();
        g.PrintSCC();
    }
    return 0;
}
//.\graph.exe el data/edgeList.txt
//.\graph.exe am data/adjMatrix.txt
