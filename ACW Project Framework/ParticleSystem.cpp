#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* const device, HWND const hwnd, const ModelType modelType, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& finalScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float lifeCycle, const float velocity, const int particleDensity, const shared_ptr<ResourceManager>& resourceManager) : initialPosition(initialPosition), initialScale(initialScale), scaleReduction(XMFLOAT3()), killScale(XMFLOAT3()), emitterType(false), spawnRate(0.0f), elapsedTime(0.0f), lifeCycle(lifeCycle), velocity(velocity), particleSpread(lifeCycle / particleDensity)
{
	auto positions = vector<XMFLOAT3>();
	auto scales = vector<XMFLOAT3>();

	//Add particle instances
	for (auto i = 1; i <= particleDensity; i++)
	{
		positions.emplace_back(XMFLOAT3(initialPosition.x, initialPosition.y - (i * particleSpread), initialPosition.z));

		auto particleScale = XMFLOAT3(initialScale.x - ((initialScale.x - finalScale.x) / particleDensity) * i, initialScale.y - ((initialScale.y - finalScale.y) / particleDensity) * i, initialScale.z - ((initialScale.z - finalScale.z) / particleDensity) * i);

		//scales.emplace_back(initialScale - ((initialScale - finalScale) / particleDensity) * i);
		scales.emplace_back(particleScale);
	}

	AddPositionComponent(positions);
	AddScaleComponent(scales);
	AddRotationComponent(0.0f, 0.0f, 0.0f);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(textureName);

	AddModelComponent(device, modelType, resourceManager);
	AddTextureComponent(device, textureNames, resourceManager);

	const auto particleShader = make_shared<ParticleShader>(device, hwnd);

	particleShader->SetParticleParameters(colourTint, transparency);

	SetShaderComponent(particleShader);
}

ParticleSystem::ParticleSystem(ID3D11Device* const device, HWND const hwnd, const XMFLOAT3& initialPosition, const XMFLOAT3& initialScale, const XMFLOAT3& scaleReduction, const XMFLOAT3& killScale, const XMFLOAT3& colourTint, const WCHAR* const textureName, const float transparency, const float spawnRate, const float velocity, const shared_ptr<ResourceManager>& resourceManager) : initialPosition(initialPosition), initialScale(initialScale), scaleReduction(scaleReduction), killScale(killScale), emitterType(true), spawnRate(spawnRate), elapsedTime(0.0f), lifeCycle(0.0f), velocity(velocity), particleSpread(0.0f)
{
	AddPositionComponent(initialPosition);
	AddScaleComponent(initialScale);
	AddRotationComponent(0.0f, 0.0f, 0.0f);

	vector<const WCHAR*> textureNames;
	textureNames.push_back(textureName);

	AddModelComponent(device, ModelType::Quad, resourceManager);
	AddTextureComponent(device, textureNames, resourceManager);

	const auto particleShader = make_shared<ParticleShader>(device, hwnd);

	particleShader->SetParticleParameters(colourTint, transparency);

	SetShaderComponent(particleShader);
}

//ParticleSystem::ParticleSystem(const ParticleSystem& other) = default;

//ParticleSystem::ParticleSystem(ParticleSystem&& other) noexcept = default;

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::UpdateParticles(const float dt)
{
	if (emitterType)
	{
		elapsedTime += dt;

		//Add a new particle if the spawn rate is reached
		if (elapsedTime > spawnRate)
		{
			GetPositionComponent()->AddPositionFront(initialPosition);
			GetScaleComponent()->AddScaleFront(initialScale);

			elapsedTime = 0.0f;
		}

		//Check if we need to remove any particles
		for (const auto scale : GetScaleComponent()->GetScales())
		{
			if (scale.x > killScale.x)
			{
				//Remove particle
				GetPositionComponent()->RemovePositionBack();
				GetScaleComponent()->RemoveScaleBack();
			}
		}

		auto positions = GetPositionComponent()->GetPositions();
		auto scales = GetScaleComponent()->GetScales();

		//Update particles
		for (unsigned int i = 0; i < positions.size(); i++)
		{
			positions[i].y = positions[i].y + abs(velocity * dt);
			//scales[i] = scales[i] - (XMFLOAT3(scaleReduction.x * dt, scaleReduction.y * dt, scaleReduction.z * dt));
			scales[i] = XMFLOAT3(scales[i].x - scaleReduction.x * dt, scales[i].y - scaleReduction.y * dt, scales[i].z - scaleReduction.z * dt);
		}

		SetPosition(positions);
		SetScale(scales);
	}
	else
	{
		auto positions = GetPositionComponent()->GetPositions();

		for (auto& position : positions)
		{
			position = XMFLOAT3(position.x + 0.0f * velocity * dt, position.y + -1.0f * velocity * dt, position.z + 0.0f * velocity * dt);

			//If the new updated position has reached the end of its lifecycle then we 
			if (position.y < initialPosition.y - lifeCycle)
			{
				positions.pop_back();
				positions.insert(positions.begin(), initialPosition);
			}
		}

		SetPosition(positions);
	}

	Update();
}

bool ParticleSystem::RenderParticles(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) const
{
	return Render(deviceContext, viewMatrix, projectionMatrix, {}, {}, cameraPosition);
}


