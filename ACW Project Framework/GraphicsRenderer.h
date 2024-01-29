#pragma once

#include <Windows.h>

#include "GraphicsDeviceManager.h"

#include "Camera.h"
#include "GameObject.h"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "LightManager.h"
#include "TextureRenderer.h"
#include "ShadowMapManager.h"
#include "Terrain.h"
#include "Rocket.h"
#include "SimulationConfigLoader.h"

auto const FULL_SCREEN = false;
auto const VSYNC_ENABLED = true;
auto const SCREEN_DEPTH = 1000.0f;
auto const SCREEN_NEAR = 0.1f;

const int SHADOW_MAP_WIDTH = 1360;
const int SHADOW_MAP_HEIGHT = 720;

class GraphicsRenderer
{
public:
	GraphicsRenderer(int screenWidth, int screenHeight, HWND const hwnd); // Default Constructor
	bool InitializeGraphicsDevice(int screenWidth, int screenHeight, HWND hwnd);
	bool InitializeResources(HWND hwnd);
	bool InitializeSceneObjects(HWND hwnd);
	
	bool InitializeGameObjects(HWND hwnd);
	GraphicsRenderer(const GraphicsRenderer& other); // Copy Constructor
	GraphicsRenderer(GraphicsRenderer&& other) noexcept; // Move Constructor
	~GraphicsRenderer(); // Destructor

	GraphicsRenderer& operator = (const GraphicsRenderer& other); // Copy Assignment Operator
	GraphicsRenderer& operator = (GraphicsRenderer&& other) noexcept; // Move Assignment Operator

	const shared_ptr<Camera>& GetCamera() const;

	void ToggleRenderOption();
	void ToggleOptionalGameObjects();
	void ResetToInitialState() const;
	void AddTimeScale(const int number);
	void RotateRocketLeft() const;
	void RotateRocketRight() const;
	void LaunchRocket() const;
	void ChangeCameraMode(const int cameraMode);
	void UpdateCameraPosition() const;

	bool UpdateFrame();

	void UpdateGameObjects();

	void UpdateCameraAndLights();

	bool GetInitializationState() const;

private:
	bool RenderFrame();

	bool  initFailed;

	shared_ptr<SimulationConfigLoader>  configuration;

	shared_ptr<GraphicsDeviceManager>  d3D;

	shared_ptr<Camera>  camera;
	shared_ptr<LightManager>  lightManager;

	shared_ptr<Terrain>  terrain;
	shared_ptr<Rocket>  rocket;

	shared_ptr<GameObject>  displacedFloor;
	shared_ptr<GameObject>  skyBox;

	vector<shared_ptr<GameObject>>  gameObjects;

	shared_ptr<ShaderManager>  shaderManager;
	shared_ptr<ResourceManager>  resourceManager;
	shared_ptr<ShadowMapManager>  shadowMapManager;

	float  windowWidth;
	float  windowHeight;

	int  renderToggle;
	bool  renderOptionalGameObjects;
	int  timeScale;
	bool  updateCamera;
	int  cameraMode;

	float  dt;
	float  fps;
	LARGE_INTEGER  start;
	LARGE_INTEGER  end;
	LARGE_INTEGER  frequency;
};