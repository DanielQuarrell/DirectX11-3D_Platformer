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
	void UpdateCamera(float dt);
	void UpdatePlayer(float dt);
	void InitialiseLevel(int level_num);
private:
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class TexturedBox>> boxes;
	std::unique_ptr<class Player> player;
	std::unique_ptr<class Camera> camera;
};