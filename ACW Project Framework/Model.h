#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>

#include "Texture.h"
#include "ResourceManager.h"

using namespace DirectX;
using namespace std;

class Model
{
public:
	Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager);
	Model(ID3D11Device* const device, const char* const modelFileName, const shared_ptr<ResourceManager>& resourceManager, const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions);

	Model(const Model& other); 
	Model(Model && other) noexcept; 
	~Model(); 

	Model& operator = (const Model& other);
	Model& operator = (Model&& other) noexcept; 

	void Update(const vector<XMFLOAT3> &scales, const vector<XMFLOAT3> &rotations, const vector<XMFLOAT3> &positions, const XMMATRIX& parentMatrix);
	bool Render(ID3D11DeviceContext* const deviceContext);

	int GetIndexCount() const;
	int GetInstanceCount() const;

	bool GetInitializationState() const;

private:
	struct InstanceType
	{
		XMMATRIX worldMatrix;
	};

	bool initFailed;
	bool bufferDescriptionSizeChange = false;
	bool updateInstanceBuffer;

	int sizeOfVertexType = 0;

	int indexCount = 0;
	int instanceCount;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* instanceBuffer = nullptr;

	InstanceType* instances = nullptr;

	shared_ptr<D3D11_BUFFER_DESC> instanceBufferDescription;
	shared_ptr<D3D11_SUBRESOURCE_DATA> instanceData;
};