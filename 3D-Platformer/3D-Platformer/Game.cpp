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
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f, 1.0f);
	wnd.Gfx().DrawTestTriangle(
		timer.Peek(),
		0.5f,
		0.5f
	);
	wnd.Gfx().EndFrame();
}