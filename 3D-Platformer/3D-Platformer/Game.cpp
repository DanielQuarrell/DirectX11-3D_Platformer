#include "Game.h"
#include "Player.h"
#include "CustomObj.h"
#include "Collectable.h"
#include <fstream>

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	player = std::make_unique<Player>(wnd.Gfx(), 0.0f, 0.0f, 0.0f);
	camera = std::make_unique<Camera>(player.get());
	colourbox = std::make_unique<Box>(wnd.Gfx(), 0.0f, 0.0f, 0.0f);

	InitialiseLevel(3);
	//Set projection and camera
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	wnd.DisableCursor();
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
	std::string levelFileName = "";
	std::fstream levelFile;
	levelFileName.append("Resources\\Level" + std::to_string(level_num) + ".txt");

	std::string itemFileName = "";
	std::fstream itemFile;
	itemFileName.append("Resources\\Level" + std::to_string(level_num) + "_Items" + ".txt");

	levelFile.open(levelFileName);
	itemFile.open(itemFileName);

	float levelWidth = 13;
	float levelHeight = 30;

	float x = 0;
	float y = 0;
	float z = 0;

	while (!levelFile.eof())
	{
		float fileValue;
		float itemValue;

		if (levelFile >> fileValue)
		{
			if (itemFile >> itemValue)
			{
				if (fileValue != -1.0f)
				{
					if (fileValue >= 0)
					{
						float numberOfBoxes = fileValue;

						for (int yVal = 0; yVal <= numberOfBoxes; yVal++)
						{
							y = yVal;
							boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx(), L"platform.png", x, y, z));
						}
					}
					else
					{
						y = fileValue * -1;
						bridge.push_back(std::make_unique<Bridge>(wnd.Gfx(), L"bridge.png", x, y, z));
					}

					//Player
					if (itemValue == 2.0f)
					{
						float spawnX = x;
						float spawnY = y + 1;
						float spawnZ = z;
						player->SetSpawnPosition(spawnX, spawnY, spawnZ);
						player->SetPosition(spawnX, spawnY, spawnZ);
					}
					//Collectable
					else if (itemValue == 3.0f)
					{
						collectables.push_back(std::make_unique<Collectable>(wnd.Gfx(), L"CHAHIN_BOTTLE_OF_SODA", x, y + 1.5f, z, 0.25f, 0.25f, 0.25f, true, true));
					}
					//Trigger
					else if (itemValue == 4.0)
					{
						pressurePlate = std::make_unique<TriggerObj>(wnd.Gfx(), L"BRB", x, y + 0.7f, z, 7.0f, 7.0f, 7.0f);
					}
					//Goal
					else if (itemValue == 5.0f)
					{
						goal = std::make_unique<CustomObj>(wnd.Gfx(), L"flag", x + 4.5f, y + 0.5f, z, DirectX::XM_PI, 0.25f, 0.25f, 0.25f, false, false);
					}
				}
			}
		}
		else
		{
			//error with file loading
			levelFile.close();
			return;
		}

		//Change position of next object
		x += 1.0f;

		if (x >= levelWidth)
		{
			x = 0.0f;
			z += 1.0f;
		}

	}
	levelFile.close();
	return;
}

void Game::UpdateFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f, 1.0f);

	//Player movement
	UpdatePlayer(dt);

	if (!pressurePlate->IsActivated())
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = pressurePlate->GetBBMinVertex();
		DirectX::XMVECTOR bMax = pressurePlate->GetBBMaxVertex();

		if (CheckCollision(aMin, aMax, bMin, bMax))
		{
			pressurePlate->Activate();

			for (auto& bridgePiece : bridge)
			{
				bridgePiece->RaiseBridge(1.0f);
			}
		}
	}

	player->SetGrounded(false);

	for (auto& box : boxes)
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = box->GetBBMinVertex();
		DirectX::XMVECTOR bMax = box->GetBBMaxVertex();
		
		if (CheckCollision(aMin, aMax, bMin, bMax))
		{
			DirectX::XMVECTOR aCenter = player->GetCenterVertex();
			DirectX::XMVECTOR bCenter = box->GetCenterVertex();

			CalculateDirection(aMin, aMax, bMin, bMax, aCenter, bCenter);
		}
	}

	for (auto& box : boxes)
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = box->GetBBMinVertex();
		DirectX::XMVECTOR bMax = box->GetBBMaxVertex();

		if (CheckCollision(aMin, aMax, bMin, bMax))
		{
			DirectX::XMVECTOR aCenter = player->GetCenterVertex();
			DirectX::XMVECTOR bCenter = box->GetCenterVertex();

			CalculateDirection(aMin, aMax, bMin, bMax, aCenter, bCenter);
		}
	}

	for (auto& bridgePiece : bridge)
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = bridgePiece->GetBBMinVertex();
		DirectX::XMVECTOR bMax = bridgePiece->GetBBMaxVertex();

		if (CheckCollision(aMin, aMax, bMin, bMax))
		{
			DirectX::XMVECTOR aCenter = player->GetCenterVertex();
			DirectX::XMVECTOR bCenter = bridgePiece->GetCenterVertex();

			CalculateDirection(aMin, aMax, bMin, bMax, aCenter, bCenter);
		}
	}

	for (auto& collectable : collectables)
	{
		DirectX::XMVECTOR aMin = player->GetBBMinVertex();
		DirectX::XMVECTOR aMax = player->GetBBMaxVertex();
		DirectX::XMVECTOR bMin = collectable->GetBBMinVertex();
		DirectX::XMVECTOR bMax = collectable->GetBBMaxVertex();

		if (CheckCollision(aMin, aMax, bMin, bMax))
		{
			collectable->SetVisibility(false);
		}
	}
	
	//Camera movement
	UpdateCamera(dt);
	player->Draw(wnd.Gfx());

	for (auto& box : boxes)
	{
		box->Update(dt);
		box->Draw(wnd.Gfx());
	}

	for (auto& bridgePiece : bridge)
	{
		bridgePiece->Update(dt);
		bridgePiece->Draw(wnd.Gfx());
	}

	for (auto& collectable : collectables)
	{
		collectable->Update(dt);
		collectable->Draw(wnd.Gfx());
	}

	pressurePlate->Update(dt);
	pressurePlate->Draw(wnd.Gfx());

	goal->Update(dt);
	goal->Draw(wnd.Gfx());

	colourbox->Update(dt);
	colourbox->Draw(wnd.Gfx());
	
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
	player->SetRotationY(playerRotation * dt);
	player->ApplyGravity(dt);
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

bool Game::CheckCollision(
	DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax, 
	DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax)
{
	return (DirectX::XMVectorGetX(aMin) < DirectX::XMVectorGetX(bMax) && DirectX::XMVectorGetX(aMax) > DirectX::XMVectorGetX(bMin) &&
		DirectX::XMVectorGetY(aMin) < DirectX::XMVectorGetY(bMax) && DirectX::XMVectorGetY(aMax) > DirectX::XMVectorGetY(bMin) &&
		DirectX::XMVectorGetZ(aMin) < DirectX::XMVectorGetZ(bMax) && DirectX::XMVectorGetZ(aMax) > DirectX::XMVectorGetZ(bMin));
}

void Game::CalculateDirection(
	DirectX::XMVECTOR aMin, DirectX::XMVECTOR aMax,
	DirectX::XMVECTOR bMin, DirectX::XMVECTOR bMax,
	DirectX::XMVECTOR aCenter, DirectX::XMVECTOR bCenter)
{
	namespace dx = DirectX;

	float xPositive = dx::XMVectorGetX(bCenter) - dx::XMVectorGetX(aCenter);
	float xNegative = (dx::XMVectorGetX(bCenter) - dx::XMVectorGetX(aCenter)) * -1;

	float yPositive = dx::XMVectorGetY(bCenter) - dx::XMVectorGetY(aCenter);
	float yNegative = (dx::XMVectorGetY(bCenter) - dx::XMVectorGetY(aCenter)) * -1;

	float zPositive = dx::XMVectorGetZ(bCenter) - dx::XMVectorGetZ(aCenter);
	float zNegative = (dx::XMVectorGetZ(bCenter) - dx::XMVectorGetZ(aCenter)) * -1;

	float entryAxis = max(max(max(xPositive, xNegative), max(yPositive, yNegative)), max(zPositive, zNegative));

	//Distance of the intesection of two objects for each axis
	float xEntryDistance = 0;
	float yEntryDistance = 0;
	float zEntryDistance = 0;

	//Determine the entry distance based on 
	if (xPositive > xNegative)
	{
		xEntryDistance = dx::XMVectorGetX(bMin) - dx::XMVectorGetX(aMax);
	}
	else
	{
		xEntryDistance = dx::XMVectorGetX(bMax) - dx::XMVectorGetX(aMin);
	}

	if (yPositive > yNegative)
	{
		yEntryDistance = dx::XMVectorGetY(bMin) - dx::XMVectorGetY(aMax);
	}
	else
	{
		yEntryDistance = dx::XMVectorGetY(bMax) - dx::XMVectorGetY(aMin);
	}

	if (zPositive > zNegative)
	{
		zEntryDistance = dx::XMVectorGetZ(bMin) - dx::XMVectorGetZ(aMax);
	}
	else 
	{
		zEntryDistance = dx::XMVectorGetZ(bMax) - dx::XMVectorGetZ(aMin);
	}

	float xVel = dx::XMVectorGetX(player->GetVelocity());
	float yVel = dx::XMVectorGetY(player->GetVelocity());
	float zVel = dx::XMVectorGetZ(player->GetVelocity());

	if (yPositive == entryAxis || yNegative == entryAxis)
	{
		yVel = 0;
		player->SetGrounded(true);
		player->MovePosition(0, yEntryDistance, 0);
	}

	float aHeight = DirectX::XMVectorGetY(aMax) - DirectX::XMVectorGetY(aMin);

	//Don't check side collisions on objects beneath the player
	if (DirectX::XMVectorGetY(aCenter) - (aHeight / 2) < DirectX::XMVectorGetY(bCenter))
	{
		if (xPositive == entryAxis || xNegative == entryAxis)
		{
			xVel = 0;
			player->MovePosition(xEntryDistance, 0, 0);
		}

		if (zPositive == entryAxis || zNegative == entryAxis)
		{
			zVel = 0;
			player->MovePosition(0, 0, zEntryDistance);
		}
	}

	player->SetVelocity(xVel, yVel, zVel);
}

