#include "LightManager.h"

LightManager::LightManager() = default;
LightManager::LightManager(const LightManager & other) = default;
LightManager::LightManager(LightManager && other) noexcept = default;
LightManager::~LightManager() = default;
LightManager& LightManager::operator=(const LightManager & other) = default;
LightManager& LightManager::operator=(LightManager && other) noexcept = default;

void LightManager::AddLight(const XMFLOAT3 & lightPosition, const XMFLOAT3 & lightPointPosition,
    const XMFLOAT4 & ambientColour, const XMFLOAT4 & diffuseColour,
    const XMFLOAT4 & specularColour, float specularPower,
    float projectionScreenWidth, float projectionScreenHeight,
    float screenNear, float screenDepth, bool orbit, bool directionalLight) {
    auto light = make_shared<Light>();
    light->SetLightOrbit(orbit);
    light->SetDirectionalLight(directionalLight);
    light->SetLightPosition(lightPosition);
    light->SetLightPointPosition(lightPointPosition);
    light->SetAmbientColour(ambientColour);
    light->SetDiffuseColour(diffuseColour);
    light->SetSpecularColour(specularColour);
    light->SetSpecularPower(specularPower);
    light->GenerateLightProjectionMatrix(projectionScreenWidth, projectionScreenHeight, screenNear, screenDepth);
    light->SetLightOrbitDirection();
    light->UpdateLightVariables(0.0f);
    lights.push_back(std::move(light));
}

const vector<shared_ptr<Light>>& LightManager::GetLightList() const {
    return lights;
}
