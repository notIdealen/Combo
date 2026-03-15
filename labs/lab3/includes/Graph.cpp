#include "Graph.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void Graph::PrintEdgeList() const
{
    for (auto el : m_edgeList)
        cout << el.first << ' ' << el.second << endl;
    cout << endl;
}

void Graph::PrintAdjacencyMatrix() const
{
    for (auto el : m_adjacencyMatrix)
    {
        for (auto v : el)
            cout << v << ' ';
        cout << endl;
    }
}

void Graph::SetEdgeListFromFile(const std::string path)
{
    ifstream inF(path);
    if (!inF.is_open())
    {
        cout << "File not open: " << path << endl;
        return;
    }
    string line;
    int x, y;
    stringstream ss;
    while (getline(inF, line))
    {
        ss << line;
        if (ss >> x >> y)
        {
            m_edgeList.push_back({x, y});
            if (m_vertexesNumber < x || m_vertexesNumber < y)
                m_vertexesNumber = x < y ? y : x;
        }
        else
        {
            cout << "INVALID line: " + line << endl;
            m_edgeList.clear();
            m_vertexesNumber = 0;
            return;
        }
        ss.clear();
    }
    inF.close();
}

void Graph::SetAdjacencyMatrixFromFile(const std::string path)
{
    ifstream inF(path);
    if (!inF.is_open())
    {
        cout << "File not open: " << path << endl;
        return;
    }

    string line;
    int token;
    stringstream ss;
    while (getline(inF, line))
    {
        m_adjacencyMatrix.push_back({});
        ss << line;
        while (ss >> token)
        {
            if (token != 0 && token != 1)
            {
                cout << "INVALID token:" << token << endl;
                m_adjacencyMatrix.clear();
                return;
            }
            m_adjacencyMatrix.back().push_back(token);
        }
        ss.clear();
    }
    for (size_t i = 0; i < m_adjacencyMatrix.size(); ++i)
    {
        if (m_adjacencyMatrix[i].size() != m_adjacencyMatrix.size())
        {
            cout << "INVALID matrix:" << endl;
            m_adjacencyMatrix.clear();
            return;
        }
    }
    m_vertexesNumber = m_adjacencyMatrix.size();
    inF.close();
}

void Graph::ConvertAMToEL()
{
    m_edgeList.clear();
    for (size_t i = 0; i < m_vertexesNumber; ++i)
        for (size_t j = 0; j < m_vertexesNumber; ++j)
            if (m_adjacencyMatrix[i][j] == 1)
                m_edgeList.push_back({i + 1, j + 1});
}

void Graph::ConvertELToAM()
{
    m_adjacencyMatrix.clear();
    m_adjacencyMatrix.assign(m_vertexesNumber, std::vector<bool>(m_vertexesNumber, 0));
    for (auto [f, s] : m_edgeList)
        m_adjacencyMatrix[f - 1][s - 1] = 1;
}

void Graph::CreateAdjacencyListFromEdgeList()
{
    m_graph.resize(m_vertexesNumber);
    m_rgraph.resize(m_vertexesNumber); 
    for (auto [k, v] : m_edgeList)
    {
        m_graph[k - 1].push_back(v - 1);
        m_rgraph[v - 1].push_back(k - 1);
    }
}

void Graph::RDFS(int v)
{
    m_visited[v] = 1;
    for (int to : m_rgraph[v])
        if (!m_visited[to])
            RDFS(to);

    m_order.push_back(v);
}

void Graph::DFS(int v, int component)
{
    m_visited[v] = component;
    for (int to : m_graph[v])
        if (!m_visited[to])
            DFS(to, component);
    
    m_order.push_back(v);
}

void Graph::FindSCC()
{
    m_visited.assign(m_graph.size(), 0);
    for (int v = 0; v < m_graph.size(); ++v)
        if (!m_visited[v])
            RDFS(v);
    
    reverse(m_order.begin(), m_order.end());

    m_visited.assign(m_graph.size(), 0);
    int counter = 0;
    for (int v : m_order)
        if (!m_visited[v])
            DFS(v, ++counter);
}

void Graph::PrintSCC()
{
    FindSCC();
    vector<int> view(m_visited);
    for (int i = 0; i < m_vertexesNumber; ++i)
    {
        if (view[i] == 0) continue;
        int component = view[i];
        cout << "SCC Vertex: ";
        for (int j = i; j < m_vertexesNumber; ++j)
        {
            if (m_visited[j] == component) 
            {
                cout << j + 1 << ' ';
                view[j] = 0;
            }
        }
        cout << endl;
    }
}
