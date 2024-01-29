#include "SimulationConfigLoader.h"
#include <fstream>
#include <string>
#include <functional>
#include <unordered_map>

SimulationConfigLoader::SimulationConfigLoader(const char* const configurationFile)
    : cameraPosition(XMFLOAT3()),
    terrainSize(XMFLOAT3()),
    terrainScale(XMFLOAT3()),
    rocketPosition(XMFLOAT3()),
    rocketRotation(XMFLOAT3()),
    rocketScale(XMFLOAT3()),
    sunlightAmbientColor(XMFLOAT4()),
    sunlightDiffuseColor(XMFLOAT4()),
    sunlightSpecularColor(XMFLOAT4()),
    sunlightSpecularIntensity(0.0f),
    moonlightAmbientColor(XMFLOAT4()),
    moonlightDiffuseColor(XMFLOAT4()),
    moonlightSpecularColor(XMFLOAT4()),
    moonlightSpecularIntensity(0.0f),
    launchPadScale(XMFLOAT3()),
    launchPadTessellationSettings(XMFLOAT4()),
    launchPadDisplacementSettings(XMFLOAT4()) {
    LoadConfiguration(configurationFile);
}

void SimulationConfigLoader::LoadConfiguration(const char* const configurationFile) {
    std::ifstream fileStream(configurationFile);
    if (fileStream.fail()) return;

    std::unordered_map<std::string, std::function<void()>> commandActions = {
        {"CameraInitialPosition", [&] { cameraPosition = ReadXMFLOAT3(fileStream); }},
        {"TerrainSize", [&] { terrainSize = ReadXMFLOAT3(fileStream); }},
        {"TerrainInitialScale", [&] { terrainScale = ReadXMFLOAT3(fileStream); }},
        {"RocketInitialPosition", [&] { rocketPosition = ReadXMFLOAT3(fileStream); }},
        {"RocketInitialRotation", [&] { rocketRotation = ReadXMFLOAT3(fileStream); }},
        {"RocketInitialScale", [&] { rocketScale = ReadXMFLOAT3(fileStream); }},
        {"SunlightAmbientColor", [&] { sunlightAmbientColor = ReadXMFLOAT4(fileStream); }},
        {"SunlightDiffuseColor", [&] { sunlightDiffuseColor = ReadXMFLOAT4(fileStream); }},
        {"SunlightSpecularColor", [&] { sunlightSpecularColor = ReadXMFLOAT4(fileStream); }},
        {"SunlightSpecularIntensity", [&] { fileStream >> sunlightSpecularIntensity; }},
        {"MoonlightAmbientColor", [&] { moonlightAmbientColor = ReadXMFLOAT4(fileStream); }},
        {"MoonlightDiffuseColor", [&] { moonlightDiffuseColor = ReadXMFLOAT4(fileStream); }},
        {"MoonlightSpecularColor", [&] { moonlightSpecularColor = ReadXMFLOAT4(fileStream); }},
        {"MoonlightSpecularIntensity", [&] { fileStream >> moonlightSpecularIntensity; }},
        {"LaunchPadInitialScale", [&] { launchPadScale = ReadXMFLOAT3(fileStream); }},
        {"LaunchPadTessellationSettings", [&] { launchPadTessellationSettings = ReadXMFLOAT4(fileStream); }},
        {"LaunchPadDisplacementSettings", [&] { launchPadDisplacementSettings = ReadXMFLOAT4(fileStream); }}
    };

    std::string command;
    while (fileStream >> command) {
        if (commandActions.find(command) != commandActions.end()) {
            commandActions[command]();
        }
    }
}

XMFLOAT3 SimulationConfigLoader::ReadXMFLOAT3(std::ifstream& fileStream) {
    float x, y, z;
    fileStream >> x >> y >> z;
    return XMFLOAT3(x, y, z);
}

XMFLOAT4 SimulationConfigLoader::ReadXMFLOAT4(std::ifstream& fileStream) {
    float x, y, z, w;
    fileStream >> x >> y >> z >> w;
    return XMFLOAT4(x, y, z, w);
}

SimulationConfigLoader::~SimulationConfigLoader()
{
}

const XMFLOAT3& SimulationConfigLoader::GetCameraPosition() const
{
    return  cameraPosition;
}

const XMFLOAT3& SimulationConfigLoader::GetTerrainDimensions() const
{
    return  terrainSize;
}

const XMFLOAT3& SimulationConfigLoader::GetTerrainScale() const
{
    return  terrainScale;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketPosition() const
{
    return  rocketPosition;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketRotation() const
{
    return  rocketRotation;
}

const XMFLOAT3& SimulationConfigLoader::GetRocketScale() const
{
    return  rocketScale;
}

const XMFLOAT4& SimulationConfigLoader::GetSunAmbient() const
{
    return  sunlightAmbientColor;
}

const XMFLOAT4& SimulationConfigLoader::GetSunDiffuse() const
{
    return  sunlightDiffuseColor;
}

const XMFLOAT4& SimulationConfigLoader::GetSunSpecular() const
{
    return  sunlightSpecularColor;
}

const float SimulationConfigLoader::GetSunSpecularPower() const
{
    return  sunlightSpecularIntensity;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonAmbient() const
{
    return  moonlightAmbientColor;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonDiffuse() const
{
    return  moonlightDiffuseColor;
}

const XMFLOAT4& SimulationConfigLoader::GetMoonSpecular() const
{
    return  moonlightSpecularColor;
}

const float SimulationConfigLoader::GetMoonSpecularPower() const
{
    return  moonlightSpecularIntensity;
}

const XMFLOAT3& SimulationConfigLoader::GetLaunchPadScale() const
{
    return  launchPadScale;
}

const XMFLOAT4& SimulationConfigLoader::GetLaunchPadTessellationValues() const
{
    return  launchPadTessellationSettings;
}

const XMFLOAT4& SimulationConfigLoader::GetLaunchPadDisplacementValues() const
{
    return  launchPadDisplacementSettings;
}
