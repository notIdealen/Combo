//Steiner-Melzak
#include <iostream>
#include <vector>
#include <cmath>

#include "includes/Utils.cpp"
#include "includes/Point.hpp"
#include "includes/SteinerGraph.hpp"
#include "includes/Const.hpp"

using namespace std;
////////////////////////////
// Угол между векторами в радианах [0, π]
double angleBetween(const Point& u, const Point& v, double eps_len = 1e-9) {
    double len_u = u.Length();
    double len_v = v.Length();
    
    if (len_u < eps_len || len_v < eps_len) {
        return 0.0; // Вырожденный вектор → угол не определён
    }
    
    double cos_theta = u.dot(v) / (len_u * len_v);
    // Защита от NaN из-за погрешности double
    cos_theta = std::max(-1.0, std::min(1.0, cos_theta));
    return std::acos(cos_theta);
}

// Проверка условия Штейнера: все три угла ≈ 120°
bool isSteinerPointValid(const Point& S, 
                         const Point& A, const Point& B, const Point& C,
                         double angle_eps = 1e-4) { // ~0.0057°

    double ang1 = angleBetween(A - S, B - S);
    double ang2 = angleBetween(B - S, C - S);
    double ang3 = angleBetween(C - S, A - S);
    
    return (std::abs(ang1 - ANGLE_120) < angle_eps) &&
           (std::abs(ang2 - ANGLE_120) < angle_eps) &&
           (std::abs(ang3 - ANGLE_120) < angle_eps);
}
///////////////////////////////



double CrossProduct(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

double Dist(const Terminal& A, const Terminal& B) {
    return (A.pos - B.pos).Length();
}

double AngleAt(Terminal& Q, Terminal& W, Terminal& E)
{
    Point v1 = Q.pos - W.pos;
    Point v2 = E.pos - W.pos;
    double mul = v1.x * v2.x + v1.y * v2.y;
    double lenV1 = v1.Length();
    double lenV2 = v2.Length();
    if (lenV1 < DELTA_LEN || lenV2 < DELTA_LEN) return PI;
    return acos(max(-1.0, min(1.0, mul / (lenV1 * lenV2))));
}

int SideOfLine(const Point& A, const Point& B, const Point& P) 
{
    double cross = CrossProduct(B - A, P - A);
    if (cross > DELTA_LEN) return 1;
    return -1;
}
bool IsValidExternalVertex(const Point& A, const Point& B, const Point& C, const Point& X) 
{
    int sideC = SideOfLine(A, B, C);
    int sideX = SideOfLine(A, B, X);
    return (sideC * sideX < 0);
}

vector<Terminal> GetPretendent(Point& A, Point& B)
{
    Point mid = (A + B) / 2;
    Point dir = B - A;
    double len = dir.Length();
    if (len < DELTA_LEN) return {};
    double h = len * SIN_60;
    Point perp = {-dir.y / len * h, dir.x / len * h};
    return {Terminal{mid + perp}, Terminal{mid - perp}};
}

bool IsPointOutsideCircle(const Point& A, const Point& B, const Point& X, const Point& C) {
    double L2 = (B - A).Length2();
    if (L2 < DELTA_LEN) return false;
    Point O = (A + B + X) * (1.0 / 3.0);
    double R2 = L2 / 3.0;
    double distC2 = (C - O).Length2();
    return distC2 > R2 + DELTA_LEN;
}

void AddNeighbors(const Terminal& A, const Terminal& B, const Terminal& C, SteinerPoint& S)
{
    const Terminal* terminals[] = {&A, &B, &C};
    string id = S.id.substr(1);

    // char xxxx = '0';
    // char yyyy = '0';
    // // cout << "SP: " << SteinerPoint::counter << endl;
    // if (SteinerPoint::counter > 195) xxxx = 'A'; else xxxx = '0';
    // if (S.id == "ST2") yyyy ='A'; else yyyy = '0';

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
    SteinerPoint F;
    F.Create({P3.pos + V * tS}, 'F', {P1.id, P2.id, P3.id});
    F.edgesLength = Dist(P1, F) + Dist(P2, F) + Dist(P3, F);
    AddNeighbors(A, B, C, F);
    return F;
}

SteinerPoint FindPointEqualTerminal(Terminal& A, Terminal& B, Terminal& C, char&& pref)
{
    SteinerPoint F{};//поменять на S
    if (B.id[0] == 'X') return F;/////////////////////////////////////
    F.SetEqual(B, pref, {A.id, C.id}); 
    F.edgesLength = Dist(A, B) + Dist(B, C);
    AddNeighbors(A, B, C, F);
    return F;
}

SteinerPoint FindFPoint(Terminal& A, Terminal& B, Terminal& C)
{
    SteinerPoint F{};

    if (AngleAt(A, B, C) >= ANGLE_120)// && AngleAt(A, B, C) <= ANGLE_240) 
        return FindPointEqualTerminal(A, B, C, 'F');

    if (AngleAt(B, A, C) >= ANGLE_120)// && AngleAt(B, A, C) <= ANGLE_240)
        return FindPointEqualTerminal(B, A, C, 'F');

    if (AngleAt(A, C, B) >= ANGLE_120)// && AngleAt(A, C, B) <= ANGLE_240)
        return FindPointEqualTerminal(A, C, B, 'F');

    return FindFPointInsideTriangle(A, B, C);
}

SteinerPoint FindSteinerPoint(Terminal& A, Terminal& B, Terminal& F, Terminal& X) 
{
    SteinerPoint S{};

    if (AngleAt(A, B, F) >= ANGLE_120) return FindPointEqualTerminal(A, B, F, 'S');

    if (AngleAt(B, A, F) >= ANGLE_120) return FindPointEqualTerminal(B, A, F, 'S');

    // if (AngleAt(A, F, B) >= ANGLE_120) return FindPointEqualTerminal(A, F, B, 'S');
    if (AngleAt(A, F, B) >= ANGLE_120) return S;

    double L2 = (B.pos-A.pos).Length2();
    // if (L2 < DELTA_LEN) return F;
    Point O = (A.pos + B.pos + X.pos) / 3.0;  // центр окружности
    double R2 = L2 / 3.0;          // R² = L²/3
    
    Point V = X.pos - F.pos;
    double V2 = V.Length2();
    // if (V2 < DELTA_LEN) return O;

    Point diff = F.pos - O;
    double tS = (diff.Length2() - R2) / V2; // Виета: t₂ = c/a

    S.Create({F.pos + V * tS}, 'S', {A.id, B.id, F.id});
    S.edgesLength = Dist(F, X);
    AddNeighbors(A, B, F, S);
    return S;
}

bool IsUsedId(const vector<string>& usedIds, const string& pid, const string& nid)
{
    for (auto id : usedIds)
        if (pid == id || nid == id) 
            return true;
    return false;
}

double CalculateGraphLength(const SteinerTree& graph)
{
    double len{};
    for (auto& p : graph.sPoints)
    {
        for (auto n : p.neighbors)
        {
            len += Dist(p, n); 
        }
    }
    return len;
}

vector<Terminal> DELETE;

SteinerTree Unwrap(SteinerPoint FPoint, vector<Terminal*> terminals)
{
    SteinerTree graph{INF};
    graph.sPoints.push_back(FPoint);
    graph.length = FPoint.edgesLength;
    while (true)
    {
        bool isBreak = false;
        auto new_end = std::remove_if(terminals.begin(), terminals.end(), [](Terminal* t) 
        { 
            return t->parents.empty(); 
        });
        terminals.erase(new_end, terminals.end());

        if (terminals.size() == 0) break;

        SteinerPoint F;//F для X по наличию в F id от X
        Terminal* X;
        int foundIdx = -1;
        for (size_t i = 0; i < terminals.size(); ++i)
        {
            X = terminals[i];
            for (auto f : graph.sPoints)
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
        // Проверка условия Штейнера: все три угла ≈ 120°
        // char x; if (S.id == "S1992") x = 'A';
        // if (S.id[1] != 'T' && !isSteinerPointValid(S.pos, A->pos, B->pos, F.pos))
        // {
        //     graph.length = INF;
        //     break;
        // }

        if (S.edgesLength <= 0) 
        {
            graph.length = INF;
            break;
        }
        graph.length = graph.length - Dist(F, *X) + S.edgesLength;

        graph.sPoints.push_back(move(S));
        terminals.push_back(A);
        terminals.push_back(B);
    }
    if (graph.length != INF)
    {
        double L = CalculateGraphLength(graph);
        graph.length = L;
    }
    
//     if (graph.sPoints.size() == 3)
//     {
//         int c = 0;
//         const int k = 1;
// //"2.05451,6.38978!"
//         for (auto p : graph.sPoints)
//         {
//             int x1 = std::floor(p.pos.x * k);
//             int y1 = std::floor(p.pos.y * k);
//             if (x1 == std::floor(2.05451 * k) && y1 == std::floor(6.38978 * k))
//             {
//                 cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
//                 ToGraphviz(DELETE, graph);
//                 cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
//             }
//         }
//     }

    return move(graph);
}

int TOTAL = 0;
int TOTAL_IN = 0;
int xCounter = 0;
SteinerPoint MelzakRecursive(vector<Terminal*> t, SteinerTree& best)
{
    SteinerPoint FPoint;
    int tNumbers = t.size();

    if (tNumbers == 3) return FindFPoint(*t[0], *t[1], *t[2]);
    
    for (size_t a = 0; a < tNumbers; ++a)
    {
        // for (size_t b = 0; b < tNumbers; ++b)
        for (size_t b = a + 1; b < tNumbers; ++b)
        {
            // if (a == b) continue;
            auto pretendents = GetPretendent(t[a]->pos, t[b]->pos);
            for (Terminal& X : pretendents)
            {
                // bool isValidPretendent = true;
                // for (size_t c = 0; c < tNumbers; ++c)
                // {
                //     if (c == a || c == b) continue;
                //     if (!IsValidExternalVertex(t[a]->pos, t[b]->pos, t[c]->pos, X.pos))
                //     {   
                //         isValidPretendent = false;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //         // continue;
                //         break;
                //     }
                // //     if (!IsPointOutsideCircle(t[a]->pos, t[b]->pos, X.pos, t[c]->pos))
                // //     {
                // //         isValidPretendent = false;
                // //         break;
                // //     }
                // }
                // if (!isValidPretendent) continue;
                
                X.id = "X" + to_string(xCounter++);

                X.parents.push_back(t[a]);
                X.parents.push_back(t[b]);

                vector<Terminal*> terminals;
                for (size_t m = 0; m < tNumbers; ++m)
                {
                    if (m == a || m == b) continue;
                    terminals.push_back(t[m]);
                }
                terminals.push_back(&X);/////////////
                FPoint = MelzakRecursive(terminals, best);
                // if (terminals.size() == 3)
                //     ++TOTAL;
                if (terminals.size() == 3 && 0 < FPoint.edgesLength)
                {
                    // ++TOTAL_IN;
                    // cout << "!!!!!!!!: " << TOTAL_IN << endl;
                    SteinerTree&& graph = Unwrap(FPoint, terminals);
                    if (graph.length < best.length)
                        best = move(graph);
                }
            }
        }
    }
    return FPoint;
}

SteinerTree MelzakAlgorithm(vector<Terminal> t)
{
    SteinerTree best{INF};
    if (t.size() <= 1) return {0};
    if (t.size() == 2)
    { 
        return SteinerTree{Dist(t[0].pos, t[1].pos)};
    }
    vector<Terminal*> toMelzak;
    for (Terminal& tp : t)
        toMelzak.push_back(&tp);
    MelzakRecursive(toMelzak, best);
    return best;
}

int main(int argc, char const *argv[])
{
    vector<Terminal> terminals = GetTerminals("_in/terminals.txt");
    DELETE = terminals;
    auto graph = MelzakAlgorithm(terminals);
    // cout << TOTAL << ' ' << TOTAL_IN << endl;
    std::cout << "BESTLength: " << graph.length << "\n";
    ToGraphviz(terminals, graph);

    return 0;
}
//g++ main.cpp -o START
