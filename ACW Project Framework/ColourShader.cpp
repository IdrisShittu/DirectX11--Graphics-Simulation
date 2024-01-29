#include "ColourShader.h"

ColourShader::ColourShader(ID3D11Device* device, HWND hwnd)
    : Shader("ColourVertexShader", "ColourHullShader", "ColourDomainShader", "ColourPixelShader", device, hwnd), inputLayout(nullptr) {

    D3D11_INPUT_ELEMENT_DESC layout[6] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    for (int i = 0; i < 4; ++i) {
        layout[2 + i] = { "INSTANCEMATRIX", static_cast<UINT>(i), DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
    }

    if (FAILED(device->CreateInputLayout(layout, 6, GetVertexShaderBuffer()->GetBufferPointer(), GetVertexShaderBuffer()->GetBufferSize(), &inputLayout))) {
        SetInitializationState(true);
        return;
    }
    GetVertexShaderBuffer()->Release();
}

ColourShader::ColourShader(const ColourShader& other) = default;

ColourShader::ColourShader(ColourShader && other) noexcept = default;

ColourShader::~ColourShader() {
    if (inputLayout) inputLayout->Release();
}

bool ColourShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& textures, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) {
    SetColourShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
    RenderShader(deviceContext, indexCount, instanceCount);
    return true;
}

bool ColourShader::SetColourShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition) {
    return SetShaderParameters(deviceContext, viewMatrix, projectionMatrix, cameraPosition);
}

void ColourShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount) const {
    deviceContext->IASetInputLayout(inputLayout);
    SetShader(deviceContext);
    deviceContext->DrawInstanced(indexCount, instanceCount, 0, 0);
}
