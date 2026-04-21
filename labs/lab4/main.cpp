//Steiner-Melzak
#include <iostream>
#include <vector>

#include "includes/Utils.cpp"
#include "includes/Point.hpp"
#include "includes/SteinerGraph.hpp"
#include "includes/Const.hpp"

using namespace std;

double Dist(const Point& A, const Point& B) {
    return (A - B).Length();
}

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

vector<Point> GetPretendent(Point& A, Point& B)
{
    Point mid = (A + B) / 2;
    Point dir = B - A;
    double len = dir.Length();
    if (len < DELTA_LEN) return {};
    double h = len * SIN_60;
    Point perp = {-dir.y / len * h, dir.x / len * h};
    return {mid + perp, mid - perp};
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

SteinerGraph FindSteinerOnCircleAmongThreePoints(const Point& A, const Point& B, const Point& C) {
    // 1. Находим самую длинную сторону
    double ab = (B - A).Length();
    double bc = (C - B).Length();
    double ca = (A - C).Length();
    
    Point P1, P2, P3;
    double maxLen;
    if (ab >= bc && ab >= ca) { P1 = A; P2 = B; P3 = C; maxLen = ab; }
    else if (bc >= ab && bc >= ca) { P1 = B; P2 = C; P3 = A; maxLen = bc; }
    else { P1 = C; P2 = A; P3 = B; maxLen = ca; }

    // if (maxLen < DELTA_LEN) return P1; // вырожденный случай
    // 2. Строим ВНЕШНИЙ равносторонний треугольник P1-P2-X
    Point mid = (P1 + P2) * 0.5;
    Point dir = P2 - P1;
    // Перпендикуляр (поворот на 90°)
    Point perp = {-dir.y, dir.x};
    double h = maxLen * SQRT_3 * 0.5; // высота равностороннего треугольника
    // Два кандидата для X
    Point X1 = mid + perp * (h / maxLen);
    Point X2 = mid - perp * (h / maxLen);
    // auto candidates = ContendingTriangle(P1, P2);
    // Выбираем тот, что лежит по другую сторону от прямой P1P2, чем P3
    double cpP3 = CrossProduct(dir, P3 - P1);
    double cpX1 = CrossProduct(dir, X1 - P1);
    Point X = (cpP3 * cpX1 < 0) ? X1 : X2;
    // 3. Описанная окружность треугольника P1-P2-X
    // Для равностороннего треугольника центр = центроид, R = L / √3
    Point O = (P1 + P2 + X) / 3.0;
    // cout << "Center circle: " << O.x << ' ' << O.y << endl;
    double R2 = maxLen * maxLen / 3.0;
    // 4. Пересечение прямой P3->X с окружностью (O, R)
    // Параметрическое уравнение прямой: P(t) = P3 + t * (X - P3)
    // Известно, что при t=1 лежит точка X (она на окружности).
    // Ищем второй корень tS через теорему Виета для квадратного уравнения |P(t)-O|^2 = R2
    Point V = X - P3;
    double V2 = V.x * V.x + V.y * V.y;
    Point diff = P3 - O;
    double diff2 = diff.x * diff.x + diff.y * diff.y;
    // t1 * t2 = (|P3-O|^2 - R^2) / |V|^2. Поскольку t1 = 1:
    double tS = (diff2 - R2) / V2;
    // 5. Точка Штейнера
    SteinerPoint sp;
    sp.Create(Point{P3 + V * tS}, {P1.id, P2.id});
    SteinerGraph sg{0};
    sg.length = Dist(P1, {sp.x, sp.y}) + Dist(P2, {sp.x, sp.y}) + Dist(P3, {sp.x, sp.y});
    sg.sPoints.push_back(sp);
    //РАССТОЯНИЕ!!!
    return sg;
}

SteinerGraph FindSteinerAmongThreePoints(Point& A, Point& B, Point& C)
{
    SteinerPoint sp;
    SteinerGraph sg{0};
    if (AngleAt(A, B, C) >= ANGLE_120) 
    { 
        sp.SetEqual(B); 
        sg.length = Dist(A, B) + Dist(B, C);
        sg.sPoints.push_back(sp);
        return sg;
    }
    if (AngleAt(B, A, C) >= ANGLE_120)
    { 
        sp.SetEqual(A);
        sg.length = Dist(A, B) + Dist(A, C);
        sg.sPoints.push_back(sp);
        return sg;
    }
    if (AngleAt(A, C, B) >= ANGLE_120)
    { 
        sp.SetEqual(C);
        sg.length = Dist(C, A) + Dist(C, B);
        sg.sPoints.push_back(sp);
        return sg;
    }
    sg = FindSteinerOnCircleAmongThreePoints(A, B, C);
    //РАССТОЯНИЕ!!!
    return sg;
}

SteinerPoint FindSteinerPoint(Point& A, Point& B, Point& S, Point& X) {
    SteinerPoint sp;
    // SteinerGraph sg{0};
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
    //РАССТОЯНИЕ!!!
    return sp;
}

// Определить, с какой стороны от прямой AB лежит точка P
// Возвращает: +1 (слева/против часовой), -1 (справа/по часовой), 0 (на прямой)
int SideOfLine(const Point& A, const Point& B, const Point& P) {
    double cross = CrossProduct(B - A, P - A);
    if (cross > DELTA_LEN) return 1;
    return -1;
}
// Проверка: является ли кандидат X ВНЕШНИМ равносторонним треугольником относительно C
bool IsValidExternalVertex(const Point& A, const Point& B, const Point& C, const Point& X) {
    int sideC = SideOfLine(A, B, C);
    int sideX = SideOfLine(A, B, X);
    // ✅ Верно, если X и C по РАЗНЫЕ стороны от AB
    return (sideC * sideX < 0);
}

//

//
SteinerGraph MelzakRecursive(vector<Point>& t)
{
    SteinerGraph sg{0};// привожу к лучшей версии
    int tNumbers = t.size();
    double minLength = INF;

    if (tNumbers == 3)
    {
        return FindSteinerAmongThreePoints(t[0], t[1], t[2]);;
    }
    // SteinerPoint::counter = 0;

    for (size_t a = 0; a < tNumbers; ++a)
    {
        Point A = t[a];

        for (size_t b = a + 1; b < tNumbers; ++b)
        {
            Point B = t[b];

            auto pretendents = GetPretendent(A, B);
            for (Point X : pretendents)
            {
                for (size_t c = 0; c < tNumbers; ++c)
                {
                    if (c == a || c == b) continue;
                    Point C = t[c];
                    if (!IsValidExternalVertex(A, B, C, X)) continue;
                    // if (!IsPointOutsideCircle(A, B, X, C)) continue;

                    vector<Point> newTerminals;
                    for (size_t m = 0; m < tNumbers; ++m)
                    {
                        if (m == a || m == b) continue;
                        newTerminals.push_back(t[m]);
                    }
                    newTerminals.push_back(X);
                    auto temp = MelzakRecursive(newTerminals);
                    
                    if (temp.length < minLength)
                    {
                        minLength = temp.length;
                        sg = temp;
                        Point S = temp.sPoints[temp.sPoints.size() - 1].pos;
                        SteinerPoint sp = FindSteinerPoint(A, B, S, X);
                        sg.sPoints.push_back(sp);
                    }
                }
            }
        }
    }

    return sg;
}

SteinerGraph MelzakAlgorithm(vector<Point>& t)
{
    if (t.size() <= 1) return {0};
    if (t.size() == 2)
    { 
        //mb add id like TS == terminal and steiner
        return SteinerGraph{Dist(t[0], t[1])};
    }
    return MelzakRecursive(t);
}

int main(int argc, char const *argv[])
{
    vector<Point> terminals = GetTerminals("_in/terminals.txt");
    auto graph = MelzakAlgorithm(terminals);
    // cout << "Min len: " << graph.length << endl;
    // PrintTerminals(terminals);
    std::cout << "Length: " << graph.length << "\n";
    PrintSteiners(graph);

    std::string outPath = "_out/viz.txt";
    ToGraphviz(outPath, terminals, graph);

    return 0;
}
//g++ main.cpp -o START
