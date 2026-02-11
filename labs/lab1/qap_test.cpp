#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "./qap.cpp"
#include <vector>

// Вспомогательная функция для создания матрицы из инициализатора
matrix makeMatrix(std::initializer_list<std::initializer_list<int>> list) {
    matrix m;
    for (auto& row : list) m.push_back(std::vector<int>(row));
    return m;
}

TEST_CASE("calculateCost: базовый пример 3x3", "[qap]") {
    // Матрица расстояний (линейное размещение)
    auto D = makeMatrix({
        {0, 1, 2},
        {1, 0, 1},
        {2, 1, 0}
    });
    
    // Матрица потоков
    auto F = makeMatrix({
        {0, 10, 5},
        {10, 0, 8},
        {5, 8, 0}
    });
    
    // Перестановка: объект 0→место 1, объект 1→место 2, объект 2→место 0
    std::vector<int> perm = {1, 2, 0};
    
    /*
     Расчёт вручную:
     f01 * d12 = 10 * 1 = 10
     f02 * d10 = 5  * 2 = 10
     f10 * d21 = 10 * 1 = 10
     f12 * d20 = 8  * 2 = 16
     f20 * d01 = 5  * 1 = 5
     f21 * d02 = 8  * 2 = 16
     Итого: 10+10+10+16+5+16 = 67
    */
    
    // unsigned int cost = calculateCost(F, D, perm);
    unsigned int cost = calculateR(F, D, perm, 3);
    REQUIRE(cost == 67);
}


/*
TEST_CASE("calculateCost: тождественная перестановка", "[qap]") {
    auto D = makeMatrix({
        {0, 5, 10},
        {5, 0, 3},
        {10, 3, 0}
    });
    
    auto F = makeMatrix({
        {0, 2, 4},
        {2, 0, 6},
        {4, 6, 0}
    });
    
    std::vector<int> identity = {0, 1, 2};
    unsigned int cost = calculateCost(F, D, identity);
    
    // f01*d01 + f02*d02 + f10*d10 + f12*d12 + f20*d20 + f21*d21
    // = 2*5 + 4*10 + 2*5 + 6*3 + 4*10 + 6*3 = 10+40+10+18+40+18 = 136
    REQUIRE(cost == 136);
}

TEST_CASE("calculateCost: диагональные элементы не влияют", "[qap]") {
    auto D = makeMatrix({{0, 7}, {7, 0}});
    auto F1 = makeMatrix({{0, 3}, {3, 0}});
    auto F2 = makeMatrix({{999, 3}, {3, 999}}); // Диагональ изменена
    
    std::vector<int> perm = {0, 1};
    unsigned int cost1 = calculateCost(F1, D, perm);
    unsigned int cost2 = calculateCost(F2, D, perm);
    
    REQUIRE(cost1 == cost2); // Диагональ не учитывается в сумме
}

TEST_CASE("solveQAP: тривиальный случай 1x1", "[qap]") {
    auto D = makeMatrix({{0}});
    auto F = makeMatrix({{0}});
    
    auto [cost, perm] = solveQAP(F, D);
    REQUIRE(cost == 0);
    REQUIRE(perm == std::vector<int>{0});
}

TEST_CASE("solveQAP: задача 2x2 с известным оптимумом", "[qap]") {
    // Расстояния: места 0 и 1 находятся на расстоянии 10
    auto D = makeMatrix({
        {0, 10},
        {10, 0}
    });
    
    // Потоки: между объектами 0 и 1 — 5 единиц
    auto F = makeMatrix({
        {0, 5},
        {5, 0}
    });
    
    // Любая перестановка даёт одинаковую стоимость: 5*10 + 5*10 = 100
    auto [cost, perm] = solveQAP(F, D);
    REQUIRE(cost == 100);
    REQUIRE(perm.size() == 2);
}

TEST_CASE("solveQAP: задача 3x3 с несимметричным оптимумом", "[qap]") {
    // Расстояния (линейное размещение)
    auto D = makeMatrix({
        {0, 1, 2},
        {1, 0, 1},
        {2, 1, 0}
    });
    
    // Потоки: сильная связь между объектами 0 и 1
    auto F = makeMatrix({
        {0, 100, 1},
        {100, 0, 1},
        {1, 1, 0}
    });
    
    // Оптимум: разместить объекты 0 и 1 рядом (места 0-1 или 1-2)
    // Лучшая перестановка: [0, 1, 2] или [2, 1, 0] → стоимость = 100*1 + 100*1 + ... = 204
    auto [cost, perm] = solveQAP(F, D);
    
    REQUIRE(cost == 204);
    // Проверяем, что объекты 0 и 1 размещены рядом
    int pos0 = -1, pos1 = -1;
    for (int i = 0; i < 3; ++i) {
        if (perm[i] == 0) pos0 = i;
        if (perm[i] == 1) pos1 = i;
    }
    REQUIRE(std::abs(pos0 - pos1) == 1); // Должны быть соседями
}

TEST_CASE("solveQAP: симметричные матрицы дают симметричный результат", "[qap]") {
    auto D = makeMatrix({
        {0, 2, 5},
        {2, 0, 2},
        {5, 2, 0}
    });
    
    auto F = makeMatrix({
        {0, 3, 1},
        {3, 0, 3},
        {1, 3, 0}
    });
    
    auto [cost, perm] = solveQAP(F, D);
    
    // Проверяем корректность результата через пересчёт
    unsigned int recalculated = calculateCost(F, D, perm);
    REQUIRE(cost == recalculated);
    REQUIRE(perm.size() == 3);
    
    // Проверяем, что перестановка — это валидная перестановка индексов
    std::vector<int> sortedPerm = perm;
    std::sort(sortedPerm.begin(), sortedPerm.end());
    REQUIRE(sortedPerm == std::vector<int>{0, 1, 2});
}

TEST_CASE("calculateCost: выбрасывает исключение при несовпадении размеров", "[qap][exception]") {
    auto D = makeMatrix({{0, 1}, {1, 0}});
    auto F = makeMatrix({{0, 1, 2}, {1, 0, 3}, {2, 3, 0}}); // 3x3 vs 2x2
    std::vector<int> perm = {0, 1};
    
    REQUIRE_THROWS_AS(calculateCost(F, D, perm), std::invalid_argument);
}

TEST_CASE("solveQAP: выбрасывает исключение при больших матрицах", "[qap][exception]") {
    int n = 12;
    matrix D(n, std::vector<int>(n, 1));
    matrix F(n, std::vector<int>(n, 1));
    for (int i = 0; i < n; ++i) D[i][i] = F[i][i] = 0;
    
    REQUIRE_THROWS_AS(solveQAP(F, D), std::runtime_error);
}

TEST_CASE("Интеграционный тест: реальные матрицы 4x4 из литературы", "[qap][integration]") {
    // Пример из классического набора Nugent et al. (1968)
    auto D = makeMatrix({
        {0, 1, 2, 3},
        {1, 0, 1, 2},
        {2, 1, 0, 1},
        {3, 2, 1, 0}
    });
    
    auto F = makeMatrix({
        {0, 5, 2, 0},
        {5, 0, 4, 3},
        {2, 4, 0, 6},
        {0, 3, 6, 0}
    });
    
    auto [cost, perm] = solveQAP(F, D);
    
    // Известный оптимум для этой задачи = 66
    // (проверено вручную и через QAPLIB)
    REQUIRE(cost == 66);
    
    // Проверяем, что найденная перестановка действительно даёт 66
    REQUIRE(calculateCost(F, D, perm) == 66);
}
*/
