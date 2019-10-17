#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"

class Game
{
public:
	Game();
	~Game();
	int Start();
private:
	void UpdateFrame();
private:
	Window wnd;
	Timer timer;
	Camera camera;
	std::vector<std::unique_ptr<class Box>> boxes;
};