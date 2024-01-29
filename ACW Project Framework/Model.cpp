#include "Model.h"

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager) : initFailed(false), bufferDescriptionSizeChange(false), updateInstanceBuffer(false), sizeOfVertexType(0), indexCount(0), instanceCount(0), vertexBuffer(nullptr), indexBuffer(nullptr), instanceBuffer(nullptr), instances(nullptr), instanceBufferDescription(nullptr), instanceData(nullptr)
{
	const auto result = resourceManager->GetModel(device, modelFileName, vertexBuffer, indexBuffer);

	if (!result)
	{
		initFailed = true;
		return;
	}

	sizeOfVertexType = resourceManager->GetSizeOfVertexType();
	indexCount = resourceManager->GetIndexCount(modelFileName);
}

Model::Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager, const vector<XMFLOAT3>& scales, const vector<XMFLOAT3>& rotations, const vector<XMFLOAT3>& positions) : Model(device, modelFileName, resourceManager)
{
	instanceCount = positions.size();
	instances = new InstanceType[instanceCount];
	if (!instances)
	{
		delete[] instances;
		instances = nullptr;
		initFailed = true;
		return;
	}

	for (unsigned int i = 0; i < instanceCount; i++) {
		auto worldMatrix = XMMatrixIdentity();
		const auto& position = i < positions.size() ? positions[i] : XMFLOAT3(0.0f, 0.0f, 0.0f);
		const auto& scale = i < scales.size() ? scales[i] : scales.back();
		const auto& rotation = i < rotations.size() ? rotations[i] : rotations.back();
		worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)) *
			XMMatrixTranslation(position.x, position.y, position.z);
		instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	updateInstanceBuffer = true;
	instanceBufferDescription = make_shared<D3D11_BUFFER_DESC>();
	instanceBufferDescription->Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDescription->ByteWidth = sizeof(InstanceType) * instanceCount;
	instanceBufferDescription->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDescription->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDescription->MiscFlags = 0;
	instanceBufferDescription->StructureByteStride = 0;
	const auto result = device->CreateBuffer(instanceBufferDescription.get(), nullptr, &instanceBuffer);
	if (FAILED(result))
	{
		delete[] instances;
		instances = nullptr;
		initFailed = true;
		return;
	}
}

Model::Model(const Model& other) = default;

Model::Model(Model&& other) noexcept = default;

Model::~Model() {
	if (instances) {
		delete[] instances;
	}
	if (instanceBuffer) {
		instanceBuffer->Release();
	}
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
}

Model& Model::operator=(const Model& other) = default;

Model& Model::operator=(Model&& other) noexcept = default;

void Model::Update(const vector<XMFLOAT3>& scales, const vector<XMFLOAT3>& rotations, const vector<XMFLOAT3>& positions, const XMMATRIX& parentMatrix) {
	if (instanceCount != positions.size()) {
		instanceCount = positions.size();
		delete[] instances;
		instances = new InstanceType[instanceCount];
		bufferDescriptionSizeChange = true;
	}

	for (unsigned int i = 0; i < instanceCount; i++) {
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMFLOAT3 scale = i < scales.size() ? scales[i] : scales.back();
		XMFLOAT3 rotation = i < rotations.size() ? rotations[i] : rotations.back();
		XMFLOAT3 position = positions[i];

		worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)) *
			XMMatrixTranslation(position.x, position.y, position.z) *
			parentMatrix;

		instances[i].worldMatrix = XMMatrixTranspose(worldMatrix);
	}

	updateInstanceBuffer = true;
}

bool Model::Render(ID3D11DeviceContext* const deviceContext) {
	if (updateInstanceBuffer) {
		if (bufferDescriptionSizeChange) {
			ID3D11Device* device;
			deviceContext->GetDevice(&device);
			instanceBuffer->Release();
			instanceBufferDescription->ByteWidth = sizeof(InstanceType) * instanceCount;
			if (FAILED(device->CreateBuffer(instanceBufferDescription.get(), nullptr, &instanceBuffer))) {
				return false;
			}
			bufferDescriptionSizeChange = false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
			return false;
		}
		memcpy(mappedResource.pData, instances, instanceCount * sizeof(InstanceType));
		deviceContext->Unmap(instanceBuffer, 0);
		updateInstanceBuffer = false;
	}

	unsigned int strides[] = { sizeOfVertexType, sizeof(InstanceType) };
	unsigned int offsets[] = { 0, 0 };
	ID3D11Buffer* bufferPointers[] = { vertexBuffer, instanceBuffer };
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	return true;
}

int Model::GetIndexCount() const {
	return indexCount;
}

int Model::GetInstanceCount() const
{
	return instanceCount;
}

bool Model::GetInitializationState() const {
	return initFailed;
}

