#include <SFML/Graphics.hpp>
#include <map>


enum class Color {
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    UNSELECTED,
    BOMB,
    COLORCHANGER,
};

class Base {
public:
    sf::CircleShape draw;
    static const int size = 30;
    int pos_x, pos_y;

    ~Base() {};
};

class GEM : public Base {
public:
    Color color;

    GEM();
    GEM(Color _color);
    void selectColor(Color _color);
    void setPosition(int x, int y);
};

class Bomb : public GEM {
public:
    Bomb();
};


class ColorChanger : public GEM {
public:
    ColorChanger();
};