#include "GraphicsDeviceManager.h"
#include <vector>
#include <minwinbase.h>

GraphicsDeviceManager::GraphicsDeviceManager(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen, bool vSyncEnabled, float screenDepth, float screenNear)
	: initFailed(false), vSyncEnabled(vSyncEnabled), videoCardMemory(0), videoCardDescription{},
	swapChain(nullptr), device(nullptr), deviceContext(nullptr), renderTargetView(nullptr),
	depthStencilBuffer(nullptr), depthStencilStateEnabled(nullptr), depthStencilStateDisabled(nullptr),
	depthStencilView(nullptr), rasterStateNormal(nullptr), rasterStateWireFrame(nullptr),
	alphaEnabledBlendState(nullptr), alphaDisableBlendState(nullptr), projectionMatrix(XMMATRIX()), orthographicMatrix(XMMATRIX()){

	unsigned int numerator = 0;
	unsigned int denominator = 0;

	InitializeFactoryAdapter(screenWidth, screenHeight, numerator, denominator);

	if (!initFailed)
	{
		InitializeDeviceAndSwapChain(screenWidth, screenHeight, numerator, denominator, fullScreen, hwnd);

		if (!initFailed)
		{
			InitializeBuffers(screenWidth, screenHeight);

			if (!initFailed)
			{
				D3D11_RASTERIZER_DESC rasterDescription{};
				ZeroMemory(&rasterDescription, sizeof(rasterDescription));

				rasterDescription.FillMode = D3D11_FILL_SOLID;
				rasterDescription.CullMode = D3D11_CULL_BACK;
				rasterDescription.FrontCounterClockwise = false;
				rasterDescription.DepthBias = 0;
				rasterDescription.DepthBiasClamp = 0.0f;
				rasterDescription.SlopeScaledDepthBias = 0.0f;
				rasterDescription.DepthClipEnable = true;
				rasterDescription.ScissorEnable = false;
				rasterDescription.MultisampleEnable = false;
				rasterDescription.AntialiasedLineEnable = false;

				if (FAILED(device->CreateRasterizerState(&rasterDescription, &rasterStateNormal)))
				{
					initFailed = true;
				}
				else
				{
					deviceContext->RSSetState(rasterStateNormal);

					ZeroMemory(&rasterDescription, sizeof(rasterDescription));
					rasterDescription.FillMode = D3D11_FILL_WIREFRAME;
					rasterDescription.CullMode = D3D11_CULL_BACK;
					rasterDescription.FrontCounterClockwise = false;
					rasterDescription.DepthBias = 0;
					rasterDescription.DepthBiasClamp = 0.0f;
					rasterDescription.SlopeScaledDepthBias = 0.0f;
					rasterDescription.DepthClipEnable = true;
					rasterDescription.ScissorEnable = false;
					rasterDescription.MultisampleEnable = false;
					rasterDescription.AntialiasedLineEnable = false;

					if (FAILED(device->CreateRasterizerState(&rasterDescription, &rasterStateWireFrame)))
					{
						initFailed = true;
					}
					else
					{
						D3D11_VIEWPORT viewport;
						ZeroMemory(&viewport, sizeof(viewport));

						viewport.TopLeftX = 0.0f;
						viewport.TopLeftY = 0.0f;
						viewport.Width = static_cast<float>(screenWidth);
						viewport.Height = static_cast<float>(screenHeight);
						viewport.MinDepth = 0.0f;
						viewport.MaxDepth = 1.0f;

						deviceContext->RSSetViewports(1, &viewport);

						auto const fieldOfView = static_cast<float>(XM_PI / 4.0f);
						auto const screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

						projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

						orthographicMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);
					}
				}
			}
		}
	}
}

GraphicsDeviceManager::GraphicsDeviceManager(const GraphicsDeviceManager& other) = default;

GraphicsDeviceManager::GraphicsDeviceManager(GraphicsDeviceManager&& other) noexcept = default;

GraphicsDeviceManager::~GraphicsDeviceManager()
{
	ReleaseComObject(alphaDisableBlendState);
	ReleaseComObject(alphaEnabledBlendState);
	ReleaseComObject(rasterStateWireFrame);
	ReleaseComObject(rasterStateNormal);
	ReleaseComObject(depthStencilView);
	ReleaseComObject(depthStencilStateDisabled);
	ReleaseComObject(depthStencilStateEnabled);
	ReleaseComObject(depthStencilBuffer);
	ReleaseComObject(renderTargetView);
	ReleaseComObject(deviceContext);
	ReleaseComObject(device);
	ReleaseComObject(swapChain);
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, nullptr);
	}
}

template <typename T>
void GraphicsDeviceManager::ReleaseComObject(T*& ptr)
{
	if (ptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

GraphicsDeviceManager& GraphicsDeviceManager::operator=(const GraphicsDeviceManager& other) = default;

void GraphicsDeviceManager::InitializeFactoryAdapter(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int& numerator, unsigned int& denominator)
{
	IDXGIFactory* factory = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory))))
	{
		initFailed = true;
		return;
	}
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		initFailed = true;
		factory->Release();
		return;
	}
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		initFailed = true;
		adapter->Release();
		factory->Release();
		return;
	}

	unsigned int numOfModes;

	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numOfModes, nullptr)))
	{
		initFailed = true;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		return;
	}

	// Create a list to hold all possible display modes for this monitor/video card combinations
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numOfModes];

	if (!displayModeList)
	{
		initFailed = true;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		return;
	}

	// Now fill the list
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numOfModes, displayModeList)))
	{
		initFailed = true;
		delete[] displayModeList;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		return;
	}

	// Now go through each possible mode and find the one that matches our monitor and store the numerator/denominator
	for (unsigned int i = 0; i < numOfModes; i++)
	{
		if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
			break; // Stop once we've found a matching mode
		}
	}

	DXGI_ADAPTER_DESC adapterDescription;

	// Get the adapter description
	if (FAILED(adapter->GetDesc(&adapterDescription)))
	{
		initFailed = true;
		delete[] displayModeList;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		return;
	}

	// Store video card memory in megabytes
	videoCardMemory = static_cast<int>(adapterDescription.DedicatedVideoMemory / 1024 / 1024);

	// Convert video card name to a character array and store it
	if (wcstombs_s(nullptr, videoCardDescription, 128, adapterDescription.Description, 128) != 0)
	{
		initFailed = true;
	}

	// Release resources
	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();
}

void GraphicsDeviceManager::InitializeDeviceAndSwapChain(unsigned int screenWidth, unsigned int screenHeight, unsigned int numerator, unsigned int denominator, bool fullScreen, HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

	swapChainDescription.BufferDesc.Width = screenWidth;
	swapChainDescription.BufferDesc.Height = screenHeight;
	swapChainDescription.BufferDesc.RefreshRate.Numerator = vSyncEnabled ? numerator : 0;
	swapChainDescription.BufferDesc.RefreshRate.Denominator = vSyncEnabled ? denominator : 1;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;

	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 1;
	swapChainDescription.OutputWindow = hwnd;
	swapChainDescription.Windowed = !fullScreen;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescription.Flags = 0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, &swapChain, &device, nullptr, &deviceContext);
	
	if (FAILED(result))
	{
		initFailed = true;
	}
}

void GraphicsDeviceManager::InitializeBuffers(unsigned int screenWidth, unsigned int screenHeight)
{
	if (!CreateBackBufferAndView()) return;
	if (!CreateDepthStencilBufferAndView(screenWidth, screenHeight)) return;
	if (!CreateDepthStencilStates()) return;
	if (!CreateBlendStates()) return;
	DisableAlphaBlending();
}

bool GraphicsDeviceManager::CreateBackBufferAndView()
{
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer))))
	{
		initFailed = true;
		return false;
	}

	if (FAILED(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView)))
	{
		initFailed = true;
		backBuffer->Release();
		return false;
	}

	backBuffer->Release();
	return true;
}

bool GraphicsDeviceManager::CreateDepthStencilBufferAndView(unsigned int screenWidth, unsigned int screenHeight)
{
	D3D11_TEXTURE2D_DESC depthBufferDescription = {};
	depthBufferDescription.Width = screenWidth;
	depthBufferDescription.Height = screenHeight;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(device->CreateTexture2D(&depthBufferDescription, nullptr, &depthStencilBuffer)))
	{
		initFailed = true;
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDescription = {};
	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDescription.StencilEnable = true;
	depthStencilDescription.StencilReadMask = 0xFF;
	depthStencilDescription.StencilWriteMask = 0xFF;
	depthStencilDescription.FrontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
	depthStencilDescription.BackFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_DECR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };

	if (FAILED(device->CreateDepthStencilState(&depthStencilDescription, &depthStencilStateEnabled)))
	{
		initFailed = true;
		return false;
	}

	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	if (FAILED(device->CreateDepthStencilState(&depthStencilDescription, &depthStencilStateDisabled)))
	{
		initFailed = true;
		return false;
	}

	deviceContext->OMSetDepthStencilState(depthStencilStateEnabled, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription = {};
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDescription, &depthStencilView)))
	{
		initFailed = true;
		return false;
	}

	SetRenderTarget();
	return true;
}

bool GraphicsDeviceManager::CreateDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription = {};
	depthStencilDescription.DepthEnable = true;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDescription.StencilEnable = true;
	depthStencilDescription.StencilReadMask = 0xFF;
	depthStencilDescription.StencilWriteMask = 0xFF;
	depthStencilDescription.FrontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
	depthStencilDescription.BackFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_DECR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };

	if (FAILED(device->CreateDepthStencilState(&depthStencilDescription, &depthStencilStateEnabled)))
	{
		initFailed = true;
		return false;
	}

	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	if (FAILED(device->CreateDepthStencilState(&depthStencilDescription, &depthStencilStateDisabled)))
	{
		initFailed = true;
		return false;
	}

	deviceContext->OMSetDepthStencilState(depthStencilStateEnabled, 1);
	return true;
}

bool GraphicsDeviceManager::CreateBlendStates()
{
	D3D11_BLEND_DESC blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(device->CreateBlendState(&blendStateDescription, &alphaEnabledBlendState)))
	{
		initFailed = true;
		return false;
	}

	blendStateDescription.RenderTarget[0].BlendEnable = false;

	if (FAILED(device->CreateBlendState(&blendStateDescription, &alphaDisableBlendState)))
	{
		initFailed = true;
		return false;
	}

	return true;
}

void GraphicsDeviceManager::EndScene()
{
	UINT syncInterval = vSyncEnabled ? 1 : 0;
	UINT presentFlags = 0;

	swapChain->Present(syncInterval, presentFlags);
}

void GraphicsDeviceManager::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };

	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11Device* GraphicsDeviceManager::GetDevice() const
{
	return device;
}

ID3D11DeviceContext* GraphicsDeviceManager::GetDeviceContext() const
{
	return deviceContext;
}

ID3D11DepthStencilView* GraphicsDeviceManager::GetDepthStencilView() const
{
	return depthStencilView;
}

void GraphicsDeviceManager::SetRenderTarget() const
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void GraphicsDeviceManager::EnableWireFrame() const
{
	deviceContext->RSSetState(rasterStateWireFrame);
}

void GraphicsDeviceManager::DisableWireFrame() const
{
	deviceContext->RSSetState(rasterStateNormal);
}

void GraphicsDeviceManager::EnabledDepthStencil() const
{
	deviceContext->OMSetDepthStencilState(depthStencilStateEnabled, 1);
}

void GraphicsDeviceManager::DisableDepthStencil() const
{
	deviceContext->OMSetDepthStencilState(depthStencilStateDisabled, 1);
}

void GraphicsDeviceManager::EnableAlphaBlending() const
{
	deviceContext->OMSetBlendState(alphaEnabledBlendState, nullptr, 0xffffffff);
}

void GraphicsDeviceManager::DisableAlphaBlending() const
{
	deviceContext->OMSetBlendState(alphaDisableBlendState, nullptr, 0xffffffff);
}

void GraphicsDeviceManager::GetProjectionMatrix(XMMATRIX& projectionMat) const
{
	projectionMat = projectionMatrix;
}

void GraphicsDeviceManager::GetOrthogonalMatrix(XMMATRIX& orthographicMat) const
{
	orthographicMat = orthographicMatrix;
}

void GraphicsDeviceManager::GetVideoCardInfo(char* const cardName, int& memory) const
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
}

bool GraphicsDeviceManager::GetInitializationState() const
{
	return initFailed;
}
