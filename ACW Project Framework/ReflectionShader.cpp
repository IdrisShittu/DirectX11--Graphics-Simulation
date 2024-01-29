#include "ReflectionShader.h"

ReflectionShader::ReflectionShader(ID3D11Device* const device, HWND const hwnd) : Shader("ReflectionVertexShader", "ReflectionHullShader", "ReflectionDomainShader", "ReflectionPixelShader", device, hwnd), inputLayout(nullptr), sampleState(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC layout[7];

	unsigned int numberOfElements = 0;

	//Setup layout of buffer data in the shader
	//Setup of the layout needs to match the struct in our Model class and the struct in the shader

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "TEXCOORD";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	layout[2].SemanticName = "NORMAL";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;

	layout[3].SemanticName = "INSTANCEMATRIX";
	layout[3].SemanticIndex = 0;
	layout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[3].InputSlot = 1;
	layout[3].AlignedByteOffset = 0;
	layout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[3].InstanceDataStepRate = 1;

	layout[4].SemanticName = "INSTANCEMATRIX";
	layout[4].SemanticIndex = 1;
	layout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[4].InputSlot = 1;
	layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[4].InstanceDataStepRate = 1;

	layout[5].SemanticName = "INSTANCEMATRIX";
	layout[5].SemanticIndex = 2;
	layout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[5].InputSlot = 1;
	layout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[5].InstanceDataStepRate = 1;

	layout[6].SemanticName = "INSTANCEMATRIX";
	layout[6].SemanticIndex = 3;
	layout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[6].InputSlot = 1;
	layout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[6].InstanceDataStepRate = 1;

	//Get count of elements in layout
	numberOfElements = sizeof(layout) / sizeof(layout[0]);

	//Create vertex input layout
	auto result = device->CreateInputLayout(layout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_SAMPLER_DESC samplerDescription;

	//Create our texture sampler state description
	//Filter determines which pixels will be used/combined for the final look of the texture on the polygon face
	//The filter used is costly in performance but best in visual looks
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDescription.MipLODBias = 0.0f;
	samplerDescription.MaxAnisotropy = 1;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDescription.BorderColor[0] = 0.0f;
	samplerDescription.BorderColor[1] = 0.0f;
	samplerDescription.BorderColor[2] = 0.0f;
	samplerDescription.BorderColor[3] = 0.0f;
	samplerDescription.MinLOD = 0.0f;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDescription, &sampleState);

	if (FAILED(result))
	{
		SetInitializationState(true);
	}
}

ReflectionShader::ReflectionShader(const ReflectionShader& other) = default;

ReflectionShader::ReflectionShader(ReflectionShader&& other) noexcept = default;

ReflectionShader::~ReflectionShader()
{
	try
	{
		//Release resources
		if (sampleState)
		{
			sampleState->Release();
			sampleState = nullptr;
		}

		if (inputLayout)
		{
			inputLayout->Release();
			inputLayout = nullptr;
		}
	}
	catch (exception& e)
	{

	}
}

ReflectionShader& ReflectionShader::operator=(const ReflectionShader& other) = default;

ReflectionShader& ReflectionShader::operator=(ReflectionShader&& other) noexcept = default;

bool ReflectionShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	const auto result = SetReflectionShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount, instanceCount);

	return true;
}

bool ReflectionShader::SetReflectionShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition)
{
	const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

	if (!result)
	{
		return false;
	}

	//Hopefully this works, doubt there is going to ever be more than 10 textures
	ID3D11ShaderResourceView* textureArray[1];

	copy(textures.begin(), textures.begin() + 1, textureArray);

	//Set the texture resource to the pixel shader
	deviceContext->PSSetShaderResources(0, 1, textureArray);

	const auto cameraBuffer = GetCameraBuffer();

	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &cameraBuffer);

	IncrementDomainBufferResourceCount();

	return true;
}

void ReflectionShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	//Set input layout
	deviceContext->IASetInputLayout(inputLayout);

	//Set our shaders
	SetShader(deviceContext);

	//Set pixel shaders sampler state
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	//Render triangle
	//deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}