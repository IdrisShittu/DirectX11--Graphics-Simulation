#include "TextureDisplacement.h"
#include <algorithm>

TextureDisplacement::TextureDisplacement(ID3D11Device* const device, HWND const hwnd) :
	Shader("TextureDisplacementVS", "TextureDisplacementHS", "TextureDisplacementDS", "TextureDisplacementPS", device, hwnd),
	inputLayout(nullptr), sampleStateWrap(nullptr), sampleStateClamp(nullptr), lightBuffer(nullptr), lightMatrixBuffer(nullptr) {

	if (GetInitializationState()) return;

	D3D11_INPUT_ELEMENT_DESC layout[9] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	HRESULT result = device->CreateInputLayout(layout, _countof(layout), GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);
	GetVertexShaderBuffer()->Release();
	if (FAILED(result)) { SetInitializationState(true); return; }

	D3D11_SAMPLER_DESC samplerWrapDesc = {
		D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f, 1, D3D11_COMPARISON_ALWAYS, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
	};
	result = device->CreateSamplerState(&samplerWrapDesc, &sampleStateWrap);
	if (FAILED(result)) { SetInitializationState(true); return; }

	D3D11_SAMPLER_DESC samplerClampDesc = {
		D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f, 1, D3D11_COMPARISON_ALWAYS, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
	};
	result = device->CreateSamplerState(&samplerClampDesc, &sampleStateClamp);
	if (FAILED(result)) { SetInitializationState(true); return; }

	D3D11_BUFFER_DESC lightMatrixBufferDesc = {
		sizeof(LightMatrixBufferType), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0
	};
	result = device->CreateBuffer(&lightMatrixBufferDesc, nullptr, &lightMatrixBuffer);
	if (FAILED(result)) { SetInitializationState(true); return; }

	D3D11_BUFFER_DESC lightBufferDesc = {
		sizeof(LightBufferType), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0
	};
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);
	if (FAILED(result)) { SetInitializationState(true); return; }

}
TextureDisplacement::TextureDisplacement(const TextureDisplacement& other) = default;
TextureDisplacement::TextureDisplacement(TextureDisplacement && other) noexcept = default;
TextureDisplacement::~TextureDisplacement()
{
	try
	{
		if (lightBuffer) { lightBuffer->Release(); lightBuffer = nullptr; }
		if (lightMatrixBuffer) { lightMatrixBuffer->Release(); lightMatrixBuffer = nullptr; }
		if (sampleStateClamp) { sampleStateClamp->Release(); sampleStateClamp = nullptr; }
		if (sampleStateWrap) { sampleStateWrap->Release(); sampleStateWrap = nullptr; }
		if (inputLayout) { inputLayout->Release(); inputLayout = nullptr; }
	}
	catch (exception&) {}
}
TextureDisplacement& TextureDisplacement::operator=(const TextureDisplacement & other) = default;
TextureDisplacement& TextureDisplacement::operator=(TextureDisplacement && other) noexcept = default;

bool TextureDisplacement::Render(ID3D11DeviceContext* const deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	SetTextureDisplacementShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, depthTextures, pointLightList, cameraPosition);
	RenderShader(deviceContext, indexCount, instanceCount);
	return true;
}

bool TextureDisplacement::SetTextureDisplacementShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
	SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
	deviceContext->DSSetShaderResources(0, 1, &textures.back());
	
	vector<ID3D11ShaderResourceView*> pixelShaderTextureArray;
	for (auto it = textures.begin(); it != textures.end() - 1; ++it)pixelShaderTextureArray.push_back(*it);
	deviceContext->PSSetShaderResources(0, pixelShaderTextureArray.size(), pixelShaderTextureArray.data());

	vector<ID3D11ShaderResourceView*> depthTextureArray(depthTextures.begin(), depthTextures.end());
	deviceContext->PSSetShaderResources(3, depthTextureArray.size(), depthTextureArray.data());

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (FAILED(deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))return false;
	
	auto* lightBufferDataPointer = static_cast<LightBufferType*>(mappedResource.pData);
	for (unsigned int i = 0; i < pointLightList.size(); i++)
	{
		lightBufferDataPointer->lights[i].ambientColour = pointLightList[i]->GetAmbientColour();
		lightBufferDataPointer->lights[i].diffuseColour = pointLightList[i]->GetDiffuseColour();
		lightBufferDataPointer->lights[i].specularColour = pointLightList[i]->GetSpecularColour();
		lightBufferDataPointer->lights[i].lightPositions = pointLightList[i]->GetLightPosition();
		lightBufferDataPointer->lights[i].specularPower = pointLightList[i]->GetSpecularPower();
		lightBufferDataPointer->lights[i].isDirectionalLight = pointLightList[i]->GetIsDirectionalLight();
		lightBufferDataPointer->lights[i].padding = XMFLOAT3();
	}

	lightBufferDataPointer->lightCount = pointLightList.size();
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &lightBuffer);
	IncrementPixelBufferResourceCount();

	if (FAILED(deviceContext->Map(lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))return false;
	auto* lightMatrixBufferDataPointer = static_cast<LightMatrixBufferType*>(mappedResource.pData);
	for (unsigned int i = 0; i < pointLightList.size(); i++)
	{
		lightMatrixBufferDataPointer->lights[i].lightViewMatrix = XMMatrixTranspose(pointLightList[i]->GetLightViewMatrix());
		lightMatrixBufferDataPointer->lights[i].lightProjectionMatrix = XMMatrixTranspose(pointLightList[i]->GetLightProjectionMatrix());
	}
	deviceContext->Unmap(lightMatrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &lightMatrixBuffer);
	IncrementDomainBufferResourceCount();

	const auto cameraBuffer = GetCameraBuffer();
	deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &cameraBuffer);
	IncrementDomainBufferResourceCount();

	return true;
}

void TextureDisplacement::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount) const {
	deviceContext->IASetInputLayout(inputLayout);
	SetShader(deviceContext);
	deviceContext->DSSetSamplers(0, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(0, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &sampleStateClamp);
	deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}