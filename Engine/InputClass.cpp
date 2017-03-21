#include "inputclass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& aInputClass)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i;
	// Initialize all the keys to being released and not pressed.
	for (i = 0; i<256; i++)
	{
		myKeys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	myKeys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	myKeys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return myKeys[key];
}
