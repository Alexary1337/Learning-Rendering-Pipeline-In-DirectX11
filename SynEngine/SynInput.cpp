#include "syninput.h"

SynInput::SynInput()
{
	SAFE_INIT(m_directInput);
	SAFE_INIT(m_keyboard);
	SAFE_INIT(m_mouse);
	SAFE_INIT(m_mouseDelta);
}

SynInput::SynInput(const SynInput& other)
{

}

SynInput::~SynInput()
{

}

bool SynInput::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_mouseDelta = new int[2];

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	m_F1_released = true;
	m_F2_released = true;
	m_F3_released = true;

	return true;

}

void SynInput::Shutdown()
{
	SAFE_DELETE(m_mouseDelta);

	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	SAFE_RELEASE(m_directInput);

	return;
}

int* SynInput::Frame(bool canRotate)
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	SAFE_CHECKEXIST(result);

	// Read the current state of the mouse.
	result = ReadMouse();
	SAFE_CHECKEXIST(result);

	if (canRotate)
	{
		// Process the changes in the mouse and keyboard.
		ProcessInput();
		SetCursorPos(m_screenWidth / 2, m_screenHeight / 2);
	}
	
	return m_mouseDelta;
}

bool SynInput::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool SynInput::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void SynInput::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	m_mouseDelta[0] = m_mouseX - m_mouseState.lX;
	m_mouseDelta[1] = m_mouseY - m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	//if (m_mouseX < 0)  { m_mouseX = 0; }
	//if (m_mouseY < 0)  { m_mouseY = 0; }

	//if (m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	//if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

bool SynInput::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void SynInput::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool SynInput::IsKeyPressed(unsigned int key)
{
	return m_keyboardState[key];
}

int SynInput::GetMouseXLocation()
{	
	return m_mouseX;
}

int SynInput::GetMouseYLocation()
{
	return m_mouseY;
}

bool SynInput::IsF1Toggled()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_F1] & 0x80)
	{
		if (m_F1_released)
		{
			m_F1_released = false;
			return true;
		}
	}
	else
	{
		m_F1_released = true;
	}

	return false;
}

bool SynInput::IsF2Toggled()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_F2] & 0x80)
	{
		if (m_F2_released)
		{
			m_F2_released = false;
			return true;
		}
	}
	else
	{
		m_F2_released = true;
	}

	return false;
}

bool SynInput::IsF3Toggled()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_F3] & 0x80)
	{
		if (m_F3_released)
		{
			m_F3_released = false;
			return true;
		}
	}
	else
	{
		m_F3_released = true;
	}

	return false;
}

bool SynInput::IsEToggled()
{
	if (m_keyboardState[DIK_E] & 0x80)
	{
		if (m_E_released)
		{
			m_E_released = false;
			return true;
		}
	}
	else
	{
		m_E_released = true;
	}

	return false;
}