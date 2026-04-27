//=============================================================================
//  Steiner Minimal Tree via Melzak's Algorithm
//  Компиляция: g++ -std=c++17 -O2 steiner_melzak.cpp -o steiner
//  Запуск: ./steiner
//  Визуализация: открыть data/tree.dot в https://dreampuf.github.io/GraphvizOnline (движок neato)
//=============================================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <limits>

using namespace std;

//=============================================================================
//  Константы и утилиты
//=============================================================================
const double EPS = 1e-9;
const double INF = numeric_limits<double>::max();
const double PI = acos(-1.0);
const double SIN_60 = sqrt(3) / 2;
const double COS_60 = 0.5;

// Глобальный счётчик для генерации уникальных ID
static int global_id_counter = 0;

//=============================================================================
//  Структура точки
//=============================================================================
struct Point {
    double x, y;
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    
    Point operator+(const Point& o) const { return {x + o.x, y + o.y}; }
    Point operator-(const Point& o) const { return {x - o.x, y - o.y}; }
    Point operator/(double d) const { return {x / d, y / d}; }
    Point operator*(double d) const { return {x * d, y * d}; }
    
    double length() const { return sqrt(x*x + y*y); }
};

double dist(const Point& A, const Point& B) {
    return (A - B).length();
}

//=============================================================================
//  Узел графа
//=============================================================================
struct GraphNode {
    string id;
    Point pos;
    bool isSteiner;
    
    GraphNode() : pos(), isSteiner(false) {}
    GraphNode(const Point& p, bool s) : pos(p), isSteiner(s) {}
};

//=============================================================================
//  Граф (результат алгоритма)
//=============================================================================
struct SteinerTree {
    double totalLength = 0;
    vector<GraphNode> nodes;
    vector<pair<string, string>> edges;
    map<string, int> nodeIndex;  // id -> индекс в nodes
    
    void rebuildIndex() {
        nodeIndex.clear();
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodeIndex[nodes[i].id] = static_cast<int>(i);
        }
    }
    
    bool hasNode(const string& id) const {
        return nodeIndex.count(id) > 0;
    }
    
    void addNodeIfNew(const GraphNode& node) {
        if (!hasNode(node.id)) {
            nodeIndex[node.id] = static_cast<int>(nodes.size());
            nodes.push_back(node);
        }
    }
};

//=============================================================================
//  Геометрические функции
//=============================================================================

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

// Угол в точке Q для треугольника P-Q-R
double angleAt(const Point& P, const Point& Q, const Point& R) {
    Point v1 = P - Q, v2 = R - Q;
    double n1 = v1.length(), n2 = v2.length();
    if (n1 * n2 < EPS) return PI;
    double dot = v1.x * v2.x + v1.y * v2.y;
    return acos(max(-1.0, min(1.0, dot / (n1 * n2))));
}

// Точка Ферма-Торричелли для 3 точек (градиентный спуск)
Point fermatTorricelli(const Point& A, const Point& B, const Point& C) {
    // Проверка на угол >= 120°
    if (angleAt(A, B, C) >= 2*PI/3) return B;
    if (angleAt(B, A, C) >= 2*PI/3) return A;
    if (angleAt(A, C, B) >= 2*PI/3) return C;
    
    // Градиентный спуск для минимизации суммы расстояний
    Point S = {(A.x+B.x+C.x)/3, (A.y+B.y+C.y)/3};
    double step = 0.1;
    
    for (int iter = 0; iter < 200; ++iter) {
        Point grad = {0, 0};
        for (const Point& P : {A, B, C}) {
            double d = dist(S, P);
            if (d > EPS) {
                grad.x += (S.x - P.x) / d;
                grad.y += (S.y - P.y) / d;
            }
        }
        double gradLen = grad.length();
        if (gradLen < EPS) break;
        S = S - grad * (step / gradLen);
        step *= 0.99;  // затухание шага
    }
    return S;
}

//=============================================================================
//  Экспорт в DOT (Graphviz)
//=============================================================================
void exportToDot(const string& filename, const SteinerTree& graph) {
    ofstream out(filename);
    if (!out) { cerr << "Error: cannot write to " << filename << endl; return; }
    
    out << "graph SteinerTree {\n";
    out << "    layout=neato;\n";
    out << "    mode=\"major\";\n";
    out << "    splines=line;\n";
    out << "    node [fontname=\"Arial\", fontsize=10];\n\n";
    
    for (const auto& node : graph.nodes) {
        if (!node.id.empty() && node.id[0] == 'P') continue;
        string color = node.isSteiner ? "#3498db" : "#e74c3c";
        string shape = node.isSteiner ? "doublecircle" : "circle";
        out << "    " << node.id << " "
            << "[pos=\"" << fixed << setprecision(4) 
            << node.pos.x << "," << node.pos.y << "!\", "
            << "label=\"" << node.id << "\", "
            << "shape=" << shape << ", "
            << "style=filled, fillcolor=\"" << color << "\"];\n";
    }
    
    out << "\n";
    for (const auto& [u, v] : graph.edges) {
        if ((u.empty()||u[0]=='P') || (v.empty()||v[0]=='P')) continue;
        out << "    " << u << " -- " << v << " [color=\"#2c3e50\", penwidth=2];\n";
    }
    out << "}\n";
    out.close();
    cout << "✓ Exported: " << filename << endl;
}
//=============================================================================
//  Вспомогательные функции для работы с графом
//=============================================================================

// Клонировать граф с заменой одного ID на другой
SteinerTree cloneWithReplace(const SteinerTree& src, const string& oldId, const string& newId) {
    SteinerTree result;
    result.totalLength = src.totalLength;
    
    for (const auto& node : src.nodes) {
        if (!node.id.empty() && node.id[0] == 'P') continue;  // фильтр псевдо-узлов
        result.nodes.push_back(node);
    }
    result.rebuildIndex();
    
    for (const auto& [u, v] : src.edges) {
        string nu = (u == oldId) ? newId : u;
        string nv = (v == oldId) ? newId : v;
        if ((nu[0]!='P') && (nv[0]!='P'))  // фильтр
            result.edges.emplace_back(nu, nv);
    }
    return result;
}

// Удалить все псевдо-узлы из графа
SteinerTree removePseudoNodes(SteinerTree graph) {
    SteinerTree result;
    result.totalLength = graph.totalLength;
    
    for (const auto& node : graph.nodes) {
        if (!node.id.empty() && node.id[0] == 'P') continue;
        result.addNodeIfNew(node);
    }
    
    for (const auto& [u, v] : graph.edges) {
        if ((u.empty()||u[0]!='P') && (v.empty()||v[0]!='P')) {
            if (result.hasNode(u) && result.hasNode(v))
                result.edges.emplace_back(u, v);
        }
    }
    result.rebuildIndex();
    return result;
}

//=============================================================================
//  Алгоритм Мелзака (рекурсивный)
//=============================================================================
SteinerTree melzakRecursive(vector<GraphNode> terminals) {
    int k = static_cast<int>(terminals.size());
    
    // === БАЗОВЫЕ СЛУЧАИ ===
    if (k == 2) {
        SteinerTree g;
        g.totalLength = dist(terminals[0].pos, terminals[1].pos);
        g.nodes = terminals;
        g.edges.emplace_back(terminals[0].id, terminals[1].id);
        g.rebuildIndex();
        return g;
    }
    
    if (k == 3) {
        Point A = terminals[0].pos, B = terminals[1].pos, C = terminals[2].pos;
        Point S = fermatTorricelli(A, B, C);
        string sId = "S" + to_string(global_id_counter++);
        
        SteinerTree g;
        // Добавляем точку Штейнера
        GraphNode steiner(S, true); steiner.id = sId;
        g.nodes.push_back(steiner);
        // Добавляем терминалы
        for (const auto& t : terminals) g.nodes.push_back(t);
        // Рёбра от Штейнера ко всем терминалам
        for (const auto& t : terminals) g.edges.emplace_back(sId, t.id);
        
        g.totalLength = dist(S,A) + dist(S,B) + dist(S,C);
        g.rebuildIndex();
        return g;
    }
    
    // === РЕКУРСИВНЫЙ СЛУЧАЙ ===
    SteinerTree best;
    best.totalLength = INF;
    
    for (int i = 0; i < k; ++i) {
        for (int j = i+1; j < k; ++j) {
            Point A = terminals[i].pos, B = terminals[j].pos;
            auto candidates = equilateralVertices(A, B);
            
            for (const Point& X : candidates) {
                // Новый список терминалов: все кроме i,j + псевдо-узел
                vector<GraphNode> newTerms;
                for (int m = 0; m < k; ++m)
                    if (m != i && m != j) newTerms.push_back(terminals[m]);
                
                string pseudoId = "P" + to_string(global_id_counter++);
                GraphNode pseudo(X, false); pseudo.id = pseudoId;
                newTerms.push_back(pseudo);
                
                // Рекурсивный вызов
                SteinerTree sub = melzakRecursive(newTerms);
                
                // Точка Штейнера для (A, B, X)
                Point S = fermatTorricelli(A, B, X);
                string sId = "S" + to_string(global_id_counter++);
                
                // === СБОРКА НОВОГО ГРАФА "С НУЛЯ" ===
                SteinerTree candidate;
                
                // 1. Копируем все узлы из sub, кроме псевдо-узла
                for (const auto& node : sub.nodes) {
                    if (node.id.empty() || node.id[0] == 'P') continue;
                    // Если это псевдо-узел — заменяем на реальную точку Штейнера
                    if (node.id == pseudoId) {
                        GraphNode real(S, true); real.id = sId;
                        candidate.nodes.push_back(real);
                    } else {
                        candidate.nodes.push_back(node);
                    }
                }
                // Добавляем саму точку Штейнера (если ещё не добавлена)
                bool hasSteiner = false;
                for (const auto& n : candidate.nodes) 
                    if (n.id == sId) { hasSteiner = true; break; }
                if (!hasSteiner) {
                    GraphNode real(S, true); real.id = sId;
                    candidate.nodes.push_back(real);
                }
                // Добавляем терминалы A и B
                candidate.nodes.push_back(terminals[i]);
                candidate.nodes.push_back(terminals[j]);
                
                candidate.rebuildIndex();
                
                // 2. Копируем рёбра из sub с заменой pseudoId → sId
                for (const auto& [u, v] : sub.edges) {
                    string nu = (u == pseudoId) ? sId : u;
                    string nv = (v == pseudoId) ? sId : v;
                    if (candidate.hasNode(nu) && candidate.hasNode(nv))
                        candidate.edges.emplace_back(nu, nv);
                }
                // 3. Добавляем рёбра от новой точки Штейнера к A и B
                candidate.edges.emplace_back(sId, terminals[i].id);
                candidate.edges.emplace_back(sId, terminals[j].id);
                
                // 4. Считаем длину
                double edgeLen = dist(S, A) + dist(S, B);
                candidate.totalLength = sub.totalLength + edgeLen;
                
                // 5. Обновляем лучший результат
                if (candidate.totalLength < best.totalLength) {
                    best = candidate;
                }
            }
        }
    }
    
    return best;
}

//=============================================================================
//  Основной интерфейс
//=============================================================================
SteinerTree solveSteiner(const vector<Point>& terminalPoints) {
    global_id_counter = 0;  // сброс счётчика
    
    // Создаём узлы терминалов с уникальными ID
    vector<GraphNode> terminals;
    for (size_t i = 0; i < terminalPoints.size(); ++i) {
        GraphNode node(terminalPoints[i], false);
        node.id = "T" + to_string(i);
        terminals.push_back(node);
    }
    
    if (terminals.empty()) return {};
    if (terminals.size() == 1) {
        SteinerTree g;
        g.addNodeIfNew(terminals[0]);
        return g;
    }
    
    return melzakRecursive(terminals);
}

//=============================================================================
//  MAIN
//=============================================================================
int main() {
    cout << fixed << setprecision(6);

    vector<Point> terminals = {
        {0, 0}, {4, 0}, {4, 8}, {0, 8}, {-2, 4}, {6, 4}
    };
    
    // === ЗАПУСК АЛГОРИТМА ===
    cout << "\nComputing Steiner tree (Melzak's algorithm)...\n";
    SteinerTree result = solveSteiner(terminals);
    
    // === СТАТИСТИКА ===
    int tCount = 0, sCount = 0;
    for (const auto& n : result.nodes)
        (n.isSteiner ? sCount : tCount)++;
    
    cout << "\n=== RESULT ===" << endl;
    cout << "Total length: " << result.totalLength << endl;
    cout << "Nodes: " << result.nodes.size() << " (T: " << tCount << ", S: " << sCount << ")\n";
    cout << "Edges: " << result.edges.size() << endl;
    
    cout << "\nNodes:" << endl;
    for (const auto& n : result.nodes) {
        cout << "  " << n.id << " @(" << n.pos.x << "," << n.pos.y 
             << ") type=" << (n.isSteiner ? "Steiner" : "Terminal") << endl;
    }
    
    cout << "\nEdges:" << endl;
    for (const auto& [u, v] : result.edges)
        cout << "  " << u << " -- " << v << endl;
    
    // === ЭКСПОРТ В DOT ===
    exportToDot("data/tree.dot", result);
    
    cout << "\n✓ Done! Open data/tree.dot in Graphviz Online (use 'neato' layout)." << endl;
    return 0;
}
