#include "TextureRenderer.h"

TextureRenderer::TextureRenderer(ID3D11Device* const device, const int textureWidth, const int textureHeight)
    : initFailed(false), renderTargetTexture(nullptr), renderTargetView(nullptr), shaderResourceView(nullptr), shader(nullptr)
{
    if (!InitializeTexture(device, textureWidth, textureHeight))
    {
        initFailed = true;
        return;
    }

    if (!InitializeRenderTargetView(device))
    {
        initFailed = true;
        return;
    }

    if (!InitializeShaderResourceView(device))
    {
        initFailed = true;
        return;
    }
}

TextureRenderer::TextureRenderer(const TextureRenderer& other) = default;

TextureRenderer::TextureRenderer(TextureRenderer && other) noexcept = default;

TextureRenderer::~TextureRenderer()
{
    ReleaseResources();
}

TextureRenderer& TextureRenderer::operator=(const TextureRenderer & other) = default;

TextureRenderer& TextureRenderer::operator=(TextureRenderer && other) noexcept = default;

void TextureRenderer::SetShader(const shared_ptr<Shader>&sh)
{
    shader = sh;
}

bool TextureRenderer::RenderObjectsToTexture(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const vector<shared_ptr<Light>>&pointLightList, const vector<shared_ptr<GameObject>>&gameObjects, const XMFLOAT3 & cameraPosition) const
{
    SetRenderTarget(deviceContext, depthStencilView);

    ClearRenderTarget(deviceContext, depthStencilView, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

    for (const auto& gameObject : gameObjects)
    {
        const auto originalShader = gameObject->GetShaderComponent();

        if (shader)
        {
            SwapShaderAndApplyVariables(gameObject);
        }

        const auto result = gameObject->Render(deviceContext, viewMatrix, projectionMatrix, {}, pointLightList, cameraPosition);

        if (shader)
        {
           
            gameObject->SetShaderComponent(originalShader);
        }

        if (!result)
        {
            return false;
        }
    }

    return true;
}

ID3D11ShaderResourceView* TextureRenderer::GetShaderResourceView() const
{
    return shaderResourceView;
}

bool TextureRenderer::GetInitializationState() const
{
    return initFailed;
}

bool TextureRenderer::InitializeTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight)
{
    D3D11_TEXTURE2D_DESC textureDescription;
    ZeroMemory(&textureDescription, sizeof(textureDescription));

    textureDescription.Width = textureWidth;
    textureDescription.Height = textureHeight;
    textureDescription.MipLevels = 1;
    textureDescription.ArraySize = 1;
    textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDescription.SampleDesc.Count = 1;
    textureDescription.SampleDesc.Quality = 0;
    textureDescription.Usage = D3D11_USAGE_DEFAULT;
    textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDescription.CPUAccessFlags = 0;
    textureDescription.MiscFlags = 0;

    auto result = device->CreateTexture2D(&textureDescription, nullptr, &renderTargetTexture);

    return SUCCEEDED(result);
}

bool TextureRenderer::InitializeRenderTargetView(ID3D11Device* const device)
{
    D3D11_TEXTURE2D_DESC textureDescription;
    renderTargetTexture->GetDesc(&textureDescription);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription;
    renderTargetViewDescription.Format = textureDescription.Format;
    renderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDescription.Texture2D.MipSlice = 0;

    auto result = device->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDescription, &renderTargetView);

    return SUCCEEDED(result);
}

bool TextureRenderer::InitializeShaderResourceView(ID3D11Device* const device)
{
    D3D11_TEXTURE2D_DESC textureDescription;
    renderTargetTexture->GetDesc(&textureDescription);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
    shaderResourceViewDescription.Format = textureDescription.Format;
    shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDescription.Texture2D.MipLevels = 1;

    auto result = device->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDescription, &shaderResourceView);

    return SUCCEEDED(result);
}

void TextureRenderer::ReleaseResources()
{
    if (shaderResourceView)
    {
        shaderResourceView->Release();
        shaderResourceView = nullptr;
    }

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }

    if (renderTargetTexture)
    {
        renderTargetTexture->Release();
        renderTargetTexture = nullptr;
    }
}

void TextureRenderer::SetRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView) const
{
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void TextureRenderer::ClearRenderTarget(ID3D11DeviceContext* const deviceContext, ID3D11DepthStencilView* const depthStencilView, const XMFLOAT4 & RGBA) const
{
    const float colour[4]{ RGBA.x, RGBA.y, RGBA.z, RGBA.w };

    deviceContext->ClearRenderTargetView(renderTargetView, colour);

    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void TextureRenderer::SwapShaderAndApplyVariables(const shared_ptr<GameObject>& gameObject) const
{
    const auto originalShader = gameObject->GetShaderComponent();

    auto mipInterval = 0.0f;
    auto mipClampMinimum = 0.0f;
    auto mipClampMaximum = 0.0f;
    auto displacementPower = 0.0f;

    gameObject->SetShaderComponent(shader);

    originalShader->GetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower);
    shader->SetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower);
}
