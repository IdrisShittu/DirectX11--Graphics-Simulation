#include "Texture.h"

Texture::Texture(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager)
    : texture(), initFailed(false)
{
    texture.reserve(textureFileNames.size()); 
    for (const auto& fileName : textureFileNames)
    {
        ID3D11ShaderResourceView* tex = nullptr;
        if (!resourceManager->GetTexture(device, fileName, tex))
        {
            initFailed = true;
            break; 
        }
        texture.push_back(tex);
    }
}

Texture::Texture(const Texture& other) = default;
Texture::Texture(Texture && other) noexcept = default;
Texture& Texture::operator=(const Texture & other) = default;
Texture& Texture::operator=(Texture && other) noexcept = default;

Texture::~Texture()
{
    for (auto& tex : texture)if (tex)tex = nullptr;
}

const vector<ID3D11ShaderResourceView*>& Texture::GetTextureList() const
{
    return texture;
}

bool Texture::GetInitializationState() const
{
    return initFailed;
}
