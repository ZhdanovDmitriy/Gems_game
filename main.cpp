#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <random>

constexpr int n = 10;
constexpr int cellSize = 50;

// Шесть предопределённых цветов
enum class CellColor { Red, Orange, Yellow, Green, Blue, Purple };

// Преобразование в sf::Color
sf::Color toSfColor(CellColor color) {
    switch (color) {
    case CellColor::Red:    return sf::Color::Red;
    case CellColor::Orange: return sf::Color(255, 165, 0);
    case CellColor::Yellow: return sf::Color::Yellow;
    case CellColor::Green:  return sf::Color::Green;
    case CellColor::Blue:   return sf::Color::Blue;
    case CellColor::Purple: return sf::Color(128, 0, 128);
    }
    return sf::Color::White;
}

int main() {
    sf::Vector2u windowSize{
        static_cast<unsigned>(n * cellSize),
        static_cast<unsigned>(n * cellSize)
    };
    sf::RenderWindow window(
        sf::VideoMode(windowSize),
        "Gems"
    );

    std::array<std::array<CellColor, n>, n> board;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(CellColor::Purple));
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            board[row][col] = static_cast<CellColor>(dist(gen));
        }
    }

    bool hasFirst = false;
    sf::Vector2i firstPos;

    while (window.isOpen()) {
        while (auto optEvent = window.pollEvent()) {
            const sf::Event& event = *optEvent;
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
            else if (auto* mbp = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mbp->button == sf::Mouse::Button::Left) {
                    int col = mbp->position.x / cellSize;
                    int row = mbp->position.y / cellSize;
                    if (col >= 0 && col < n && row >= 0 && row < n) {
                        if (!hasFirst) {
                            firstPos = { col, row };
                            hasFirst = true;
                        }
                        else {
                            sf::Vector2i secondPos{ col, row };
                            std::swap(
                                board[firstPos.y][firstPos.x],
                                board[secondPos.y][secondPos.x]
                            );
                            hasFirst = false;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < n; ++col) {
                sf::RectangleShape rect({
                    static_cast<float>(cellSize),
                    static_cast<float>(cellSize)
                    });
                rect.setPosition({
                    static_cast<float>(col * cellSize),
                    static_cast<float>(row * cellSize)
                    });
                rect.setFillColor(toSfColor(board[row][col]));
                window.draw(rect);
            }
        }
        window.display();
    }

    return 0;
}
