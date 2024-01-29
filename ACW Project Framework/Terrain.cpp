#include "Terrain.h"

Terrain::Terrain(ID3D11Device* device, const XMFLOAT3& voxelArea, const XMFLOAT3& cubeScale, const shared_ptr<Shader>& shader, const shared_ptr<ResourceManager>& resourceManager) :
    initFailed(false)
{
    InitializeTerrain(voxelArea, cubeScale);

    vector<const WCHAR*> textureNames = { L"FloorColour.dds", L"FloorNormal.dds", L"FloorSpecular.dds" };

    AddComponents(device, shader, resourceManager, textureNames);
    SetTessellationVariables(1.0f, 20.0f, 3.0f, 1.0f);

    if (GetInitializationState()) {
        initFailed = true;
        MessageBox(nullptr, "Could not initialize model object.", "Error", MB_OK);
    }
}

Terrain::~Terrain()
{
}

void Terrain::InitializeTerrain(const XMFLOAT3& voxelArea, const XMFLOAT3& cubeScale)
{
    const int x = static_cast<int>(voxelArea.x / 2);
    const int y = static_cast<int>(voxelArea.y);
    const int z = static_cast<int>(voxelArea.z / 2);
    const int cubeScaleX = static_cast<int>(cubeScale.x);
    const int cubeScaleY = static_cast<int>(cubeScale.y);
    const int cubeScaleZ = static_cast<int>(cubeScale.z);

    initialTerrainPositions.clear();

    for (int i = -x * cubeScaleX; i < x; i += cubeScaleX)
        for (int j = -y * cubeScaleY - cubeScaleY / 2; j < -(cubeScaleY / 2); j += cubeScaleY)
            for (int k = -z * cubeScaleZ; k < z; k += cubeScaleZ)
                initialTerrainPositions.emplace_back(XMFLOAT3(i, j, k));
}

void Terrain::AddComponents(ID3D11Device* device, const shared_ptr<Shader>& shader, const shared_ptr<ResourceManager>& resourceManager, const vector<const WCHAR*>& textureNames)
{
    AddScaleComponent(XMFLOAT3(1.0f, 1.0f, 1.0f)); // Adjust the scale as needed
    AddPositionComponent(initialTerrainPositions);
    AddRotationComponent(0.0f, 0.0f, 0.0f);
    AddRigidBodyComponent(true, 1.0f, 0.0f, 0.0f);
    AddModelComponent(device, ModelType::LowPolyCube, resourceManager);
    AddTextureComponent(device, textureNames, resourceManager);
    SetShaderComponent(shader);
}

void Terrain::UpdateTerrain()
{
    Update();
}

bool Terrain::RenderTerrain(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const vector<ID3D11ShaderResourceView*>& depthTextures, const vector<shared_ptr<Light>>& pointLightList, const XMFLOAT3& cameraPosition) const
{
    return Render(deviceContext, viewMatrix, projectionMatrix, depthTextures, pointLightList, cameraPosition);
}

void Terrain::ResetTerrainState()
{
    const auto terrainPositions = GetPositionComponent()->GetPositions();

    for (unsigned int i = 0; i < terrainPositions.size(); i++) {
        if (terrainPositions[i].y < -200.0f) {
            GetPositionComponent()->TranslatePositionAt(XMFLOAT3(0.0f, 500.0f, 0.0f), i);
            UpdateInstanceData();
        }
    }
}
