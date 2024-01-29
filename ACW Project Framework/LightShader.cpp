#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, HWND hwnd)
    : Shader("LightVertexShader", "LightHullShader", "LightDomainShader", "LightPixelShader", device, hwnd),
    inputLayout(nullptr), sampleState(nullptr), lightBuffer(nullptr)
{
    if (GetInitializationState()) return;

    D3D11_INPUT_ELEMENT_DESC layout[7] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INSTANCEMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };

    const unsigned int numberOfElements = sizeof(layout) / sizeof(layout[0]);

    auto result = device->CreateInputLayout(layout, numberOfElements, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

    if (FAILED(result) || (GetVertexShaderBuffer()->Release(), SetVertexShaderBuffer(nullptr), false))
    {
        SetInitializationState(true);
        return;
    }

    D3D11_SAMPLER_DESC samplerDescription = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, 1, D3D11_COMPARISON_ALWAYS,
        { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
    };

    result = device->CreateSamplerState(&samplerDescription, &sampleState);

    if (FAILED(result))
    {
        SetInitializationState(true);
        return;
    }
}

LightShader::~LightShader()
{
    try
    {
        if (lightBuffer) { lightBuffer->Release(); lightBuffer = nullptr; }
        if (sampleState) { sampleState->Release(); sampleState = nullptr; }
        if (inputLayout) { inputLayout->Release(); inputLayout = nullptr; }
    }
    catch (exception&) {}
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
    auto const result = SetLightShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, pointLightList, cameraPosition);

    if (!result) return false;

    RenderShader(deviceContext, indexCount, instanceCount);
    return true;
}

bool LightShader::SetLightShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition)
{
    const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);

    if (!result) return false;

    ID3D11ShaderResourceView* textureArray[1];

    copy(textures.begin(), textures.begin() + 1, textureArray);

    deviceContext->PSSetShaderResources(0, 1, textureArray);

    auto mappedResource = GetMappedSubResource();

    const auto failed = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(failed)) return false;

    auto* lightBufferDataPointer = static_cast<LightBufferType*>(mappedResource.pData);

    for (unsigned int i = 0; i < pointLightList.size(); i++)
    {
        lightBufferDataPointer->lights[i].ambientColour = pointLightList[i]->GetAmbientColour();
        lightBufferDataPointer->lights[i].diffuseColour = pointLightList[i]->GetDiffuseColour();

        const auto lightPos = pointLightList[i]->GetLightPosition();
        lightBufferDataPointer->lights[i].lightPositions = XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, 0.0f);
    }

    lightBufferDataPointer->lightCount = pointLightList.size();
    lightBufferDataPointer->padding = XMFLOAT3();
    lightBufferDataPointer = nullptr;

    deviceContext->Unmap(lightBuffer, 0);
    deviceContext->PSSetConstantBuffers(GetPixelBufferResourceCount(), 1, &lightBuffer);
    IncrementPixelBufferResourceCount();
    return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount) const
{
    deviceContext->IASetInputLayout(inputLayout);
    SetShader(deviceContext);
    deviceContext->PSSetSamplers(0, 1, &sampleState);
    deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
