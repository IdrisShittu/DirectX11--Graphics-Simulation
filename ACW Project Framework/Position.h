#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>

using namespace DirectX;
using namespace std;

class Position
{
public:
	Position();
	explicit Position(const XMFLOAT3& position);
	Position(float x, float y, float z);
	explicit Position(const vector<XMFLOAT3>& positions);
	Position(const Position& other); 
	Position(Position&& other) noexcept; 
	~Position();

	Position& operator = (const Position& other); 
	Position& operator = (Position&& other) noexcept; 

	const vector<XMFLOAT3>& GetPositions() const;
	const XMFLOAT3& GetPositionAt(const int index) const;

	void AddPositionBack(const XMFLOAT3& position);
	void AddPositionBack(const float x, const float y, const float z);

	void AddPositionFront(const XMFLOAT3& position);
	void AddPositionFront(const float x, const float y, const float z);

	void RemovePositionBack();

	void SetPositionAt(const XMFLOAT3& newPosition, const int index);
	void SetPositionAt(const float x, const float y, const float z, const int index);

	void TranslateAllPositions(const XMFLOAT3& translate);
	void TranslateAllPositions(const float x, const float y, const float z);

	void TranslatePositionAt(const XMFLOAT3& translate, const int index);
	void TranslatePositionAt(const float x, const float y, const float z, const int index);

private:
	vector<XMFLOAT3> positions;
};

