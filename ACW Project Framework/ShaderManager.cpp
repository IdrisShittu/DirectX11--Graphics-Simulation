#include "ShaderManager.h"

ShaderManager::ShaderManager(ID3D11Device* const device, HWND const hwnd)
    : initFailed(false),
    colourShader(nullptr),
    lightShader(make_shared<LightShader>(device, hwnd)),
    texture2DShader(make_shared<Texture2DShader>(device, hwnd)),
    textureCubeShader(make_shared<TextureCubeShader>(device, hwnd)),
    textureNormalShader(make_shared<TextureNormalMappingShader>(device, hwnd)),
    textureNormalSpecularShader(make_shared<TextureNormalSpecularShader>(device, hwnd)),
    textureDisplacementShader(make_shared<TextureDisplacement>(device, hwnd)),
    depthShader(make_shared<DepthShader>(device, hwnd)) {

    initFailed = !(lightShader && !lightShader->GetInitializationState()) ||
        !(texture2DShader && !texture2DShader->GetInitializationState()) ||
        !(textureCubeShader && !textureCubeShader->GetInitializationState()) ||
        !(textureNormalShader && !textureNormalShader->GetInitializationState()) ||
        !(textureNormalSpecularShader && !textureNormalSpecularShader->GetInitializationState()) ||
        !(textureDisplacementShader && !textureDisplacementShader->GetInitializationState()) ||
        !(depthShader && !depthShader->GetInitializationState());
}

ShaderManager::ShaderManager(const ShaderManager& other) = default;
ShaderManager::ShaderManager(ShaderManager && other) noexcept = default;
ShaderManager::~ShaderManager() = default;
ShaderManager& ShaderManager::operator=(const ShaderManager & other) = default;
ShaderManager& ShaderManager::operator=(ShaderManager && other) noexcept = default;

const shared_ptr<Shader>& ShaderManager::GetColourShader() const { return colourShader; }
const shared_ptr<Shader>& ShaderManager::GetLightShader() const { return lightShader; }
const shared_ptr<Shader>& ShaderManager::GetTexture2DShader() const { return texture2DShader; }
const shared_ptr<Shader>& ShaderManager::GetTextureCubeShader() const { return textureCubeShader; }
const shared_ptr<Shader>& ShaderManager::GetTextureNormalShader() const { return textureNormalShader; }
const shared_ptr<Shader>& ShaderManager::GetTextureNormalSpecularShader() const { return textureNormalSpecularShader; }
const shared_ptr<Shader>& ShaderManager::GetTextureDisplacementShader() const { return textureDisplacementShader; }
const shared_ptr<Shader>& ShaderManager::GetDepthShader() const { return depthShader; }

bool ShaderManager::GetInitializationState() const { return initFailed; }
