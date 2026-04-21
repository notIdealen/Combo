#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <string>
#include <utility>

using namespace std;

const double EPS = 1e-9;
const double INF = numeric_limits<double>::max();
const double PI = acos(-1.0);
const double SIN_60 = sqrt(3) / 2;

// ==================== ГЕОМЕТРИЯ ====================
struct Point {
    double x, y;
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(double k) const { return {x * k, y * k}; }
    Point operator/(double k) const { return {x / k, y / k}; }
    
    double length() const { return sqrt(x*x + y*y); }
    
    // Для сравнения точек с учётом погрешности
    bool operator==(const Point& o) const {
        return fabs(x - o.x) < EPS && fabs(y - o.y) < EPS;
    }
};

double dist(const Point& a, const Point& b) {
    return (a - b).length();
}

// Две возможные вершины равностороннего треугольника на отрезке AB
vector<Point> equilateralVertices(const Point& A, const Point& B) {
    Point mid = (A + B) / 2;
    Point dir = B - A;
    double len = dir.length();
    if (len < EPS) return {};
    double h = len * SIN_60;
    Point perp = {-dir.y / len * h, dir.x / len * h};
    return {mid + perp, mid - perp};
}

// ==================== ТОЧКА ФЕРМА-ТОРРИЧЕЛЛИ ====================
double angleAt(const Point& P, const Point& Q, const Point& R) {
    Point v1 = P - Q, v2 = R - Q;
    double n1 = v1.length(), n2 = v2.length();
    if (n1 * n2 < EPS) return PI;
    double dot = v1.x * v2.x + v1.y * v2.y;
    return acos(max(-1.0, min(1.0, dot / (n1 * n2))));
}

Point fermatTorricelli(const Point& A, const Point& B, const Point& C) {
    if (angleAt(A, B, C) >= 2*PI/3) return B;
    if (angleAt(B, A, C) >= 2*PI/3) return A;
    if (angleAt(A, C, B) >= 2*PI/3) return C;
    
    auto candidates = equilateralVertices(A, B);
    Point bestS = A;
    double minSum = INF;
    
    for (const Point& X : candidates) {
        Point S = {(A.x+B.x+C.x)/3, (A.y+B.y+C.y)/3};
        for (int iter = 0; iter < 100; ++iter) {
            Point grad = {0, 0};
            for (const Point& P : {A, B, C}) {
                double d = dist(S, P);
                if (d > EPS) {
                    grad.x += (S.x - P.x) / d;
                    grad.y += (S.y - P.y) / d;
                }
            }
            S.x -= 0.01 * grad.x;
            S.y -= 0.01 * grad.y;
        }
        double sum = dist(S,A) + dist(S,B) + dist(S,C);
        if (sum < minSum) { minSum = sum; bestS = S; }
    }
    return bestS;
}

// ==================== СТРУКТУРЫ ДЛЯ ТОПОЛОГИИ ====================

// Ребро графа: хранит индексы точек + длину
struct Edge {
    int u, v;           // индексы в массиве allPoints
    double length;
    Edge(int u_, int v_, double len = 0) : u(u_), v(v_), length(len) {}
};

// Результат: длина + точки Штейнера + рёбра
struct SteinerResult {
    double totalLength;
    vector<Point> steinerPoints;    // только точки Штейнера
    vector<Edge> edges;             // рёбра (индексы в allPoints)
    
    SteinerResult() : totalLength(0) {}
    SteinerResult(double len, vector<Point> pts, vector<Edge> eds = {})
        : totalLength(len), steinerPoints(move(pts)), edges(move(eds)) {}
};

// Вспомогательная структура: точка + её тип + глобальный индекс
struct IndexedPoint {
    Point pos;
    bool isSteiner;
    int globalIdx;  // уникальный индекс для этого экземпляра
    
    IndexedPoint(Point p, bool s, int idx) : pos(p), isSteiner(s), globalIdx(idx) {}
};

// ==================== АЛГОРИТМ МЕЛЗАКА С ТОПОЛОГИЕЙ ====================

// Глобальный счётчик для уникальных индексов
static int point_counter = 0;

// Рекурсивная функция: возвращает результат + все точки (терминалы + Штейнер)
pair<SteinerResult, vector<IndexedPoint>> 
melzakRecursiveWithTopology(vector<IndexedPoint> terminals) {
    
    int k = terminals.size();
    
    // === БАЗОВЫЕ СЛУЧАИ ===
    
    // 2 точки: прямое ребро
    if (k == 2) {
        SteinerResult res;
        res.totalLength = dist(terminals[0].pos, terminals[1].pos);
        res.edges.emplace_back(terminals[0].globalIdx, terminals[1].globalIdx, res.totalLength);
        return {res, terminals};  // возвращаем те же точки
    }
    
    // 3 точки: точка Ферма + 3 ребра
    if (k == 3) {
        Point S = fermatTorricelli(terminals[0].pos, terminals[1].pos, terminals[2].pos);
        int sIdx = point_counter++;  // уникальный индекс для новой точки Штейнера
        
        SteinerResult res;
        res.steinerPoints.push_back(S);
        
        double len = 0;
        for (int i = 0; i < 3; ++i) {
            double d = dist(S, terminals[i].pos);
            len += d;
            res.edges.emplace_back(sIdx, terminals[i].globalIdx, d);
        }
        res.totalLength = len;
        
        // Добавляем точку Штейнера к списку всех точек
        vector<IndexedPoint> all = terminals;
        all.emplace_back(S, true, sIdx);
        
        return {res, all};
    }
    
    // === РЕКУРСИВНЫЙ СЛУЧАЙ ===
    SteinerResult best;
    best.totalLength = INF;
    vector<IndexedPoint> bestAllPoints;
    
    for (int i = 0; i < k; ++i) {
        for (int j = i+1; j < k; ++j) {
            Point A = terminals[i].pos, B = terminals[j].pos;
            auto candidates = equilateralVertices(A, B);
            
            for (const Point& X : candidates) {
                // Новый список: все кроме i,j + псевдо-терминал
                vector<IndexedPoint> newTerms;
                for (int m = 0; m < k; ++m)
                    if (m != i && m != j) newTerms.push_back(terminals[m]);
                
                int pseudoIdx = point_counter++;  // временный индекс для псевдо-узла
                newTerms.emplace_back(X, false, pseudoIdx);  // isSteiner=false для псевдо
                
                // Рекурсия
                auto [sub, subAll] = melzakRecursiveWithTopology(newTerms);
                
                // Восстанавливаем реальную точку Штейнера для (A, B, X)
                Point S = fermatTorricelli(A, B, X);
                int sIdx = point_counter++;  // реальный индекс для точки Штейнера
                
                // === СБОРКА РЕЗУЛЬТАТА ===
                SteinerResult candidate = sub;
                
                // 1. Заменяем псевдо-узел на реальную точку Штейнера в рёбрах
                for (auto& e : candidate.edges) {
                    if (e.u == pseudoIdx) e.u = sIdx;
                    if (e.v == pseudoIdx) e.v = sIdx;
                }
                
                // 2. Добавляем саму точку Штейнера в список
                candidate.steinerPoints.push_back(S);
                
                // 3. Добавляем рёбра от новой точки Штейнера к A и B
                double w1 = dist(S, A), w2 = dist(S, B);
                candidate.edges.emplace_back(sIdx, terminals[i].globalIdx, w1);
                candidate.edges.emplace_back(sIdx, terminals[j].globalIdx, w2);
                
                // 4. Обновляем длину
                candidate.totalLength = sub.totalLength + w1 + w2;
                
                // 5. Собираем все точки (заменяя псевдо на реальный)
                vector<IndexedPoint> allPoints;
                for (const auto& p : subAll) {
                    if (p.globalIdx == pseudoIdx) {
                        allPoints.emplace_back(S, true, sIdx);  // заменяем
                    } else {
                        allPoints.push_back(p);
                    }
                }
                // Добавляем терминалы A и B, если их нет
                bool hasA = false, hasB = false;
                for (const auto& p : allPoints) {
                    if (p.globalIdx == terminals[i].globalIdx) hasA = true;
                    if (p.globalIdx == terminals[j].globalIdx) hasB = true;
                }
                if (!hasA) allPoints.push_back(terminals[i]);
                if (!hasB) allPoints.push_back(terminals[j]);
                
                // 6. Обновляем лучший результат
                if (candidate.totalLength < best.totalLength) {
                    best = candidate;
                    bestAllPoints = move(allPoints);
                }
            }
        }
    }
    
    return {best, bestAllPoints};
}

// ==================== ОСНОВНОЙ ИНТЕРФЕЙС ====================
struct FullSteinerResult {
    double totalLength;
    vector<Point> terminals;        // исходные терминалы
    vector<Point> steinerPoints;    // найденные точки Штейнера
    vector<Edge> edges;             // рёбра (индексы в combinedPoints)
    vector<Point> allPoints;        // [terminals..., steinerPoints...] для удобного доступа
    
    // Получить координаты точки по индексу из edge
    Point getPoint(int idx) const {
        return allPoints[idx];
    }
};

FullSteinerResult solveSteinerWithEdges(const vector<Point>& terminalPoints) {
    point_counter = 0;  // сброс счётчика
    
    // Создаём терминалы с уникальными индексами
    vector<IndexedPoint> terminals;
    for (const auto& p : terminalPoints) {
        terminals.emplace_back(p, false, point_counter++);
    }
    
    if (terminals.size() <= 1) {
        FullSteinerResult res;
        res.terminals = terminalPoints;
        res.allPoints = terminalPoints;
        return res;
    }
    
    auto [steinerRes, allIndexed] = melzakRecursiveWithTopology(terminals);
    
    // Собираем финальный результат
    FullSteinerResult result;
    result.totalLength = steinerRes.totalLength;
    result.terminals = terminalPoints;
    result.steinerPoints = steinerRes.steinerPoints;
    result.edges = steinerRes.edges;
    
    // allPoints: сначала терминалы, потом точки Штейнера
    result.allPoints = terminalPoints;
    result.allPoints.insert(result.allPoints.end(), 
                           steinerRes.steinerPoints.begin(), 
                           steinerRes.steinerPoints.end());
    
    return result;
}

// ==================== ПРИМЕР ИСПОЛЬЗОВАНИЯ ====================
int main() {
    cout << fixed << setprecision(6);
    
    cout << "=== Steiner Tree with Edges ===" << endl;
    vector<Point> terminals = {
        {0, 0}, {4, 0}, {4, 2}, {0, 2}, {7, 11}
    };
    
    FullSteinerResult result = solveSteinerWithEdges(terminals);
    
    cout << "\nTotal length: " << result.totalLength << endl;
    
    cout << "\nTerminals (" << result.terminals.size() << "):" << endl;
    for (size_t i = 0; i < result.terminals.size(); ++i) {
        cout << "  T" << i << ": (" << result.terminals[i].x 
             << ", " << result.terminals[i].y << ")\n";
    }
    
    cout << "\nSteiner points (" << result.steinerPoints.size() << "):" << endl;
    for (size_t i = 0; i < result.steinerPoints.size(); ++i) {
        cout << "  S" << i << ": (" << result.steinerPoints[i].x 
             << ", " << result.steinerPoints[i].y << ")\n";
    }
    
    cout << "\nEdges (" << result.edges.size() << "):" << endl;
    cout << "  Format: [idx_u, idx_v] (length) -> (x1,y1) -- (x2,y2)" << endl;
    for (const auto& e : result.edges) {
        Point pu = result.getPoint(e.u);
        Point pv = result.getPoint(e.v);
        cout << "  [" << e.u << "," << e.v << "] (" << e.length << ") -> "
             << "(" << pu.x << "," << pu.y << ") -- "
             << "(" << pv.x << "," << pv.y << ")\n";
    }
    
    // === ЭКСПОРТ ДЛЯ GRAPHVIZ ===
    cout << "\n=== DOT format for Graphviz ===" << endl;
    cout << "graph SteinerTree {" << endl;
    cout << "    layout=neato; mode=\"major\"; splines=line;" << endl;
    
    // Терминалы
    for (size_t i = 0; i < result.terminals.size(); ++i) {
        const auto& p = result.terminals[i];
        cout << "    T" << i << " [pos=\"" << p.x << "," << p.y 
             << "!\", label=\"T" << i << "\", shape=circle, "
             << "style=filled, fillcolor=\"#e74c3c\"];" << endl;
    }
    // Точки Штейнера
    for (size_t i = 0; i < result.steinerPoints.size(); ++i) {
        const auto& p = result.steinerPoints[i];
        // Индекс в allPoints: terminals.size() + i
        int idx = result.terminals.size() + i;
        cout << "    S" << idx << " [pos=\"" << p.x << "," << p.y 
             << "!\", label=\"S" << i << "\", shape=doublecircle, "
             << "style=filled, fillcolor=\"#3498db\"];" << endl;
    }
    // Рёбра
    cout << endl;
    for (const auto& e : result.edges) {
        // Определяем, терминал это или Штейнер, для правильного имени
        string nameU = (e.u < (int)result.terminals.size()) 
                     ? "T" + to_string(e.u) 
                     : "S" + to_string(e.u);
        string nameV = (e.v < (int)result.terminals.size()) 
                     ? "T" + to_string(e.v) 
                     : "S" + to_string(e.v);
        cout << "    " << nameU << " -- " << nameV << " [penwidth=2];" << endl;
    }
    cout << "}" << endl;
    
    return 0;
}
