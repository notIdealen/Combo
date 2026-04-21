#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "GraphNode.hpp"
#include "Point.hpp"

struct SteinerGraph {
    double totalLength;
    std::vector<GraphNode> nodes;
    std::vector<std::pair<std::string, std::string>> edges; // Рёбра хранятся как пары ID ("ID1", "ID2")
    
    // Быстрый доступ к узлу по ID для добавления рёбер
    std::map<std::string, int> nodeIndex; 
};
