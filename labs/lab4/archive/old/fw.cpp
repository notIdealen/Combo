#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>

#include "includes/Point.hpp"

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
    Point operator/(double k) const { return {x / k, y / k}; }//new
    
    double length() const { return sqrt(x*x + y*y); }
};

double dist(const Point& a, const Point& b) {
    return (a - b).length();
}

// Поворот точки на 60° против часовой стрелки
Point rotate60(const Point& p, const Point& center) {
    double dx = p.x - center.x;
    double dy = p.y - center.y;
    double cos60 = 0.5;
    double sin60 = SIN_60;
    return {
        center.x + dx * cos60 - dy * sin60,
        center.y + dx * sin60 + dy * cos60
    };
}

// Две возможные вершины равностороннего треугольника на отрезке AB
// равносторонние вертикали
vector<Point> equilateralVertices(const Point& A, const Point& B) 
{
    //середина отрезка
    // Point mid = {(A.x + B.x) / 2, (A.y + B.y) / 2};
    Point mid = (A + B) / 2;
    //координаты вектора, длины по х,у относительно точки А
    // Point dir = {B.x - A.x, B.y - A.y};
    Point dir = B - A;
    //длина вектора АВ
    double len = dir.length();
    if (len < EPS) return {};
    // Высота равностороннего треугольника
    double h = len * SIN_60;
    // Перпендикуляр определенного размера к AB
    Point perp = {-dir.y / len * h, dir.x / len * h};
    //точка С для построение равностороннего треугольника
    //2 варианта расположения
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
    if (angleAt(A, B, C) >= 2 * PI / 3) return B;
    if (angleAt(B, A, C) >= 2 * PI / 3) return A;
    if (angleAt(A, C, B) >= 2 * PI / 3) return C;
    // Иначе используем конструкцию Мелзака
    auto candidates = equilateralVertices(A, B);
    Point bestS = A;
    double minSum = INF;
    
    for (const Point& X : candidates) {
        Point S = {(A.x + B.x + C.x) / 3, (A.y + B.y + C.y) / 3};
        
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
        
        double sum = dist(S, A) + dist(S, B) + dist(S, C);
        if (sum < minSum) {
            minSum = sum;
            bestS = S;
        }
    }
    return bestS;
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
    if (k <= 1) return {0, {}};
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

int main() {
    cout << fixed << setprecision(6);
    
    cout << L"=== Square 1x1 ===" << endl;
    vector<Point> square = {{0, 0}, {4, 0}, {4, 2}, {0, 2}, {7, 11}, {34, 9}, {23, 17}};
    SteinerResult points = solveSteinerMelzak(square);
    // cout << "Min len: " << points.length << endl;
    // cout << "Shteiner points: " << points.steinerPoints.size() << endl;
    // for (size_t i = 0; i < points.steinerPoints.size(); ++i) {
    //     cout << "  S" << i << ": (" << points.steinerPoints[i].x 
    //          << ", " << points.steinerPoints[i].y << ")" << endl;
    // }
    // cout << endl;
    return 0;
}
//g++ fw.cpp -o fw
//TESTS
// // Пример 2: Равносторонний треугольник
// cout << "=== Triangle ===" << endl;
// vector<Point> triangle = {{0, 0}, {1, 0}, {0.5, sqrt(3)/2}};
// SteinerResult res2 = solveSteinerMelzak(triangle);
// cout << "Min len: " << res2.length << endl;
// cout << "Waiting: ~" << sqrt(3) << endl;
// cout << endl;

// // Пример 3: Три точки в линию
// cout << "=== 3 points in line ===" << endl;
// vector<Point> line = {{0, 0}, {1, 0}, {2, 0}};
// SteinerResult res3 = solveSteinerMelzak(line);
// cout << "Min len: " << res3.length << endl;
// cout << "waiting: 2.0 (wo Steiner points)" << endl;
