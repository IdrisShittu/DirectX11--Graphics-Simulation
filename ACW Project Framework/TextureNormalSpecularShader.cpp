#include "TextureNormalSpecularShader.h"

TextureNormalSpecularShader::TextureNormalSpecularShader(ID3D11Device* device, HWND hwnd) : Shader("TextureNormalSpecularVS", "TextureNormalSpecularHS", "TextureNormalSpecularDS", "TextureNormalSpecularPS", device, hwnd), inputLayout(nullptr), sampleStateWrap(nullptr), sampleStateClamp(nullptr), lightMatrixBuffer(nullptr), lightBuffer(nullptr) {
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

    auto result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]), GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout);
    if (FAILED(result)) {
        SetInitializationState(true);
        return;
    }

    GetVertexShaderBuffer()->Release();
    SetVertexShaderBuffer(nullptr);

    D3D11_SAMPLER_DESC samplerDesc = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 1, D3D11_COMPARISON_ALWAYS, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
    };
    result = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
    if (FAILED(result)) {
        SetInitializationState(true);
        return;
    }

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
    if (FAILED(result)) SetInitializationState(true);

    D3D11_BUFFER_DESC bufferDesc = { sizeof(LightMatrixBufferType), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE };
    result = device->CreateBuffer(&bufferDesc, nullptr, &lightMatrixBuffer);
    if (FAILED(result)) {
        SetInitializationState(true);
        return;
    }

    bufferDesc.ByteWidth = sizeof(LightBufferType);
    result = device->CreateBuffer(&bufferDesc, nullptr, &lightBuffer);
    if (FAILED(result)) SetInitializationState(true);
}

TextureNormalSpecularShader::TextureNormalSpecularShader(const TextureNormalSpecularShader& other) = default;
TextureNormalSpecularShader::TextureNormalSpecularShader(TextureNormalSpecularShader && other) noexcept = default;
TextureNormalSpecularShader::~TextureNormalSpecularShader() {
    if (lightBuffer) lightBuffer->Release();
    if (lightMatrixBuffer) lightMatrixBuffer->Release();
    if (sampleStateClamp) sampleStateClamp->Release();
    if (sampleStateWrap) sampleStateWrap->Release();
    if (inputLayout) inputLayout->Release();
}

TextureNormalSpecularShader& TextureNormalSpecularShader::operator=(const TextureNormalSpecularShader & other) = default;
TextureNormalSpecularShader& TextureNormalSpecularShader::operator=(TextureNormalSpecularShader && other) noexcept = default;

bool TextureNormalSpecularShader::Render(ID3D11DeviceContext * ctx, int indexCount, int instanceCount, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const vector<ID3D11ShaderResourceView*>&depthTextures, const vector<shared_ptr<Light>>&lights, const XMFLOAT3 & cameraPosition) {
    if (!SetTextureNormalShaderParameters(ctx, viewMatrix, projectionMatrix, textures, depthTextures, lights, cameraPosition)) return false;
    RenderShader(ctx, indexCount, instanceCount);
    return true;
}

bool TextureNormalSpecularShader::SetTextureNormalShaderParameters(ID3D11DeviceContext * ctx, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<ID3D11ShaderResourceView*>&textures, const vector<ID3D11ShaderResourceView*>&depthTextures, const vector<shared_ptr<Light>>&lights, const XMFLOAT3 & cameraPosition) {
    if (!SetShaderParameters(ctx, viewMatrix, projectionMatrix, cameraPosition)) return false;
    ctx->PSSetShaderResources(0, 3, textures.data());
    ctx->PSSetShaderResources(3, depthTextures.size(), depthTextures.data());
    return true;
}

void TextureNormalSpecularShader::RenderShader(ID3D11DeviceContext * ctx, int indexCount, int instanceCount) const {
    ctx->IASetInputLayout(inputLayout);
    SetShader(ctx);
    ctx->PSSetSamplers(0, 1, &sampleStateWrap);
    ctx->PSSetSamplers(1, 1, &sampleStateClamp);
    ctx->DrawInstanced(indexCount, instanceCount, 0, 0);
}
