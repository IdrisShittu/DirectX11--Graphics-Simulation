#include "FireJetParticleSystem.h"



FireJetParticleSystem::FireJetParticleSystem(ID3D11Device* const device, HWND const hwnd, const ModelType modelType, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& finalScale, const float transparency, const float lifeCycle, const float velocity, const int particleDensity, const shared_ptr<ResourceManager>& resourceManager) :
ParticleSystem(device, hwnd, modelType, initialPosition, initialScale, finalScale, XMFLOAT3(0.7f, 0.3f, 0.0f), L"BaseColour.dds", transparency, lifeCycle, velocity, particleDensity, resourceManager) {

}


FireJetParticleSystem::~FireJetParticleSystem()
{
}

void FireJetParticleSystem::UpdateFireJetParticleSystem(const float dt)
{
	UpdateParticles(dt);
}

bool FireJetParticleSystem::RenderFireJetParticleSystem(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	return RenderParticles(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
}
