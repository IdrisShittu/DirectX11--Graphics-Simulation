#include "RigidBody.h"

RigidBody::RigidBody(bool useGravity, float mass, float drag, float angularDrag)
    : useGravity(useGravity), mass(mass), drag(drag), angularDrag(angularDrag) {}

RigidBody::RigidBody(const RigidBody& other) = default;
RigidBody::RigidBody(RigidBody && other) noexcept = default;
RigidBody::~RigidBody() = default;
RigidBody& RigidBody::operator=(const RigidBody & other) = default;
RigidBody& RigidBody::operator=(RigidBody && other) noexcept = default;

bool RigidBody::GetUseGravity() const {
    return useGravity;
}

float RigidBody::GetMass() const {
    return mass;
}

float RigidBody::GetDrag() const {
    return drag;
}

float RigidBody::GetAngularDrag() const {
    return angularDrag;
}

void RigidBody::SetUseGravity(bool useGravity) {
    this->useGravity = useGravity;
}

void RigidBody::SetMass(float mass) {
    this->mass = mass;
}

void RigidBody::SetDrag(float drag) {
    this->drag = drag;
}

void RigidBody::SetAngularDrag(float angularDrag) {
    this->angularDrag = angularDrag;
}
