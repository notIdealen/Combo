#include <iostream>
#include <vector>
#include <cmath>

#include "includes/Point.hpp"
#include "includes/SteinerGraph.hpp"
#include "includes/Const.hpp"

using namespace std;

double AngleAt(Point& Q, Point& W, Point& E)
{
    Point v1 = Q - W;
    Point v2 = E - W;
    double mul = v1.x * v2.x + v1.y * v2.y;
    double lenV1 = v1.Length();
    double lenV2 = v2.Length();
    if (lenV1 < DELTA_LEN || lenV2 < DELTA_LEN) return PI;
    return acos(max(-1.0, min(1.0, mul / (lenV1 * lenV2))));
}

SteinerPoint FindSteinerPoint(Point& A, Point& B, Point& S, Point& X) 
{
    SteinerPoint sp;
    if (AngleAt(A, B, S) >= ANGLE_120) 
    { 
        sp.SetEqual(B); 
        return sp;
    }
    if (AngleAt(B, A, S) >= ANGLE_120)
    { 
        sp.SetEqual(A);
        return sp;
    }
    double L2 = (B-A).Length2();
    // if (L2 < DELTA_LEN) return C;
    Point O = (A + B + X) / 3.0;  // центр окружности
    double R2 = L2 / 3.0;          // R² = L²/3
    Point V = X - S;
    double V2 = V.Length2();
    // if (V2 < DELTA_LEN) return O;
    Point diff = S - O;
    double tS = (diff.Length2() - R2) / V2; // Виета: t₂ = c/a
    sp.Create(Point{S + V * tS}, {A.id, B.id});
    return sp;
}

int main() {
    Point A{"T0", 0, 0};
    Point B{"T1", 2, 2};
    Point S1{"S1", 3, 1};
    Point S2{"S2", 6, 3};
    Point X{"F0", -0.732, 2.732};
    auto NS1 = FindSteinerPoint(A, B, S1, X);
    std::cout << "NS1: " << NS1.x << ' ' << NS1.y << std::endl;
    auto NS2 = FindSteinerPoint(A, B, S2, X);
    std::cout << "NS2: " << NS2.x << ' ' << NS2.y << std::endl;
    // std::cout << "Edges: " << s.edgesIds[0] << ' ' << s.edgesIds[1] << std::endl;
    return 0;
}
