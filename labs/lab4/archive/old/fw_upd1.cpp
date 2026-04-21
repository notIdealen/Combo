#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "includes/Point.hpp"
#include "includes/Const.hpp"
#include "includes/Utils.hpp"
#include "includes/GraphNode.hpp"
#include "includes/SteinerGraph.hpp"

#include <string>
#include <map>
#include <set>

using namespace std;

//111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
// ... ваши существующие инклуды ...

// Уникальный счётчик для генерации имён узлов
static int node_counter = 0;

// struct GraphNode {
//     string id;      // Уникальный ID: "T0", "S1" и т.д.
//     Point pos;      // Координаты
//     bool isSteiner; // Флаг типа узла
    
//     GraphNode(const Point& p, bool steiner) : pos(p), isSteiner(steiner) {
//         id = (steiner ? "S" : "T") + to_string(node_counter++);
//     }
// };

// struct SteinerGraph {
//     double totalLength;
//     vector<GraphNode> nodes;
//     vector<pair<string, string>> edges; // Рёбра хранятся как пары ID ("ID1", "ID2")
    
//     // Быстрый доступ к узлу по ID для добавления рёбер
//     map<string, int> nodeIndex; 
// };
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


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
// struct SteinerResult {
//     double length;
//     vector<Point> steinerPoints;
// };
/*
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
    if (terminals.size() < 2) return {0, {}};
    return melzakRecursive(terminals);
}
*/

//222222222222222
// Вспомогательная функция: добавить узел в граф и вернуть его ID
string addNode(SteinerGraph& graph, const Point& pos, bool isSteiner) {
    GraphNode node(pos, isSteiner);
    graph.nodes.push_back(node);
    graph.nodeIndex[node.id] = graph.nodes.size() - 1;
    return node.id;
}

// Рекурсивная функция Мелзака (возвращает граф)
// SteinerGraph melzakRecursiveGraph(vector<GraphNode> terminals) {
//     int k = terminals.size();
//     SteinerGraph result;
    
//     // Базовый случай: 2 точки -> просто соединяем их
//     if (k == 2) {
//         result.totalLength = dist(terminals[0].pos, terminals[1].pos);
//         result.nodes = terminals;
//         result.edges.emplace_back(terminals[0].id, terminals[1].id);
//         for(int i=0; i<k; ++i) result.nodeIndex[terminals[i].id] = i;
//         return result;
//     }
    
//     // Базовый случай: 3 точки -> точка Ферма
//     if (k == 3) {
//         Point S = fermatTorricelli(terminals[0].pos, terminals[1].pos, terminals[2].pos);
//         string sId = addNode(result, S, true);
        
//         result.totalLength = dist(S, terminals[0].pos) + dist(S, terminals[1].pos) + dist(S, terminals[2].pos);
        
//         // Переносим терминалы в результат и добавляем рёбра
//         for(const auto& t : terminals) {
//             result.nodes.push_back(t);
//             result.nodeIndex[t.id] = result.nodes.size() - 1;
//             result.edges.emplace_back(sId, t.id);
//         }
//         return result;
//     }
    
//     SteinerGraph best;
//     best.totalLength = INF;
    
//     // Перебор пар для объединения
//     for (int i = 0; i < k; ++i) {
//         for (int j = i + 1; j < k; ++j) {
//             Point A = terminals[i].pos;
//             Point B = terminals[j].pos;
//             auto candidates = equilateralVertices(A, B);
            
//             for (const Point& X : candidates) {
//                 // Формируем новый список терминалов для рекурсии
//                 vector<GraphNode> newTerminals;
//                 for (int m = 0; m < k; ++m) {
//                     if (m != i && m != j) {
//                         newTerminals.push_back(terminals[m]);
//                     }
//                 }
//                 // Добавляем псевдо-терминал X
//                 // ВАЖНО: мы помечаем его как isSteiner=false временно, 
//                 // чтобы отличать от реальных точек Штейнера, если нужно, 
//                 // но для алгоритма это просто "узел".
//                 newTerminals.emplace_back(X, false); 
                
//                 // Рекурсия
//                 SteinerGraph sub = melzakRecursiveGraph(newTerminals);
                
//                 // Находим реальную точку Штейнера для тройки (A, B, X)
//                 Point S = fermatTorricelli(A, B, X);
                
//                 // Восстанавливаем рёбра:
//                 // 1. Находим в подграфе 'sub', какой узел соответствует псевдо-терминалу X.
//                 //    Поскольку мы только что добавили его последним в newTerminals, 
//                 //    в подграфе он тоже будет последним (или ищем по координатам с допуском).
//                 string pseudoId = newTerminals.back().id; 
                
//                 // 2. Создаём реальную точку Штейнера в финальном графе
//                 string sId = addNode(result, S, true);
                
//                 // 3. Связываем точку Штейнера с исходными точками A и B
//                 result.edges.emplace_back(sId, terminals[i].id);
//                 result.edges.emplace_back(sId, terminals[j].id);
                
//                 // 4. Связываем точку Штейнера с подграфом (заменяя связь с псевдо-узлом)
//                 //    Копируем все рёбра из sub, но если ребро ведёт в pseudoId, ведём его в sId
//                 for (const auto& [u, v] : sub.edges) {
//                     string newU = (u == pseudoId) ? sId : u;
//                     string newV = (v == pseudoId) ? sId : v;
//                     result.edges.emplace_back(newU, newV);
//                 }
                
//                 // Считаем длину (упрощённо: длина подграфа + новые рёбра)
//                 double edgeLen = dist(S, A) + dist(S, B);
//                 // (Для точности нужно пересчитать длину связи с подграфом, но для структуры это не критично)
//                 double totalLen = sub.totalLength + edgeLen; 
                
//                 if (totalLen < best.totalLength) {
//                     best.totalLength = totalLen;
//                     // Здесь нужно аккуратно слить узлы, чтобы не было дублей. 
//                     // Для простоты можно просто вернуть результат, если это первый успешный,
//                     // или реализовать слияние векторов nodes/edges.
//                     // В реальном коде здесь нужен "merge" графов.
//                 }
//             }
//         }
//     }
//     // Примечание: полный код слияния графов (merge) занимает много места. 
//     // Если алгоритм работает медленно, начните с k=4 для отладки.
//     return best; 
// }
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Вспомогательная: клонировать граф с заменой одного ID на другой
SteinerGraph cloneWithReplace(const SteinerGraph& src, const string& oldId, const string& newId) {
    SteinerGraph result;
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

SteinerGraph melzakRecursiveGraph(vector<GraphNode> terminals) {
    int k = terminals.size();
    SteinerGraph result;
    
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
    SteinerGraph best;
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
                SteinerGraph sub = melzakRecursiveGraph(newTerminals);
                
                // Находим реальную точку Штейнера для (A, B, X)
                Point S = fermatTorricelli(A, B, X);
                string sId = "S" + to_string(node_counter++);
                
                // Клонируем подграф, заменяя псевдо-узел на реальную точку Штейнера
                SteinerGraph candidate = cloneWithReplace(sub, pseudoId, sId);
                
                // Добавляем саму точку Штейнера
                GraphNode steiner(S, true);
                steiner.id = sId;
                candidate.nodes.push_back(steiner);
                candidate.nodeIndex[sId] = candidate.nodes.size() - 1;
                
                // Добавляем рёбра от точки Штейнера к A и B
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
    
    return best;
}
// ==================== ПРИМЕР ИСПОЛЬЗОВАНИЯ ====================

int main() {

    // vector<GraphNode> terminalNodes = { {{0, 0}, false}, {{4, 0}, false}, {{4, 2}, false}, {{0, 2}, false}, {{7, 11}, false} };
    // SteinerGraph graph = melzakRecursiveGraph(terminalNodes);

    // vector<Point> steiner;
    // for (auto point : graph.nodes)
    //     steiner.push_back(point.pos);
    // vector<Point> terminals = { {0, 0}, {4, 0}, {4, 2},{0, 2},{7, 11} };
    vector<GraphNode> terminalNodes = {
        GraphNode({0, 0}, false),
        GraphNode({4, 0}, false),
        GraphNode({4, 2}, false),
        GraphNode({0, 2}, false),
        GraphNode({7, 11}, false)
    };

    // 2. Запускаем алгоритм
    node_counter = 0;
    SteinerGraph graph = melzakRecursiveGraph(terminalNodes);

    // 3. Разделяем узлы на терминалы и точки Штейнера для экспорта
    vector<Point> terminals, steiners;
    for (const auto& node : graph.nodes) {
        if (node.isSteiner) {
            steiners.push_back(node.pos);
        } else {
            terminals.push_back(node.pos);
        }
    }

    // 5. Вывод статистики для отладки
    cout << "\n=== Result ===" << endl;
    cout << "Total length: " << graph.totalLength << endl;
    cout << "Nodes: " << graph.nodes.size() 
         << " (T: " << terminals.size() << ", S: " << steiners.size() << ")" << endl;
    cout << "Edges: " << graph.edges.size() << endl;
    for (const auto& [u, v] : graph.edges) {
        cout << "  " << u << " -- " << v << endl;
    }

    // exportToDot("data/toViz.txt", graph.nodes, graph.edges);
    // exportToDot1("data/toViz.txt", terminals, steiners, graph.edges);
    return 0;
}
//g++ fw_upd1.cpp -o fw
