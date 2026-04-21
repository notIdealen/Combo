#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "Point.hpp"


struct GraphNode {
    inline static int node_counter = 0;
    std::string id;      // Уникальный ID: "T0", "S1" и т.д.
    Point pos;      // Координаты
    bool isSteiner; // Флаг типа узла
    
    GraphNode(const Point& p, bool steiner) : pos(p), isSteiner(steiner) {}
    //     id = (steiner ? "S" : "T") + std::to_string(node_counter++);
    // }
};
