#pragma once
#include <DirectXMath.h>
#include <fstream>
#include <d3d11.h>

using namespace DirectX;
using namespace std;

class SimulationConfigLoader
{
public:
	explicit SimulationConfigLoader(const char* const configurationFileName);
	void LoadConfiguration(const char* const configurationFile);
	XMFLOAT3 ReadXMFLOAT3(std::ifstream& fileStream);
	XMFLOAT4 ReadXMFLOAT4(std::ifstream& fileStream);
	~SimulationConfigLoader();

	SimulationConfigLoader& operator = (const SimulationConfigLoader& other) = default;
	SimulationConfigLoader& operator = (SimulationConfigLoader&& other) noexcept = default; 

	const XMFLOAT3& GetCameraPosition() const;

	const XMFLOAT3& GetTerrainDimensions() const;
	const XMFLOAT3& GetTerrainScale() const;

	const XMFLOAT3& GetRocketPosition() const;
	const XMFLOAT3& GetRocketRotation() const;
	const XMFLOAT3& GetRocketScale() const;

	const XMFLOAT4& GetSunAmbient() const;
	const XMFLOAT4& GetSunDiffuse() const;
	const XMFLOAT4& GetSunSpecular() const;
	const float GetSunSpecularPower() const;

	const XMFLOAT4& GetMoonAmbient() const;
	const XMFLOAT4& GetMoonDiffuse() const;
	const XMFLOAT4& GetMoonSpecular() const;
	const float GetMoonSpecularPower() const;

	const XMFLOAT3& GetLaunchPadScale() const;
	const XMFLOAT4& GetLaunchPadTessellationValues() const;
	const XMFLOAT4& GetLaunchPadDisplacementValues() const;

private:

	XMFLOAT3  rocketPosition;
	XMFLOAT3  rocketRotation;
	XMFLOAT3  rocketScale;

	XMFLOAT3  terrainSize;
	XMFLOAT3  terrainScale;

	XMFLOAT4  sunlightAmbientColor;
	XMFLOAT4  sunlightDiffuseColor;
	XMFLOAT4  sunlightSpecularColor;
	float  sunlightSpecularIntensity;

	XMFLOAT4  moonlightAmbientColor;
	XMFLOAT4  moonlightDiffuseColor;
	XMFLOAT4  moonlightSpecularColor;
	float  moonlightSpecularIntensity;

	XMFLOAT3  cameraPosition;

	XMFLOAT3  launchPadScale;
	XMFLOAT4  launchPadTessellationSettings;
	XMFLOAT4  launchPadDisplacementSettings;

};
