#include "Game.h"
#include "Player.h"
#include <fstream>

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	player = std::make_unique<Player>(wnd.Gfx(), 0.0f, 0.0f, 0.0f);
	camera = std::make_unique<Camera>(player.get());

	InitialiseLevel(2);
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
					player->SetPosition(x, 2, z);
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

	for (auto& box : boxes)
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = box->GetBBMinVertex();
		DirectX::XMVECTOR bMax = box->GetBBMaxVertex();

 		float normalX = 0.0f;
		float normalY = 0.0f;
		float normalZ = 0.0f;

		float collisionTime = SweptAABB(aMin, aMax, bMin, bMax, normalX, normalY, normalZ);
			
		if (collisionTime < 1.0f)
		{
			float remainingTime = 1.0f - collisionTime;

			float xVel = DirectX::XMVectorGetX(player->GetVelocity());
			float yVel = DirectX::XMVectorGetY(player->GetVelocity());
			float zVel = DirectX::XMVectorGetZ(player->GetVelocity());

			// If the normal is not some ridiculously small (or zero) value.
			if (abs(normalX) > 0.0001f)
			{
				// Bounce the velocity along that axis.
				xVel *= -1;
			}
			if (abs(normalY) > 0.0001f)
			{
				yVel *= -1;
			}
			if (abs(normalZ) > 0.0001f)
			{
				zVel *= -1;
			}

			player->SetVelocity(xVel, yVel, zVel);
		}

		box->Update(dt);
		box->Draw(wnd.Gfx());
	}

	player->Update(dt);
	player->Draw(wnd.Gfx());
	
	//Camera movement
	UpdateCamera(dt);
	wnd.Gfx().EndFrame();
}

void Game::UpdatePlayer(float dt)
{
	float verticle = 0;
	float horizontal = 0;
	float playerRotation = 0;

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
		playerRotation = -1;
	}
	if (wnd.keyboard.KeyIsPressed(VK_RIGHT)) //E
	{
		playerRotation = 1;
	}
	if (wnd.keyboard.KeyIsPressed(VK_SPACE))
	{
		player->Jump();
	}

	player->SetPlayerInput(horizontal, verticle);
	player->SetEularY(playerRotation * dt);
	player->ApplyGravity(dt);
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

bool Game::CheckCollision(
	DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax, 
	DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax)
{
	return (DirectX::XMVectorGetX(aMin) < DirectX::XMVectorGetX(bMax) && DirectX::XMVectorGetX(aMax) > DirectX::XMVectorGetX(bMin) &&
		DirectX::XMVectorGetY(aMin) < DirectX::XMVectorGetY(bMax) && DirectX::XMVectorGetY(aMax) > DirectX::XMVectorGetY(bMin) &&
		DirectX::XMVectorGetZ(aMin) < DirectX::XMVectorGetZ(bMax) && DirectX::XMVectorGetZ(aMax) > DirectX::XMVectorGetZ(bMin));
}

float Game::SweptAABB(
	DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax,
	DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax, 
	float& normalX, float& normalY, float& normalZ)
{
	namespace dx = DirectX;
	dx::XMVECTOR velocity = player->GetVelocity();

	//distance between the moving object and the stationary object in terms of when the moving object would "enter" the colliding object
	float xEntryDistance;
	float yEntryDistance;
	float zEntryDistance;

	//distance in terms of when the moving object would "exit" the colliding object
	float xExitDistance;
	float yExitDistance; 
	float zExitDistance;

	// Find the distance between the objects on the near and far sides for both x and y and z
	// Depending on the direction of the velocity, we'll reverse the calculation order to maintain the right sign (positive/negative).
	if (dx::XMVectorGetX(velocity) > 0.0f)
	{
		xEntryDistance = dx::XMVectorGetX(bMin) - dx::XMVectorGetX(aMax);
		xExitDistance = dx::XMVectorGetX(bMax) - dx::XMVectorGetX(aMin);
	}
	else
	{
		xEntryDistance = dx::XMVectorGetX(bMax) - dx::XMVectorGetX(aMin);
		xExitDistance = dx::XMVectorGetX(bMin) - dx::XMVectorGetX(aMax);
	}

	if (dx::XMVectorGetY(velocity) > 0.0f)
	{
		yEntryDistance = dx::XMVectorGetY(bMin) - dx::XMVectorGetY(aMax);
		yExitDistance = dx::XMVectorGetY(bMax) - dx::XMVectorGetY(aMin);
	}
	else
	{
		yEntryDistance = dx::XMVectorGetY(bMax) - dx::XMVectorGetY(aMin);
		yExitDistance = dx::XMVectorGetY(bMin) - dx::XMVectorGetY(aMax);
	}

	if (dx::XMVectorGetZ(velocity) > 0.0f)
	{
		zEntryDistance = dx::XMVectorGetZ(bMin) - dx::XMVectorGetZ(aMax);
		zExitDistance = dx::XMVectorGetZ(bMax) - dx::XMVectorGetZ(aMin);
	}
	else
	{
		zEntryDistance = dx::XMVectorGetZ(bMax) - dx::XMVectorGetZ(aMin);
		zExitDistance = dx::XMVectorGetZ(bMin) - dx::XMVectorGetZ(aMax);
	}

	// These variables stand for the time at which the moving object would enter/exit the stationary object.
	float xEntryTime;
	float yEntryTime;
	float zEntryTime;

	float xExitTime;
	float yExitTime;
	float zExitTime;

	if (dx::XMVectorGetX(velocity) == 0.0f)
	{
		// Setting this to postivie infinity will ignore this variable.
		xEntryTime = -std::numeric_limits<float>::infinity();
		xExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		// If there is a velocity in the x-axis, then we can determine the time of collision based on the distance divided by the velocity. (Assuming velocity does not change.)
		xEntryTime = xEntryDistance / dx::XMVectorGetX(velocity);
		xExitTime = xExitDistance / dx::XMVectorGetX(velocity);
	}

	if (dx::XMVectorGetY(velocity) == 0.0f)
	{
		// Setting this to postivie infinity will ignore this variable.
		yEntryTime = -std::numeric_limits<float>::infinity();
		yExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		// If there is a velocity in the x-axis, then we can determine the time of collision based on the distance divided by the velocity. (Assuming velocity does not change.)
		yEntryTime = yEntryDistance / dx::XMVectorGetY(velocity);
		yExitTime = yExitDistance / dx::XMVectorGetY(velocity);
	}

	if (dx::XMVectorGetZ(velocity) == 0.0f)
	{
		// Setting this to postivie infinity will ignore this variable.
		zEntryTime = -std::numeric_limits<float>::infinity();
		zExitTime = std::numeric_limits<float>::infinity();
	}
	else
	{
		// If there is a velocity in the x-axis, then we can determine the time of collision based on the distance divided by the velocity. (Assuming velocity does not change.)
		zEntryTime = zEntryDistance / dx::XMVectorGetZ(velocity);
		zExitTime = zExitDistance / dx::XMVectorGetZ(velocity);
	}

	// Get the maximum entry time to determine the latest collision, which is actually when the objects are colliding. (Because all 3 axes must collide.)
	float entryTime = max(max(xEntryTime, yEntryTime), zEntryTime);
	// Get the minimum exit time to determine when the objects are no longer colliding. (AKA the objects passed through one another.)
	float exitTime = min(min(xExitTime, yExitTime), zExitTime);

	// If anything in the following statement is true, there's no collision.
	// If entryTime > exitTime, that means that one of the axes is exiting the "collision" before the other axes are crossing, thus they don't cross the object in unison and there's no collison.
	// If all three of the entry times are less than zero, then the collision already happened (or we missed it, but either way..)
	// If any of the entry times are greater than 1.0f, then the collision isn't happening this update/physics step so we'll move on.
	if (entryTime > exitTime || xEntryTime < 0.0f && yEntryTime < 0.0f && zEntryTime < 0.0f || xEntryTime > 1.0f || yEntryTime > 1.0f || zEntryTime > 1.0f)
	{
		// With no collision, we pass out zero'd normals.
		normalX = 0.0f;
		normalY = 0.0f;
		normalZ = 0.0f;
		return 1.0f;
	}
	else // If there was a collision
	{
		// Calculate normal of collided surface
		if (xEntryTime > yEntryTime && xEntryTime > zEntryTime) // If the x-axis is the last to cross, then that is the colliding axis.
		{
			if (xEntryDistance < 0.0f) // Determine the normal based on positive or negative.
			{
				normalX = 1.0f;
				normalY = 0.0f;
				normalZ = 0.0f;
			}
			else
			{
				normalX = -1.0f;
				normalY=  0.0f;
				normalZ = 0.0f;
			}
		}
		else if (yEntryTime > xEntryTime&& yEntryTime > zEntryTime)
		{
			if (yEntryDistance < 0.0f)
			{
				normalX = 0.0f;
				normalY = 1.0f;
				normalZ = 0.0f;
			}
			else
			{
				normalX = 0.0f;
				normalY = -1.0f;
				normalZ = 0.0f;
			}
		}
		else if (zEntryTime > xEntryTime&& zEntryTime > yEntryTime)
		{
			if (zEntryDistance < 0.0f)
			{
				normalX = 0.0f;
				normalY = 0.0f;
				normalZ = 1.0f;
			}
			else
			{
				normalX = 0.0f;
				normalY = 0.0f;
				normalZ = -1.0f;
			}
		}

		// Return the time of collision
		return entryTime;
	}
}

/*
float Game::SweptAABB(TexturedBox *box, float& normalX, float& normalY, float& normalZ)
{
	//How far away the closest edges of the objects are from each other
	float xInvEntry;
	float yInvEntry;
	float zInvEntry;

	//The distances to the far side of the object
	float xInvExit;
	float yInvExit;
	float zInvExit;

	// find the distance between the objects on the near and far sides for both x and y and z
	if (DirectX::XMVectorGetX(player->GetVelocity()) > 0.0f)
	{
		xInvEntry = DirectX::XMVectorGetX(box->GetBBMinVertex()) - 
			(DirectX::XMVectorGetX(player->GetBBMinVertex()) + DirectX::XMVectorGetX(player->GetBBMaxVertex()));

		xInvExit = (DirectX::XMVectorGetX(box->GetBBMinVertex()) + DirectX::XMVectorGetX(box->GetBBMaxVertex())) - 
			DirectX::XMVectorGetX(player->GetBBMinVertex());
	}
	else
	{
		xInvEntry = (DirectX::XMVectorGetX(box->GetBBMinVertex()) + DirectX::XMVectorGetX(box->GetBBMaxVertex())) -
			DirectX::XMVectorGetX(player->GetBBMinVertex());

		xInvExit = DirectX::XMVectorGetX(box->GetBBMinVertex()) -
			(DirectX::XMVectorGetX(player->GetBBMinVertex()) + DirectX::XMVectorGetX(player->GetBBMaxVertex()));
	}

	if (DirectX::XMVectorGetY(player->GetVelocity()) > 0.0f)
	{
		yInvEntry = DirectX::XMVectorGetY(box->GetBBMinVertex()) -
			(DirectX::XMVectorGetY(player->GetBBMinVertex()) + DirectX::XMVectorGetY(player->GetBBMaxVertex()));

		yInvExit = (DirectX::XMVectorGetY(box->GetBBMinVertex()) + DirectX::XMVectorGetY(box->GetBBMaxVertex())) -
			DirectX::XMVectorGetY(player->GetBBMinVertex());
	}
	else
	{
		yInvEntry = (DirectX::XMVectorGetY(box->GetBBMinVertex()) + DirectX::XMVectorGetY(box->GetBBMaxVertex())) -
			DirectX::XMVectorGetY(player->GetBBMinVertex());

		yInvExit = DirectX::XMVectorGetY(box->GetBBMinVertex()) -
			(DirectX::XMVectorGetY(player->GetBBMinVertex()) + DirectX::XMVectorGetY(player->GetBBMaxVertex()));
	}

	if (DirectX::XMVectorGetZ(player->GetVelocity()) > 0.0f)
	{
		zInvEntry = DirectX::XMVectorGetZ(box->GetBBMinVertex()) -
			(DirectX::XMVectorGetZ(player->GetBBMinVertex()) + DirectX::XMVectorGetZ(player->GetBBMaxVertex()));

		zInvExit = (DirectX::XMVectorGetZ(box->GetBBMinVertex()) + DirectX::XMVectorGetZ(box->GetBBMaxVertex())) -
			DirectX::XMVectorGetZ(player->GetBBMinVertex());
	}
	else
	{
		zInvEntry = (DirectX::XMVectorGetZ(box->GetBBMinVertex()) + DirectX::XMVectorGetZ(box->GetBBMaxVertex())) -
			DirectX::XMVectorGetZ(player->GetBBMinVertex());

		zInvExit = DirectX::XMVectorGetZ(box->GetBBMinVertex()) -
			(DirectX::XMVectorGetZ(player->GetBBMinVertex()) + DirectX::XMVectorGetZ(player->GetBBMaxVertex()));
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
	float xEntry;
	float yEntry;
	float zEntry;

	float xExit;
	float yExit;
	float zExit;

	if (DirectX::XMVectorGetX(player->GetVelocity()) == 0.0f)
	{
		xEntry = -FLT_MAX;
		xExit = FLT_MAX;
	}
	else
	{
		xEntry = xInvEntry / DirectX::XMVectorGetX(player->GetVelocity());
		xExit = xInvExit / DirectX::XMVectorGetX(player->GetVelocity());
	}

	if (DirectX::XMVectorGetY(player->GetVelocity()) == 0.0f)
	{
		yEntry = -FLT_MAX;
		yExit = FLT_MAX;
	}
	else
	{
		yEntry = yInvEntry / DirectX::XMVectorGetY(player->GetVelocity());
		yExit = yInvExit / DirectX::XMVectorGetY(player->GetVelocity());
	}

	if (DirectX::XMVectorGetZ(player->GetVelocity()) == 0.0f)
	{
		zEntry = -FLT_MAX;
		zExit = FLT_MAX;
	}
	else
	{
		zEntry = zInvEntry / DirectX::XMVectorGetZ(player->GetVelocity());
		zExit = zInvExit / DirectX::XMVectorGetZ(player->GetVelocity());
	}

	// find the earliest/latest times of collision
	float entryTime = max(xEntry, yEntry, zEntry);
	float exitTime = min(xExit, yExit, zExit);

	//If there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
	{
		normalX = 0.0f;
		normalY = 0.0f;
		normalZ = 0.0f;
		return 1.0f;
	}
	else // if there was a collision
	{
		//Calculate normal of collided surface
		if (xEntry > yEntry && xEntry > zEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalX = 1.0f;
				normalY = 0.0f;
				normalZ = 0.0f;
			}
			else
			{
				normalX = -1.0f;
				normalY = 0.0f;
				normalZ = 0.0f;
			}
		}
		else if(yEntry > xEntry && yEntry > zEntry)
		{
			if (yInvEntry < 0.0f)
			{
				normalX = 0.0f;
				normalY = 1.0f;
				normalZ = 0.0f;
			}
			else
			{
				normalX = 0.0f;
				normalY = -1.0f;
				normalZ = 0.0f;
			}
		}
		else if (zEntry > xEntry && zEntry > yEntry)
		{
			if (zInvEntry < 0.0f)
			{
				normalX = 0.0f;
				normalY = 0.0f;
				normalZ = 1.0f;
			}
			else
			{
				normalX = 0.0f;
				normalY = 0.0f;
				normalZ = -1.0f;
			}
		}

		// return the time of collision
		return entryTime;
	}
}
*/