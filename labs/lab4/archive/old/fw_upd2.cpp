#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "includes/Point.hpp"
// #include "includes/Utils.hpp"//внутри graphNode двойное обьявления

#include <string>
#include <map>
#include <set>

using namespace std;

const double EPS = 1e-9;
const double INF = std::numeric_limits<double>::max();
const double PI = acos(-1.0);
const double SIN_60 = sqrt(3) / 2;

static int node_counter = 0;

struct GraphNode {
    std::string id;      // Уникальный ID: "T0", "S1" и т.д.
    Point pos;      // Координаты
    bool isSteiner; // Флаг типа узла
    
    GraphNode(const Point& p, bool steiner) : pos(p), isSteiner(steiner) {}
};

struct SteinerTree {
    double totalLength;
    std::vector<GraphNode> nodes;
    std::vector<std::pair<std::string, std::string>> edges; // Рёбра хранятся как пары ID ("ID1", "ID2")
    
    // Быстрый доступ к узлу по ID для добавления рёбер
    std::map<std::string, int> nodeIndex; 
};

void exportToDot(const string& filename, 
                 const vector<GraphNode>& nodes,
                 const vector<pair<string,string>>& edges)
{
    ofstream out(filename);
    out << "graph SteinerTree {\n";
    out << "    layout=neato; overlap=false; splines=line;\n";
    
    for (const auto& node : nodes) {
        string color = node.isSteiner ? "#3498db" : "#e74c3c";
        string shape = node.isSteiner ? "doublecircle" : "circle";
        out << "    " << node.id << " "
            << "[pos=\"" << node.pos.x << "," << node.pos.y << "!\", "
            << "label=\"" << node.id << "\", "
            << "shape=" << shape << ", "
            << "style=filled, fillcolor=\"" << color << "\"];\n";
    }
    
    for (const auto& [u, v] : edges) {
        out << "    " << u << " -- " << v << " [penwidth=2];\n";
    }
    out << "}\n";
    out.close();
    cout << "✓ Exported: " << filename << endl;
}

double dist(const Point& A, const Point& B) {
    return (A - B).length();
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
    // Если есть угол >= 120°, точка Штейнера — эта вершина
    if (angleAt(A, B, C) >= 2 * PI / 3) return B;
    if (angleAt(B, A, C) >= 2 * PI / 3) return A;
    if (angleAt(A, C, B) >= 2 * PI / 3) return C;
    
    // Иначе используем конструкцию Мелзака
    auto candidates = equilateralVertices(A, B);
    Point bestS = A;
    double minSum = INF;
    
    for (const Point& X : candidates) {
        // Точка пересечения XC с описанной окружностью ABX
        // Упрощённо: ищем точку S на отрезке XC, минимизирующую |SA|+|SB|+|SC|
        // Численная оптимизация (градиентный спуск)
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
// Вспомогательная функция: добавить узел в граф и вернуть его ID
string addNode(SteinerTree& graph, const Point& pos, bool isSteiner) {
    GraphNode node(pos, isSteiner);
    graph.nodes.push_back(node);
    graph.nodeIndex[node.id] = graph.nodes.size() - 1;
    return node.id;
}

// Вспомогательная: клонировать граф с заменой одного ID на другой
SteinerTree cloneWithReplace(const SteinerTree& src, const string& oldId, const string& newId) {
    SteinerTree result;
    result.totalLength = src.totalLength;
    
    // Копируем узлы
    for (const auto& node : src.nodes) {
        result.nodes.push_back(node);
        result.nodeIndex[node.id] = result.nodes.size() - 1;
    }
    
    // Копируем рёбра с заменой ID
    for (const auto& [u, v] : src.edges) {
        string newU = (u == oldId) ? newId : u;
        string newV = (v == oldId) ? newId : v;
        result.edges.emplace_back(newU, newV);
    }
    return result;
}

// Добавить узел в граф, если его ещё нет, вернуть его ID
string ensureNode(SteinerTree& graph, const GraphNode& node) {
    auto it = graph.nodeIndex.find(node.id);
    if (it == graph.nodeIndex.end()) {
        // Узла с таким ID нет — добавляем
        graph.nodeIndex[node.id] = graph.nodes.size();
        graph.nodes.push_back(node);
    }
    // Если узел уже есть — ничего не делаем, просто возвращаем ID
    return node.id;
}
// Удалить все псевдо-узлы (с ID, начинающимся на "P")
SteinerTree removePseudoNodes(SteinerTree graph) {
    SteinerTree result;
    result.totalLength = graph.totalLength;
    
    for (const auto& node : graph.nodes) {
        // Пропускаем псевдо-узлы
        if (!node.id.empty() && node.id[0] == 'P') {
            continue;
        }
        result.nodeIndex[node.id] = result.nodes.size();
        result.nodes.push_back(node);
    }
    
    // Копируем только валидные рёбра
    for (const auto& [u, v] : graph.edges) {
        if (result.nodeIndex.count(u) && result.nodeIndex.count(v)) {
            result.edges.emplace_back(u, v);
        }
    }
    
    return result;
}
SteinerTree melzakRecursiveGraph(vector<GraphNode> terminals) {
    int k = terminals.size();
    SteinerTree result;
    
    // === БАЗОВЫЕ СЛУЧАИ ===
    
    // 2 точки: просто ребро между ними
    if (k == 2) {
        result.totalLength = dist(terminals[0].pos, terminals[1].pos);
        result.nodes = terminals;
        result.edges.emplace_back(terminals[0].id, terminals[1].id);
        for (int i = 0; i < k; ++i) 
            result.nodeIndex[terminals[i].id] = i;
        return result;
    }
    
    // 3 точки: точка Ферма + 3 ребра
    if (k == 3) {
        Point A = terminals[0].pos, B = terminals[1].pos, C = terminals[2].pos;
        Point S = fermatTorricelli(A, B, C);
        
        string sId = "S" + to_string(node_counter++);
        GraphNode steiner(S, true);
        steiner.id = sId;
        
        result.nodes.push_back(steiner);
        result.nodeIndex[sId] = 0;
        
        for (int i = 0; i < 3; ++i) {
            result.nodes.push_back(terminals[i]);
            result.nodeIndex[terminals[i].id] = result.nodes.size() - 1;
            result.edges.emplace_back(sId, terminals[i].id);
        }
        
        result.totalLength = dist(S, A) + dist(S, B) + dist(S, C);
        return result;
    }
    
    // === РЕКУРСИВНЫЙ СЛУЧАЙ ===
    SteinerTree best;
    best.totalLength = INF;
    
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            Point A = terminals[i].pos, B = terminals[j].pos;
            auto candidates = equilateralVertices(A, B);
            
            for (const Point& X : candidates) {
                // Новый список терминалов: все кроме i,j + псевдо-узел X
                vector<GraphNode> newTerminals;
                for (int m = 0; m < k; ++m) {
                    if (m != i && m != j) 
                        newTerminals.push_back(terminals[m]);
                }
                
                // Создаём псевдо-терминал с уникальным временным ID
                string pseudoId = "P" + to_string(node_counter++);
                GraphNode pseudo(X, false);
                pseudo.id = pseudoId;
                newTerminals.push_back(pseudo);
                
                // Рекурсивный вызов
                SteinerTree sub = melzakRecursiveGraph(newTerminals);
                
                // Находим реальную точку Штейнера для (A, B, X)
                Point S = fermatTorricelli(A, B, X);
                string sId = "S" + to_string(node_counter++);
                
                // Клонируем подграф, заменяя псевдо-узел на реальную точку Штейнера
                SteinerTree candidate = cloneWithReplace(sub, pseudoId, sId);
                
                // Добавляем саму точку Штейнера
                GraphNode steiner(S, true);
                steiner.id = sId;

                ensureNode(candidate, steiner);  // ← используем ensureNode

                // Добавляем рёбра от точки Штейнера к A и B
                // Сначала гарантируем, что терминалы есть в графе!
                ensureNode(candidate, terminals[i]);  // ← ВАЖНО
                ensureNode(candidate, terminals[j]);  // ← ВАЖНО

                candidate.edges.emplace_back(sId, terminals[i].id);
                candidate.edges.emplace_back(sId, terminals[j].id);

                // Считаем длину
                double edgeLen = dist(S, A) + dist(S, B);
                candidate.totalLength = sub.totalLength + edgeLen;

                // Обновляем лучший результат
                if (candidate.totalLength < best.totalLength) {
                    best = candidate;
                }
            }
        }
    }
    best = removePseudoNodes(best);
    return best;
}

// ==================== ПРИМЕР ИСПОЛЬЗОВАНИЯ ====================
int main() {
    cout << fixed << setprecision(6);
    
    // Координаты терминалов
    vector<Point> terminalPoints = {
        {0, 0}, {4, 0}, {4, 2}, {0, 2}, {8, 1}
    };
    
    // Создаём GraphNode с корректными ID
    vector<GraphNode> terminalNodes;
    for (size_t i = 0; i < terminalPoints.size(); ++i) {
        GraphNode node(terminalPoints[i], false);
        node.id = "T" + to_string(i);  // ← ВАЖНО: задаём ID вручную
        terminalNodes.push_back(node);
    }

    // Сброс глобального счётчика для точек Штейнера
    node_counter = 0;
    
    // Запуск алгоритма
    SteinerTree graph = melzakRecursiveGraph(terminalNodes);

    // === ОТЛАДКА: вывод всех узлов ===
    cout << "\n=== All Nodes ===" << endl;
    map<string, int> idCount;
    for (const auto& node : graph.nodes) {
        idCount[node.id]++;
        cout << "  " << node.id << " @(" << node.pos.x << "," << node.pos.y 
            << ") isSteiner=" << node.isSteiner << endl;
    }

    // Проверка на дубликаты ID
    cout << "\n=== Duplicate IDs ===" << endl;
    for (const auto& [id, cnt] : idCount) {
        if (cnt > 1) {
            cout << "  WARNING: ID '" << id << "' appears " << cnt << " times!" << endl;
        }
    }
    // Подсчёт типов узлов
    int terminalCount = 0, steinerCount = 0;
    for (const auto& node : graph.nodes) {
        if (node.isSteiner) steinerCount++;
        else terminalCount++;
    }

    // Вывод результата
    cout << "\n=== Result ===" << endl;
    cout << "Total length: " << graph.totalLength << endl;
    cout << "Nodes: " << graph.nodes.size() 
         << " (T: " << terminalCount << ", S: " << steinerCount << ")" << endl;
    cout << "Edges: " << graph.edges.size() << endl;
    for (const auto& [u, v] : graph.edges) {
        cout << "  " << u << " -- " << v << endl;
    }
    
    exportToDot("data/toViz.dot", graph.nodes, graph.edges);
    return 0;
}
//g++ fw_upd2.cpp -o fw
