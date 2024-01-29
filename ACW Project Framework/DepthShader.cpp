#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd)
    : Shader("DepthVertexShader", "DepthHullShader", "DepthDomainShader", "DepthPixelShader", device, hwnd), inputLayout(nullptr)
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

    auto result = device->CreateInputLayout(layout, 7, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);

    if (FAILED(result) || (GetVertexShaderBuffer()->Release(), SetVertexShaderBuffer(nullptr), false))
    {
        SetInitializationState(true);
        return;
    }

    D3D11_SAMPLER_DESC samplerWrapDescription = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, 0.0f, 1, D3D11_COMPARISON_ALWAYS,
        { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
    };

    result = device->CreateSamplerState(&samplerWrapDescription, &sampleStateWrap);

    if (FAILED(result))
    {
        SetInitializationState(true);
        return;
    }
}

DepthShader::DepthShader(const DepthShader& other) = default;

DepthShader::DepthShader(DepthShader && other) noexcept = default;

DepthShader::~DepthShader()
{
    try { if (inputLayout) { inputLayout->Release(); inputLayout = nullptr; } }
    catch (exception&) {}
}

DepthShader& DepthShader::operator=(const DepthShader & other) = default;

DepthShader& DepthShader::operator=(DepthShader && other) noexcept = default;

bool DepthShader::Render(ID3D11DeviceContext * deviceContext, int indexCount, int instanceCount, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const vector<ID3D11ShaderResourceView*>&depthTextures, const vector<shared_ptr<Light>>&pointLightList, const XMFLOAT3 & cameraPosition)
{
    auto result = SetDepthShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);
    if (!result) return false;
    RenderShader(deviceContext, indexCount, instanceCount);
    return true;
}

bool DepthShader::SetDepthShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const XMFLOAT3 & cameraPosition)
{
    const auto result = SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
    if (textures.size() == 4) deviceContext->DSSetShaderResources(0, 1, &textures.back());
    const auto cameraBuffer = GetCameraBuffer();
    deviceContext->DSSetConstantBuffers(GetDomainBufferResourceCount(), 1, &cameraBuffer);
    IncrementDomainBufferResourceCount();

    return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount, int instanceCount) const
{
    deviceContext->IASetInputLayout(inputLayout);
    SetShader(deviceContext);
    deviceContext->DSSetSamplers(0, 1, &sampleStateWrap);
    deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
