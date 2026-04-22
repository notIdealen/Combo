#pragma once

#include <cmath>
#include <string>

struct Point {
    // std::string id{};
    double x{}, y{};

    // Point() {};
    // Point(double x_, double y_) : x(x_), y(y_) {}
    // Point(std::string id_, double x_, double y_) : id(id_), x(x_), y(y_) {}
    
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
    Terminal(){}
    Terminal(Point p) : pos{p}{}
    Terminal(std::string id_, Point p) : id{id_}, pos{p}{}
};
