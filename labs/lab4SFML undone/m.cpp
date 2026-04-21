#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

const double EPS = 1e-9;
const double INF = numeric_limits<double>::max();
const double PI = acos(-1.0);
const double SIN_60 = sqrt(3) / 2;

// ==================== ГЕОМЕТРИЯ ====================

struct Point {
    double x, y;
    int edgeNum{0};
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(double k) const { return {x * k, y * k}; }
    Point operator/(double k) const { return {x / k, y / k}; }//new
    
    double length() const { return sqrt(x*x + y*y); }
};

double dist(const Point& a, const Point& b) {
    return (a - b).length();
}

// Две возможные вершины равностороннего треугольника на отрезке AB
// равносторонние вертикали
vector<Point> equilateralVertices(const Point& A, const Point& B) 
{
    Point mid = (A + B) / 2;
    Point dir = B - A;
    double len = dir.length();
    if (len < EPS) return {};
    double h = len * SIN_60;
    Point perp = {-dir.y / len * h, dir.x / len * h};
    return {mid + perp, mid - perp};
}

// ==================== ТОЧКА ФЕРМА-ТОРРИЧЕЛЛИ ====================
// Проверка угла в точке Q для треугольника PQR
double angleAt(const Point& P, const Point& Q, const Point& R) {
    Point v1 = P - Q;
    Point v2 = R - Q;
    double dot = v1.x * v2.x + v1.y * v2.y;
    double n1 = v1.length(), n2 = v2.length();
    
    if (n1 * n2 < EPS) return PI;
    return acos(max(-1.0, min(1.0, dot / (n1 * n2))));
}

// Нахождение точки Ферма-Торричелли для 3 точек
Point fermatTorricelli(const Point& A, const Point& B, const Point& C) {
    if (angleAt(A, B, C) >= 2*PI/3) return B;
    if (angleAt(B, A, C) >= 2*PI/3) return A;
    if (angleAt(A, C, B) >= 2*PI/3) return C;
    
    Point S = {(A.x+B.x+C.x)/3, (A.y+B.y+C.y)/3};
    
    for (int iter = 0; iter < 4000; ++iter) {
        // Защита от деления на ноль
        for (const Point& P : {A, B, C}) {
            if (dist(S, P) < 1e-10) {
                S = S + Point{1e-8, 1e-8};
                break;
            }
        }
        
        double wA = 1.0 / dist(S, A);
        double wB = 1.0 / dist(S, B);
        double wC = 1.0 / dist(S, C);
        double W = wA + wB + wC;
        
        Point newS = {
            (wA*A.x + wB*B.x + wC*C.x) / W,
            (wA*A.y + wB*B.y + wC*C.y) / W
        };
        
        if (dist(S, newS) < 1e-12) return newS;
        S = newS;
    }
    return S;
}
// ==================== АЛГОРИТМ МЕЛЗАКА ====================
struct SteinerResult {
    double length;
    vector<Point> steinerPoints;
};

// Рекурсивная функция Мелзака
SteinerResult melzakRecursive(vector<Point> terminals) {
    int k = terminals.size();
    
    // Базовые случаи
    // if (k <= 1) return {0, {}};
    if (k == 2) return {dist(terminals[0], terminals[1]), {}};
    if (k == 3) {
        Point S = fermatTorricelli(terminals[0], terminals[1], terminals[2]);
        double len = dist(S, terminals[0]) + dist(S, terminals[1]) + dist(S, terminals[2]);
        return {len, {S}};
    }
    
    SteinerResult best = {INF, {}};
    
    // Перебор всех пар терминалов для объединения
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            Point A = terminals[i];
            Point B = terminals[j];
            auto candidates = equilateralVertices(A, B);
            
            for (const Point& X : candidates) {
                // Новый набор: заменяем {A, B} на псевдо-терминал X
                vector<Point> newTerminals;
                for (int m = 0; m < k; ++m) {
                    if (m != i && m != j) {
                        newTerminals.push_back(terminals[m]);
                    }
                }
                newTerminals.push_back(X);
                
                // Рекурсивный вызов
                SteinerResult sub = melzakRecursive(newTerminals);
                
                // Восстанавливаем точку Штейнера для пары A, B
                Point S = fermatTorricelli(A, B, X);
                double edgeLen = dist(S, A) + dist(S, B);
                
                // Полная длина = поддерево + рёбра к A и B
                double totalLen = sub.length + edgeLen;
                
                if (totalLen < best.length) {
                    best.length = totalLen;
                    best.steinerPoints = sub.steinerPoints;
                    best.steinerPoints.push_back(S);
                }
            }
        }
    }
    return best;
}

// Основной интерфейс
SteinerResult solveSteinerMelzak(const vector<Point>& terminals) {
    if (terminals.size() <= 1) return {0, {}};
    return melzakRecursive(terminals);
}

// ==================== ПРИМЕР ИСПОЛЬЗОВАНИЯ ====================
std::vector<Point> SetTerminals(std::string path)
{   
    std::ifstream in(path);
    if (!in.is_open()) return {};
    std::string line{};
    std::vector<Point> terminals{};
    double x{}, y{};
    while (getline(in, line))
    {
        if (line.empty()) break;
        std::stringstream ss(line);
        ss >> x >> y;
        terminals.push_back({x, y});
    }
    return move(terminals);
}

int main() {
    vector<Point> terminals = SetTerminals("_in/gviz.txt");
    SteinerResult points = solveSteinerMelzak(terminals);

     // === ЭКСПОРТ ДЛЯ GRAPHVIZ ===
    cout << "\n=== DOT format for Graphviz ===" << endl;
    cout << "graph SteinerTree {" << endl;
    cout << "    layout=neato; mode=\"major\"; splines=line;" << endl;
    
    // Терминалы
    for (size_t i = 0; i < terminals.size(); ++i) {
        const auto& p = terminals[i];
        cout << "    T" << i << " [pos=\"" << p.x << "," << p.y 
             << "!\", label=\"T" << i << "\", shape=circle, "
             << "style=filled, fillcolor=\"#e74c3c\"];" << endl;
    }
    // Точки Штейнера
    for (size_t i = 0; i < points.steinerPoints.size(); ++i) {
        const auto& p = points.steinerPoints[i];
        cout << "    S" << i << " [pos=\"" << p.x << "," << p.y 
             << "!\", label=\"S" << i << "\", shape=doublecircle, "
             << "style=filled, fillcolor=\"#3498db\"];" << endl;
    }
    // Рёбра
    struct Edge
    {
        string terminal;
        string steiner;
        double minDist;
    };
    vector<Edge> edges{};

    for (size_t i = 0; i < terminals.size(); ++i)
    {
        Edge edge;
        edge.minDist = -1;
        edge.terminal = "T" + to_string(i);
        int steinerPoint{-1};
        for (size_t j = 0; j < points.steinerPoints.size(); ++j)
        {
            auto distance = dist(terminals[i], points.steinerPoints[j]);
            if (edge.minDist < 0 || distance < edge.minDist)
            {
                edge.minDist = distance;
                edge.steiner = "S" + to_string(j);
                steinerPoint = j;
            }
        }
        points.steinerPoints[steinerPoint].edgeNum += 1;
        edges.push_back(edge);
    }
    //steiner edges
    for (size_t i = 0; i < points.steinerPoints.size() - 1; ++i)
    {
        while (points.steinerPoints[i].edgeNum < 3)
        {
            Edge edge;
            edge.minDist = -1;
            edge.terminal = "S" + to_string(i);
            int steinerPoint{-1};
            for (size_t j = i + 1; j < points.steinerPoints.size(); ++j)
            {
                auto distance = dist(points.steinerPoints[i], points.steinerPoints[j]);
                if (points.steinerPoints[j].edgeNum < 3 && (edge.minDist < 0 || distance < edge.minDist))
                {
                    edge.minDist = distance;
                    edge.steiner = "S" + to_string(j);
                    steinerPoint = j;
                }
            }
            points.steinerPoints[steinerPoint].edgeNum += 1;
            edges.push_back(edge);
            points.steinerPoints[i].edgeNum += 1;
        }
    }
    cout << endl;
    for (const auto& e : edges) {
        cout << "    " << e.terminal << " -- " << e.steiner << " [penwidth=2];" << endl;
    }
    cout << "}" << endl;
    return 0;
}
//g++ m.cpp -o fw
