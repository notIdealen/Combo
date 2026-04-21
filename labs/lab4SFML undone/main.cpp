//  Построение минимального дерева Штейнера
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <sstream>
#include <fstream>
using namespace std;
using namespace sf;

struct Point {
    double x, y;
};

std::vector<Point> SetTerminals(std::string path)
{   
    std::ifstream in(path);
    if (!in.is_open()) return {};
    std::string line{};
    std::vector<Point> terminals{};
    double x{}, y{};
    while (getline(in, line))
    {
        std::stringstream ss(line);
        ss >> x >> y;
        terminals.push_back({x, y});
    }
    return move(terminals);
}

// Вычисление расстояния между двумя точками
double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Сумма расстояний от точки p до всех остальных
double totalDistance(Point p, const vector<Point>& terminals) {
    double sum = 0;
    for (const auto& t : terminals) {
        sum += distance(p, t);
    }
    return sum;
}

// Метод градиентного спуска для поиска точки Штейнера
Point steinerPoint(const vector<Point>& terminals) {
    double step = 0.1;
    double precision = 1e-6;
    Point steiner = { 0, 0 };

    for (const auto& t : terminals) {
        steiner.x += t.x;
        steiner.y += t.y;
    }
    steiner.x /= terminals.size();
    steiner.y /= terminals.size();

    double prevDist = totalDistance(steiner, terminals);

    while (step > precision) {
        bool improved = false;

        for (double dx : {-step, step}) {
            for (double dy : {-step, step}) {
                Point newPoint = { steiner.x + dx, steiner.y + dy };
                double newDist = totalDistance(newPoint, terminals);

                if (newDist < prevDist) {
                    steiner = newPoint;
                    prevDist = newDist;
                    improved = true;
                }
            }
        }

        if (!improved) {
            step /= 2;
        }
    }

    return steiner;
}

// Основная функция
int main() {
    // Размер окна
    const int WIDTH = 600, HEIGHT = 600;

    // Исходные точки
    vector<Point> terminals = SetTerminals("_in/terminals.txt");

    // Найдём точку Штейнера
    Point s = steinerPoint(terminals);

    // Создание окна SFML
    RenderWindow window(VideoMode({WIDTH, HEIGHT}), "Steiner Tree Visualization");

    while (window.isOpen()) {
        // Event event;
        while (const std::optional ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Очистка экрана
        window.clear(Color::Black);

        // Рисуем рёбра (жёлтые линии)
        for (const auto& t : terminals) {
            Vertex line[] = {
                {Vector2f({s.x, s.y}), Color::Yellow},
                {Vector2f(t.x, t.y), Color::Yellow}
            };
            window.draw(line, 2, PrimitiveType::Lines);
        }

        // Рисуем терминальные точки (красные)
        for (const auto& t : terminals) {
            CircleShape terminal(5);
            terminal.setPosition({t.x - 5, t.y - 5});
            terminal.setFillColor(Color::Red);
            window.draw(terminal);
        }

        // Рисуем точку Штейнера (синяя)
        CircleShape steiner(5);
        steiner.setPosition({s.x - 5, s.y - 5});
        steiner.setFillColor(Color::Blue);
        window.draw(steiner);

        // Отображение кадра
        window.display();
    }

    return 0;
}
