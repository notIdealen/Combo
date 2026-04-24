#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#include "Point.hpp"
#include "SteinerGraph.hpp"

std::vector<Terminal> GetTerminals(std::string path)
{   
    std::ifstream in(path);
    if (!in.is_open()) return {};
    std::string line{};
    std::vector<Terminal> terminals{};
    double x{}, y{};
    int i = 0;
    while (getline(in, line))
    {
        if (line.empty()) break;
        std::stringstream ss(line);
        ss >> x >> y;
        terminals.push_back(Terminal{"T" + std::to_string(i), {x, y}});
        ++i;
    }
    in.close();
    return move(terminals);
}

void PrintTerminals(std::vector<Terminal>& points)
{
    for (const Terminal& p : points)
    {
        std::cout << p.id << ' ' << p.pos.x << ' ' << p.pos.y << std::endl;
    }
}
void PrintSteiners(SteinerGraph& points)
{
    for (const SteinerPoint& p : points.sPoints)
    {
        std::cout << p.id;
        std::cout << ' ' << p.pos.x << ' ' << p.pos.y << ", edges: "; 
        for (const std::string& e : p.edgesIds)
            std::cout << e << ',';
        std::cout << std::endl;
    }
}

void SteinersToGraphviz(SteinerGraph points)
{
    std::string nodeSize = "0.4";
    for (size_t i = 0; i < points.sPoints.size(); ++i) {
    const auto& s = points.sPoints[i];
    std::cout << "    " << s.id << " [pos=\"" << s.pos.x << "," << s.pos.y 
    << "!\", label=\"" << s.id << "\", shape=doublecircle, "
    << "style=filled, fillcolor=\"#3498db\", width=" << nodeSize <<", height=" << nodeSize << ", fixedsize=true];" << std::endl;
    }
}

void TerminalsToGraphviz(std::vector<Terminal> terminals)
{
    std::string nodeSize = "0.4";
    for (size_t i = 0; i < terminals.size(); ++i) 
    {
        const auto& t = terminals[i];
        std::cout << "    " << t.id << " [pos=\"" << t.pos.x << "," << t.pos.y 
        << "!\", label=\"" << t.id << "\", shape=rectangle, "
        << "style=filled, fillcolor=\"#4ed70a\", width=" << nodeSize <<", height=" << nodeSize << ", fixedsize=true];" << std::endl;
    }
    
}

void ToGraphviz(std::vector<Terminal> terminals, SteinerGraph points)
{
    std::string nodeSize = "0.4";
    // === ЭКСПОРТ ДЛЯ GRAPHVIZ ===
    std::cout << "graph SteinerTree {" << std::endl;
    std::cout << "    layout=neato; mode=\"major\"; splines=line;" << std::endl;
    // Терминалы
    for (size_t i = 0; i < terminals.size(); ++i) 
    {
        const auto& t = terminals[i];
        std::cout << "    " << t.id << " [pos=\"" << t.pos.x << "," << t.pos.y 
        << "!\", label=\"" << t.id << "\", shape=rectangle, "
        << "style=filled, fillcolor=\"#e74c3c\", width=" << nodeSize <<", height=" << nodeSize << ", fixedsize=true];" << std::endl;
    }
    // Точки Штейнера
    for (size_t i = 0; i < points.sPoints.size(); ++i) {
        const auto& s = points.sPoints[i];
        std::cout << "    " << s.id << " [pos=\"" << s.pos.x << "," << s.pos.y 
        << "!\", label=\"" << s.id << "\", shape=doublecircle, "
        << "style=filled, fillcolor=\"#3498db\", width=" << nodeSize <<", height=" << nodeSize << ", fixedsize=true];" << std::endl;
    }
    // Рёбра
    for (size_t i = 0; i < points.sPoints.size(); ++i)
    {
        const auto& s = points.sPoints[i];
        for (auto edge : s.edgesIds)
            std::cout << "    " << s.id << " -- " << edge << " [penwidth=2];" << std::endl;
    }
    std::cout << "}" << std::endl;
}
