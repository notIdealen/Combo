#pragma once
#include <vector>
#include <cmath>
#include <limits>
#include <numeric>
#include <algorithm>
#include "Structs.hpp"

struct DSU 
{
    std::vector<int> parent, rank;

    explicit DSU(int n) : parent(n), rank(n, 0) 
    {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int Find(int x) 
    {
        return parent[x] == x ? x : parent[x] = Find(parent[x]);
    }

    bool Unite(int x, int y) 
    {
        x = Find(x); 
        y = Find(y);
        if (x == y) return false;
        if (rank[x] < rank[y]) std::swap(x, y);
        parent[y] = x;
        if (rank[x] == rank[y]) ++rank[x];
        return true;
    }
};

double BoruvkaAlgorithm(const std::vector<Terminal>& points) 
{
    int n = points.size();
    if (n <= 1) return 0.0;

    DSU dsu(n);
    double mstWeight = 0.0;
    int components = n;

    while (components > 1) 
    {
        std::vector<double> bestWeight(n, INF);
        std::vector<int> bestU(n, -1);
        std::vector<int> bestV(n, -1);

        for (int i = 0; i < n; ++i) 
        {
            for (int j = i + 1; j < n; ++j) 
            {
                int rootI = dsu.Find(i);
                int rootJ = dsu.Find(j);
                if (rootI == rootJ) continue;

                double dist = std::hypot(points[i].pos.x - points[j].pos.x, 
                                         points[i].pos.y - points[j].pos.y);

                if (dist < bestWeight[rootI]) 
                {
                    bestWeight[rootI] = dist;
                    bestU[rootI] = i; bestV[rootI] = j;
                }
                if (dist < bestWeight[rootJ]) 
                {
                    bestWeight[rootJ] = dist;
                    bestU[rootJ] = i; bestV[rootJ] = j;
                }
            }
        }

        bool added = false;
        for (int i = 0; i < n; ++i) 
        {
            if (bestU[i] == -1) continue;
            if (dsu.Unite(bestU[i], bestV[i])) 
            {
                mstWeight += bestWeight[i];
                --components;
                added = true;
            }
        }
        if (!added) break;
    }
    return mstWeight;
}
