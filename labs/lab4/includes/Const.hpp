#pragma once

#include <cmath>
#include <limits>

inline constexpr double DELTA_LEN = 1e-9;
inline constexpr double DELTA_ANGEL = 1e-4; // допуск ~0.05°
inline constexpr double INF = std::numeric_limits<double>::max();
const double PI = acos(-1.0);
const double SQRT_3 = sqrt(3);
const double SIN_60 = SQRT_3 / 2;
const double ANGLE_120 = 2 * PI / 3;
const double ANGLE_240 = ANGLE_120 * 2;
inline constexpr double MOVE_EPS = 1e-8;
inline constexpr int MAX_ITERS = 1000;
