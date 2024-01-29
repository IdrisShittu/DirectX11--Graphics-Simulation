#include "ResourceManager.h"
#include <iostream>
#include <string>
#include <sstream>

ResourceManager::ResourceManager(){}

ResourceManager::ResourceManager(const ResourceManager& other) = default;

ResourceManager::~ResourceManager() {
	auto releaseResources = [](auto& resources) {
		for (auto& resource : resources) {
			if (resource.second) {
				resource.second->Release();
				resource.second = nullptr;
			}
		}
	};
	releaseResources(textures);
	releaseResources(indexBuffers);
	releaseResources(vertexBuffers);
}

ResourceManager& ResourceManager::operator=(const ResourceManager& other) = default;

ResourceManager& ResourceManager::operator=(ResourceManager&& other) noexcept = default;

bool ResourceManager::GetModel(ID3D11Device* const device, const char* const modelFileName, ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer) {
	if (vertexBuffers.count(modelFileName) == 0 && !LoadModel(device, modelFileName)) return false;
	vertexBuffer = vertexBuffers[modelFileName];
	indexBuffer = indexBuffers[modelFileName];
	return true;
}

bool ResourceManager::GetTexture(ID3D11Device* const device, const WCHAR* const textureFileName, ID3D11ShaderResourceView*& texture) {
	if (textures.count(textureFileName) == 0 && !LoadTexture(device, textureFileName)) {
		return false;
	}
	texture = textures[textureFileName];
    return true;
}

int ResourceManager::GetSizeOfVertexType() const {
	return sizeof(VertexType);
}

int ResourceManager::GetIndexCount(const char* const modelFileName) const {
	return indexCount.at(modelFileName);
}

bool ResourceManager::LoadModel(ID3D11Device* const device, const char* const modelFileName)
{
	ifstream fin;
	fin.open(modelFileName);

	vector<XMFLOAT3> positions;
	vector<XMFLOAT2> textures;
	vector<XMFLOAT3> normals;
	char cmd[256] = { 0 };
	auto faceCount = 0, vertexCount = 0, indCount = 0;
	VertexType* vertices = nullptr;
	unsigned long* indices = nullptr;

	while (!fin.eof())
	{
		float x, y, z;
		fin >> cmd;
		if (0 == strcmp(cmd, "faces"))
		{
			fin >> faceCount;
			vertices = new VertexType[faceCount * 3];
			vertexCount = faceCount * 3;
			indices = new unsigned long[faceCount * 3];
			indCount = faceCount * 3;
		}
		if (0 == strcmp(cmd, "v"))
		{
			fin >> x >> y >> z;
			positions.emplace_back(XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "vn"))
		{
			fin >> x >> y >> z;
			normals.emplace_back(XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "vt"))
		{
			fin >> x >> y >> z;
			textures.emplace_back(XMFLOAT2(x, y));
		}
		else if (0 == strcmp(cmd, "f"))
		{
			int value;
			auto count = 0;
			while (0 == strcmp(cmd, "f"))
			{
				VertexType* tempVertexFace[3];
				for (auto i = 0; i < 3; i++)
				{
					fin >> value;
					vertices[count].position = positions[value - 1];
					fin.ignore();
					fin >> value;
					vertices[count].texture = textures[value - 1];
					fin.ignore();
					fin >> value;
					vertices[count].normal = normals[value - 1];
					fin.ignore();
					indices[count] = count;
					tempVertexFace[i] = &vertices[count];
					count++;
				}

				CalculateTangentBinormal(tempVertexFace);
				fin >> cmd;
			}
		}
	}


	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	if (!CreateBuffers(device, vertices, indices, vertexCount, indCount, &vertexBuffer, &indexBuffer)) {
		delete[] vertices;
		delete[] indices;
		return false;
	}

	indexCount[modelFileName] = indCount;
	vertexBuffers[modelFileName] = vertexBuffer;
	indexBuffers[modelFileName] = indexBuffer;

	delete[] vertices;
	delete[] indices;

	return true;
}

void ResourceManager::CalculateTangentBinormal(VertexType* tempVertexFace[3]) {
	auto subtract3 = [](const XMFLOAT3& a, const XMFLOAT3& b) -> XMFLOAT3 {return { a.x - b.x, a.y - b.y, a.z - b.z };};
	auto subtract2 = [](const XMFLOAT2& a, const XMFLOAT2& b) -> XMFLOAT2 {return { a.x - b.x, a.y - b.y };};

	XMFLOAT3 edge1 = subtract3(tempVertexFace[1]->position, tempVertexFace[0]->position);
	XMFLOAT3 edge2 = subtract3(tempVertexFace[2]->position, tempVertexFace[0]->position);
	XMFLOAT2 deltaUV1 = subtract2(tempVertexFace[1]->texture, tempVertexFace[0]->texture);
	XMFLOAT2 deltaUV2 = subtract2(tempVertexFace[2]->texture, tempVertexFace[0]->texture);

	float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
	float invDet = 1.0f / (det == 0.0f ? 1.0f : det);  

	XMFLOAT3 tangent = {
		invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
		invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
		invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z),
	};
	XMFLOAT3 binormal = {
		invDet * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x),
		invDet * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y),
		invDet * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z),
	};

	NormalizeVector(tangent);
	NormalizeVector(binormal);

	XMFLOAT3 normal = CrossProduct(tangent, binormal);
	NormalizeVector(normal);

	for (int i = 0; i < 3; ++i) {
		tempVertexFace[i]->tangent = tangent;
		tempVertexFace[i]->binormal = binormal;
		tempVertexFace[i]->normal = normal;
	}
}

XMFLOAT3 ResourceManager::CrossProduct(const XMFLOAT3& a, const XMFLOAT3& b) {
	XMFLOAT3 result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

void ResourceManager::NormalizeVector(XMFLOAT3& vector) {
	float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (length != 0.0f) {
		vector.x /= length;
		vector.y /= length;
		vector.z /= length;
	}
}

bool ResourceManager::CreateBuffer(ID3D11Device* const device, const void* data, UINT dataSize, UINT bindFlags, ID3D11Buffer** buffer) {
	D3D11_BUFFER_DESC bufferDesc = {dataSize,D3D11_USAGE_DEFAULT,bindFlags,0,0,0};
	D3D11_SUBRESOURCE_DATA subData = {data,0,0 };
	return SUCCEEDED(device->CreateBuffer(&bufferDesc, &subData, buffer));
}

bool ResourceManager::CreateBuffers(ID3D11Device* const device, VertexType* vertices, unsigned long* indices, int vertexCount, int indCount, ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer) {
	UINT vertexBufferSize = sizeof(VertexType) * vertexCount;
	UINT indexBufferSize = sizeof(unsigned long) * indCount;

	return CreateBuffer(device, vertices, vertexBufferSize, D3D11_BIND_VERTEX_BUFFER, vertexBuffer) &&
		CreateBuffer(device, indices, indexBufferSize, D3D11_BIND_INDEX_BUFFER, indexBuffer);
}

bool ResourceManager::LoadTexture(ID3D11Device* const device, const WCHAR* textureFileName) {
	ID3D11ShaderResourceView* texture = nullptr;
	HRESULT result = CreateDDSTextureFromFile(device, textureFileName, nullptr, &texture);

	if (SUCCEEDED(result)) {
		textures[textureFileName] = texture;
		return true;
	}
	delete texture;
	return false;
}
