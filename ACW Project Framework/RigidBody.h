#pragma once
class RigidBody
{
public:
	RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag);
	RigidBody(const RigidBody& other); 
	RigidBody(RigidBody&& other) noexcept; 
	~RigidBody(); 

	RigidBody& operator = (const RigidBody& other); 
	RigidBody& operator = (RigidBody&& other) noexcept; 

	bool GetUseGravity() const;
	float GetMass() const;
	float GetDrag() const;
	float GetAngularDrag() const;

	void SetUseGravity(const bool useGravity);
	void SetMass(const float mass);
	void SetDrag(const float drag);
	void SetAngularDrag(const float angularDrag);

private:
	bool useGravity;
	float mass;
	float drag;
	float angularDrag;
};

