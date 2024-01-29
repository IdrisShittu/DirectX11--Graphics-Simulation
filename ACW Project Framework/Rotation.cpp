#include "Rotation.h"

Rotation::Rotation() {}

Rotation::Rotation(const XMFLOAT3& rotation) {
    rotations.push_back(rotation);
}

Rotation::Rotation(float x, float y, float z) {
    rotations.emplace_back(XMFLOAT3(x, y, z));
}

Rotation::Rotation(const vector<XMFLOAT3>& rot) : rotations(rot) {}

Rotation::Rotation(const Rotation& other) = default;

Rotation::Rotation(Rotation && other) noexcept = default;

Rotation::~Rotation() = default;

const vector<XMFLOAT3>& Rotation::GetRotations() const {
    return rotations;
}

const XMFLOAT3& Rotation::GetRotationAt(int index) const {
    if (index < rotations.size()) {
        return rotations[index];
    }
    static XMFLOAT3 emptyRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    return emptyRotation;
}

void Rotation::AddRotationBack(const XMFLOAT3& rotation) {
    rotations.push_back(rotation);
}

void Rotation::AddRotationBack(float x, float y, float z) {
    rotations.emplace_back(XMFLOAT3(x, y, z));
}

void Rotation::AddRotationFront(const XMFLOAT3& rotation) {
    rotations.insert(rotations.begin(), rotation);
}

void Rotation::AddRotationFront(float x, float y, float z) {
    rotations.insert(rotations.begin(), XMFLOAT3(x, y, z));
}

void Rotation::RemoveRotationBack() {
    if (!rotations.empty()) {
        rotations.pop_back();
    }
}

void Rotation::SetRotationAt(const XMFLOAT3& rotation, const int index) {
    if (index < rotations.size()) {
        rotations[index] = rotation;
    }
}

void Rotation::SetRotationAt(const float x,const float y,const float z, int index) {
    if (index < rotations.size()) {
        rotations[index] = XMFLOAT3(x, y, z);
    }
}
