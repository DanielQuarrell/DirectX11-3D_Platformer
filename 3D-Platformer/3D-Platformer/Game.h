#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "Box.h"
#include "TexturedBox.h"
#include "Bridge.h"
#include "Trigger.h"
#include "TriggerObj.h"

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
	void CalculateDirection(DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax, DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax, DirectX::XMVECTOR aCenter, DirectX::XMVECTOR bCenter);
	void UpdatePlayer(float dt);
	void InitialiseLevel(int level_num);
private:
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class TexturedBox>> boxes;
	std::vector<std::unique_ptr<class Bridge>> bridge;
	std::unique_ptr<class TriggerObj> pressurePlate;
	std::unique_ptr<class CustomObj> goal;
	std::unique_ptr<class Player> player;
	std::unique_ptr<class Camera> camera;

	std::unique_ptr<class Box> colourbox;
};