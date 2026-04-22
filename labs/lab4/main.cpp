//Steiner-Melzak
#include <iostream>
#include <vector>

#include "includes/Utils.cpp"
#include "includes/Point.hpp"
#include "includes/SteinerGraph.hpp"
#include "includes/Const.hpp"

using namespace std;

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

// Псевдоскалярное произведение (2D cross product)
double CrossProduct(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

// Проверка: лежит ли точка C ВНЕ описанной окружности равностороннего △ABX
// Возвращает true, если C снаружи (можно строить точку Штейнера)
bool IsPointOutsideCircle(const Point& A, const Point& B, const Point& X, const Point& C) {
    // Длина стороны равностороннего треугольника
    double L2 = (B - A).Length2();
    if (L2 < DELTA_LEN) return false;  // вырождение
    // Центр окружности = центроид равностороннего треугольника
    Point O = (A + B + X) * (1.0 / 3.0);
    // Квадрат радиуса: R² = L² / 3
    double R2 = L2 / 3.0;
    // Квадрат расстояния от C до центра
    double distC2 = (C - O).Length2();
    // C снаружи, если |C-O|² > R² (с запасом на погрешность)
    return distC2 > R2 + DELTA_LEN;
}

SteinerGraph FindFPointOnCircle(const Terminal& A, const Terminal& B, const Terminal& C) {
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

    SteinerGraph sg{-1};
    F.edgesLength = Dist(P1, F) + Dist(P2, F) + Dist(P3, F);
    sg.length = F.edgesLength;
    // cout << "DIST: " << sg.length << endl;
    sg.sPoints.push_back(F);
    return sg;
}

SteinerGraph FindFPoint(Terminal& A, Terminal& B, Terminal& C)
{
    SteinerPoint F;
    SteinerGraph sg{-1};

    if (AngleAt(A, B, C) >= ANGLE_120) 
    { 
        if (B.id[0] == 'X') return sg;

        F.SetEqual(B, 'F', {A.id, C.id}); 
        F.edgesLength = Dist(A, B) + Dist(B, C);
        sg.length = F.edgesLength;
        sg.sPoints.push_back(F);
        return sg;
    }
    if (AngleAt(B, A, C) >= ANGLE_120)
    {
        if (A.id[0] == 'X') return sg;

        F.SetEqual(A, 'F', {B.id, C.id});
        F.edgesLength = Dist(B, A) + Dist(A, C);
        sg.length = F.edgesLength;
        sg.sPoints.push_back(F);
        return sg;
    }
    if (AngleAt(A, C, B) >= ANGLE_120)
    { 
        if (C.id[0] == 'X') return sg;

        F.SetEqual(C, 'F', {A.id, C.id});
        F.edgesLength = Dist(A, C) + Dist(C, B);
        sg.length = F.edgesLength;
        sg.sPoints.push_back(F);
        return sg;
    }

    sg = FindFPointOnCircle(A, B, C);
    return sg;
}
SteinerPoint FindSteinerPoint(Terminal& A, Terminal& B, Terminal& F, Terminal& X) 
{
    SteinerPoint S;

    if (AngleAt(A, B, F) >= ANGLE_120) 
    {
        if (B.id[0] == 'X') 
            return S;
        S.SetEqual(B, 'S', {A.id, F.id});
        S.edgesLength = Dist(A, F);
        return S;
    }
    if (AngleAt(B, A, F) >= ANGLE_120)
    {
        if (A.id[0] == 'X') 
            return S;
        S.SetEqual(A, 'S', {B.id, F.id});
        S.edgesLength = Dist(B, F);
        return S;
    }
    if (AngleAt(A, F, B) >= ANGLE_120)
    {
        return S;
    }

    double L2 = (B.pos-A.pos).Length2();
    // if (L2 < DELTA_LEN) return C;
    Point O = (A.pos + B.pos + X.pos) / 3.0;  // центр окружности
    double R2 = L2 / 3.0;          // R² = L²/3
    
    Point V = X.pos - F.pos;
    double V2 = V.Length2();
    // if (V2 < DELTA_LEN) return O;

    Point diff = F.pos - O;
    double tS = (diff.Length2() - R2) / V2; // Виета: t₂ = c/a

    S.Create({F.pos + V * tS}, 'S', {A.id, B.id, F.id});
    S.edgesLength = Dist(A, B) + Dist(B, F);
    return S;
}

// Определить, с какой стороны от прямой AB лежит точка P
// Возвращает: +1 (слева/против часовой), -1 (справа/по часовой), 0 (на прямой)
int SideOfLine(const Point& A, const Point& B, const Point& P) 
{
    double cross = CrossProduct(B - A, P - A);
    if (cross > DELTA_LEN) return 1;
    return -1;
}
// Проверка: является ли кандидат X ВНЕШНИМ равносторонним треугольником относительно C
bool IsValidExternalVertex(const Point& A, const Point& B, const Point& C, const Point& X) 
{
    int sideC = SideOfLine(A, B, C);
    int sideX = SideOfLine(A, B, X);
    // ✅ Верно, если X и C по РАЗНЫЕ стороны от AB
    // !!!!!!! а если на линии? мб <= 0
    return (sideC * sideX < 0);
}

// double recalculateLength(const SteinerGraph& g) {
//     double total = 0.0;
//     for (const auto& e : g.edges) {
//         total += e.weight;
//     }
//     return total;
// }

SteinerGraph MelzakRecursive(vector<Terminal>& t)
{
    int xCounter = 0;
    SteinerGraph bestGraph{0};
    int tNumbers = t.size();
    double bestLength = INF;

    if (tNumbers == 3)
    {
        return FindFPoint(t[0], t[1], t[2]);
    }
    
    for (size_t a = 0; a < tNumbers; ++a)
    {
        for (size_t b = a + 1; b < tNumbers; ++b)
        {
            Terminal A = t[a];
            Terminal B = t[b];
            auto pretendents = GetPretendent(A.pos, B.pos);
            for (Terminal X : pretendents)
            {
                for (size_t c = 0; c < tNumbers; ++c)
                {
                    if (c == a || c == b) continue;
                    Terminal C = t[c];
                    if (!IsValidExternalVertex(A.pos, B.pos, C.pos, X.pos)) break;//?
                    if (!IsPointOutsideCircle(A.pos, B.pos, X.pos, C.pos)) break;//?
                    // if (!IsValidExternalVertex(A.pos, B.pos, C.pos, X.pos)) continue;
                    // if (!IsPointOutsideCircle(A.pos, B.pos, X.pos, C.pos)) continue;
                    X.id = "X" + to_string(xCounter++);
                    vector<Terminal> newTerminals;
                    for (size_t m = 0; m < tNumbers; ++m)
                    {
                        if (m == a || m == b) continue;
                        newTerminals.push_back(t[m]);
                    }
                    newTerminals.push_back(X);
                    auto temp = MelzakRecursive(newTerminals);

                    SteinerPoint F;//F для X
                    for (const auto& f : temp.sPoints)
                    {
                        for (size_t i = 0; i < f.edgesIds.size(); ++i)
                            if (f.edgesIds[i] == X.id) { F = f; break; }
                    }

                    //получить точку штейнера
                    auto S = FindSteinerPoint(A, B, F, X);
                    // double currLength = Dist()

                    // cout << "TempLen: " << temp.length << ", bl(" << bestLength << ")" << '\n';

                    if (0 < temp.length && temp.length < bestLength)
                    {
                    //     cout << "BEST before: " << bestLength;
                    //     bestLength = temp.length;
                    //     cout << ", BEST after: " << bestLength << '\n';
                    //     temp.sPoints.push_back(F);
                    //     bestGraph = move(temp);

                    //     SteinersToGraphviz(bestGraph);
                    //     TerminalsToGraphviz({X});
                    //     cout << "\n";
                    }
                    // cout << "----------------------------\n";
                }
            }
        }
    }
    return bestGraph;
}

SteinerGraph MelzakAlgorithm(vector<Terminal>& t)
{
    if (t.size() <= 1) return {0};
    if (t.size() == 2)
    { 
        return SteinerGraph{Dist(t[0].pos, t[1].pos)};
    }
    return MelzakRecursive(t);
}

int main(int argc, char const *argv[])
{
    vector<Terminal> terminals = GetTerminals("_in/terminals.txt");
    auto graph = MelzakAlgorithm(terminals);

    std::cout << "BESTLength: " << graph.length << "\n";
    // PrintSteiners(graph);
    // ToGraphviz(terminals, graph);

    return 0;
}
//g++ main.cpp -o START
