#pragma once
#include "Window.h"
#include "Timer.h"

class Game
{
public:
	Game();
	int Start();
private:
	void UpdateFrame();
private:
	Window wnd;
	Timer timer;
};