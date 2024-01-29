#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class DepthShader : public Shader
{
public:
	DepthShader(ID3D11Device* const device, HWND const hwnd);
	DepthShader(const DepthShader& other); // Copy Constructor
	DepthShader(DepthShader&& other) noexcept; // Move Constructor
	~DepthShader() override; // Destructor

	DepthShader& operator = (const DepthShader& other); // Copy Assignment Operator
	DepthShader& operator = (DepthShader&& other) noexcept; //Move Assignment Operator

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) override;

private:
	bool SetDepthShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition);
	void RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const;

	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampleStateWrap;

};

