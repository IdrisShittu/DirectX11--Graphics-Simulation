#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class ParticleShader : public Shader
{
public:
	ParticleShader(ID3D11Device* const device, HWND const hwnd); // Default Constructor
	void CreateBuffer(ID3D11Device* device, UINT byteWidth, D3D11_BIND_FLAG bindFlags, ID3D11Buffer** buffer);
	ParticleShader(const ParticleShader& other); // Copy Constructor
	ParticleShader(ParticleShader&& other) noexcept; // Move Constructor
	~ParticleShader() override; // Destructor

	ParticleShader& operator = (const ParticleShader& other); // Copy Assignment Operator
	ParticleShader& operator = (ParticleShader&& other) noexcept; // Move Assignment Operator

	void SetParticleParameters(const XMFLOAT3& colourTint, const float transparency);

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition);

	bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) override;

private:
	bool SetParticleShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition);
	void RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const;

	struct InverseViewBuffer {
		XMMATRIX inverseViewMatrix;
	};

	struct ParticleParametersBuffer
	{
		XMFLOAT3 colourTint;
		float transparency;
	};

	float transparency;

	XMFLOAT3 colourTint;

	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampleState;

	ID3D11Buffer* inverseViewMatrixBuffer;
	ID3D11Buffer* particleParametersBuffer;
};

