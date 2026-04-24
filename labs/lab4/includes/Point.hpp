#pragma once

#include <cmath>
#include <array>
#include <memory>
#include <string>

struct Point {
    double x{}, y{};
    
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(double k) const { return {x * k, y * k}; }
    Point operator/(double k) const { return {x / k, y / k}; }//new
    
    double Length() const { return sqrt(x*x + y*y); }
    double Length2() const { return x*x + y*y; }
};

struct Terminal
{
    std::string id{};
    Point pos{};
    std::array<std::shared_ptr<Terminal>, 2> parents;
    // bool isX{false};
    Terminal(){}
    Terminal(Point p) : pos{p}{}
    Terminal(std::string id_, Point p) : id{id_}, pos{p}{}
};

