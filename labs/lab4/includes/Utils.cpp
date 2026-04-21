#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#include "Point.hpp"
#include "SteinerGraph.hpp"

std::vector<Point> GetTerminals(std::string path)
{   
    std::ifstream in(path);
    if (!in.is_open()) return {};
    std::string line{};
    std::vector<Point> terminals{};
    double x{}, y{};
    int i = 0;
    while (getline(in, line))
    {
        if (line.empty()) break;
        std::stringstream ss(line);
        ss >> x >> y;
        terminals.push_back(Point{"T" + std::to_string(i), x, y});
        ++i;
    }
    in.close();
    return move(terminals);
}

void PrintTerminals(std::vector<Point>& points)
{
    for (const Point& p : points)
    {
        std::cout << p.id << ' ' << p.x << ' ' << p.y << std::endl;
    }
}
void PrintSteiners(SteinerGraph& points)
{
    for (const SteinerPoint& p : points.sPoints)
    {
        std::cout << p.id;
        std::cout << ' ' << p.x << ' ' << p.y << ", edges: "; 
        for (const std::string& e : p.edgesIds)
            std::cout << e << ',';
        std::cout << std::endl;
    }
}

std::string ToGraphviz(std::string path, std::vector<Point> terminals, SteinerGraph points)
{
    std::ofstream out(path);
     // === ЭКСПОРТ ДЛЯ GRAPHVIZ ===
    std::cout << "graph SteinerTree {" << std::endl;
    std::cout << "    layout=neato; mode=\"major\"; splines=line;" << std::endl;
    // Терминалы
    for (size_t i = 0; i < terminals.size(); ++i) 
    {
        const auto& p = terminals[i];
        std::cout << "    T" << i << " [pos=\"" << p.x << "," << p.y 
        << "!\", label=\"T" << i << "\", shape=rectangle, "
        << "style=filled, fillcolor=\"#e74c3c\"];" << std::endl;
    }
    // Точки Штейнера
    for (size_t i = 0; i < points.sPoints.size(); ++i) {
        const auto& p = points.sPoints[i];
        std::cout << "    S" << i << " [pos=\"" << p.x << "," << p.y 
        << "!\", label=\"S" << i << "\", shape=doublecircle, "
        << "style=filled, fillcolor=\"#3498db\"];" << std::endl;
    }
    /*
    // Рёбра
    struct Edge
    {
        string terminal;
        string steiner;
        double minDist;
    };
    vector<Edge> edges{};

    for (size_t i = 0; i < terminals.size(); ++i)
    {
        Edge edge;
        edge.minDist = -1;
        edge.terminal = "T" + to_string(i);
        int steinerPoint{-1};
        for (size_t j = 0; j < points.steinerPoints.size(); ++j)
        {
            auto distance = dist(terminals[i], points.steinerPoints[j]);
            if (edge.minDist < 0 || distance < edge.minDist)
            {
                edge.minDist = distance;
                edge.steiner = "S" + to_string(j);
                steinerPoint = j;
            }
        }
        points.steinerPoints[steinerPoint].edgeNum += 1;
        edges.push_back(edge);
    }
    //steiner edges
    for (size_t i = 0; i < points.steinerPoints.size() - 1; ++i)
    {
        while (points.steinerPoints[i].edgeNum < 3)
        {
            Edge edge;
            edge.minDist = -1;
            edge.terminal = "S" + to_string(i);
            int steinerPoint{-1};
            for (size_t j = i + 1; j < points.steinerPoints.size(); ++j)
            {
                auto distance = dist(points.steinerPoints[i], points.steinerPoints[j]);
                if (points.steinerPoints[j].edgeNum < 3 && (edge.minDist < 0 || distance < edge.minDist))
                {
                    edge.minDist = distance;
                    edge.steiner = "S" + to_string(j);
                    steinerPoint = j;
                }
            }
            points.steinerPoints[steinerPoint].edgeNum += 1;
            edges.push_back(edge);
            points.steinerPoints[i].edgeNum += 1;
        }
    }
    cout << endl;
    for (const auto& e : edges) {
        cout << "    " << e.terminal << " -- " << e.steiner << " [penwidth=2];" << endl;
    }
    */
    std::cout << "}" << std::endl;
    out.close();
    return "";
}
