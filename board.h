#include <SFML/Graphics.hpp>
#include "structions.h"


class Board {
public:
    GEM** matrix;
    GEM* selected = NULL;
    static const int size_x = 10, size_y = 10;
    const int width = 1080, height = 720;
    float indent_x, indent_y;
    bool fallAnimation = false;
    std::vector<GEM*> check;

    Board();

    GEM* getGemByPos(sf::Vector2i const& position);

    bool isNear(GEM* first, GEM* second);

    bool checkCondition(std::vector<GEM*> data);

    void swap(GEM* first, GEM* second);

    void swap(GEM* first, GEM* second, bool onlyNear);

    void fall();

    void explosion(GEM* bomb);

    void randomColorChange(GEM* colorChanger);

    void select(GEM* gem);

    void checkEvents(sf::RenderWindow& window, sf::Event& event);

    void checkToDelete();

    void update(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);

    void run();
};