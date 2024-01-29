#include "ShadowMapManager.h"

ShadowMapManager::ShadowMapManager(HWND hwnd, ID3D11Device* device, const shared_ptr<Shader>& depthShadr, int lightCount, int shadowMapWidth, int shadowMapHeight)
    : initFailed(false), depthShader(depthShadr), renderToTextures(lightCount)
{
    for (auto& textureRenderer : renderToTextures) {
        textureRenderer = make_shared<TextureRenderer>(device, shadowMapWidth, shadowMapHeight);
        if (!textureRenderer || textureRenderer->GetInitializationState()) {
            MessageBox(hwnd, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
            initFailed = true;
            return;
        }
        textureRenderer->SetShader(depthShader);
    }
}

ShadowMapManager::ShadowMapManager(const ShadowMapManager& other) = default;
ShadowMapManager::ShadowMapManager(ShadowMapManager && other) noexcept = default;
ShadowMapManager::~ShadowMapManager() = default;
ShadowMapManager& ShadowMapManager::operator=(const ShadowMapManager & other) = default;
ShadowMapManager& ShadowMapManager::operator=(ShadowMapManager && other) noexcept = default;

void ShadowMapManager::AddShadowMap(ID3D11Device * device, int shadowMapWidth, int shadowMapHeight)
{
    auto textureRenderer = make_shared<TextureRenderer>(device, shadowMapWidth, shadowMapHeight);
    if (!textureRenderer || textureRenderer->GetInitializationState()) {
        MessageBox(nullptr, "Failed to initialize a render to texture in ShadowMapManager", "Error", MB_OK);
        initFailed = true;
        return;
    }
    textureRenderer->SetShader(depthShader);
    renderToTextures.push_back(textureRenderer);
}

bool ShadowMapManager::GenerateShadowMapResources(ID3D11DeviceContext * deviceContext, ID3D11DepthStencilView * depthStencilView, const vector<shared_ptr<Light>>&pointLightList, const vector<shared_ptr<GameObject>>&gameObjects, const XMFLOAT3 & cameraPosition)
{
    shadowMapShaderResources.clear();
    for (unsigned int i = 0; i < renderToTextures.size(); i++) {
        if (!renderToTextures[i]->RenderObjectsToTexture(deviceContext, depthStencilView, pointLightList[i]->GetLightViewMatrix(), pointLightList[i]->GetLightProjectionMatrix(), pointLightList, gameObjects, cameraPosition)) {
            return false;
        }
        shadowMapShaderResources.push_back(renderToTextures[i]->GetShaderResourceView());
    }
    return true;
}

const vector<ID3D11ShaderResourceView*>& ShadowMapManager::GetShadowMapResources() const
{
    return shadowMapShaderResources;
}

bool ShadowMapManager::GetInitializationState() const
{
    return initFailed;
}
