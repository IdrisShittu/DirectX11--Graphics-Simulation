#include "Keyboard.h"

Keyboard::Keyboard() : canProcessKey(true), keyStates{ false }{}

void Keyboard::SetKeyPressed(unsigned int key) {
    keyStates[key] = true;
}

void Keyboard::SetKeyReleased(unsigned int key) {
    keyStates[key] = false;
}

bool Keyboard::IsKeyPressed(unsigned int key) const {
    return keyStates[key];
}

bool Keyboard::IsKeyReleased(unsigned int key) const {
    return !keyStates[key];
}

bool Keyboard::CanProcessKey() const {
    return canProcessKey;
}

void Keyboard::SetCanProcessKey(bool canProcess) {
    canProcessKey = canProcess;
}
