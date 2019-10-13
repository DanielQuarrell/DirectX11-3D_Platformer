#include "Game.h"

Game::Game() : wnd(800, 600, "DirectX 3D Platformer") {}

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
	wnd.Gfx().ClearBuffer(0.0f, 1.0f, 1.0f, 1.0f);
	wnd.Gfx().EndFrame();
}