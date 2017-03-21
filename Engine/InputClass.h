#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass& aInputClass);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool myKeys[256];
};
