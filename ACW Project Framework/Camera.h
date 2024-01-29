#pragma once

#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

class Camera
{
public:
	Camera(); 
	Camera(const Camera& other); 
	Camera(Camera && other);
	~Camera();

	Camera& operator = (const Camera& other);
	

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(const float x, const float y, const float z);
	void SetRotation(const float x, const float y, const float z);
	void AddPositionX(const float x);
	void AddPositionY(const float y);
	void AddPositionZ(const float z);
	void AddRotationX(const float x);
	void AddRotationY(const float y);
	void AddRotationZ(const float z);
	void GetViewMatrix(XMMATRIX &viewMatrix) const;
	void Render();

private:
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT4X4 viewMatrix;
};