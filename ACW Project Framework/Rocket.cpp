#include "Rocket.h"

Rocket::Rocket(ID3D11Device* const device, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale, const shared_ptr<ShaderManager>& shaderManager, const shared_ptr<ResourceManager>& resourceManager) : initFailed(false), rocketLaunched(false), blastRadius(5.0f), initialVelocity(25.0f), gravity(-9.81f), velocity(XMFLOAT2()), angularVelocity(XMFLOAT2()), initialLauncherPosition(XMFLOAT3()), initialLauncherRotation(XMFLOAT3()), lookAtRocketPosition(XMFLOAT3()), lookAtRocketConePosition(XMFLOAT3()), rocketCone(nullptr), rocketBody(nullptr), rocketCap(nullptr), rocketLauncher(nullptr)
{
    vector<const WCHAR*> textureNames = {L"BaseColour.dds", L"BaseNormal.dds", L"BaseSpecular.dds", L"BaseDisplacement.dds"};
	auto initGameObject = [&](shared_ptr<GameObject>& obj, const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scl, ModelType model, shared_ptr<GameObject> parent = nullptr) {
		obj = make_shared<GameObject>();
		obj->AddPositionComponent(pos);
		obj->AddRotationComponent(rot);
		obj->AddScaleComponent(scl);
		obj->AddModelComponent(device, model, resourceManager);
		obj->AddTextureComponent(device, textureNames, resourceManager);
		obj->SetShaderComponent(shaderManager->GetTextureDisplacementShader());

		if (parent)obj->AddParentGameObject(parent);
	
		if (obj->GetInitializationState()) {
			MessageBox(nullptr, "Initialization failed for a game object.", "Error", MB_OK);
			return false;
		}
		return true;
	};

	if (!initGameObject(rocketBody, position, rotation, { scale.x, 6.0f * scale.y, scale.z }, ModelType::LowPolyCylinder) ||
		!initGameObject(rocketCap, { 0.0f, -3.0f * scale.y, 0.0f }, {}, { 0.92f, 0.1f, 0.92f }, ModelType::Sphere, rocketBody) ||
		!initGameObject(rocketLauncher, { 1.3f * scale.x, 0.0f, 0.0f }, {}, { 0.3f * scale.x, 8.0f * scale.y, 0.3f * scale.z }, ModelType::LowPolyCylinder, rocketBody) ||
		!initGameObject(rocketCone, { 0.0f, 3.0f * scale.y + 1.0f, 0.0f }, {}, { 1.0f, 2.0f, 1.0f }, ModelType::Cone, rocketBody)) {
		initFailed = true;
		return;
	}

	initialLauncherPosition = position;
	initialLauncherRotation = rotation;

	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixIdentity();

	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)));
	rocketMatrix = XMMatrixMultiply(rocketMatrix, XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z));

	auto posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -0.6f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightScale = XMVECTOR();
	auto lightRotation = XMVECTOR();
	auto lightPosition = XMVECTOR();

	XMMatrixDecompose(&lightScale, &lightRotation, &lightPosition, posMatrix);

	posMatrix = XMMatrixIdentity();

	posMatrix = XMMatrixMultiply(posMatrix, XMMatrixTranslation(0.0f, -1.2f, 0.0f));

	posMatrix = posMatrix * rocketMatrix;

	auto lightPointPosition = XMVECTOR();
	XMMatrixDecompose(&lightScale, &lightRotation, &lightPointPosition, posMatrix);

	auto lightPositionFloat = XMFLOAT3();
	auto lightPointPositionFloat = XMFLOAT3();

	XMStoreFloat3(&lightPositionFloat, lightPosition);
	XMStoreFloat3(&lightPointPositionFloat, lightPointPosition);

	rocketLauncher->Update();
}

Rocket::~Rocket()
{
}

void Rocket::AdjustRotationLeft() const
{
	if (!rocketLaunched)
	{
		auto launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0);
		launchAngle.z = max(launchAngle.z - XM_PIDIV4 / 30.0f, 0.0f);
		rocketBody->SetRotation(launchAngle);
		rocketLauncher->Update();
	}
}

void Rocket::AdjustRotationRight() const
{
	if (!rocketLaunched)
	{
		auto launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0);
		const float angleIncrement = XM_PIDIV4 / 30.0f;
		const float minAngle = -XM_PIDIV2 + XM_PIDIV4 / 10.0f;
		launchAngle.z = max(launchAngle.z + angleIncrement, minAngle);
		rocketBody->SetRotation(launchAngle);
		rocketLauncher->Update();
	}
}

void Rocket::LaunchRocket()
{
	if (!rocketLaunched)
	{
		float launchAngle = rocketBody->GetRotationComponent()->GetRotationAt(0).z;
		float launchAngleRad = XM_PIDIV2 + launchAngle; 
		velocity.x = initialVelocity * cos(launchAngleRad);
		velocity.y = initialVelocity * sin(launchAngleRad);
		float totalAngularMovement = launchAngleRad + XM_PIDIV2;
		float vSquared = velocity.x * velocity.x + velocity.y * velocity.y;
		float totalTimeOfJourney = (vSquared * sin(launchAngleRad) + sqrt(vSquared * sin(launchAngleRad) * sin(launchAngleRad) + 2.0f * gravity * 3.0f)) / gravity;
		totalTimeOfJourney *= 2.0f;
		angularVelocity.x = totalAngularMovement / totalTimeOfJourney;
		angularVelocity.y = angularVelocity.x; 
		rocketLaunched = true;
	}
}

const bool Rocket::RocketLaunched() const
{
	return rocketLaunched;
}

const XMFLOAT3& Rocket::GetLauncherPosition() const
{
	return initialLauncherPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketPosition()
{
	const auto& rocketBodyPosition = rocketBody->GetPositionComponent()->GetPositionAt(0);
	const auto& rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	lookAtRocketPosition = { rocketBodyPosition.x, rocketBodyPosition.y, rocketBodyPosition.z - rocketBodyScale.x };
	return lookAtRocketPosition;
}

const XMFLOAT3& Rocket::GetLookAtRocketConePosition()
{
	const auto& rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto& rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto& rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

	XMVECTOR lookAtRocketConePos;
	XMMatrixDecompose(
		nullptr,
		nullptr,
		&lookAtRocketConePos,
		XMMatrixTranslation(0.0f, 0.6f, 0.0f) *
		XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z) *
		XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)) *
		XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z)
	);

	XMStoreFloat3(&lookAtRocketConePosition, lookAtRocketConePos);
	lookAtRocketConePosition.z -= rocketBodyScale.x;

	return lookAtRocketConePosition;
}

const shared_ptr<GameObject> Rocket::GetRocketBody() const
{
	return rocketBody;
}

const shared_ptr<GameObject> Rocket::GetRocketCone() const
{
	return rocketCone;
}

const shared_ptr<GameObject> Rocket::GetRocketCap() const
{
	return rocketCap;
}

const shared_ptr<GameObject> Rocket::GetRocketLauncher() const
{
	return rocketLauncher;
}

bool Rocket::CheckForTerrainCollision(const shared_ptr<Terrain>& terrain, XMFLOAT3& outCollisionPosition, float& outBlastRadius)
{
	const auto& scale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto& rot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto& pos = rocketBody->GetPositionComponent()->GetPositionAt(0);

	auto rocketMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z)) *
		XMMatrixTranslation(pos.x, pos.y, pos.z);

	auto posMatrix = XMMatrixTranslation(0.0f, 0.6f, 0.0f) * rocketMatrix;

	XMVECTOR rocketConeScale, rocketConeRotation, rocketConePosition;
	XMMatrixDecompose(&rocketConeScale, &rocketConeRotation, &rocketConePosition, posMatrix);

	XMFLOAT3 conePositionFloat;
	XMStoreFloat3(&conePositionFloat, rocketConePosition);

	if (conePositionFloat.y < 0.0f)
	{
		if (conePositionFloat.y < -200.0f)
		{
			ResetRocketState();
			return false;
		}

		auto terrainPositions = terrain->GetPositionComponent()->GetPositions();
		const auto terrainCubeRadius = terrain->GetScaleComponent()->GetScaleAt(0).x;

		for (const auto& position : terrainPositions)
		{
			auto distance = XMFLOAT3(position.x - conePositionFloat.x, position.y - conePositionFloat.y, position.z - conePositionFloat.z);
			float size = XMVectorGetX(XMVector3Length(XMLoadFloat3(&distance)));
			float radiusSum = XMVectorGetX(rocketConeScale) + terrainCubeRadius;
			if (size * size <= radiusSum * radiusSum)
			{
				outCollisionPosition = position;
				outBlastRadius = blastRadius;
				UpdateTerrainPositions(const_cast<std::shared_ptr<Terrain>&>(terrain), terrainPositions, conePositionFloat, radiusSum + blastRadius);
				ResetRocketState();
				return true;
			}
		}
		return false;
	}
}

void Rocket::UpdateTerrainPositions(std::shared_ptr<Terrain>& terrain, const std::vector<DirectX::XMFLOAT3>& terrainPositions, const DirectX::XMFLOAT3& conePosition, float radiusSum)
{
	for (const auto& terrainPosition : terrainPositions)
	{
		DirectX::XMFLOAT3 distanceVector = {
			terrainPosition.x - conePosition.x,
			terrainPosition.y - conePosition.y,
			terrainPosition.z - conePosition.z
		};
		float distanceSquared = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&distanceVector)));
		if (distanceSquared <= radiusSum * radiusSum)
		{
			terrain->GetPositionComponent()->TranslatePositionAt(XMFLOAT3(0.0f, -500.0f, 0.0f), &terrainPosition - &terrainPositions[0]);
		}
	}
	terrain->UpdateInstanceData();
}

void Rocket::ResetRocketState()
{
	rocketBody->SetPosition(initialLauncherPosition);
	rocketBody->SetRotation(initialLauncherRotation);
	rocketLaunched = false;
	rocketLauncher->Update();
}

void Rocket::UpdateRocket(const float deltaTime)
{
	if (rocketLaunched)
	{
		velocity.y += gravity * deltaTime;
		if (velocity.y < 0.0f)
		{
			XMFLOAT3 rocketRotation = rocketBody->GetRotationComponent()->GetRotationAt(0);
			
			rocketRotation.z += angularVelocity.x;
			rocketBody->SetRotation(rocketRotation);
		}
		XMFLOAT3 currentRocketPosition = rocketBody->GetPositionComponent()->GetPositionAt(0);
		currentRocketPosition.x += velocity.x * deltaTime;
		currentRocketPosition.y += velocity.y * deltaTime;
		rocketBody->SetPosition(currentRocketPosition.x, currentRocketPosition.y, currentRocketPosition.z);
	}
	rocketBody->Update();
	rocketCone->Update();
	rocketCap->Update();
	UpdateLightPosition();
}

void Rocket::UpdateLightPosition() const
{
	const auto rocketBodyScale = rocketBody->GetScaleComponent()->GetScaleAt(0);
	const auto rocketBodyRot = rocketBody->GetRotationComponent()->GetRotationAt(0);
	const auto rocketBodyPos = rocketBody->GetPositionComponent()->GetPositionAt(0);

	XMMATRIX rocketMatrix = XMMatrixScaling(rocketBodyScale.x, rocketBodyScale.y, rocketBodyScale.z) *
		XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rocketBodyRot.x, rocketBodyRot.y, rocketBodyRot.z)) *
		XMMatrixTranslation(rocketBodyPos.x, rocketBodyPos.y, rocketBodyPos.z);

	XMMATRIX posMatrix = XMMatrixTranslation(0.0f, -0.6f, 0.0f) * rocketMatrix;

	XMVECTOR lightScale, lightRotation, lightPosition;
	XMMatrixDecompose(&lightScale, &lightRotation, &lightPosition, posMatrix);

	posMatrix = XMMatrixTranslation(0.0f, -1.2f, 0.0f) * rocketMatrix;

	XMVECTOR lightPointPosition;
	XMMatrixDecompose(&lightScale, &lightRotation, &lightPointPosition, posMatrix);

	XMFLOAT3 lightPositionFloat, lightPointPositionFloat;
	XMStoreFloat3(&lightPositionFloat, lightPosition);
	XMStoreFloat3(&lightPointPositionFloat, lightPointPosition);
}

bool Rocket::RenderRocket(const shared_ptr<GraphicsDeviceManager>& d3dContainer, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	auto deviceContext = d3dContainer->GetDeviceContext();

	if (!rocketBody->Render(deviceContext, viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition) ||
		!rocketCone->Render(deviceContext, viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition) ||
		!rocketCap->Render(deviceContext, viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition) ||
		!rocketLauncher->Render(deviceContext, viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition))
	{
		return false;
	}

	if (rocketLaunched)
	{
		d3dContainer->DisableDepthStencil();
		d3dContainer->EnableAlphaBlending();
		d3dContainer->EnabledDepthStencil();
		d3dContainer->DisableAlphaBlending();
	}

	return true;
}
