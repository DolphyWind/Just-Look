#include "JustLook.h"
#include "my_res.h"

void JustLook::handleEvents()
{
	while (window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed) window.close();
		if (e.type == sf::Event::Resized && window.hasFocus())
		{
			sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
			sf::View view(visibleArea);
			view.move(movedDistance.x, movedDistance.y);
			window.setView(view);
			scaleSprite();
			setZoom(zoom);
		}
		if (e.type == sf::Event::MouseWheelMoved && window.hasFocus())
			Zoom(e.mouseWheel.delta);
	}
}

void JustLook::handleInputs()
{
	if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) && sf::Keyboard::isKeyPressed(sf::Keyboard::O))
		openImage();

	if (imageOpened)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (lPress)
			{
				lastMs = sf::Mouse::getPosition(window);
				lPress = false;
			}
			else
			{
				sf::Vector2i ms = sf::Mouse::getPosition(window);

				sf::Vector2i last_pos(lastMs.x, lastMs.y);
				sf::Vector2f MS(window.mapPixelToCoords(last_pos).x, window.mapPixelToCoords(last_pos).y);

				if (!(MS.x >= s.getPosition().x && MS.x <= s.getPosition().x + s.getScale().x * image.getSize().x && MS.y >= s.getPosition().y && MS.y <= s.getPosition().y + s.getScale().y * image.getSize().y)) return;

				float dx, dy;
				dx = (lastMs.x - ms.x) / zoom;
				dy = (lastMs.y - ms.y) / zoom;
				movedDistance.x += dx;
				movedDistance.y += dy;
				sf::View view = window.getView();
				view.move(dx, dy);
				window.setView(view);
				lastMs = ms;
			}
		}
		else lPress = true;
	}
}

void JustLook::draw()
{
	window.clear(backgroundColor);

	if (!imageOpened) window.draw(Ctrl_O);
	if (imageOpened) window.draw(s);

	window.display();
}

void JustLook::openImage()
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "png files (*.png)\0*.png\0jpg files (*.jpg)\0*.jpg\0bmp files (*.bmp)\0*.bmp\0tga files (*.tga)\0*.tga\0gif files (*.gif)\0*.gif\0psd files (*.psd)\0*.psd\0hdr files (*hdr)\0*.hdr\0pic files (*.pic)\0*.pic\0All files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	if (fileNameStr != "")
	{
		imageOpened = loadImageFromFile(fileNameStr);
		if (imageOpened)
		{
			sf::String title = L"Just Look - " + std::wstring(fileNameStr.begin(), fileNameStr.end());
			window.setTitle(title);
			sf::FloatRect visibleArea(0, 0, window.getSize().x, window.getSize().y);
			sf::View view(visibleArea);
			window.setView(view);
			scaleSprite();
			setZoom(1);
			movedDistance = sf::Vector2f();
		}
		else window.setTitle("Just Look");
	}
}

void JustLook::createWindow()
{
	window.create(sf::VideoMode(1280, 720), "Just Look");
	window.setFramerateLimit(60);

	hwnd = window.getSystemHandle();
}

bool JustLook::loadImageFromFile(std::string filename)
{
	if (!image.loadFromFile(filename))
	{
		std::string message = "Failed to load \"" + filename + "\"";
		MessageBoxA(hwnd, message.c_str(), "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	sf::String title = L"Just Look - " + std::wstring(filename.begin(), filename.end());
	window.setTitle(title);
	return true;
}

void JustLook::scaleSprite()
{
	if (!imageOpened) return;
	t = sf::Texture();
	t.loadFromImage(image);
	s = sf::Sprite();
	s.setTexture(t);
	float scale;
	float x_ratio, y_ratio;
	x_ratio = float(window.getSize().x) / float(image.getSize().x);
	y_ratio = float(window.getSize().y) / float(image.getSize().y);
	scale = (x_ratio > y_ratio ? y_ratio : x_ratio);
	sf::Vector2f newSize(float(image.getSize().x) * scale, float(image.getSize().y) * scale);
	s.setScale(scale, scale);
	s.setPosition((float(window.getSize().x) - newSize.x) / 2, (float(window.getSize().y) - newSize.y) / 2);
}

void JustLook::Zoom(float level)
{
	if (!imageOpened) return;
	if (zoom + level < 0) zoom += level / 2;
	else zoom += level;
	if (zoom > 1) zoom = int(zoom);
	if (zoom < minZoom) zoom = minZoom;
	if (zoom > maxZoom) zoom = maxZoom;

	sf::View view = window.getView();
	sf::Vector2i ms = sf::Mouse::getPosition(window);
	sf::Vector2f newSize(window.getSize().x / zoom, window.getSize().y / zoom);
	view.setSize(newSize);
	window.setView(view);
}

void JustLook::setZoom(float level)
{
	if (!imageOpened) return;
	zoom = level;
	if (zoom < minZoom) zoom = minZoom;
	if (zoom > maxZoom) zoom = maxZoom;

	sf::View view = window.getView();
	sf::Vector2i ms = sf::Mouse::getPosition(window);
	sf::Vector2f newSize(window.getSize().x / zoom, window.getSize().y / zoom);
	view.setSize(newSize);
	//view.setCenter(ms.x, ms.y);
	window.setView(view);
}

JustLook::JustLook(int argc, char* argv[])
{
	this->createWindow();
	if (argc == 1) imageOpened = false;
	else imageOpened = loadImageFromFile(argv[1]);
	if (imageOpened)
	{
		sf::FloatRect visibleArea(0, 0, window.getSize().x, window.getSize().y);
		sf::View view(visibleArea);
		window.setView(view);
		scaleSprite();
		setZoom(1);
		movedDistance = sf::Vector2f();
	}
	sf::Image icon;
	icon.loadFromMemory(&my_icon[0], my_icon.size());
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	font.loadFromMemory(&opensans_font[0], opensans_font.size());
	Ctrl_O.setFont(font);
	Ctrl_O.setString("Press Ctrl+O to load image.");
	Ctrl_O.setFillColor(sf::Color::Black);
}

void JustLook::run()
{
	while (window.isOpen())
	{
		handleEvents();
		if (window.hasFocus()) handleInputs();
		Ctrl_O.setPosition(movedDistance);
		
		draw();
	}
}
