#include "GraphicsEngine.h"
#include <functional>

GraphicsEngine::GraphicsEngine() : hInstance(nullptr), hwnd(nullptr), appName("Rocket Simulation") {}

GraphicsEngine::~GraphicsEngine() {
	Shutdown();
}

bool GraphicsEngine::Initialize() {
	input = std::make_unique<Keyboard>();
	if (!input) {
		return false;
	}

	int screenWidth = 0;
	int screenHeight = 0;
	if (!InitializeWindows(screenWidth, screenHeight)) {
		return false;
	}

	graphics = std::make_unique<GraphicsRenderer>(screenWidth, screenHeight, hwnd);
	if (!graphics) {
		return false;
	}

	return true;
}

void GraphicsEngine::Execute() {
	MSG msg = {};
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else {
			if (!Update()) {
				break;
			}
		}
	}
}

bool GraphicsEngine::Update() {
	if (ShouldExitApplication()) {
		return false;
	}
	ProcessInputActions();
	UpdateCameraPositionAndControls();
	return graphics->UpdateFrame();
}

bool GraphicsEngine::ShouldExitApplication() const {
	return input->IsKeyPressed(VK_ESCAPE);
}

void GraphicsEngine::ProcessInputActions() {
	if (AreAnyKeysUp()) {
		input->SetCanProcessKey(true);
	}

	ProcessKeyAction(0x50, [&]() { graphics->ResetToInitialState(); });
	ProcessKeyAction(0x52, [&]() { graphics->ResetToInitialState(); });
	ProcessKeyAction(VK_F11, [&]() { graphics->LaunchRocket(); });

	ProcessTimeScaleAndRocketRotation();
	ProcessCameraModeChanges();
	ProcessRenderingOptions();
}

bool GraphicsEngine::AreAnyKeysUp() const {
	return input->IsKeyReleased(0x52) && input->IsKeyReleased(0x50) && input->IsKeyReleased(0x54) &&
		input->IsKeyReleased(VK_F1) && input->IsKeyReleased(VK_F2) && input->IsKeyReleased(VK_F3) &&
		input->IsKeyReleased(VK_F4) && input->IsKeyReleased(VK_F5) && input->IsKeyReleased(VK_F6) &&
		input->IsKeyReleased(VK_F7) && input->IsKeyReleased(VK_F11);
}

void GraphicsEngine::ProcessKeyAction(unsigned int key, std::function<void()> action) {
	if (input->IsKeyPressed(key) && input->CanProcessKey()) {
		action();
		input->SetCanProcessKey(false);
	}
}

void GraphicsEngine::ProcessTimeScaleAndRocketRotation() {
	if (input->IsKeyPressed(0x10)) {
		ProcessKeyAction(0x54, [&]() { graphics->AddTimeScale(1); });
		ProcessKeyAction(VK_OEM_COMMA, [&]() { graphics->RotateRocketLeft(); });
		ProcessKeyAction(VK_OEM_PERIOD, [&]() { graphics->RotateRocketRight(); });
	}
	else {
		ProcessKeyAction(0x54, [&]() { graphics->AddTimeScale(-1); });
	}
}

void GraphicsEngine::ProcessCameraModeChanges() {
	ProcessKeyAction(VK_F1, [&]() { graphics->ChangeCameraMode(0); });
	ProcessKeyAction(VK_F2, [&]() { graphics->ChangeCameraMode(1); });
	ProcessKeyAction(VK_F3, [&]() { graphics->ChangeCameraMode(2); });
	ProcessKeyAction(VK_F4, [&]() { graphics->ChangeCameraMode(3); });
	ProcessKeyAction(VK_F5, [&]() { graphics->ChangeCameraMode(4); });
}

void GraphicsEngine::ProcessRenderingOptions() {
	ProcessKeyAction(VK_F6, [&]() { graphics->ToggleRenderOption(); });
}

void GraphicsEngine::UpdateCameraPositionAndControls() {
	if (input->IsKeyPressed(VK_CONTROL)) {
		AdjustCameraPosition();
	}
	else {
		RotateCamera();
	}
}

void GraphicsEngine::AdjustCameraPosition(){
	if (input->IsKeyPressed(0x57)) graphics->GetCamera()->AddRotationZ(0.1f);
	if (input->IsKeyPressed(0x53)) graphics->GetCamera()->AddPositionZ(-0.1f);
	if (input->IsKeyPressed(0x44)) graphics->GetCamera()->AddPositionX(0.1f);
	if (input->IsKeyPressed(0x41)) graphics->GetCamera()->AddPositionX(-0.1f);
	if (input->IsKeyPressed(VK_PRIOR)) graphics->GetCamera()->AddPositionY(0.1f);
	if (input->IsKeyPressed(VK_NEXT)) graphics->GetCamera()->AddPositionY(-0.1f);
}

void GraphicsEngine::RotateCamera() {
	if (input->IsKeyPressed(0x57)) graphics->GetCamera()->AddRotationX(-0.8f);
	if (input->IsKeyPressed(0x53)) graphics->GetCamera()->AddRotationX(0.8f);
	if (input->IsKeyPressed(0x41)) graphics->GetCamera()->AddRotationY(-0.8f);
	if (input->IsKeyPressed(0x44)) graphics->GetCamera()->AddRotationY(0.8f);
}

LRESULT CALLBACK GraphicsEngine::MessageHandler(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	switch (umsg) {
	case WM_KEYDOWN:
		HandleKeyDown(static_cast<unsigned int>(wparam));
		return 0;

	case WM_KEYUP:
		HandleKeyUp(static_cast<unsigned int>(wparam));
		return 0;

	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

void GraphicsEngine::HandleKeyDown(unsigned int key) {
	input->SetKeyPressed(key);
}

void GraphicsEngine::HandleKeyUp(unsigned int key) {
	input->SetKeyReleased(key);
}

bool GraphicsEngine::InitializeWindows(int& screenWidth, int& screenHeight) {
	RegisterWindowClass();
	AdjustScreenSize(screenWidth, screenHeight);
	CreateApplicationWindow(screenWidth, screenHeight);
	FinalizeWindowSetup();
	return true;
}

void GraphicsEngine::RegisterWindowClass() {
	appHandle = this;
	hInstance = GetModuleHandle(nullptr);
	appName = "Rocket Simulation";

	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = appName;
	windowClass.hIconSm = windowClass.hIcon;

	RegisterClassEx(&windowClass);
}

void GraphicsEngine::AdjustScreenSize(int& screenWidth, int& screenHeight) {
	if (FULL_SCREEN) {
		DEVMODE devMode = {};
		devMode.dmSize = sizeof(devMode);
		devMode.dmPelsWidth = static_cast<unsigned long>(GetSystemMetrics(SM_CXSCREEN));
		devMode.dmPelsHeight = static_cast<unsigned long>(GetSystemMetrics(SM_CYSCREEN));
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		screenWidth = devMode.dmPelsWidth;
		screenHeight = devMode.dmPelsHeight;
	}
	else {
		screenWidth = 1360;
		screenHeight = 720;
	}
}

void GraphicsEngine::CreateApplicationWindow(int screenWidth, int screenHeight) {
	int posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	int posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight,
		nullptr, nullptr, hInstance, nullptr);
}

void GraphicsEngine::FinalizeWindowSetup() {
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowCursor(true);
}

void GraphicsEngine::ShutdownWindows() {

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(hwnd);
	hwnd = nullptr;

	UnregisterClass(appName, hInstance);
	hInstance = nullptr;

	appHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	switch (umsg) {
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

	default:
		if (appHandle) {
			return appHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
		else {
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void GraphicsEngine::Shutdown() {
	try
	{

	}
	catch (exception& e)
	{
		ShutdownWindows();
	}
}