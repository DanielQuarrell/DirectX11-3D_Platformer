#include "Game.h"
#include "TexturedBox.h"
#include <fstream>

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	InitialiseLevel(1);

	//Set projection and camera
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

Game::~Game()
{
}

int Game::Start()
{
	//Message loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		UpdateFrame();
	}
}

void Game::InitialiseLevel(int level_num)
{
	std::string file_name = "";
	std::fstream path_file;
	file_name.append("Resources\\Level" + std::to_string(level_num) + ".txt");

	path_file.open(file_name);

	float levelWidth = 13;
	float levelHeight = 30;

	float x = 0;
	float y = 0;
	float z = 0;

	while (!path_file.eof())
	{
		float fileValue;

		if (path_file >> fileValue)
		{
			if (fileValue != -1.0f)
			{
				y = fileValue;

				boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx(), L"platform.png", x, y, z));
			}
		}
		else
		{
			//error with file loading
			path_file.close();
			return;
		}

		x += 1.0f;

		if (x >= levelWidth)
		{
			x = 0.0f;
			z += 1.0f;
		}
		else
		{

		}

	}
	path_file.close();
	return;
}

void Game::UpdateFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f, 1.0f);

	//Camera movement
	float rMovement = 0;
	float thetaMovement = 0;
	float cubeZmov = 0;

	if (wnd.keyboard.KeyIsPressed(VK_UP))
	{
		rMovement = -5;
	}
	if (wnd.keyboard.KeyIsPressed(VK_DOWN))
	{
		rMovement = 5;
	}
	if (wnd.keyboard.KeyIsPressed(VK_LEFT))
	{
		thetaMovement = -1;
	}
	if (wnd.keyboard.KeyIsPressed(VK_RIGHT))
	{
		thetaMovement = 1;
	}

	camera.SetMovementTransform(rMovement * dt, thetaMovement * dt);
	wnd.Gfx().SetCamera(camera.GetMatrix());

	//Cube movement

	float verticle = 0;
	float horizontal = 0;
	float cubeRotation = 0;

	if (wnd.keyboard.KeyIsPressed(0x57)) //W
	{
		verticle = 1;
	}
	if (wnd.keyboard.KeyIsPressed(0x53)) //S
	{
		verticle = -1;
	}
	if (wnd.keyboard.KeyIsPressed(0x41)) //A
	{
		horizontal = -1;
	}
	if (wnd.keyboard.KeyIsPressed(0x44)) //D
	{
		horizontal = 1;
	}
	if (wnd.keyboard.KeyIsPressed(0x51)) //Q
	{
		cubeRotation = -1;
	}
	if (wnd.keyboard.KeyIsPressed(0x45)) //E
	{
		cubeRotation = 1;
	}

	boxes[0].get()->SetVelocity(horizontal * 10, 0.0f, verticle * 10);
	boxes[0].get()->SetEularY(cubeRotation * dt);

	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	
	wnd.Gfx().EndFrame();
}