#include "ParticleShader.h"

ParticleShader::ParticleShader(ID3D11Device* const device, HWND const hwnd) : Shader("ParticleVertexShader", "ParticleHullShader", "ParticleDomainShader", "ParticlePixelShader", device, hwnd), transparency(0.0f), colourTint(0.0f, 0.0f, 0.0f), inputLayout(nullptr), sampleState(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC layout[6];

	unsigned int numberOfElements = 0;

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

	layout[2].SemanticName = "INSTANCEMATRIX";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[2].InputSlot = 1;
	layout[2].AlignedByteOffset = 0;
	layout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[2].InstanceDataStepRate = 1;

	layout[3].SemanticName = "INSTANCEMATRIX";
	layout[3].SemanticIndex = 1;
	layout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[3].InputSlot = 1;
	layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[3].InstanceDataStepRate = 1;

	layout[4].SemanticName = "INSTANCEMATRIX";
	layout[4].SemanticIndex = 2;
	layout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[4].InputSlot = 1;
	layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[4].InstanceDataStepRate = 1;

	layout[5].SemanticName = "INSTANCEMATRIX";
	layout[5].SemanticIndex = 3;
	layout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[5].InputSlot = 1;
	layout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	layout[5].InstanceDataStepRate = 1;

	numberOfElements = sizeof(layout) / sizeof(layout[0]);

	auto result = device->CreateInputLayout(layout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

	GetVertexShaderBuffer()->Release();
	SetVertexShaderBuffer(nullptr);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_SAMPLER_DESC samplerDescription;

	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.MipLODBias = 0.0f;
	samplerDescription.MaxAnisotropy = 1;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
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
		return;
	}

	D3D11_BUFFER_DESC inverseViewMatrixBufferDescription;

	inverseViewMatrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	inverseViewMatrixBufferDescription.ByteWidth = sizeof(InverseViewBuffer);
	inverseViewMatrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	inverseViewMatrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	inverseViewMatrixBufferDescription.MiscFlags = 0;
	inverseViewMatrixBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&inverseViewMatrixBufferDescription, nullptr, &inverseViewMatrixBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}

	D3D11_BUFFER_DESC particleParametersBufferDescription;

	particleParametersBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	particleParametersBufferDescription.ByteWidth = sizeof(ParticleParametersBuffer);
	particleParametersBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	particleParametersBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	particleParametersBufferDescription.MiscFlags = 0;
	particleParametersBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&particleParametersBufferDescription, nullptr, &particleParametersBuffer);

	if (FAILED(result))
	{
		SetInitializationState(true);
		return;
	}
}

ParticleShader::ParticleShader(const ParticleShader& other) = default;

ParticleShader::ParticleShader(ParticleShader&& other) noexcept = default;

ParticleShader::~ParticleShader()
{
	try
	{
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

void ParticleShader::SetParticleParameters(const XMFLOAT3& clTint, const float tr)
{
	colourTint = clTint;
	transparency = tr;
}

bool ParticleShader::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	SetParticleShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);
	RenderShader(deviceContext, indexCount, instanceCount);
	return true;
}

bool ParticleShader::SetParticleShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition)
{
	SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
	ID3D11ShaderResourceView* textureArray[1];
	copy(textures.begin(), textures.end(), textureArray);
	deviceContext->PSSetShaderResources(0, static_cast<UINT>(textures.size()), textureArray);
	auto mappedResource = GetMappedSubResource();
	auto failed = deviceContext->Map(inverseViewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto* inverseViewMatrixBufferDataPointer = static_cast<InverseViewBuffer*>(mappedResource.pData);
	const auto inverseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	inverseViewMatrixBufferDataPointer->inverseViewMatrix = XMMatrixTranspose(inverseViewMatrix);
	deviceContext->Unmap(inverseViewMatrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(GetVertexBufferResourceCount(), 1, &inverseViewMatrixBuffer);
	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &inverseViewMatrixBuffer);
	IncrementVertexBufferResourceCount();
	IncrementDomainBufferResourceCount();
	failed = deviceContext->Map(particleParametersBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto* pPBufferDataPointer = static_cast<ParticleParametersBuffer*>(mappedResource.pData);
	pPBufferDataPointer->colourTint = colourTint;
	pPBufferDataPointer->transparency = transparency;
	deviceContext->Unmap(particleParametersBuffer, 0);
	deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &particleParametersBuffer);
	IncrementPixelBufferResourceCount();
	return true;
}

void ParticleShader::RenderShader(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount) const
{
	deviceContext->IASetInputLayout(inputLayout);
	SetShader(deviceContext);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
