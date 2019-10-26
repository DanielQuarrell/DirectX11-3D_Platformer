#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "TexturedBox.h"

class Game
{
public:
	Game();
	~Game();
	int Start();
private:
	void UpdateFrame();
	void UpdateCamera(float dt);
	bool CheckCollision(DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax, DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax);
	float SweptAABB(DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax, DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax, float& normalX, float& normalY, float& normalZ);
	void UpdatePlayer(float dt);
	void InitialiseLevel(int level_num);
private:
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class TexturedBox>> boxes;
	std::unique_ptr<class Player> player;
	std::unique_ptr<class Camera> camera;
};