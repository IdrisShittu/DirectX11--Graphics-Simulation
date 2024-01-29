#pragma once
#include <Windows.h>
#include <memory>
#include "Keyboard.h"
#include "GraphicsRenderer.h"
#include <functional>

class GraphicsEngine {
public:
    GraphicsEngine();
    ~GraphicsEngine();
    GraphicsEngine(const GraphicsEngine&) = delete;
    GraphicsEngine& operator=(const GraphicsEngine&) = delete;
    GraphicsEngine(GraphicsEngine&&) noexcept = delete;
    GraphicsEngine& operator=(GraphicsEngine&&) noexcept = delete;

    bool Initialize();
    void Execute();
    void Shutdown();

    LRESULT MessageHandler(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam);

    void HandleKeyDown(unsigned int key);

    void HandleKeyUp(unsigned int key);

private:
    bool Update();
    bool ShouldExitApplication() const;
    void ProcessInputActions();
    bool AreAnyKeysUp() const;
    void ProcessKeyAction(unsigned int key, std::function<void()> action);
    void ProcessTimeScaleAndRocketRotation();
    void ProcessCameraModeChanges();
    void ProcessRenderingOptions();
    void UpdateCameraPositionAndControls();
    void AdjustCameraPosition();
    void RotateCamera();

    bool InitializeWindows(int& screenWidth, int& screenHeight);
    void RegisterWindowClass();
    void AdjustScreenSize(int& screenWidth, int& screenHeight);
    void CreateApplicationWindow(int screenWidth, int screenHeight);
    void FinalizeWindowSetup();
    void ShutdownWindows();

    HINSTANCE hInstance;
    HWND hwnd;
    LPCSTR appName;

    std::unique_ptr<Keyboard> input;
    std::unique_ptr<GraphicsRenderer> graphics;
};

static LRESULT CALLBACK WndProc(HWND const hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

static GraphicsEngine* appHandle = nullptr;