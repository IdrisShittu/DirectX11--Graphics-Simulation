#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <atlbase.h>
#include <string>
#include <vector>
#include "Light.h"

const int MAX_LIGHTS = 16;

using namespace DirectX;
using namespace std;

class Shader
{
public:
	Shader(const string& vertexShaderFileName, const string& hullShaderFileName, const string& domainShaderFileName, const string& pixelShaderFileName, ID3D11Device* const device, HWND const hwnd);
	Shader(const Shader& other); 
	Shader(Shader&& other) noexcept; 
	virtual ~Shader();
	Shader& operator = (const Shader& other); 
	Shader& operator = (Shader&& other) noexcept; 
	bool GetInitializationState() const;
	int GetVertexBufferResourceCount() const;
	int GetHullBufferResourceCount() const;
	int GetDomainBufferResourceCount() const;
	int GetPixelBufferResourceCount() const;
	void IncrementVertexBufferResourceCount();
	void IncrementHullBufferResourceCount();
	void IncrementDomainBufferResourceCount();
	void IncrementPixelBufferResourceCount();
	ID3D10Blob* GetVertexShaderBuffer() const;
	ID3D11Buffer* GetMatrixBuffer() const;
	ID3D11Buffer* GetCameraBuffer() const;
	const D3D11_MAPPED_SUBRESOURCE& GetMappedSubResource() const;
	void SetRenderModeStates(const int nonTextured, const int texturedDiffuse, const int displacementEnabled);
	void GetTessellationVariables(float& maxTessellationDistance, float& minTessellationDistance, float& maxTessellationFactor, float& minTessellationFactor) const;
	void SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor);
	void GetDisplacementVariables(float& mipInterval, float& mipClampMinimum, float& mipClampMaximum, float& displacementPower) const;
	void SetDisplacementVariables(const float& mipInterval, const float& mipClampMinimum, const float& mipClampMaximum, const float& displacementPower);
	void SetInitializationState(const bool state);
	void SetVertexShaderBuffer(ID3D10Blob* const vertexShaderBuffer);
	virtual bool Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) = 0;

protected:

	bool SetShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition);
	bool UpdateMatrixBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	bool UpdateTessellationBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, float maxTessDistance, float minTessDistance, float maxTessFactor, float minTessFactor);
	bool UpdateDisplacementBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, float mipInterval, float mipClampMinimum, float mipClampMaximum, float displacementPower, bool displacementRenderMode);
	bool UpdateRenderModeBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, bool nonTextureRenderMode, bool textureDiffuseRenderMode);
	bool UpdateCameraBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, XMFLOAT3 cameraPosition);
	void SetShader(ID3D11DeviceContext* const deviceContext) const;
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND const hwnd, const LPCSTR& shaderFileName) const;

private:

	struct MatrixBufferType {
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};
	struct TessellationBufferType
	{
		float maxTessellationDistance;
		float minTessellationDistance;
		float maxTessellationFactor;
		float minTessellationFactor;
	};
	struct DisplacementBuffer
	{
		float mipInterval;
		float mipMinimum;
		float mipMaximum;
		float displacementPower;
		int displacementEnabled;
		XMFLOAT3 padding;
	};
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
	struct RenderModeBufferType
	{
		int nonTexture;
		int textureDiffuse;
		XMFLOAT2 padding;
	};

	bool initFailed;

	int vertexBufferResourceCount;
	int hullBufferResourceCount;
	int domainBufferResourceCount;
	int pixelBufferResourceCount;
	int nonTextureRenderMode;
	int textureDiffuseRenderMode;
	int displacementRenderMode;

	float maxTessellationDistance;
	float minTessellationDistance;
	float maxTessellationFactor;
	float minTessellationFactor;
	float mipInterval;
	float mipClampMinimum;
	float mipClampMaximum;
	float displacementPower;

	ID3D10Blob* vertexShaderBuffer;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11VertexShader* vertexShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11PixelShader* pixelShader;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* displacementBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* renderModeBuffer;
};

