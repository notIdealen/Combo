#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "Point.hpp"
#include "GraphNode.hpp"

using namespace std;

// Функция экспорта в DOT (Graphviz)
void exportToDot1(const string& filename, 
                 const vector<Point>& terminals, 
                 const vector<Point>& steiners,
                 const vector<pair<string,string>>& edges) // рёбра в формате {"T0", "S4"}
{
    ofstream out(filename);
    out << "graph SteinerTree {\n";
    out << "    layout=neato; overlap=false;\n";
    
    // Терминалы (красные)
    for (size_t i = 0; i < terminals.size(); ++i) {
        out << "    T" << i << " [pos=\"" << terminals[i].x << "," << terminals[i].y << "!\", "
            << "label=\"T" << i << "\", shape=circle, style=filled, fillcolor=\"#e74c3c\"];\n";
    }
    // Точки Штейнера (синие)
    for (size_t i = 0; i < steiners.size(); ++i) {
        out << "    S" << i << " [pos=\"" << steiners[i].x << "," << steiners[i].y << "!\", "
            << "label=\"S" << i << "\", shape=doublecircle, style=filled, fillcolor=\"#3498db\"];\n";
    }
    // Рёбра
    for (const auto& [u, v] : edges) {
        out << "    " << u << " -- " << v << " [color=\"#2c3e50\", penwidth=2];\n";
    }
    out << "}\n";
    out.close();
    cout << "Saved to " << filename << endl;
}
/*
// Функция экспорта в JSON (для HTML)
void exportToJSON(const string& filename,
                  const vector<Point>& terminals,
                  const vector<Point>& steiners,
                  const vector<pair<string,string>>& edges)
{
    ofstream out(filename);
    out << "{\n  \"nodes\": [\n";
    for (size_t i = 0; i < terminals.size(); ++i) {
        out << "    {\"id\": \"T" << i << "\", \"x\": " << terminals[i].x 
            << ", \"y\": " << terminals[i].y << ", \"type\": \"terminal\"}";
        if (i + 1 < terminals.size() || !steiners.empty()) out << ",";
        out << "\n";
    }
    for (size_t i = 0; i < steiners.size(); ++i) {
        out << "    {\"id\": \"S" << i << "\", \"x\": " << steiners[i].x 
            << ", \"y\": " << steiners[i].y << ", \"type\": \"steiner\"}";
        if (i < steiners.size() - 1) out << ",";
        out << "\n";
    }
    out << "  ],\n  \"edges\": [\n";
    for (size_t i = 0; i < edges.size(); ++i) {
        out << "    [\"" << edges[i].first << "\", \"" << edges[i].second << "\"]";
        if (i < edges.size() - 1) out << ",";
        out << "\n";
    }
    out << "  ]\n}\n";
    out.close();
    cout << "Saved to " << filename << endl;
}
*/
void exportToDot(const string& filename, 
                 const vector<GraphNode>& nodes,  // Все узлы вместе
                 const vector<pair<string,string>>& edges)
{
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "Error: cannot open " << filename << endl;
        return;
    }
    
    out << "graph SteinerTree {\n";
    out << "    layout=neato; overlap=false; splines=line;\n";
    out << "    node [fontname=\"Arial\", fontsize=10];\n\n";
    
    // Отрисовка узлов с их реальными ID
    for (const auto& node : nodes) {
        string color = node.isSteiner ? "#3498db" : "#e74c3c";
        string shape = node.isSteiner ? "doublecircle" : "circle";
        string label = node.id; // Используем сгенерированный ID: "T0", "S1"
        
        out << "    " << node.id << " "
            << "[pos=\"" << node.pos.x << "," << node.pos.y << "!\", "
            << "label=\"" << label << "\", "
            << "shape=" << shape << ", "
            << "style=filled, fillcolor=\"" << color << "\"];\n";
    }
    
    // Отрисовка рёбер
    out << "\n";
    for (const auto& [u, v] : edges) {
        out << "    " << u << " -- " << v << " [color=\"#2c3e50\", penwidth=2];\n";
    }
    
    out << "}\n";
    out.close();
    cout << "Exported to " << filename << endl;
}
