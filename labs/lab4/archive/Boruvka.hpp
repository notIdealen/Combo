#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <numeric>

// ========== Disjoint Set Union (DSU) ==========
struct DSU {
    std::vector<int> parent;
    std::vector<int> rank;
    
    explicit DSU(int n) : parent(n), rank(n, 0) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    
    int Find(int x) {
        if (parent[x] != x)
            parent[x] = Find(parent[x]); // сжатие пути
        return parent[x];
    }
    
    bool Unite(int x, int y) {
        x = Find(x); y = Find(y);
        if (x == y) return false;
        // объединение по рангу
        if (rank[x] < rank[y]) std::swap(x, y);
        parent[y] = x;
        if (rank[x] == rank[y]) ++rank[x];
        return true;
    }
};

// ========== Структура ребра ==========
struct Edge {
    int u, v;
    double weight;
};

// ========== Результат алгоритма ==========
struct MSTResult {
    std::vector<Edge> edges;
    double totalWeight = 0.0;
    bool isConnected = false;
};

// ========== Алгоритм Борувки ==========
MSTResult boruvkaMST(int V, const std::vector<Edge>& edges) {
    if (V <= 1) return MSTResult{};
    
    DSU dsu(V);
    MSTResult result;
    result.totalWeight = 0.0;
    int components = V;
    
    // Пока больше одной компоненты связности
    while (components > 1) {
        // Для каждой компоненты храним индекс минимального ребра, ведущего наружу
        std::vector<int> bestEdgeIdx(V, -1);
        std::vector<double> bestWeight(V, std::numeric_limits<double>::max());
        
        // 1. Поиск минимального ребра для каждой компоненты
        for (size_t i = 0; i < edges.size(); ++i) {
            const auto& e = edges[i];
            int rootU = dsu.Find(e.u);
            int rootV = dsu.Find(e.v);
            
            if (rootU == rootV) continue; // ребро внутри компоненты не подходит
            
            if (e.weight < bestWeight[rootU]) {
                bestWeight[rootU] = e.weight;
                bestEdgeIdx[rootU] = static_cast<int>(i);
            }
            if (e.weight < bestWeight[rootV]) {
                bestWeight[rootV] = e.weight;
                bestEdgeIdx[rootV] = static_cast<int>(i);
            }
        }
        
        // 2. Добавление найденных рёбер в MST
        bool added = false;
        for (int i = 0; i < V; ++i) {
            if (bestEdgeIdx[i] == -1) continue;
            
            const Edge& e = edges[bestEdgeIdx[i]];
            if (dsu.Unite(e.u, e.v)) {
                result.totalWeight += e.weight;
                result.edges.push_back(e);
                --components;
                added = true;
            }
        }
        
        // Если ни одно ребро не добавлено, граф несвязный
        if (!added) break;
    }
    
    result.isConnected = (components == 1);
    return result;
}

// ========== Пример использования ==========
int main() {
    // Вершины: 0, 1, 2, 3, 4
    const int V = 5;
    std::vector<Edge> edges = {
        {0, 1,  }, {0, 2, 2.5}, {0, 4, 3.1},
        {1, 2, 1.2}, {1, 3, 5.0},
        {2, 3, 3.3}, {2, 4, 2.0},
        {3, 4, 4.5}
    };
    
    MSTResult mst = boruvkaMST(V, edges);
    
    std::cout << "MST Total Weight: " << mst.totalWeight << "\n";
    std::cout << "Connected: " << (mst.isConnected ? "Yes" : "No") << "\n";
    std::cout << "Edges in MST:\n";
    for (const auto& e : mst.edges) {
        std::cout << "  " << e.u << " -- " << e.v << " (w=" << e.weight << ")\n";
    }
    
    return 0;
}
