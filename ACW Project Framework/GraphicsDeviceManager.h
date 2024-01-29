#pragma once

//Link DirectX libraries we are using to this object
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d3d11.lib")

//DirectX libraries
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include <Windows.h>

using namespace DirectX;
using namespace std;

class GraphicsDeviceManager
{
public:
	GraphicsDeviceManager(int const screenWidth, int const screenHeight, HWND const hwnd, bool const fullScreen, bool const vSyncEnabled, float const screenDepth, float const screenNear);
	GraphicsDeviceManager(const GraphicsDeviceManager& other); // Copy Constructor
	GraphicsDeviceManager(GraphicsDeviceManager && other) noexcept; // Move Constructor
	~GraphicsDeviceManager(); // Destructor

	GraphicsDeviceManager& operator = (const GraphicsDeviceManager& other); // Copy Assignment Operator
	
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	ID3D11DepthStencilView* GetDepthStencilView() const;
	void SetRenderTarget() const;

	void EnableWireFrame() const;
	void DisableWireFrame() const;

	void EnabledDepthStencil() const;
	void DisableDepthStencil() const;

	void EnableAlphaBlending() const;
	void DisableAlphaBlending() const;

	void GetProjectionMatrix(XMMATRIX& projectionMatrix) const;
	void GetOrthogonalMatrix(XMMATRIX& orthographicMatrix) const;

	void GetVideoCardInfo(char* const cardName, int& memory) const;

	bool GetInitializationState() const;

private:

	void InitializeFactoryAdapter(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int &numerator, unsigned int &denominator);
	void InitializeDeviceAndSwapChain(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int const numerator, unsigned int const denominator, bool const fullScreen, HWND const hwnd);
	void InitializeBuffers(unsigned int const screenWidth, unsigned int const screenHeight);

	bool CreateBackBufferAndView();

	bool CreateDepthStencilBufferAndView(unsigned int screenWidth, unsigned int screenHeight);

	bool CreateDepthStencilStates();

	bool CreateBlendStates();

	bool initFailed;

	bool vSyncEnabled;
	int videoCardMemory;
	char videoCardDescription[128];

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	ID3D11RenderTargetView* renderTargetView;

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilStateEnabled;
	ID3D11DepthStencilState* depthStencilStateDisabled;;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11RasterizerState* rasterStateNormal;
	ID3D11RasterizerState* rasterStateWireFrame;

	ID3D11BlendState* alphaEnabledBlendState;
	ID3D11BlendState* alphaDisableBlendState;

	XMMATRIX projectionMatrix;
	XMMATRIX orthographicMatrix;
	template<typename T>
	void ReleaseComObject(T*& ptr);
};
