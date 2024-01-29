#include "GameObject.h"

GameObject::GameObject() : initFailed(false), updateInstanceData(false), maxTessellationDistance(1.0f), minTessellationDistance(1.0f), maxTessellationFactor(1.0f), minTessellationFactor(1.0f), mipInterval(0.0f), mipClampMinimum(0.0f), mipClampMaximum(0.0f), displacementPower(0.0f), position(nullptr), rotation(nullptr), scale(nullptr), rigidBody(nullptr), model(nullptr), texture(nullptr), shader(nullptr), parentObject(nullptr)
{}

GameObject::~GameObject()
{

}

void GameObject::AddPositionComponent() {
	position = make_shared<Position>();
}

void GameObject::AddPositionComponent(const XMFLOAT3& pos) {
	position = make_shared<Position>(pos);
}

void GameObject::AddPositionComponent(const float x, const float y, const float z) {
	position = make_shared<Position>(x, y, z);
}

void GameObject::AddPositionComponent(const vector<XMFLOAT3>& poss) {
	position = make_shared<Position>(poss);
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	position->SetPositionAt(pos, 0);
	updateInstanceData = true;
}

void GameObject::SetPosition(const float x, const float y, const float z)
{
	position->SetPositionAt(x, y, z, 0);
	updateInstanceData = true;
}

void GameObject::SetPosition(const vector<XMFLOAT3>& positions)
{
	const auto& currentPositions = position->GetPositions();
	size_t count = min(positions.size(), currentPositions.size());
	for (size_t i = 0; i < count; ++i)position->SetPositionAt(positions[i], i);
	updateInstanceData = true;
}

void GameObject::AddRotationComponent() {
	rotation = make_shared<Rotation>();
}

void GameObject::AddRotationComponent(const XMFLOAT3& r) {
	rotation = make_shared<Rotation>(r);
}

void GameObject::AddRotationComponent(const float x, const float y, const float z) {
	rotation = make_shared<Rotation>(x, y, z);
}

void GameObject::AddRotationComponent(const vector<XMFLOAT3>& rots)
{
	rotation = make_shared<Rotation>(rots);
}

void GameObject::SetRotation(const XMFLOAT3& r)
{
	rotation->SetRotationAt(r, 0);
	updateInstanceData = true;
}

void GameObject::SetRotation(const float x, const float y, const float z)
{
	rotation->SetRotationAt(x, y, z, 0);
	updateInstanceData = true;
}

void GameObject::SetRotation(const vector<XMFLOAT3>& rotations)
{
	const auto& currentRotations = rotation->GetRotations();
	size_t count = min(rotations.size(), currentRotations.size());
	for (size_t i = 0; i < count; ++i)rotation->SetRotationAt(rotations[i], i);
	updateInstanceData = true;
}

void GameObject::AddScaleComponent() {
	scale = make_shared<Scale>();
}

void GameObject::AddScaleComponent(const XMFLOAT3& sc) {
	scale = make_shared<Scale>(sc);
}

void GameObject::AddScaleComponent(const float x, const float y, const float z) {
	scale = make_shared<Scale>(x, y, z);
}

void GameObject::AddScaleComponent(const vector<XMFLOAT3>& scales)
{
	scale = make_shared<Scale>(scales);
}

void GameObject::SetScale(const XMFLOAT3& sc)
{
	scale->SetScaleAt(sc, 0);
	updateInstanceData = true;
}

void GameObject::SetScale(const float x, const float y, const float z)
{
	scale->SetScaleAt(x, y, z, 0);
	updateInstanceData = true;
}

void GameObject::SetScale(const vector<XMFLOAT3>& scales)
{
	const auto& currentScales = scale->GetScales();
	size_t count = min(scales.size(), currentScales.size());

	for (size_t i = 0; i < count; ++i)scale->SetScaleAt(scales[i], i);
	updateInstanceData = true;
}

void GameObject::AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag) {
	rigidBody = make_shared<RigidBody>(useGravity, mass, drag, angularDrag);
}

void GameObject::AddModelComponent(ID3D11Device* const device, ModelType modelType, const shared_ptr<ResourceManager>& resourceManager)
{
	const char* modelFileName = GetModelFileName(modelType);
	model = make_shared<Model>(device, modelFileName, resourceManager, scale->GetScales(), rotation->GetRotations(), position->GetPositions());
}

const char* GameObject::GetModelFileName(ModelType modelType) const
{
	switch (modelType)
	{
	case ModelType::Sphere:            return "sphere.obj";
	case ModelType::SphereInverted:    return "SphereInverted.obj";
	case ModelType::HighPolyCube:      return "cubeHigh.obj";
	case ModelType::LowPolyCube:       return "cubeLow.obj";
	case ModelType::Plane:             return "plane.obj";
	case ModelType::HighPolyCylinder:  return "cylinderHigh.obj";
	case ModelType::LowPolyCylinder:   return "cylinderLow.obj";
	case ModelType::Cone:              return "cone.obj";
	case ModelType::Quad:              return "quad.obj";
	default:                           return nullptr;
	}
}

void GameObject::AddTextureComponent(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager) {
	texture = make_shared<Texture>(device, textureFileNames, resourceManager);
}

void GameObject::SetShaderComponent(const shared_ptr<Shader>& sh) {
	shader = sh;
}

void GameObject::AddParentGameObject(const shared_ptr<GameObject>& ptObject)
{
	parentObject = ptObject;
	updateInstanceData = true;
}

void GameObject::SetTessellationVariables(const float& mxTessellationDistance, const float& mnTessellationDistance, const float& mxTessellationFactor, const float& mnTessellationFactor)
{
	maxTessellationDistance = mxTessellationDistance;
	minTessellationDistance = mnTessellationDistance;
	maxTessellationFactor = mxTessellationFactor;
	minTessellationFactor = mnTessellationFactor;
}

void GameObject::SetDisplacementVariables(const float& mipInterv, const float& mipClampMin, const float& mipClampMax, const float& displacementPow)
{
	mipInterval = mipInterv;
	mipClampMinimum = mipClampMin;
	mipClampMaximum = mipClampMax;
	displacementPower = displacementPow;
}

const shared_ptr<Position>& GameObject::GetPositionComponent() const {
	return position;
}

const shared_ptr<Rotation>& GameObject::GetRotationComponent() const {
	return rotation;
}

const shared_ptr<Scale>& GameObject::GetScaleComponent() const {
	return scale;
}

const shared_ptr<RigidBody>& GameObject::GetRigidBodyComponent() const {
	return rigidBody;
}

const shared_ptr<Model>& GameObject::GetModelComponent() const
{
	return model;
}

const shared_ptr<Shader>& GameObject::GetShaderComponent() const
{
	return shader;
}

int GameObject::GetIndexCount() const {
	return model->GetIndexCount();
}

const vector<ID3D11ShaderResourceView*>& GameObject::GetTextureList() const {
	return texture->GetTextureList();
}

bool GameObject::GetInitializationState() const {
	return initFailed;
}

void GameObject::UpdateInstanceData()
{
	updateInstanceData = true;
}

bool GameObject::Update()
{
	if (updateInstanceData || parentObject)
	{
		if (model)
		{
			XMMATRIX parentObjectMatrix = XMMatrixIdentity();

			if (parentObject)
			{
				const auto scale = parentObject->GetScaleComponent()->GetScaleAt(0);
				const auto rotation = parentObject->GetRotationComponent()->GetRotationAt(0);
				const auto position = parentObject->GetPositionComponent()->GetPositionAt(0);
				parentObjectMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
				parentObjectMatrix *= XMMatrixTranslation(position.x, position.y, position.z);
			}
			model->Update(scale->GetScales(), rotation->GetRotations(), position->GetPositions(), parentObjectMatrix);
		}
		updateInstanceData = false;
	}
	return true;
}

bool GameObject::Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
	model->Render(deviceContext);
	shader->SetTessellationVariables(maxTessellationDistance, minTessellationDistance, maxTessellationFactor, minTessellationFactor);
	shader->SetDisplacementVariables(mipInterval, mipClampMinimum, mipClampMaximum, displacementPower * scale->GetScaleAt(0).x);
	return shader->Render(deviceContext, GetIndexCount(), model->GetInstanceCount(), viewMatrix, projectionMatrix, GetTextureList(), depthTextures, pointLightList, cameraPosition);
}

