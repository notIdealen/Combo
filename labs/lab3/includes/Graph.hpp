#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <map>

using EdgeList = std::vector<std::pair<int, int>>;
using AdjacencyMatrix = std::vector<std::vector<bool>>;

class Graph
{
public:
    void SetEdgeListFromFile(const std::string path);
    void SetAdjacencyMatrixFromFile(const std::string path);
    void PrintEdgeList() const;
    void PrintAdjacencyMatrix() const;
    void ConvertAMToEL();
    void ConvertELToAM();
    void CreateAdjacencyListFromEdgeList();
    void FindSCC();
    void DFS(std::vector<std::vector<int>>& graph, int v, int component);
    void PrintSCC();
    
    AdjacencyMatrix GetAdjacencyMatrix();
    EdgeList GetEdgeList();
    std::vector<std::vector<int>> GetAdjacencyList();
    std::vector<int> GetVisited();

private:
    int m_vertexesNumber = 0;
    AdjacencyMatrix m_adjacencyMatrix{};
    EdgeList m_edgeList{};
    // FROM LESSON
    std::vector<std::vector<int>> m_graph{};
    std::vector<std::vector<int>> m_rgraph{};
    std::vector<int> m_visited, m_order;
};
