#pragma once

class InputManager
{
public:
	InputManager(); // Default Constructor
	InputManager(const InputManager& other); // Copy Constructor
	InputManager(InputManager && other) noexcept; // Move Constructor
	virtual ~InputManager(); // Destructor

	InputManager& operator = (const InputManager& other); // Copy Assignment Operator
	InputManager& operator = (InputManager&& other) noexcept; // Move Assignment Operator

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);

	bool IsKeyDown(unsigned int key) const;
	bool IsKeyUp(unsigned int key) const;

	bool DoOnce() const;
	void ToggleDoOnce(const bool toggle);

private:
	bool m_doOnce;
	bool m_keys[256];
};