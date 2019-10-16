#pragma once
#include "Window.h"
#include "Timer.h"

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
	std::vector<std::unique_ptr<class Box>> boxes;
};