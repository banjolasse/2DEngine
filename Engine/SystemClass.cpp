#include "SystemClass.h"

SystemClass::SystemClass()
{
	myInput = nullptr;
	myGraphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& aSystemClass)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	myInput = new InputClass;
	if (!myInput)
	{
		return false;
	}

	// Initialize the input object.
	myInput->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	myGraphics = new GraphicsClass;
	if (!myGraphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = myGraphics->Initialize(screenWidth, screenHeight, myHWND);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// Release the graphics object.
	if (myGraphics != nullptr)
	{
		myGraphics->Shutdown();
		delete myGraphics;
		myGraphics = nullptr;
	}

	// Release the input object.
	if (myInput != nullptr)
	{
		delete myInput;
		myInput = nullptr;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if (myInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = myGraphics->Frame();
	if (!result)
	{
		return false;
	}
	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND aHWND, UINT aUINT, WPARAM aWPARAM, LPARAM aLPARAM)
{
	switch (aUINT)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		myInput->KeyDown((unsigned int)aWPARAM);
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		myInput->KeyUp((unsigned int)aWPARAM);
		return 0;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
		return DefWindowProc(aHWND, aUINT, aWPARAM, aLPARAM);
	}
	}
}

void SystemClass::InitializeWindows(int& aScreenWidth, int& aScreenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	myHinstance = GetModuleHandle(nullptr);

	// Give the application a name.
	myApplicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = myHinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = myApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	aScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	aScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)aScreenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)aScreenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		aScreenWidth = 800;
		aScreenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - aScreenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - aScreenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	myHWND = CreateWindowEx(WS_EX_APPWINDOW, myApplicationName, myApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, aScreenWidth, aScreenHeight, nullptr, nullptr, myHinstance, nullptr);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(myHWND, SW_SHOW);
	SetForegroundWindow(myHWND);
	SetFocus(myHWND);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// Remove the window.
	DestroyWindow(myHWND);
	myHWND = nullptr;

	// Remove the application instance.
	UnregisterClass(myApplicationName, myHinstance);
	myHinstance = nullptr;

	// Release the pointer to this class.
	ApplicationHandle = nullptr;

	return;
}

LRESULT CALLBACK WndProc(HWND aHWND, UINT aUINT, WPARAM aWPARAM, LPARAM aLPARAM)
{
	switch (aUINT)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(aHWND, aUINT, aWPARAM, aLPARAM);
	}
	}
}
