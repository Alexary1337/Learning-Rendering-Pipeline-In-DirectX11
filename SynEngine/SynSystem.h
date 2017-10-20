#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "syninput.h"
#include "syngraphics.h"
#include "common.h"

class SynSystem
{
public:
	SynSystem();
	SynSystem(const SynSystem&);
	~SynSystem();

	bool Initialize();
	void Shutdown();
	void Run();
	void ActivateWF();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	SynInput* m_Input;
	SynGraphics* m_Graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SynSystem* ApplicationHandle = 0;

#endif