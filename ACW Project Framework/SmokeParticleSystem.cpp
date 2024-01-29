#include "SmokeParticleSystem.h"

SmokeParticleSystem::SmokeParticleSystem(ID3D11Device* const device, HWND const hwnd, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& scaleReduction, const XMFLOAT3& killScale, const float transparency, const float spawnRate, const float velocity, const shared_ptr<ResourceManager>& resourceManager) :
ParticleSystem(device, hwnd, initialPosition, initialScale, scaleReduction, killScale, XMFLOAT3(0.7f, 0.7f, 0.7f), L"BaseColour.dds", transparency, spawnRate, velocity, resourceManager)
{}

SmokeParticleSystem::~SmokeParticleSystem()
{
}

void SmokeParticleSystem::UpdateSmokeParticleSystem(const float dt)
{
	UpdateParticles(dt);
}

bool SmokeParticleSystem::RenderSmokeParticleSystem(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	return RenderParticles(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
}