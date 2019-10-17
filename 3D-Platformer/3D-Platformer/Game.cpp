#include "Game.h"
#include "Box.h"

Game::Game() : wnd(800, 600, "DirectX 3D Platformer") 
{
	//Create boxes
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 5; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}

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

	float rMovement = 0;
	float thetaMovement = 0;

	if (wnd.keyboard.KeyIsPressed(VK_UP))
	{
		rMovement = -5 * dt;
	}
	if (wnd.keyboard.KeyIsPressed(VK_DOWN))
	{
		rMovement = 5 * dt;
	}
	if (wnd.keyboard.KeyIsPressed(VK_LEFT))
	{
		thetaMovement = -1 * dt;
	}
	if (wnd.keyboard.KeyIsPressed(VK_RIGHT))
	{
		thetaMovement = 1 * dt;
	}

	camera.SetMovementTransform(rMovement, thetaMovement);


	wnd.Gfx().SetCamera(camera.GetMatrix());

	for (auto& b : boxes)
	{
		b->Update(dt * 0);
		b->Draw(wnd.Gfx());
	}
	
	wnd.Gfx().EndFrame();
}