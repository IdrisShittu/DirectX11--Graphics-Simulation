#pragma once
#include "Camera.h"
#include <cmath>

Camera::Camera() : position(XMFLOAT3(0, 0, 0)), rotation(XMFLOAT3(0, 0, 0)) {}

Camera::Camera(const Camera& other) = default;

Camera::Camera(Camera && other) = default;

Camera::~Camera() = default;

Camera& Camera::operator=(const Camera & other) = default;

XMFLOAT3 Camera::GetPosition() const { return position; }

XMFLOAT3 Camera::GetRotation() const { return rotation; }

void Camera::SetPosition(const XMFLOAT3 & newPosition) { position = newPosition; }

void Camera::SetPosition(const float x, const float y, const float z) { position = XMFLOAT3(x, y, z); }

void Camera::SetRotation(const float x, const float y, const float z) { rotation = XMFLOAT3(x, y, z); }

void Camera::AddPositionX(const float x) { position.x += x; }

void Camera::AddPositionY(const float y) { position.y += y; }

void Camera::AddPositionZ(const float z) { position.z += z; }

void Camera::AddRotationX(const float x) { rotation.x += x; }

void Camera::AddRotationY(const float y) { rotation.y += y; }

void Camera::AddRotationZ(const float z) { rotation.z += z; }

void Camera::GetViewMatrix(XMMATRIX & viewMat) const { viewMat = XMLoadFloat4x4(&viewMatrix); }

void Camera::Render() {
    const XMVECTOR positionVector = XMLoadFloat3(&position);
    const float yaw = XMConvertToRadians(rotation.x);
    const float pitch = XMConvertToRadians(rotation.y);
    const float roll = XMConvertToRadians(rotation.z);

    const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    const XMVECTOR upVector = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);
    XMVECTOR lookAtVector = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

    lookAtVector = XMVectorAdd(positionVector, lookAtVector);
    XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(positionVector, lookAtVector, upVector));
}
