#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>

using namespace DirectX;
using namespace std;

class Rotation
{
public:
	Rotation(); 
	explicit Rotation(const XMFLOAT3& rotation);
	Rotation(const float x, const float y, const float z);
	explicit Rotation(const vector<XMFLOAT3>& rotations);
	Rotation(const Rotation& other); 
	Rotation(Rotation&& other) noexcept; 
	~Rotation();

	const vector<XMFLOAT3>& GetRotations() const;
	const XMFLOAT3& GetRotationAt(const int index) const;

	void AddRotationBack(const XMFLOAT3& rotation);
	void AddRotationBack(const float x, const float y, const float z);

	void AddRotationFront(const XMFLOAT3& rotation);
	void AddRotationFront(const float x, const float y, const float z);

	void RemoveRotationBack();

	void SetRotationAt(const XMFLOAT3& rotation, const int index);
	void SetRotationAt(const float x, const float y, const float z, const int index);

private:
	vector<XMFLOAT3> rotations;

};

