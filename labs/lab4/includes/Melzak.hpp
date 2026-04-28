#pragma once
//Steiner-Melzak
#include <vector>
#include <set>
#include <cmath>
// #include <iostream>

#include "Structs.hpp"
#include "Const.hpp"

using namespace std;

double Dist(const Terminal& A, const Terminal& B) {
    return (A.pos - B.pos).Length();
}

double AngleAt(Terminal& Q, Terminal& W, Terminal& E)
{
    Point v1 = Q.pos - W.pos;
    Point v2 = E.pos - W.pos;
    double scalar = v1.x * v2.x + v1.y * v2.y;
    double lenV1 = v1.Length();
    double lenV2 = v2.Length();
    if (lenV1 < DELTA_LEN || lenV2 < DELTA_LEN) return PI;
    return acos(max(-1.0, min(1.0, scalar / (lenV1 * lenV2))));
}

vector<Terminal> GetPretendent(Point& A, Point& B)
{
    Point mid = (A + B) / 2;
    Point dir = B - A;
    double len = dir.Length();
    // if (len < DELTA_LEN) return {};
    double h = len * SIN_60;
    Point perp = {-dir.y / len * h, dir.x / len * h};
    return {Terminal{mid + perp}, Terminal{mid - perp}};
}

void AddNeighbors(const Terminal& A, const Terminal& B, const Terminal& C, SteinerPoint& S)
{
    const Terminal* terminals[] = {&A, &B, &C};
    const string id = S.id.substr(1);

    for (const Terminal* t : terminals)
    {
        if (id == t->id) continue;
        if (!t->id.empty() && 
            (t->id[0] == 'T' || t->id[0] == 'S' || t->id[0] == 'F')) 
        {
            S.neighbors.push_back(*t);
        }
    }
}

double CrossProduct(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

SteinerPoint FindFPointInsideTriangle(const Terminal& A, const Terminal& B, const Terminal& C) {
    double ab = (B.pos - A.pos).Length();
    double bc = (C.pos - B.pos).Length();
    double ca = (A.pos - C.pos).Length();
    Terminal P1, P2, P3;
    double maxLen;
    if (ab >= bc && ab >= ca) { P1 = A; P2 = B; P3 = C; maxLen = ab; }
    else if (bc >= ab && bc >= ca) { P1 = B; P2 = C; P3 = A; maxLen = bc; }
    else { P1 = C; P2 = A; P3 = B; maxLen = ca; }
    Point mid = (P1.pos + P2.pos) * 0.5;
    Point dir = P2.pos - P1.pos;
    Point perp = {-dir.y, dir.x};
    double h = maxLen * SQRT_3 * 0.5;
    Point X1 = mid + perp * (h / maxLen);
    Point X2 = mid - perp * (h / maxLen);
    double cpP3 = CrossProduct(dir, P3.pos - P1.pos);
    double cpX1 = CrossProduct(dir, X1 - P1.pos);
    Point X = (cpP3 * cpX1 < 0) ? X1 : X2;
    Point O = (P1.pos + P2.pos + X) / 3.0;
    double R2 = maxLen * maxLen / 3.0;
    Point V = X - P3.pos;
    double V2 = V.x * V.x + V.y * V.y;
    Point diff = P3.pos - O;
    double diff2 = diff.x * diff.x + diff.y * diff.y;
    double tS = (diff2 - R2) / V2;
    // 5. Точка Штейнера
    SteinerPoint S;
    S.Create({P3.pos + V * tS}, 'F', {P1.id, P2.id, P3.id});
    AddNeighbors(A, B, C, S);
    return S;
}

SteinerPoint FindPointEqualTerminal(Terminal& A, Terminal& B, Terminal& C, char&& pref)
{
    SteinerPoint S{};
    if (B.id[0] == 'X') { S.invalid = true; return S;}
    S.SetEqual(B, pref, {A.id, C.id}); 
    AddNeighbors(A, B, C, S);
    return S;
}

SteinerPoint FindFPoint(Terminal& A, Terminal& B, Terminal& C)
{
    SteinerPoint F{};
    if (AngleAt(A, B, C) > ANGLE_120) 
        return FindPointEqualTerminal(A, B, C, 'F');
    if (AngleAt(B, A, C) > ANGLE_120)
        return FindPointEqualTerminal(B, A, C, 'F');
    if (AngleAt(A, C, B) > ANGLE_120)
        return FindPointEqualTerminal(A, C, B, 'F');
    return FindFPointInsideTriangle(A, B, C);
}

SteinerPoint FindSteinerPoint(Terminal& A, Terminal& B, Terminal& F, Terminal& X) 
{
    SteinerPoint S{};
    if (AngleAt(A, B, F) > ANGLE_120) return FindPointEqualTerminal(A, B, F, 'S');
    if (AngleAt(B, A, F) > ANGLE_120) return FindPointEqualTerminal(B, A, F, 'S');
    if (AngleAt(A, F, B) > ANGLE_120) { S.invalid = true; return S;}
    double L2 = (B.pos-A.pos).Length2();
    // if (L2 < DELTA_LEN) return F;
    Point O = (A.pos + B.pos + X.pos) / 3.0;
    double R2 = L2 / 3.0;
    Point V = X.pos - F.pos;
    double V2 = V.Length2();
    // if (V2 < DELTA_LEN) return O;
    Point diff = F.pos - O;
    double tS = (diff.Length2() - R2) / V2;
    S.Create({F.pos + V * tS}, 'S', {A.id, B.id, F.id});
    AddNeighbors(A, B, F, S);
    return S;
}

double CalculateGraphLength(const SteinerTree& graph)
{
    double len{};
    set<string> ids;
    for (auto& p : graph.sPoints)
    {
        ids.insert(p.id);
        for (auto n : p.neighbors) 
            if (!ids.count(n.id)) len += Dist(p, n); 
    }
    return len;
}

Point ComputeTorricelliPoint(const Point& P1, const Point& P2, const Point& P3) 
{
    double d12 = (P2 - P1).Length2();
    if (d12 < DELTA_LEN * DELTA_LEN) return (P1 + P3) * 0.5;
    Point mid = (P1 + P2) * 0.5;
    Point dir = P2 - P1;
    double len = dir.Length();
    double h = len * SIN_60;
    Point perp = {-dir.y, dir.x};
    Point Q1 = mid + perp * (h / len);
    Point Q2 = mid - perp * (h / len);
    double cpP3 = dir.x * (P3.y - P1.y) - dir.y * (P3.x - P1.x);
    double cpQ1 = dir.x * (Q1.y - P1.y) - dir.y * (Q1.x - P1.x);
    Point Q = (cpP3 * cpQ1 < 0) ? Q1 : Q2;
    Point O = (P1 + P2 + Q) * (1.0 / 3.0);
    double R2 = (P1 - O).Length2();
    Point W = P3 - O;
    Point V = Q - P3;
    double a = V.Length2();
    if (a < DELTA_LEN * DELTA_LEN) return O;
    double c = W.Length2() - R2;
    double tF = c / a;
    return P3 + V * tF;
}

double angleBetween(const Point& u, const Point& v, double eps_len = 1e-9) {
    double len_u = u.Length();
    double len_v = v.Length();
    if (len_u < eps_len || len_v < eps_len) return 0.0;
    double cos_theta = u.dot(v) / (len_u * len_v);
    cos_theta = std::max(-1.0, std::min(1.0, cos_theta));
    return std::acos(cos_theta);
}

void AddMissingNeighbors(SteinerTree& graph)
{
    for (auto& p : graph.sPoints)
    {
        if (p.neighbors.size() == 3) continue;
        for (auto& o : graph.sPoints)
        {
            if (p.id == o.id) continue;
            for (auto& n : o.neighbors)
            {
                if (p.id == n.id) 
                {
                    auto it = std::find_if(p.neighbors.begin(), 
                        p.neighbors.end(), 
                        [&](const auto& n){ return n.id == o.id; });
                    if (it == p.neighbors.end()) p.neighbors.push_back(o);
                }
            }
        }
    }
}

void RelaxSteinerTree(SteinerTree& graph, int max_iter = 5) 
{
    bool changed = true;
    int iter = 0;

    while (changed && iter < max_iter) {
        changed = false;
        ++iter;
        for (auto& s : graph.sPoints) {
            if (s.neighbors.size() != 3) continue;

            const Point& p1 = s.neighbors[0].pos;
            const Point& p2 = s.neighbors[1].pos;
            const Point& p3 = s.neighbors[2].pos;

            double ang1 = angleBetween(p1 - s.pos, p2 - s.pos);
            double ang2 = angleBetween(p2 - s.pos, p3 - s.pos);
            double ang3 = angleBetween(p3 - s.pos, p1 - s.pos);

            if (std::abs(ang1 - ANGLE_120) > DELTA_ANGEL ||
                std::abs(ang2 - ANGLE_120) > DELTA_ANGEL ||
                std::abs(ang3 - ANGLE_120) > DELTA_ANGEL) 
            {
                Point new_pos = ComputeTorricelliPoint(p1, p2, p3);
                if ((new_pos - s.pos).Length() > MOVE_EPS) {
                    s.pos = new_pos;
                    changed = true;
                }
            }
        }
    }
    graph.length = CalculateGraphLength(graph); 
}

// int64_t unsigned treeCounter = 0;///////////////////////

SteinerTree Unwrap(SteinerPoint FPoint, vector<Terminal*> terminals)
{
    SteinerTree graph{0};
    graph.sPoints.push_back(FPoint);
    while (true)
    {
        bool isBreak = false;
        auto new_end = std::remove_if(terminals.begin(), terminals.end(), [](Terminal* t) 
        { 
            return t->parents.empty(); 
        });
        terminals.erase(new_end, terminals.end());

        if (terminals.size() == 0) break;

        SteinerPoint F;
        Terminal* X;
        int foundIdx = -1;
        for (size_t i = 0; i < terminals.size(); ++i)
        {
            X = terminals[i];
            for (auto& f : graph.sPoints)
            {
                for (auto id : f.edgesIds)
                {
                    if (id == X->id) 
                    { 
                        F = f; 
                        foundIdx = i;
                        isBreak= true;
                        break; 
                    }
                }
                if (isBreak) break;
            }
            if (isBreak) break;
        }

        terminals.erase(terminals.begin() + foundIdx);

        Terminal*& A = X->parents[0];
        Terminal*& B = X->parents[1];

        SteinerPoint&& S = FindSteinerPoint(*A, *B, F, *X);
        if (S.invalid) 
        {
            graph.length = INF;
            break;
        }
        graph.sPoints.push_back(move(S));
        terminals.push_back(A);
        terminals.push_back(B);
    }
    if (graph.length != INF)
    {
        AddMissingNeighbors(graph);
        graph.length = CalculateGraphLength(graph);
    }
    return move(graph);
}

int64_t xCounter = 0;
SteinerPoint MelzakRecursive(vector<Terminal*> t, SteinerTree& MST)
{
    SteinerPoint FPoint;
    int tNumbers = t.size();

    if (tNumbers == 3) return FindFPoint(*t[0], *t[1], *t[2]);
    
    for (size_t a = 0; a < tNumbers; ++a)
    {
        SteinerPoint::counter = 0;
        for (size_t b = a + 1; b < tNumbers; ++b)
        {
            auto pretendents = GetPretendent(t[a]->pos, t[b]->pos);
            for (Terminal& X : pretendents)
            {
                X.id = "X" + to_string(xCounter++);

                X.parents.push_back(t[a]);
                X.parents.push_back(t[b]);

                vector<Terminal*> terminals;
                for (size_t m = 0; m < tNumbers; ++m)
                {
                    if (m == a || m == b) continue;
                    terminals.push_back(t[m]);
                }
                terminals.push_back(&X);

                FPoint = MelzakRecursive(terminals, MST);
                // ++treeCounter;
                if (terminals.size() == 3 && 0 < !FPoint.invalid)
                {
                    SteinerTree&& graph = Unwrap(FPoint, terminals);
                    if (graph.length < MST.length)
                    {

                        // std::cout << ">t:" << treeCounter << " c:" << SteinerPoint::counter  << '<' << endl;
                        MST = move(graph);
                    }
                }
            }
        }
    }
    return FPoint;
}

SteinerTree MelzakAlgorithm(vector<Terminal> t)
{
    SteinerTree MST{INF};
    if (t.size() <= 1) return {0};
    if (t.size() == 2)
    { 
        return SteinerTree{Dist(t[0].pos, t[1].pos)};
    }
    vector<Terminal*> toMelzak;
    for (Terminal& tp : t)
        toMelzak.push_back(&tp);
    MelzakRecursive(toMelzak, MST);
    RelaxSteinerTree(MST, MAX_ITERS);
    return MST;
}
