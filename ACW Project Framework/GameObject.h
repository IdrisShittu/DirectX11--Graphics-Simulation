#pragma once

#include "Model.h"
#include "Texture.h"
#include "Position.h"
#include "Rotation.h"
#include "Scale.h"
#include "RigidBody.h"
#include "Shader.h"
#include "Light.h"

using namespace std;
using namespace DirectX;

enum class ModelType {
	Sphere,
	SphereInverted,
	HighPolyCube,
	LowPolyCube,
	Plane,
	HighPolyCylinder,
	LowPolyCylinder,
	Cone,
	Quad
};

class GameObject
{
public:

	GameObject(); 
	virtual ~GameObject(); 
	void AddPositionComponent();
	void AddPositionComponent(const XMFLOAT3& position);
	void AddPositionComponent(const float x, const float y, const float z);
	void AddPositionComponent(const vector<XMFLOAT3>& positions);
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const vector<XMFLOAT3>& positions);
	void AddRotationComponent();
	void AddRotationComponent(const XMFLOAT3& rotation);
	void AddRotationComponent(const float x, const float y, const float z);
	void AddRotationComponent(const vector<XMFLOAT3>& rotations);
	void SetRotation(const XMFLOAT3& rotation);
	void SetRotation(const float x, const float y, const float z);
	void SetRotation(const vector<XMFLOAT3>& rotations);
	void AddScaleComponent();
	void AddScaleComponent(const XMFLOAT3& scale);
	void AddScaleComponent(const float x, const float y, const float z);
	void AddScaleComponent(const vector<XMFLOAT3>& scales);
	void SetScale(const XMFLOAT3& scale);
	void SetScale(const float x, const float y, const float z);
	void SetScale(const vector<XMFLOAT3>& scales);
	void AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag);
	void AddModelComponent(ID3D11Device* const device, const ModelType modelType, const shared_ptr<ResourceManager>& resourceManager);
	const char* GetModelFileName(ModelType modelType) const;
	void AddTextureComponent(ID3D11Device* const device, const vector<const WCHAR*>& textureFileNames, const shared_ptr<ResourceManager>& resourceManager);
	void SetShaderComponent(const shared_ptr<Shader>& shader);
	void AddParentGameObject(const shared_ptr<GameObject>& parentObject);
	void SetTessellationVariables(const float& maxTessellationDistance, const float& minTessellationDistance, const float& maxTessellationFactor, const float& minTessellationFactor);
	void SetDisplacementVariables(const float &mipInterval, const float &mipClampMinimum, const float &mipClampMaximum, const float &displacementPower);
	const shared_ptr<Position>& GetPositionComponent() const;
	const shared_ptr<Rotation>& GetRotationComponent() const;
	const shared_ptr<Scale>& GetScaleComponent() const;
	const shared_ptr<RigidBody>& GetRigidBodyComponent() const;
	const shared_ptr<Model>& GetModelComponent() const;
	const shared_ptr<Shader>& GetShaderComponent() const;
	int GetIndexCount() const;
	const vector<ID3D11ShaderResourceView*>& GetTextureList() const;
	bool GetInitializationState() const;
	void UpdateInstanceData();
	bool Update();
	bool Render(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const;

private:

	bool initFailed;
	bool updateInstanceData;

	float maxTessellationDistance;
	float minTessellationDistance;
	float maxTessellationFactor;
	float minTessellationFactor;

	float mipInterval;
	float mipClampMinimum;
	float mipClampMaximum;
	float displacementPower;

	shared_ptr<Position> position;
	shared_ptr<Rotation> rotation;
	shared_ptr<Scale> scale;
	shared_ptr<RigidBody> rigidBody;
	shared_ptr<Model> model;
	shared_ptr<Texture> texture;
	shared_ptr<Shader> shader;
	shared_ptr<GameObject> parentObject;
};

