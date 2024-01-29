#pragma once
#include "ShaderManager.h"
#include "GraphicsDeviceManager.h"
#include "Terrain.h"

using namespace std;
using namespace DirectX;

class Rocket
{
public:
	Rocket(ID3D11Device* const device, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale, const shared_ptr<ShaderManager>& shaderManager, const shared_ptr<ResourceManager>& resourceManager);
	~Rocket();

	void AdjustRotationLeft() const;
	void AdjustRotationRight() const;
	void LaunchRocket();
	const bool RocketLaunched() const;

	const XMFLOAT3& GetLauncherPosition() const;
	const XMFLOAT3& GetLookAtRocketPosition();
	const XMFLOAT3& GetLookAtRocketConePosition();
	const shared_ptr<GameObject> GetRocketBody() const;
	const shared_ptr<GameObject> GetRocketCone() const;
	const shared_ptr<GameObject> GetRocketCap() const;
	const shared_ptr<GameObject> GetRocketLauncher() const;
	
	bool CheckForTerrainCollision(const shared_ptr<Terrain>& terrain, XMFLOAT3& outCollisionPosition, float& outBlastRadius);

	void UpdateTerrainPositions(shared_ptr<Terrain>& terrain, const vector<XMFLOAT3>& terrainPositions, const XMFLOAT3& conePositionFloat, float radiusSum);

	void ResetRocketState();

	void UpdateRocket(const float dt);
	bool RenderRocket(const shared_ptr<GraphicsDeviceManager>& d3dContainer, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const;

private:
	void UpdateLightPosition() const;

	bool initFailed;

	bool rocketLaunched;

	float blastRadius;
	float initialVelocity;
	float gravity;
	XMFLOAT2 velocity;
	XMFLOAT2 angularVelocity;

	XMFLOAT3 initialLauncherPosition;
	XMFLOAT3 initialLauncherRotation;
	XMFLOAT3 lookAtRocketPosition;
	XMFLOAT3 lookAtRocketConePosition;

	shared_ptr<GameObject> rocketCone;
	shared_ptr<GameObject> rocketBody;
	shared_ptr<GameObject> rocketCap;
	shared_ptr<GameObject> rocketLauncher;
};