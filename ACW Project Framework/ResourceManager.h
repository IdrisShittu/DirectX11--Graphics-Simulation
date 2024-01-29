#pragma once
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include <DDSTextureLoader.h>

using namespace std;
using namespace DirectX;

class ResourceManager
{
public:
	ResourceManager();
	ResourceManager(const ResourceManager& other); // Copy Constructor
	
	~ResourceManager();

	ResourceManager& operator = (const ResourceManager& other); // Copy Assignment Operator
	ResourceManager& operator = (ResourceManager&& other) noexcept; // Move Assignment Operator

	bool GetModel(ID3D11Device* const device, const char* const modelFileName, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer);
	bool GetTexture(ID3D11Device* const device, const WCHAR* const textureFileName, ID3D11ShaderResourceView* &texture);

	int GetSizeOfVertexType() const;
	int GetIndexCount(const char* modelFileName) const;

private:

	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	bool LoadModel(ID3D11Device* const device, const char* const modelFileName);
	void CalculateTangentBinormal(VertexType* tempVertexFace[3]);
	XMFLOAT3 CrossProduct(const XMFLOAT3& a, const XMFLOAT3& b);
	bool CreateBuffer(ID3D11Device* const device, const void* data, UINT dataSize, UINT bindFlags, ID3D11Buffer** buffer);
	void NormalizeVector(XMFLOAT3& vector);
	bool CreateBuffers(ID3D11Device* const device, VertexType* vertices, unsigned long* indices, int vertexCount, int indCount, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);
	bool LoadTexture(ID3D11Device* const device, const WCHAR* textureFileName);

	map<const char*, int> indexCount;
	map<const char*, int> instanceCount;

	map<const char*, ID3D11Buffer*> vertexBuffers;
	map<const char*, ID3D11Buffer*> indexBuffers;

	map<const WCHAR*, ID3D11ShaderResourceView*> textures;
};

