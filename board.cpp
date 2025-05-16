#include "board.h"
#include <iostream>

template<typename T>
int find(std::vector<T*>* array, T* element) {
    for (int index = 0; index < array->size(); index++) {
        if ((*array)[index] == element)
            return index;
    }

    return -1;
}

template<typename T>
int find(std::vector<T> array, T element) {
    for (int index = 0; index < array.size(); index++) {
        if (array[index] == element)
            return index;
    }

    return -1;
}

std::vector<GEM*> getNear(GEM** matrix, GEM* gem) {
    std::vector<GEM*> result;

    for (int add_x = -1; add_x <= 1; add_x++) {
        for (int add_y = -1; add_y <= 1; add_y++) {
            if (abs(add_x) + abs(add_y) != 1)
                continue;

            if ((gem->pos_x + add_x) < 0)
                continue;
            if ((gem->pos_x + add_x) == Board::size_x)
                continue;
            if ((gem->pos_y + add_y) < 0)
                continue;
            if ((gem->pos_y + add_y) == Board::size_y)
                continue;

            result.push_back(&matrix[gem->pos_x + add_x][gem->pos_y + add_y]);
        }
    }

    return result;
}

void recursionSearch(GEM* current, Color color, GEM** matrix, std::vector<GEM*>* visited) {
    if (color == Color::BOMB || current->color != color)
        return;

    visited->push_back(current);

    for (auto toVisit : getNear(matrix, current)) {
        if (find<GEM>(visited, toVisit) == -1)
            recursionSearch(toVisit, color, matrix, visited);
    }
}

std::vector<GEM*> searchInWidth(GEM** matrix, GEM* gem) {
    std::vector<GEM*> result;
    recursionSearch(gem, gem->color, matrix, &result);

    return result;
}

Board::Board() {
    matrix = new GEM * [size_y];

    indent_x = static_cast<float>(width - 2 * GEM::size * size_x) / 2,
        indent_y = static_cast<float>(height - 2 * GEM::size * size_y) / 2;

    for (int row = 0; row < size_y; row++) {
        matrix[row] = new GEM[size_x];

        for (int col = 0; col < size_x; col++) {
            auto* gem = &matrix[row][col];

            gem->draw.setPosition(sf::Vector2f(indent_x + 2 * GEM::size * col, indent_y + 2 * GEM::size * row));
            gem->draw.setOutlineColor(sf::Color(255, 255, 255));
            gem->setPosition(row, col);
        }
    }

    for (int row = 0; row < size_y; row++) {
        for (int col = 0; col < size_x; col++) {
            std::vector<Color> colors = { Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW, Color::CYAN };
            auto* gem = &matrix[row][col];

            while (true) {
                int index = rand() % colors.size();
                Color color = colors[index];
                colors.erase(colors.begin() + index);

                gem->selectColor(color);

                if (searchInWidth(matrix, gem).size() < 3)
                    break;
            }
        }
    }
}

GEM* Board::getGemByPos(sf::Vector2i const& position) {
    int size = matrix[0][0].size;

    int indent_x = (width - 2 * size * size_x) / 2,
        indent_y = (height - 2 * size * size_y) / 2;

    int board_x = 2 * size * size_x,
        board_y = 2 * size * size_y;

    if (!(indent_x <= position.x && position.x <= (indent_x + board_x)) || !(indent_y <= position.y && position.y <= (indent_y + board_y)))
        return NULL;

    return &matrix[(position.y - indent_y) / 2 / size][(position.x - indent_x) / 2 / size];
}

bool Board::isNear(GEM* first, GEM* second) {
    int add_x = abs(first->pos_x - second->pos_x);
    int add_y = abs(first->pos_y - second->pos_y);

    if (add_x > 1 || add_y > 1)
        return false;

    return add_x ^ add_y;
}

bool Board::checkCondition(std::vector<GEM*> data) {
    if (data.size() >= 3) {
        for (auto element : data) {
            int probability = rand() % 25;
            if (probability == 0) {
                auto bomb = Bomb();
                bomb.draw.setPosition(element->draw.getPosition().x, element->draw.getPosition().y);
                bomb.setPosition(element->pos_x, element->pos_y);

                matrix[element->pos_x][element->pos_y] = bomb;
            }
            else if (probability <= 3) {
                auto colorChanger = ColorChanger();

                colorChanger.draw.setPosition(element->draw.getPosition().x, element->draw.getPosition().y);
                colorChanger.setPosition(element->pos_x, element->pos_y);

                matrix[element->pos_x][element->pos_y] = colorChanger;
            }
            else {
                element->selectColor(Color::UNSELECTED);
            }
        }

        return true;
    }

    return false;
}

void Board::swap(GEM* first, GEM* second) {
    auto save = *first;

    matrix[first->pos_x][first->pos_y] = *second;
    matrix[second->pos_x][second->pos_y] = save;

    auto save_x = first->pos_x, save_y = first->pos_y;
    first->setPosition(second->pos_x, second->pos_y);
    second->setPosition(save_x, save_y);

    auto savePos = first->draw.getPosition();
    first->draw.setPosition(second->draw.getPosition().x, second->draw.getPosition().y);
    second->draw.setPosition(savePos);
}

void Board::swap(GEM* first, GEM* second, bool onlyNear) {
    if ((!isNear(first, second)) && onlyNear)
        return;

    swap(first, second);
}

void Board::fall() {
    if (!fallAnimation)
        return;

    sf::sleep(sf::milliseconds(500));

    for (int col = 0; col < size_y; col++) {
        for (int row = size_x - 1; row >= 0; row--) {
            auto* gem = &matrix[row][col];

            if (gem->color != Color::UNSELECTED)
                continue;

            bool wasSwap = false;

            for (int rowIndex = row - 1; rowIndex >= 0; rowIndex--) {
                auto* gemToChange = &matrix[rowIndex][col];

                if (gemToChange->color == Color::UNSELECTED)
                    continue;

                swap(gem, gemToChange);
                check.push_back(gem);

                wasSwap = true;
                break;
            }

            if (!wasSwap)
                break;
        }
    }

    fallAnimation = false;
}

void Board::explosion(GEM* bomb) {
    auto placeholder = GEM();
    placeholder.draw.setOutlineColor(sf::Color(255, 255, 255));
    placeholder.setPosition(bomb->pos_x, bomb->pos_y);
    placeholder.draw.setPosition(bomb->draw.getPosition().x, bomb->draw.getPosition().y);
    placeholder.selectColor(Color::UNSELECTED);

    matrix[bomb->pos_x][bomb->pos_y] = placeholder;

    std::vector<GEM*> inGame;

    for (int row = 0; row < size_x; row++) {
        for (int col = 0; col < size_y; col++) {
            if (matrix[row][col].color == Color::UNSELECTED)
                continue;
            inGame.push_back(&matrix[row][col]);
        }
    }

    for (int _ = 0; _ < ((inGame.size() >= 4) ? 4 : inGame.size()); _++) {
        int index = rand() % inGame.size();
        auto gem = inGame[index];
        gem->selectColor(Color::UNSELECTED);

        inGame.erase(inGame.begin() + index);
    }
}

void Board::randomColorChange(GEM* colorChanger) {
    auto placeholder = GEM();
    placeholder.draw.setOutlineColor(sf::Color(255, 255, 255));
    placeholder.setPosition(colorChanger->pos_x, colorChanger->pos_y);
    placeholder.draw.setPosition(colorChanger->draw.getPosition().x, colorChanger->draw.getPosition().y);
    placeholder.selectColor(Color::UNSELECTED);

    matrix[colorChanger->pos_x][colorChanger->pos_y] = placeholder;

    std::vector<GEM*> inGame;

    for (int row = 0; row < size_x; row++) {
        for (int col = 0; col < size_y; col++) {
            if (matrix[row][col].color == Color::UNSELECTED || isNear(&matrix[row][col], &placeholder)
                || matrix[row][col].color == Color::COLORCHANGER || matrix[row][col].color == Color::BOMB)
                continue;
            inGame.push_back(&matrix[row][col]);
        }
    }

    for (int _ = 0; _ < ((inGame.size() >= 2) ? 2 : inGame.size()); _++) {
        int index = rand() % inGame.size();
        auto gem = inGame[index];
        std::vector<Color> colors = { Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW, Color::CYAN };

        colors.erase(colors.begin() + find<Color>(colors, gem->color));
        gem->selectColor((Color)(rand() % colors.size()));

        inGame.erase(inGame.begin() + index);
        check.push_back(gem);
    }
}

void Board::select(GEM* gem) {
    if (gem == NULL || gem->color == Color::UNSELECTED)
        return;

    if (gem->color == Color::BOMB || gem->color == Color::COLORCHANGER) {
        if (gem->color == Color::BOMB)
            explosion(gem);
        else
            randomColorChange(gem);

        fallAnimation = true;

        if (selected != NULL) {
            selected->draw.setOutlineThickness(0);
            selected = NULL;
        }

        return;
    }

    if (selected == NULL) {
        selected = gem;
        selected->draw.setOutlineThickness(5);
    }
    else {
        selected->draw.setOutlineThickness(0);

        if (gem != selected) {
            swap(selected, gem, true);
            check.push_back(selected);
            check.push_back(gem);
        }

        selected = NULL;
    }
}

void Board::checkEvents(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::Closed)
        window.close();
    if (event.type == sf::Event::MouseButtonPressed)
        select(getGemByPos(sf::Mouse::getPosition(window)));
}

void Board::checkToDelete() {
    for (auto gem : check) {
        bool result = checkCondition(searchInWidth(matrix, gem));

        if (!fallAnimation && result)
            fallAnimation = true;
    }

    check.clear();
}

void Board::render(sf::RenderWindow& window) {
    for (int row = 0; row < size_x; row++) {
        for (int col = 0; col < size_y; col++) {
            GEM gem = matrix[row][col];

            if (gem.color != Color::UNSELECTED)
                continue;

            window.draw(gem.draw);
        }
    }

    for (int row = 0; row < size_x; row++) {
        for (int col = 0; col < size_y; col++) {
            GEM gem = matrix[row][col];

            if (gem.color == Color::UNSELECTED)
                continue;

            window.draw(gem.draw);
        }
    }
}

void Board::update(sf::RenderWindow& window) {
    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            checkEvents(window, event);
        }

        window.clear();
        render(window);
        window.display();
        checkToDelete();
        fall();
    }
}

void Board::run() {
    sf::RenderWindow window(sf::VideoMode(width, height), "GAMES");

    update(window);
}