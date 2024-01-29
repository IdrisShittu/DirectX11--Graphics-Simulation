#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "GameObject.h"
#include "TextureRenderer.h"

using namespace std;

class ShadowMapManager
{
public:
	ShadowMapManager(HWND const hwnd, ID3D11Device* const device, const shared_ptr<Shader>& depthShader, const int lightCount, const int shadowMapWidth, const int shadowMapHeight);
	ShadowMapManager(const ShadowMapManager& other); // Copy Constructor
	ShadowMapManager(ShadowMapManager&& other) noexcept; // Move Constructor
	~ShadowMapManager();

	ShadowMapManager& operator = (const ShadowMapManager& other); // Copy Assignment Operator
	ShadowMapManager& operator = (ShadowMapManager&& other) noexcept; // Move Assignment Operator

	void AddShadowMap(ID3D11Device* const device, const int shadowMapWidth, const int shadowMapHeight);

	bool GenerateShadowMapResources(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition);

	const vector<ID3D11ShaderResourceView*>& GetShadowMapResources() const;

	bool GetInitializationState() const;

private:

	bool initFailed;

	//Depth shader
	shared_ptr<Shader> depthShader;

	//List of renderToTexture instances
	vector<shared_ptr<TextureRenderer>> renderToTextures;

	//List of shader resource views, don't have to delete as these exist within renderToTexture so they will dispose of the resource
	vector<ID3D11ShaderResourceView*> shadowMapShaderResources;
};

