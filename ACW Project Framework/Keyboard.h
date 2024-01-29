#pragma once

#include <array>

class Keyboard {
public:
    Keyboard();
    ~Keyboard() = default;

    Keyboard(const Keyboard& other) = default;
    Keyboard(Keyboard&& other) noexcept = default;
    Keyboard& operator=(const Keyboard& other) = default;
    Keyboard& operator=(Keyboard&& other) noexcept = default;

    void SetKeyPressed(unsigned int key);
    void SetKeyReleased(unsigned int key);
    bool IsKeyPressed(unsigned int key) const;
    bool IsKeyReleased(unsigned int key) const;
    bool CanProcessKey() const;
    void SetCanProcessKey(bool canProcess);

private:
    std::array<bool, 256> keyStates{};
    bool canProcessKey{ true };
};
