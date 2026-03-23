#define CATCH_CONFIG_MAIN
#include <iostream>

#include "catch.hpp"
#include "includes/Graph.hpp"

using namespace std;

TEST_CASE("Read adjacency matrix from file")
{
    const string path = "data/adjMatrix_test.txt";
    Graph g;
    AdjacencyMatrix t = {
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {1, 0, 0, 1},
        {0, 0, 0, 0}
    };
    g.SetAdjacencyMatrixFromFile(path);
    REQUIRE(g.GetAdjacencyMatrix() == t);
}

TEST_CASE("Read invalid adjacency matrix from file")
{
    const string path = "data/adjMatrix_test_i.txt";
    Graph g;
    AdjacencyMatrix t{};
    g.SetAdjacencyMatrixFromFile(path);
    REQUIRE(g.GetAdjacencyMatrix() == t);
}

TEST_CASE("Read edge list from file")
{
    const string path = "data/edgeList_test.txt";
    Graph g;
    EdgeList t = {
        {1, 2},
        {2, 3},
        {3, 1},
        {3, 4}
    };
    g.SetEdgeListFromFile(path);
    REQUIRE(g.GetEdgeList() == t);
}

TEST_CASE("Read invalid edge list from file")
{
    const string path = "data/edgeList_test_i.txt";
    Graph g;
    EdgeList t{};
    g.SetEdgeListFromFile(path);
    REQUIRE(g.GetEdgeList() == t);
}

TEST_CASE("Get adjacency list")
{
    const string path = "data/edgeList_test.txt";
    Graph g;
    EdgeList t{};
    g.SetEdgeListFromFile(path);
    g.CreateAdjacencyListFromEdgeList();
    auto al = g.GetAdjacencyList();
    vector<vector<int>> gt{
        {1},
        {2},
        {0, 3},
        {}
    };
    REQUIRE(al == gt);
}

TEST_CASE("Convert edge list to adjacency matrix")
{
    const string path = "data/edgeList_test.txt";
    Graph g;
    AdjacencyMatrix t = {
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {1, 0, 0, 1},
        {0, 0, 0, 0}
    };
    g.SetEdgeListFromFile(path);
    g.ConvertELToAM();
    REQUIRE(g.GetAdjacencyMatrix() == t);
}

TEST_CASE("Convert adjacency matrix to edge list")
{
    const string path = "data/adjMatrix_test.txt";
    Graph g;
    EdgeList t = {
        {1, 2},
        {2, 3},
        {3, 1},
        {3, 4}
    };
    g.SetAdjacencyMatrixFromFile(path);
    g.ConvertAMToEL();
    REQUIRE(g.GetEdgeList() == t);
}

TEST_CASE("find SCC")
{
    const string path = "data/edgeList.txt";
    Graph g;
    g.SetEdgeListFromFile("data/edgeList.txt");
    g.CreateAdjacencyListFromEdgeList();
    g.FindSCC();
    auto visited = g.GetVisited();
    vector<int> t = {7, 7, 8, 7, 8, 7, 4, 6, 5, 6, 6, 3, 2, 1};
    REQUIRE(visited == t);
}

TEST_CASE("find SCC simple")
{
    const string path = "data/edgeList_test9.txt";
    Graph g;
    g.SetEdgeListFromFile(path);
    g.CreateAdjacencyListFromEdgeList();
    g.FindSCC();
    auto visited = g.GetVisited();
    vector<int> t = {1, 1, 1, 1};
    REQUIRE(visited == t);
}
