#include "syninput.h"

SynInput::SynInput()
{

}

SynInput::SynInput(const SynInput& other)
{

}

SynInput::~SynInput()
{

}

void SynInput::Initialize()
{
	int i;

	// Initialize all the keys to being released and not pressed.
	for (i = 0; i<256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void SynInput::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}

void SynInput::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}

bool SynInput::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}