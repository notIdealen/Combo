#include "Point.hpp"

struct CircumCircleAroundTriangle
{
    Point center;
    double R2;
    double L2;
};

CircumCircleAroundTriangle GetCircumCircleAroundTriangle(const Point& A, const Point& B, const Point& C)
{
    CircumCircleAroundTriangle cct;
    cct.center = (A + B + C) / 3.0;
    cct.L2 = (B - A).Length2();
    cct.R2 = cct.L2 / 3.0;
    return cct;
}
