#include "Position.h"

Position::Position() {}

Position::Position(const XMFLOAT3& position) : positions(1, position) {}

Position::Position(float x, float y, float z) : positions(1, XMFLOAT3(x, y, z)) {}

Position::Position(const vector<XMFLOAT3>& pos) : positions(pos) {}

Position::Position(const Position& other) = default;

Position::Position(Position && other) noexcept = default;

Position::~Position() = default;

Position& Position::operator=(const Position & other) = default;

const vector<XMFLOAT3>& Position::GetPositions() const {
	return positions;
}

const XMFLOAT3& Position::GetPositionAt(const int index) const {
	return positions[index];
}

void Position::AddPositionBack(const XMFLOAT3 & position) {
	positions.push_back(position);
}

void Position::AddPositionBack(float x, float y, float z) {
	positions.emplace_back(x, y, z);
}

void Position::AddPositionFront(const XMFLOAT3 & position)
{
	positions.insert(positions.begin(), position);
}

void Position::AddPositionFront(const float x, const float y, const float z)
{
	positions.insert(positions.begin(), XMFLOAT3(x, y, z));
}

void Position::RemovePositionBack() {
	if (!positions.empty()) {
		positions.pop_back();
	}
}

void Position::SetPositionAt(const XMFLOAT3 & newPosition, const int index) {
	if (index >= 0 && index < positions.size()) {
		positions[index] = newPosition;
	}
}

void Position::SetPositionAt(const float x, const float y, const float z, const int index) {
	if (index >= 0 && index < positions.size()) {
		positions[index] = XMFLOAT3(x, y, z);
	}
}

void Position::TranslateAllPositions(const XMFLOAT3 & translate)
{
	for (auto position : positions)
	{
		position = XMFLOAT3(position.x + translate.x, position.y + translate.y, position.z + translate.z);
	}
}

void Position::TranslateAllPositions(const float x, const float y, const float z)
{
	for (auto position : positions)
	{
		position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	}
}

void Position::TranslatePositionAt(const XMFLOAT3 & translate, const int index)
{
	positions[index] = XMFLOAT3(positions[index].x + translate.x, positions[index].y + translate.y, positions[index].z + translate.z);
}

void Position::TranslatePositionAt(const float x, const float y, const float z, const int index)
{
	positions[index] = XMFLOAT3(positions[index].x + x, positions[index].y + y, positions[index].z + z);
}
