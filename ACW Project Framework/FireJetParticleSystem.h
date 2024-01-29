#pragma once
#include "ParticleSystem.h"

class FireJetParticleSystem : public ParticleSystem
{
public:
	FireJetParticleSystem(ID3D11Device* const device, HWND const hwnd, const ModelType modelType, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& finalScale, const float transparency, const float lifeCycle, const float velocity, const int particleDensity, const shared_ptr<ResourceManager>& resourceManager);
	FireJetParticleSystem(const FireJetParticleSystem& other);
	FireJetParticleSystem(FireJetParticleSystem&& other) noexcept;
	~FireJetParticleSystem();

	FireJetParticleSystem& operator = (const FireJetParticleSystem& other);
	FireJetParticleSystem& operator = (FireJetParticleSystem&& other) noexcept;

	void UpdateFireJetParticleSystem(const float dt);
	bool RenderFireJetParticleSystem(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const;
};

