#ifndef _INPUT_H_
#define _INPUT_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "common.h"
#include <dinput.h>

class SynInput
{
public:
	SynInput();
	SynInput(const SynInput&);
	~SynInput();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	int* Frame(bool);

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	int GetMouseXLocation();
	int GetMouseYLocation();
	bool IsKeyPressed(unsigned int key);
	bool IsF1Toggled();
	bool IsF2Toggled();
	bool IsF3Toggled();
	bool IsEToggled();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	bool m_F1_released;
	bool m_F2_released;
	bool m_F3_released;
	bool m_E_released;

private:

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int* m_mouseDelta;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};
#endif