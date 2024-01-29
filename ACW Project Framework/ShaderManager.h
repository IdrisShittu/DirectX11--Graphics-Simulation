#pragma once

#include <memory>

//Include all shader header files
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureDisplacement.h"
#include "TextureCubeShader.h"
#include "ReflectionShader.h"
#include "Texture2DShader.h"
#include "TextureNormalMappingShader.h"
#include "TextureNormalSpecularShader.h"
#include "TextureDisplacement.h"
#include "DepthShader.h"

class ShaderManager
{
public:

	ShaderManager(ID3D11Device* const device, HWND const hwnd); //Default Constructor
	ShaderManager(const ShaderManager& other); //Copy Constructor
	ShaderManager(ShaderManager&& other) noexcept; //Move Constructor
	~ShaderManager(); //Destructor

	ShaderManager& operator = (const ShaderManager& other); //Copy Assignment Operator
	ShaderManager& operator = (ShaderManager&& other) noexcept; //Move Assignment Operator

	const shared_ptr<Shader>& GetColourShader() const;
	const shared_ptr<Shader>& GetLightShader() const;
	const shared_ptr<Shader>& GetTexture2DShader() const;
	const shared_ptr<Shader>& GetTextureCubeShader() const;
	const shared_ptr<Shader>& GetTextureNormalShader() const;
	const shared_ptr<Shader>& GetTextureNormalSpecularShader() const;
	const shared_ptr<Shader>& GetTextureDisplacementShader() const;
	const shared_ptr<Shader>& GetDepthShader() const;

	bool GetInitializationState() const;

private:

	bool initFailed;

	shared_ptr<Shader> colourShader;
	shared_ptr<Shader> lightShader;
	shared_ptr<Shader> texture2DShader;
	shared_ptr<Shader> textureCubeShader;
	shared_ptr<Shader> textureNormalShader;
	shared_ptr<Shader> textureNormalSpecularShader;
	shared_ptr<Shader> textureDisplacementShader;
	shared_ptr<Shader> depthShader;
};

