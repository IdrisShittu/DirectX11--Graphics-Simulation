#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class LightShader : public Shader
{
public:
	LightShader(ID3D11Device* const device, HWND const hwnd); // Default Constructor
	LightShader(const LightShader& other); // Copy Constructor
	LightShader(LightShader&& other) noexcept; // Move Constructor
	~LightShader() override; // Destructor

	LightShader& operator = (const LightShader& other); // Copy Assignment Operator
	LightShader& operator = (LightShader&& other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) override;

private: 
	bool SetLightShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition);
	void RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const;

	struct PointLights
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT4 lightPositions;
	};

	struct LightBufferType {
		PointLights lights[MAX_LIGHTS];
		int lightCount;
		XMFLOAT3 padding;
	};

	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};