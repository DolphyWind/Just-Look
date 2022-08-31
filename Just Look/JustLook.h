#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <string>
#include <sstream>
#include <cmath>

class JustLook
{
private:
	sf::RenderWindow window;
	HWND hwnd;

	sf::Image image;
	bool imageOpened;
	sf::Texture t;
	sf::Sprite s;
	
	sf::Event e;
	sf::Color backgroundColor = sf::Color(238, 243, 250);
	void handleEvents();
	void handleInputs();
	void draw();
	void openImage();

	void createWindow();
	bool loadImageFromFile(std::string filename);
	void scaleSprite();

	float zoom = 1;
	const float maxZoom = 10;
	const float minZoom = 0.5;
	void Zoom(float level);
	void setZoom(float level);

	bool lPress = true;
	sf::Vector2i lastMs;
	sf::Vector2f movedDistance;

	sf::Font font;
	sf::Text Ctrl_O;
public:
	JustLook(int argc, char* argv[]);
	void run();
};

