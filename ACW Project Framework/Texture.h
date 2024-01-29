#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include "ResourceManager.h"

using namespace DirectX;
class Texture
{
public:
	Texture(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager); // Default Constructor
	Texture(const Texture& other); 
	Texture(Texture&& other) noexcept;
	~Texture(); 
	Texture& operator = (const Texture& other);
	Texture& operator = (Texture&& other) noexcept;
	const vector<ID3D11ShaderResourceView*>& GetTextureList() const;
	bool GetInitializationState() const;

private:
	vector<ID3D11ShaderResourceView*> texture;
	bool initFailed;
};