#include "GraphicsRenderer.h"
#include <algorithm>

GraphicsRenderer::GraphicsRenderer(int screenWidth, int screenHeight, HWND const hwnd)
	: initFailed(false), d3D(nullptr), camera(nullptr), lightManager(nullptr), 
	terrain(nullptr), rocket(nullptr),displacedFloor(nullptr), skyBox(nullptr), gameObjects(), 
	shaderManager(nullptr), resourceManager(nullptr), shadowMapManager(nullptr), renderToggle(0),
	renderOptionalGameObjects(false), timeScale(1), updateCamera(false),
	cameraMode(0), dt(0.0f), fps(0.0f), start({ 0 }), end({ 0 }), frequency({ 0 })
{
	configuration = make_shared<SimulationConfigLoader>("Configuration.txt");

	windowWidth = screenWidth;
	windowHeight = screenHeight;

	if (!InitializeGraphicsDevice(screenWidth, screenHeight, hwnd) ||
		!InitializeResources(hwnd) ||
		!InitializeSceneObjects(hwnd))
	{
		initFailed = true;
	}

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
}

bool GraphicsRenderer::InitializeGraphicsDevice(int screenWidth, int screenHeight, HWND hwnd) {
	d3D = make_shared<GraphicsDeviceManager>(screenWidth, screenHeight, hwnd, FULL_SCREEN, VSYNC_ENABLED, SCREEN_DEPTH, SCREEN_NEAR);
	return d3D->GetInitializationState() == false;
}

bool GraphicsRenderer::InitializeResources(HWND hwnd) {
	d3D->GetInitializationState();
	shaderManager = make_shared<ShaderManager>(d3D->GetDevice(), hwnd);
	shaderManager->GetInitializationState();
	resourceManager = make_shared<ResourceManager>();
	return true;
}

bool GraphicsRenderer::InitializeSceneObjects(HWND hwnd) {
	
	camera = make_shared<Camera>();
	camera->SetPosition(configuration->GetCameraPosition());
	lightManager = make_shared<LightManager>();
	const auto terrainDimensions = configuration->GetTerrainDimensions();
	auto rocketPosition = configuration->GetRocketPosition();
	rocketPosition.x += -terrainDimensions.z;

	terrain = make_shared<Terrain>(d3D->GetDevice(), XMFLOAT3(80, 10, 40), XMFLOAT3(1, 1, 1), shaderManager->GetTextureDisplacementShader(), resourceManager);
	rocket = make_shared<Rocket>(d3D->GetDevice(), rocketPosition, configuration->GetRocketRotation(), configuration->GetRocketScale(), shaderManager, resourceManager);

	lightManager->AddLight(XMFLOAT3(0.0f, 0.0f, -terrainDimensions.z), XMFLOAT3(0.0f, 0.0f, 0.0f), configuration->GetSunAmbient(), configuration->GetSunDiffuse(), configuration->GetSunSpecular(), configuration->GetSunSpecularPower(), terrainDimensions.x, terrainDimensions.z, 1, terrainDimensions.z, true, true);
	lightManager->AddLight(XMFLOAT3(-terrainDimensions.x, -terrainDimensions.x, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), configuration->GetSunAmbient(), configuration->GetMoonDiffuse(), configuration->GetMoonSpecular(), configuration->GetMoonSpecularPower(), terrainDimensions.z, terrainDimensions.x, 1, terrainDimensions.x, true, true);

	shadowMapManager = make_shared<ShadowMapManager>(hwnd, d3D->GetDevice(), shaderManager->GetDepthShader(), lightManager->GetLightList().size(), SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	InitializeGameObjects(hwnd);

	return true;
}

bool GraphicsRenderer::InitializeGameObjects(HWND hwnd) {

	vector<const WCHAR*> textureNames = { L"BaseColour.dds", L"BaseNormal.dds", L"BaseSpecular.dds", L"BaseDisplacement.dds"};
	auto gameObject = make_shared<GameObject>();
	gameObject->AddPositionComponent(XMFLOAT3(8.0f, 0.0f, 0.0f));
	gameObject->AddRotationComponent(0.0f, 0.0f, 0.0f);
	gameObject->AddScaleComponent(1.0f, 1.0f, 1.0f);
	gameObject->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	gameObject->AddModelComponent(d3D->GetDevice(), ModelType::Sphere, resourceManager);
	gameObject->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);
	gameObject->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	gameObject->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	gameObjects.push_back(gameObject);
	gameObject->GetInitializationState();
		
	textureNames = { L"BaseColour.dds", L"BaseNormal.dds", L"BaseSpecular.dds", L"BaseDisplacement.dds" };

	auto newGameObject = make_shared<GameObject>();
	newGameObject->AddPositionComponent(XMFLOAT3(5.0f, 2.0f, 0.0f));
	newGameObject->AddRotationComponent(0.0f, 0.0f, 0.0f);
	newGameObject->AddScaleComponent(1.0f, 6.0f, 1.0f);
	newGameObject->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	newGameObject->AddModelComponent(d3D->GetDevice(), ModelType::HighPolyCylinder, resourceManager);
	newGameObject->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);
	newGameObject->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	newGameObject->SetTessellationVariables(5.0f, 20.0f, 8.0f, 1.0f);
	newGameObject->SetDisplacementVariables(20.0f, 0.0f, 6.0f, 0.15f);

	gameObjects.push_back(newGameObject);
	newGameObject->GetInitializationState();
		
	textureNames.clear();
	textureNames = { L"BaseColour.dds", L"BaseNormal.dds", L"BaseSpecular.dds", L"BaseDisplacement.dds" };

	const auto launchPadTessellationValues = configuration->GetLaunchPadTessellationValues();
	const auto launchPadDisplacementValues = configuration->GetLaunchPadDisplacementValues();

	const auto terrainDimensions = configuration->GetTerrainDimensions();

	displacedFloor = make_shared<GameObject>();
	displacedFloor->AddScaleComponent(configuration->GetLaunchPadScale());
	displacedFloor->AddPositionComponent(XMFLOAT3(-terrainDimensions.z, 0.5f, 0.0f));
	displacedFloor->AddRotationComponent(0.0f, 0.0f, 0.0f);
	displacedFloor->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	displacedFloor->AddModelComponent(d3D->GetDevice(), ModelType::Plane, resourceManager);
	displacedFloor->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);
	displacedFloor->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	displacedFloor->SetTessellationVariables(launchPadTessellationValues.x, launchPadTessellationValues.y, launchPadTessellationValues.z, launchPadTessellationValues.w);
	displacedFloor->SetDisplacementVariables(launchPadDisplacementValues.x, launchPadDisplacementValues.y, launchPadDisplacementValues.z, launchPadDisplacementValues.w);

	gameObjects.back()->GetInitializationState();
	
	textureNames.clear();
	textureNames = { L"FloorColour.dds", L"FloorNormal.dds", L"FloorSpecular.dds", L"FloorDisplacement.dds" };

	gameObjects.push_back(make_shared<GameObject>());
	gameObjects.back()->AddScaleComponent(1.0f, 1.0f, 1.0f);
	gameObjects.back()->AddPositionComponent(XMFLOAT3(-2.0f, 0.0f, 0.0f));
	gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	gameObjects.back()->AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
	gameObjects.back()->AddModelComponent(d3D->GetDevice(), ModelType::HighPolyCube, resourceManager);
	gameObjects.back()->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);
	gameObjects.back()->SetShaderComponent(shaderManager->GetTextureDisplacementShader());
	gameObjects.back()->SetDisplacementVariables(5.0f, 0.0f, 6.0f, 0.02f);

	gameObjects.back()->GetInitializationState();
	
	textureNames.clear();
	textureNames.push_back(L"skybox.dds");

	skyBox = make_shared<GameObject>();
	skyBox->AddPositionComponent(0.0f, 0.0f, 0.0f);
	skyBox->AddRotationComponent(0.0f, 0.0f, 0.0f);
	skyBox->AddScaleComponent(300.0f, 300.0f, 300.0f);
	skyBox->AddModelComponent(d3D->GetDevice(), ModelType::SphereInverted, resourceManager);
	skyBox->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);
	skyBox->SetShaderComponent(shaderManager->GetTextureCubeShader());

	gameObjects.push_back(make_shared<GameObject>());
	gameObjects.back()->AddPositionComponent(0.0f, 0.0f, 0.0f);
	gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	gameObjects.back()->AddScaleComponent(2.0f, 2.0f, 2.0f);
	gameObjects.back()->AddModelComponent(d3D->GetDevice(), ModelType::Sphere, resourceManager);
	gameObjects.back()->AddTextureComponent(d3D->GetDevice(), textureNames, resourceManager);

	return true;
}

GraphicsRenderer::GraphicsRenderer(const GraphicsRenderer& other) = default;

GraphicsRenderer::GraphicsRenderer(GraphicsRenderer&& other) noexcept = default;

GraphicsRenderer::~GraphicsRenderer()
{
}

GraphicsRenderer& GraphicsRenderer::operator=(const GraphicsRenderer&) = default;

GraphicsRenderer& GraphicsRenderer::operator=(GraphicsRenderer&&) noexcept = default;

const shared_ptr<Camera>& GraphicsRenderer::GetCamera() const
{
	return  camera;
}

void GraphicsRenderer::ToggleRenderOption() {
	renderToggle = (renderToggle + 1) % 5;

	switch (renderToggle) {
	case 0:
		shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 0, 0);
		d3D->DisableWireFrame();
		break;
	case 1:
		d3D->EnableWireFrame();
		break;
	case 2:
		shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(1, 0, 1);
		break;
	case 3:
		shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 1);
		break;
	case 4:
		shaderManager->GetTextureDisplacementShader()->SetRenderModeStates(0, 1, 0);
		break;
	default:
		break;
	}
}

void GraphicsRenderer::ToggleOptionalGameObjects() {
	renderOptionalGameObjects = !renderOptionalGameObjects;
}

void GraphicsRenderer::ResetToInitialState() const {
	rocket->ResetRocketState();
	terrain->ResetTerrainState();
}

void GraphicsRenderer::AddTimeScale(const int number)
{
	timeScale = (timeScale + number < 1) ? 1 : timeScale + number;
}

void GraphicsRenderer::RotateRocketLeft() const {
	rocket->AdjustRotationLeft();
}

void GraphicsRenderer::RotateRocketRight() const {
	rocket->AdjustRotationRight();
}

void GraphicsRenderer::LaunchRocket() const {
	rocket->LaunchRocket();
}

void GraphicsRenderer::ChangeCameraMode(const int camMode) {
	cameraMode = camMode;
	updateCamera = (cameraMode >= 2 && cameraMode <= 4);

	switch (cameraMode) {
	case 0: {
		const auto position = rocket->GetLauncherPosition();
		camera->SetPosition(position.x, position.y, position.z - 10.0f);
		camera->SetRotation(0.0f, 0.0f, 0.0f);
		break;
	}
	case 1:
		camera->SetPosition(0.0f, 10.0f, -30.0f);
		camera->SetRotation(0.0f, 45.0f, 0.0f);
		break;
	default:
		camera->SetRotation(0.0f, 0.0f, 0.0f);
		break;
	}
}

void GraphicsRenderer::UpdateCameraPosition() const {
	if (!updateCamera) return;

	XMFLOAT3 cameraPosition;
	switch (cameraMode) {
	case 2:
		cameraPosition = rocket->GetLookAtRocketPosition();
		cameraPosition.z -= 20.0f;
		break;
	case 3:
		cameraPosition = rocket->GetLookAtRocketConePosition();
		cameraPosition.z -= 3.0f;
		break;
	case 4:
		cameraPosition = rocket->GetLookAtRocketPosition();
		cameraPosition.z -= 3.0f;
		break;
	default:
		return;
	}

	camera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
}

bool GraphicsRenderer::UpdateFrame() {
	QueryPerformanceCounter(&end);
	dt = static_cast<float>((end.QuadPart - start.QuadPart) / static_cast<double>(frequency.QuadPart));
	start = end;

	dt *= timeScale;
	fps = static_cast<int>(1.0 / dt);
	XMFLOAT3 collisionPosition;
	float blastRadius = 0.0f;

	UpdateGameObjects();

	if (rocket->CheckForTerrainCollision(terrain, collisionPosition, blastRadius)) {
		
	}

	terrain->UpdateTerrain();
	rocket->UpdateRocket(dt);
	UpdateCameraAndLights();


	return RenderFrame();
}

void GraphicsRenderer::UpdateGameObjects() {
	displacedFloor->Update();
	skyBox->Update();
	for (const auto& gameObject : gameObjects) {
		gameObject->Update();
	}
}

void GraphicsRenderer::UpdateCameraAndLights() {
	UpdateCameraPosition();
	for (const auto& light : lightManager->GetLightList()) {
		light->UpdateLightVariables(dt);
	}
}

bool GraphicsRenderer::RenderFrame() {
	camera->Render();

	std::vector<shared_ptr<GameObject>> gameObjects;

	if (renderOptionalGameObjects) {
		gameObjects.insert(gameObjects.end(), gameObjects.begin(), gameObjects.end());
	}

	gameObjects.insert(gameObjects.end(), {
		static_pointer_cast<GameObject>(terrain),
		displacedFloor,
		skyBox,
		rocket->GetRocketBody(),
		rocket->GetRocketCone(),
		rocket->GetRocketCap(),
		rocket->GetRocketLauncher()
		});

	shadowMapManager->GenerateShadowMapResources(d3D->GetDeviceContext(), d3D->GetDepthStencilView(), lightManager->GetLightList(), gameObjects, camera->GetPosition());

	d3D->SetRenderTarget();

	XMMATRIX viewMatrix, projectionMatrix;
	d3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);
	camera->GetViewMatrix(viewMatrix);
	d3D->GetProjectionMatrix(projectionMatrix);

	std::vector<shared_ptr<Light>> lightList = lightManager->GetLightList();


	for (const auto& gameObject : gameObjects) {
		gameObject->Render(d3D->GetDeviceContext(), viewMatrix, projectionMatrix, shadowMapManager->GetShadowMapResources(), lightList, camera->GetPosition());
	}

	terrain->RenderTerrain(d3D->GetDeviceContext(), viewMatrix, projectionMatrix, shadowMapManager->GetShadowMapResources(), lightList, camera->GetPosition());
	rocket->RenderRocket(d3D, viewMatrix, projectionMatrix, shadowMapManager->GetShadowMapResources(), lightList, camera->GetPosition());

	d3D->DisableDepthStencil();
	d3D->EnableAlphaBlending();

	
		
	d3D->EnabledDepthStencil();
	d3D->DisableAlphaBlending();

	d3D->EndScene();

	return true;
}

bool GraphicsRenderer::GetInitializationState() const {
	return  initFailed;
}
