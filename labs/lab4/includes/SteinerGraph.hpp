#pragma once

#include <vector>
#include <string>
#include "Point.hpp"

struct SteinerPoint {
    inline static int counter = 0;
    std::string id{"S"};
    double x, y;
    Point pos;
    std::vector<std::string> edgesIds;

    SteinerPoint() {};

    void SetEqual(Point& p)
    {
        x = p.x;
        y = p.y;
        pos = p;
        edgesIds.push_back(p.id);
        id.append(p.id);
    }

    void AddEdgesIds(std::vector<std::string> edges)
    {
        for (auto e : edges)
            edgesIds.push_back(e);
    }
    void Create(Point p, std::vector<std::string> edges)
    {
        id.append(std::to_string(counter));
        ++counter;
        x = p.x;
        y = p.y;
        pos = p;
        AddEdgesIds(edges);
    }
};

struct SteinerGraph {
    double length;
    std::vector<SteinerPoint> sPoints{};

    SteinerGraph(double length_) : length(length_){}
};


