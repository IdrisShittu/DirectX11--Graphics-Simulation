#include "Scale.h"

Scale::Scale() {
}

Scale::Scale(const XMFLOAT3& scale) {
    scales.push_back(scale);
}

Scale::Scale(const float x, const float y, const float z) {
    scales.emplace_back(XMFLOAT3(x, y, z));
}

Scale::Scale(const vector<XMFLOAT3>& sc)
    : scales(sc) {
}

Scale::Scale(const Scale& other) = default;

Scale::Scale(Scale && other) noexcept = default;

Scale::~Scale() = default;

Scale& Scale::operator=(const Scale & other) = default;

const vector<XMFLOAT3>& Scale::GetScales() const {
    return scales;
}

const XMFLOAT3& Scale::GetScaleAt(const int index) const {
    return scales.at(index);
}

void Scale::AddScaleBack(const XMFLOAT3& scale) {
    scales.push_back(scale);
}

void Scale::AddScaleBack(const float x, const float y, const float z) {
    scales.emplace_back(XMFLOAT3(x, y, z));
}

void Scale::AddScaleFront(const XMFLOAT3& scale) {
    scales.insert(scales.begin(), scale);
}

void Scale::AddScaleFront(const float x, const float y, const float z) {
    scales.insert(scales.begin(), XMFLOAT3(x, y, z));
}

void Scale::RemoveScaleBack() {
    if (!scales.empty()) {
        scales.pop_back();
    }
}

void Scale::SetScaleAt(const XMFLOAT3& newScale, const int index) {
    if (index >= 0 && index < scales.size()) {
        scales[index] = newScale;
    }
}

void Scale::SetScaleAt(const float x, const float y, const float z, const int index) {
    if (index >= 0 && index < scales.size()) {
        scales[index] = XMFLOAT3(x, y, z);
    }
}

void Scale::IncreaseAllScales(const XMFLOAT3& scalar) {
    for (auto& scale : scales) {
        scale.x += scalar.x;
        scale.y += scalar.y;
        scale.z += scalar.z;
    }
}

void Scale::IncreaseAllScales(const float x, const float y, const float z) {
    for (auto& scale : scales) {
        scale.x += x;
        scale.y += y;
        scale.z += z;
    }
}

void Scale::IncreaseScaleAt(const XMFLOAT3& scalar, const int index) {
    if (index >= 0 && index < scales.size()) {
        scales[index].x += scalar.x;
        scales[index].y += scalar.y;
        scales[index].z += scalar.z;
    }
}

void Scale::IncreaseScaleAt(const float x, const float y, const float z, const int index) {
    if (index >= 0 && index < scales.size()) {
        scales[index].x += x;
        scales[index].y += y;
        scales[index].z += z;
    }
}
