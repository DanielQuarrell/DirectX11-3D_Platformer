#pragma once
#include "GameObjectBase.h"

class TexturedBox : public GameObjectBase<TexturedBox>
{
public:
	TexturedBox(Graphics& gfx, float _x, float _y, float _z);
	void SetVelocity(float _x, float _y, float _z);
	void SetPosition(float x, float y, float z);
	void SetEularX(float angle);
	void SetEularY(float angle);
	void SetEularZ(float angle);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	float xVel = 0.0f;
	float yVel = 0.0f;
	float zVel = 0.0f;
	//Position
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;
	//Rotation
	float eularX = 0.0f;
	float eularY = 0.0f;
	float eularZ = 0.0f;
	//Model transform
	DirectX::XMFLOAT3X3 modelTransform;
};
