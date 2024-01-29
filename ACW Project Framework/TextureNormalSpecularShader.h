#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class TextureNormalSpecularShader : public Shader
{
public:
	TextureNormalSpecularShader(ID3D11Device* const device, HWND const hwnd); // Default Constructor
	TextureNormalSpecularShader(const TextureNormalSpecularShader& other); // Copy Constructor
	TextureNormalSpecularShader(TextureNormalSpecularShader&& other) noexcept; // Move Constructor
	~TextureNormalSpecularShader() override;

	TextureNormalSpecularShader& operator = (const TextureNormalSpecularShader& other); // Copy Assignment Operator
	TextureNormalSpecularShader& operator = (TextureNormalSpecularShader&& other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) override;

private:
	bool SetTextureNormalShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition);
	void RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const;


	struct PointLightMatrix
	{
		XMMATRIX lightViewMatrix;
		XMMATRIX lightProjectionMatrix;
	};

	struct LightMatrixBufferType
	{
		PointLightMatrix lights[MAX_LIGHTS];
		int lightCount;

		XMFLOAT3 padding;
	};

	struct PointLights
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT4 specularColour;
		XMFLOAT3 lightPositions;
		float specularPower;
		int isDirectionalLight;
		XMFLOAT3 padding;
	};

	struct LightBufferType {
		PointLights lights[MAX_LIGHTS];
		int lightCount;
		XMFLOAT3 padding;
	};

	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampleStateWrap;
	ID3D11SamplerState* sampleStateClamp;
	ID3D11Buffer* lightMatrixBuffer;
	ID3D11Buffer* lightBuffer;
};

