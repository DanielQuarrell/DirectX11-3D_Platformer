#include "Game.h"
#include "Player.h"
#include "CustomObj.h"
#include <fstream>

Game::Game() : wnd(800, 600, "DirectX 3D Platformer")
{
	player = std::make_unique<Player>(wnd.Gfx(), 0.0f, 0.0f, 0.0f);
	camera = std::make_unique<Camera>(player.get());
	button = std::make_unique<CustomObj>(wnd.Gfx(), L"BRB", 7.0f, 2.0f, 3.0f, 5.0, 5.0f, 5.0f);

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
				float numberOfBoxes;

				if (fileValue == -2.0f)
				{
					float spawnX = x;
					float spawnY = 2;
					float spawnZ = z;
					player->SetSpawnPosition(spawnX, spawnY, spawnZ);
					player->SetPosition(spawnX, spawnY, spawnZ);

					boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx(), L"platform.png", x, 0.0f, z));
				}
				else if (fileValue == -3.0f)
				{
					pressurePlate = std::make_unique<Trigger>(wnd.Gfx(), L"trigger.png", x, 2.5f, z);
					boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx(), L"platform.png", x, 2.0f, z));
				}
				else if (fileValue == -4.0)
				{
					bridge.push_back(std::make_unique<Bridge>(wnd.Gfx(), L"bridge.png", x, 0.0f, z));
				}
				else
				{
					numberOfBoxes = fileValue;

					for (int yVal = 0; yVal <= numberOfBoxes; yVal++)
					{
						y = yVal;
						boxes.push_back(std::make_unique<TexturedBox>(wnd.Gfx(), L"platform.png", x, y, z));
					}
				}
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
				bridgePiece->MoveTowards(3.0f, 1.0f);
			}
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

	player->Update(dt);
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

	pressurePlate->Update(dt);
	pressurePlate->Draw(wnd.Gfx());

	button->Update(dt);
	button->Draw(wnd.Gfx());
	
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

	float highestAxis = max(max(max(xPositive, xNegative), max(yPositive, yNegative)), max(zPositive, zNegative));


	//distance between the moving object and the stationary object in terms of when the moving object would "enter" the colliding object
	float xEntryDistance = 0;
	float yEntryDistance = 0;
	float zEntryDistance = 0;

	// Find the distance between the objects on the near and far sides for both x and y and z
	// Depending on the direction of the velocity, we'll reverse the calculation order to maintain the right sign (positive/negative).
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

	if (highestAxis < 1.0f)
	{
		if (xPositive == highestAxis || xNegative == highestAxis)
		{
			xVel = 0;
			player->MovePosition(xEntryDistance, 0, 0);
		}

		if (yPositive == highestAxis || yNegative == highestAxis)
		{
			yVel = 0;
			player->SetGrounded(true);
			player->MovePosition(0, yEntryDistance, 0);
		}

		if (zPositive == highestAxis || zNegative == highestAxis)
		{
			zVel = 0;
			player->MovePosition(0, 0, zEntryDistance);
		}
	}

	player->SetVelocity(xVel, yVel, zVel);
}

