#include "Game.h"
#include "TexturedBox.h"
#include "Player.h"
#include <fstream>

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	player = std::make_unique<Player>(wnd.Gfx(), 0.0f, 0.0f, 0.0f);
	camera = std::make_unique<Camera>(player.get());

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
				if (fileValue == -2.0f)
				{
					player->SetPosition(x, 1, z);
					y = 0;
				}
				else
				{
					y = fileValue;
				}

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

	}
	path_file.close();
	return;
}

void Game::UpdateFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f, 1.0f);

	//Player movement
	UpdatePlayer(dt);

	//Camera movement
	UpdateCamera(dt);

	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}

	player->Draw(wnd.Gfx());
	
	wnd.Gfx().EndFrame();
}

void Game::UpdatePlayer(float dt)
{
	float verticle = 0;
	float horizontal = 0;
	float liftyboi = 0;
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
	if (wnd.keyboard.KeyIsPressed(VK_LEFT)) //Q
	{
		cubeRotation = -1;
	}
	if (wnd.keyboard.KeyIsPressed(VK_RIGHT)) //E
	{
		cubeRotation = 1;
	}
	if (wnd.keyboard.KeyIsPressed(0x52)) //R
	{
		liftyboi = 1;
	}
	if (wnd.keyboard.KeyIsPressed(0x46)) //F
	{
		liftyboi = -1;
	}
	player->SetVelocity(horizontal, liftyboi * 10, verticle);
	player->SetEularY(cubeRotation * dt);

	player->Update(dt);
}

void Game::UpdateCamera(float dt)
{
	float rMovement = 0;

	if (wnd.keyboard.KeyIsPressed(VK_UP))
	{
		rMovement = -5;
	}
	if (wnd.keyboard.KeyIsPressed(VK_DOWN))
	{
		rMovement = 5;
	}

	camera->SetMovementTransform(rMovement * dt);
	wnd.Gfx().SetCamera(camera->GetMatrix());
}
