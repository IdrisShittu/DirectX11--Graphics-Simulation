#include "System.h"

System::System() : m_initializationFailed(false), m_applicationName(nullptr), m_hInstance(nullptr), m_hwnd(nullptr), m_input(nullptr), m_graphics(nullptr) {
	auto screenWidth = 0;
	auto screenHeight = 0;

	//Initialize the Windows API
	InitializeWindows(screenWidth, screenHeight);

	//Create our input object for reading keyboard inputs
	m_input = new InputManager();

	if (!m_input)
	{
		m_initializationFailed = true;
		return;
	}

	//Create our graphics object for handling the rendering of all the graphics
	m_graphics = new GraphicsRenderer(screenWidth, screenHeight, m_hwnd);

	if (m_graphics->GetInitializationState())
	{
		m_initializationFailed = true;
	}
};

// We don't need a copy/ move constructor for this class but it's best to have it defined so the compiler doesn't generate it for us
System::System(const System& other) = default;

System::System(System&& other) noexcept = default;

System::~System()
{
	//Release graphics object

	if (m_graphics)
	{
		delete m_graphics;
		m_graphics = nullptr;
	}

	if (m_input)
	{
		delete m_input;
		m_input = nullptr;
	}

	// Shutdown window
	try
	{
		
	}
	catch (exception& e)
	{
		ShutdownWindows();
	}
}

System& System::operator=(const System& other) = default;

System& System::operator=(System&& other) noexcept = default;

//This is the main application loop where we do all the application processing through the frame function until we quit. The frame function is called each loop.
void System::Run() {
	
	MSG message;
	auto done = false;

	//Initialize message structure by filling a block of memory with zeros
	ZeroMemory(&message, sizeof(message));

	//Loop until we get a quit message from the window or the user
	while (!done)
	{
		//Handle windows messages
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//If windows ends the application then we exit out
		if (message.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Else we do the frame processing
			auto const result = Frame();

			if (!result)
			{
				done = true;
			}
		}
	}
}

bool System::GetInitializationState() const
{
	return m_initializationFailed;
}


//This is where all the application processing happens, we check to see if the user wants to quit the application, if not then we call the graphics objects frame function which will render the graphics.
//More code will be added here as we add more functionality to the framework/ application.
bool System::Frame() {
	
	//Check if user wants to quit the application
	if (m_input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//For keys we limit to one execution on press we check if its up first
	if (m_input->IsKeyUp(0x52) && m_input->IsKeyUp(0x50) && m_input->IsKeyUp(0x54) && m_input->IsKeyUp(VK_F1) && m_input->IsKeyUp(VK_F2) && m_input->IsKeyUp(VK_F3) && m_input->IsKeyUp(VK_F4) && m_input->IsKeyUp(VK_F5) && m_input->IsKeyUp(VK_F6) && m_input->IsKeyUp(VK_F7) && m_input->IsKeyUp(VK_F11))
	{
		m_input->ToggleDoOnce(true);
	}

	//Reset simulation
	if (m_input->IsKeyDown(0x52) && m_input->DoOnce())
	{
		m_graphics->ResetToInitialState();
		m_input->ToggleDoOnce(false);
	}

	//Pause Simulation
	if (m_input->IsKeyDown(0x50) && m_input->DoOnce())
	{
		//m_graphics->TogglePauseSimulation();
		m_input->ToggleDoOnce(false);
	}

	//Launch Rocket
	if (m_input->IsKeyDown(VK_F11) && m_input->DoOnce())
	{
		m_graphics->LaunchRocket();
		m_input->ToggleDoOnce(false);
	}

	//Increase/Decrease TimeScale and rotate rocket
	if (m_input->IsKeyDown(0x10))
	{
		if (m_input->IsKeyDown(0x54) && m_input->DoOnce())
		{
			m_graphics->AddTimeScale(1);
			m_input->ToggleDoOnce(false);
		}

		if (m_input->IsKeyDown(VK_OEM_COMMA))
		{
			m_graphics->RotateRocketLeft();
		}

		if (m_input->IsKeyDown(VK_OEM_PERIOD))
		{
			m_graphics->RotateRocketRight();
		}
	}
	else
	{
		if (m_input->IsKeyDown(0x54) && m_input->DoOnce())
		{
			m_graphics->AddTimeScale(-1);
			m_input->ToggleDoOnce(false);
		}
	}

	//Change camera position
	if (m_input->IsKeyDown(VK_F1) && m_input->DoOnce())
	{
		m_graphics->ChangeCameraMode(0);
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F2) && m_input->DoOnce())
	{
		m_graphics->ChangeCameraMode(1);
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F3) && m_input->DoOnce())
	{
		m_graphics->ChangeCameraMode(2);
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F4) && m_input->DoOnce())
	{
		m_graphics->ChangeCameraMode(3);
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F5) && m_input->DoOnce())
	{
		m_graphics->ChangeCameraMode(4);
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F6) && m_input->DoOnce())
	{
		m_graphics->ToggleRenderOption();
		m_input->ToggleDoOnce(false);
	}

	if (m_input->IsKeyDown(VK_F7) && m_input->DoOnce())
	{
		m_graphics->ToggleOptionalGameObjects();
		m_input->ToggleDoOnce(false);
	}

	//Camera Controls
	if (m_input->IsKeyDown(VK_CONTROL))
	{
		if (m_input->IsKeyDown(0x57))
		{
			m_graphics->GetCamera()->AddPositionZ(0.1f);
		}

		if (m_input->IsKeyDown(0x53))
		{
			m_graphics->GetCamera()->AddPositionZ(-0.1f);
		}

		if (m_input->IsKeyDown(0x44))
		{
			m_graphics->GetCamera()->AddPositionX(0.1f);
		}

		if (m_input->IsKeyDown(0x41))
		{
			m_graphics->GetCamera()->AddPositionX(-0.1f);
		}

		if (m_input->IsKeyDown(VK_PRIOR))
		{
			m_graphics->GetCamera()->AddPositionY(0.1f);
		}

		if (m_input->IsKeyDown(VK_NEXT))
		{
			m_graphics->GetCamera()->AddPositionY(-0.1f);
		}
	}
	else
	{
		if (m_input->IsKeyDown(0x57))
		{
			//Rotate up
			m_graphics->GetCamera()->AddRotationY(-0.8f);
		}

		if (m_input->IsKeyDown(0x53))
		{
			//Rotate down
			m_graphics->GetCamera()->AddRotationY(0.8f);
		}

		if (m_input->IsKeyDown(0x44))
		{
			//Rotate right
			m_graphics->GetCamera()->AddRotationX(0.8f);
		}

		if (m_input->IsKeyDown(0x41))
		{
			//Rotate left
			m_graphics->GetCamera()->AddRotationX(-0.8f);
		}
	}


	//Call graphics objects update frame processing function
	auto const result = m_graphics->UpdateFrame();

	return result;
}

//Our MessageHandler where we direct the windows system messages into. With this we can listen for certain information.
//For now we just read key presses and key releases and notifiy our input object, all other information we just pass back to the default windows message handler.
LRESULT CALLBACK System::MessageHandler(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	
	switch(umsg)
	{
		//Check if a key has been pressed
		case WM_KEYDOWN:
		{
			// If a key is pressed then send it to our input object
			m_input->KeyDown(static_cast<unsigned int>(wparam));
			return 0;
		}

		//Check if a key has been released
		case WM_KEYUP:
		{
			//If a key is released then send it to our input object
			m_input->KeyUp(static_cast<unsigned int>(wparam));
			return 0;
		}

		//Send any other messages back to the default windows message handler
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//This is where we build the window we are rendering to. We start by initializing a default window which can be full screen or a set size, depending on the global variable FULL_SCREEN in the GraphicsClass.h
void System::InitializeWindows(int& screenWidth, int& screenHeight) {
	
	//The windows class structure where we define the window information and register it
	WNDCLASSEX windowClass;

	//The device mode structure about the initialization and environment of our display device
	DEVMODE deviceEnvironment;

	int positionX;
	int positionY;

	//Get an external pointer to our object
	applicationHandle = this;

	//Get the instance of our application
	m_hInstance = GetModuleHandle(nullptr);

	//Name our application
	m_applicationName = "Real Time Graphics FrameWork ACW";

	//Define our windows class with default settings
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //To do with redrawing the window if a movement or size adjustment occurs (CS_OWNDC allocates a unique device context)
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = m_hInstance;
	windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = m_applicationName;
	windowClass.hIconSm = windowClass.hIcon;

	//Register our windows class
	RegisterClassEx(&windowClass);

	//Get the resolution of the users screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the display device depending on whether we are in full screen mode or windowed, if it's fullscreen we set the screen to the max size and 32bit, else to a defined resolution
	if (FULL_SCREEN)
	{
		//Can we just do ZeroMemory?
		//Initialize device structure by filling a block of memory with zeros
		ZeroMemory(&deviceEnvironment, sizeof(deviceEnvironment));
		//memset(&deviceEnvironment, 0, sizeof(deviceEnvironment));

		deviceEnvironment.dmSize = sizeof(deviceEnvironment);
		deviceEnvironment.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		deviceEnvironment.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		deviceEnvironment.dmBitsPerPel = 32;
		deviceEnvironment.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change display settings to fullscreen using device environment
		ChangeDisplaySettings(&deviceEnvironment, CDS_FULLSCREEN);

		//Set the position of the window to the top left corner
		positionX = 0;
		positionY = 0;
	}
	else
	{
		screenWidth = 1360;
		screenHeight = 720;

		//Place window in the centre of the screen
		positionX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		positionY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the screen settings and get the handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, positionX, positionY, screenWidth, screenHeight, nullptr, nullptr, m_hInstance, nullptr);

	//Show the window and bring it to the front of the applications
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the cursor
	ShowCursor(true);
}

//Reverts the screen settings and releases the window class and any handles associated with it
void System::ShutdownWindows() {
	
	//Show the cursor
	ShowCursor(true);

	//Fix display settings if we're in full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	//Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	//Remove the application instance by un-registering our window class
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;

	//Release the pointer to this object
	applicationHandle = nullptr;
}

//This is where windows will sends its message to for us to intercept and use with our message handler, if we can't use it then we just return it back to the main windows message handler inside the MessageHandler function
//We intercept this by giving this prototype function to the window procedure when we defined the window class structure for our window class (WNDCLASSEX), this way we hook into the messaging functionality and intercept messages
LRESULT CALLBACK WndProc(HWND const hwnd, UINT const umsg, WPARAM const wparam, LPARAM const lparam) {
	
	switch(umsg)
	{
		//Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		//Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		//All other messages pass to our message handler
		default:
		{
			return applicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
		}
	}
}
