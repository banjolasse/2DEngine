#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass& aSystemClass);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND aHWND, UINT aUINT, WPARAM aWPARAM, LPARAM aLPARAM);

private:
	bool Frame();
	void InitializeWindows(int& aScreenWidth, int& aScreenHeight);
	void ShutdownWindows();

	LPCWSTR myApplicationName;
	HINSTANCE myHinstance;
	HWND myHWND;

	InputClass* myInput;
	GraphicsClass* myGraphics;
};


static LRESULT CALLBACK WndProc(HWND aHWND, UINT aUINT, WPARAM aWPARAM, LPARAM aLPARAM);
static SystemClass* ApplicationHandle = nullptr;
