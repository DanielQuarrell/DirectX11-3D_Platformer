#include "Game.h"
#include "TexturedBox.h"

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	//Create boxes
	/*
	for (auto i = 0; i < 5; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), 
			i * 3, - 5 + i, 0
			));
	}
	*/
	
	boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx()));

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

	//camera.SetMovementTransform(rMovement * dt, thetaMovement * dt);
	wnd.Gfx().SetCamera(camera.GetMatrix());

	//Cube movement

	float verticle = 0;
	float horizontal = 0;

	if (wnd.keyboard.KeyIsPressed(0x57)) //W
	{
		verticle = -1;
	}
	if (wnd.keyboard.KeyIsPressed(0x53)) //S
	{
		verticle = 1;
	}
	if (wnd.keyboard.KeyIsPressed(0x41)) //A
	{
		horizontal = -1;
	}
	if (wnd.keyboard.KeyIsPressed(0x44)) //D
	{
		horizontal = 1;
	}
	boxes[0].get()->SetVelocity(verticle * 10, 0.0f, horizontal * 10);
	boxes[0].get()->SetEularY(thetaMovement * dt);

	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	
	wnd.Gfx().EndFrame();
}