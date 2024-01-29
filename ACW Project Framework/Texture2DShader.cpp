#include "Texture2DShader.h"

Texture2DShader::Texture2DShader(ID3D11Device* device, HWND hwnd)
    : Shader("Texture2DVertexShader", "Texture2DHullShader", "Texture2DDomainShader", "Texture2DPixelShader", device, hwnd),
    inputLayout(nullptr), sampleState(nullptr) {

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INSTANCEMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };

    if (FAILED(device->CreateInputLayout(layout, 6, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout))) {
        SetInitializationState(true);
        return;
    }

    GetVertexShaderBuffer()->Release();
    SetVertexShaderBuffer(nullptr);

    D3D11_SAMPLER_DESC samplerDesc = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 1, D3D11_COMPARISON_ALWAYS, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f, D3D11_FLOAT32_MAX
    };

    if (FAILED(device->CreateSamplerState(&samplerDesc, &sampleState))) SetInitializationState(true);
}

Texture2DShader::~Texture2DShader() {
    if (sampleState) sampleState->Release();
    if (inputLayout) inputLayout->Release();
}

bool Texture2DShader::Render(ID3D11DeviceContext* deviceContext, const int indexCount, const int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) {
    SetTextureShaderParameters(deviceContext, viewMatrix, projectionMatrix, textures, cameraPosition);
    RenderShader(deviceContext, indexCount, instanceCount);
    return true;
}

bool Texture2DShader::SetTextureShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const XMFLOAT3& cameraPosition) {
    SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
    deviceContext->PSSetShaderResources(0, 1, textures.data());
    return true;
}

void Texture2DShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount) const {
    deviceContext->IASetInputLayout(inputLayout);
    SetShader(deviceContext);
    deviceContext->PSSetSamplers(0, 1, &sampleState);
    deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
