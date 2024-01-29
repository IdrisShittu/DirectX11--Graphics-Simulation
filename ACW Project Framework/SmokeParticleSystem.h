#pragma once
#include "ParticleSystem.h"

class SmokeParticleSystem : ParticleSystem
{
public:
	SmokeParticleSystem(ID3D11Device* const device, HWND const hwnd, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& scaleReduction, const XMFLOAT3& killScale, const float transparency, const float spawnRate, const float velocity, const shared_ptr<ResourceManager>& resourceManager);
	//SmokeParticleSystem(const SmokeParticleSystem& other);
	//SmokeParticleSystem(SmokeParticleSystem&& other) noexcept;
	~SmokeParticleSystem();

	SmokeParticleSystem& operator = (const SmokeParticleSystem& other);
	SmokeParticleSystem& operator = (SmokeParticleSystem&& other) noexcept;

	void UpdateSmokeParticleSystem(const float dt);
	bool RenderSmokeParticleSystem(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const;
};

