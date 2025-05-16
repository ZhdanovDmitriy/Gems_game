#include "structions.h"


std::map<Color, sf::Color> sfCOLOR = {
	{Color::RED, sf::Color(255, 0, 0)},
	{Color::GREEN, sf::Color(0, 255, 0)},
	{Color::BLUE, sf::Color(0, 0, 255)},
	{Color::YELLOW, sf::Color(255, 255, 0)},
	{Color::CYAN, sf::Color(0, 255, 255)},
	{Color::UNSELECTED, sf::Color(0, 0, 0)}
};

GEM::GEM() : Base() {
	color = Color::UNSELECTED;
	draw = sf::CircleShape(size - 2);
	draw.setFillColor(sf::Color::Black);
}

void GEM::selectColor(Color _color) {
	color = _color;
	draw.setFillColor(sfCOLOR[_color]);
}

void GEM::setPosition(int x, int y) {
	pos_x = x;
	pos_y = y;
}

GEM::GEM(Color _color) : GEM() {
	selectColor(_color);
}

Bomb::Bomb() : GEM() {
	color = Color::BOMB;
	draw = sf::CircleShape(size-2, 4);
	draw.setFillColor(sf::Color(144, 144, 144));
}

ColorChanger::ColorChanger() : GEM() {
	color = Color::COLORCHANGER;
	draw = sf::CircleShape(size, 4);
	draw.setFillColor(sf::Color(255, 0, 255));
}