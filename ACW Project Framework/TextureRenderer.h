#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "GameObject.h"

using namespace std;
using namespace DirectX;

class TextureRenderer
{
public:
	TextureRenderer(ID3D11Device* const device, const int textureWidth, const int textureHeight);
	TextureRenderer(const TextureRenderer& other); // Copy Constructor
	TextureRenderer(TextureRenderer&& other) noexcept; // Move Constructor
	~TextureRenderer(); 

	TextureRenderer& operator = (const TextureRenderer& other); // Copy Assignment Operator
	TextureRenderer& operator = (TextureRenderer&& other) noexcept; // Move Assignment Operator

	void SetShader(const shared_ptr<Shader>& shader);
	bool RenderObjectsToTexture(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<shared_ptr<Light>>& pointLightList, const vector<shared_ptr<GameObject>>& gameObjects, const XMFLOAT3& cameraPosition) const;

	ID3D11ShaderResourceView* GetShaderResourceView() const;

	bool GetInitializationState() const;

	bool InitializeTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight);

	bool InitializeRenderTargetView(ID3D11Device* const device);

	bool InitializeShaderResourceView(ID3D11Device* const device);

	void ReleaseResources();

private:
	void SetRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView) const;
	void ClearRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4& RGBA) const;

	void SwapShaderAndApplyVariables(const shared_ptr<GameObject>& gameObject) const;

	bool initFailed;

	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;

	mutable shared_ptr<Shader> shader;

};

