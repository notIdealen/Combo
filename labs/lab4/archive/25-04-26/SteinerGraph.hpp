#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Point.hpp"

struct SteinerPoint : Terminal
{
    inline static size_t counter = 0;
    std::vector<std::string> edgesIds{};
    double edgesLength{-1};
    std::vector<Terminal> neighbors;///////////
    bool invalid{false};

    //создаю свою точку
    void Create(Point p, char pref, std::vector<std::string> ids)
    {
        id.append(pref + std::to_string(counter));
        ++counter;
        pos = p;
        AddEdgesIds(ids);
    }
    //создаю точку из терминала
    void SetEqual(Terminal& p, char pref, std::vector<std::string> ids)
    {
        pos = p.pos;
        edgesIds.push_back(p.id);
        id.append(pref + p.id);
        AddEdgesIds(ids);
    }

    void AddEdgesIds(std::vector<std::string> ids)
    {
        for (auto e : ids)
            edgesIds.push_back(e);
    }


};

struct SteinerTree {
    double length;
    std::vector<SteinerPoint> sPoints{};

    SteinerTree(double length_) : length(length_){}
};
