#pragma once

#include <cmath>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

struct Point {
    double x{}, y{};
    
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(double k) const { return {x * k, y * k}; }
    Point operator/(double k) const { return {x / k, y / k}; }
    
    double Length() const { return std::hypot(x, y); }
    double Length2() const { return x*x + y*y; }

    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
};

struct Terminal
{
    std::string id{};
    Point pos{};
    std::vector<Terminal*> parents{};
    Terminal(){}
    Terminal(Point p) : pos{p}{}
    Terminal(std::string id_, Point p) : id{id_}, pos{p}{}
};

struct SteinerPoint : Terminal
{
    inline static size_t counter = 0;
    std::vector<std::string> edgesIds{};
    std::vector<Terminal> neighbors;
    bool invalid{false};

    void Create(Point p, char pref, std::vector<std::string> ids)
    {
        id.append(pref + std::to_string(counter));
        ++counter;
        pos = p;
        AddEdgesIds(ids);
    }
    void SetEqual(Terminal& p, char pref, std::vector<std::string> ids)
    {
        pos = p.pos;
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
