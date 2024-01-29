#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class Texture2DShader : public Shader
{
public:
	Texture2DShader(ID3D11Device* const device, HWND const hwnd); // Default Constructor
	Texture2DShader(const Texture2DShader& other); // Copy Constructor
	Texture2DShader(Texture2DShader&& other) noexcept; // Move Constructor
	~Texture2DShader() override; // Destructor

	Texture2DShader& operator = (const Texture2DShader& other); // Copy Assignment Operator
	Texture2DShader& operator = (Texture2DShader&& other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) override;

private:
	bool SetTextureShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition);
	void RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const;

	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampleState;
};