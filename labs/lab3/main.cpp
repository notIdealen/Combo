//
//g++
#include <iostream>

#include "includes/Graph.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    Graph g;
    g.SetEdgeListFromFile("data/edgeList.txt");
    g.CreateAdjacencyListFromEdgeList();
    g.PrintSCC();
    return 0;
}
