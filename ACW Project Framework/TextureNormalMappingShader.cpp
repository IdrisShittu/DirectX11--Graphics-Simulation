#include "TextureNormalMappingShader.h"

TextureNormalMappingShader::TextureNormalMappingShader(ID3D11Device* device, HWND hwnd) : Shader("TextureNormalVertexShader", "TextureNormalHullShader", "TextureNormalDomainShader", "TextureNormalPixelShader", device, hwnd), inputLayout(nullptr), sampleState(nullptr), lightBuffer(nullptr) {
    if (GetInitializationState()) return;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
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

    auto res = device->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]), GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);
    if (FAILED(res)) {
        SetInitializationState(true);
        return;
    }

    GetVertexShaderBuffer()->Release();
    SetVertexShaderBuffer(nullptr);

    D3D11_SAMPLER_DESC samplerDesc = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 1, D3D11_COMPARISON_ALWAYS, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
    };

    res = device->CreateSamplerState(&samplerDesc, &sampleState);
    if (FAILED(res)) SetInitializationState(true);

    D3D11_BUFFER_DESC lightBufferDesc = { sizeof(LightBufferType), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE };
    res = device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);
    if (FAILED(res)) SetInitializationState(true);
}

TextureNormalMappingShader::TextureNormalMappingShader(const TextureNormalMappingShader& other) = default;
TextureNormalMappingShader::TextureNormalMappingShader(TextureNormalMappingShader && other) noexcept = default;
TextureNormalMappingShader::~TextureNormalMappingShader() {
    if (lightBuffer) lightBuffer->Release();
    if (sampleState) sampleState->Release();
    if (inputLayout) inputLayout->Release();
}

TextureNormalMappingShader& TextureNormalMappingShader::operator=(const TextureNormalMappingShader & other) = default;
TextureNormalMappingShader& TextureNormalMappingShader::operator=(TextureNormalMappingShader && other) noexcept = default;

bool TextureNormalMappingShader::Render(ID3D11DeviceContext * ctx, int indexCount, int instanceCount, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const vector<ID3D11ShaderResourceView*>&depthTextures, const vector<shared_ptr<Light>>&lights, const XMFLOAT3 & cameraPosition) {
    if (!SetTextureNormalShaderParameters(ctx, viewMatrix, projectionMatrix, textures, lights, cameraPosition)) return false;
    RenderShader(ctx, indexCount, instanceCount);
    return true;
}

bool TextureNormalMappingShader::SetTextureNormalShaderParameters(ID3D11DeviceContext * ctx, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const vector<shared_ptr<Light>>&lights, const XMFLOAT3 & cameraPosition) {
    if (!SetShaderParameters(ctx, viewMatrix, projectionMatrix, cameraPosition)) return false;
    ctx->PSSetShaderResources(0, 2, textures.data());
    return true;
}

void TextureNormalMappingShader::RenderShader(ID3D11DeviceContext * ctx, int indexCount, int instanceCount) const {
    ctx->IASetInputLayout(inputLayout);
    SetShader(ctx);
    ctx->PSSetSamplers(0, 1, &sampleState);
    ctx->DrawInstanced(indexCount, instanceCount, 0, 0);
}
