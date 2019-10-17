#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const;
	void SpawnControlWindow();
	void Reset();
private:
};
