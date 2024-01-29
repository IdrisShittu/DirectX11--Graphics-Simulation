#include "Shader.h"

template<typename T>
bool CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer*& buffer, UINT byteWidth) {
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT result = device->CreateBuffer(&bufferDesc, nullptr, &buffer);
	if (FAILED(result))return false;
	return true;
}

Shader::Shader(const string& vertexShaderFileName, const string& hullShaderFileName, const string& domainShaderFileName, const string& pixelShaderFileName, ID3D11Device* const device, HWND const hwnd) : initFailed(false), vertexBufferResourceCount(0), hullBufferResourceCount(0), domainBufferResourceCount(0), pixelBufferResourceCount(0), nonTextureRenderMode(0), textureDiffuseRenderMode(0), displacementRenderMode(0), maxTessellationDistance(1.0f), minTessellationDistance(1.0f), maxTessellationFactor(0.0f), minTessellationFactor(0.0f), mipInterval(0.0f), mipClampMinimum(0.0f), mipClampMaximum(0.0f), displacementPower(0.0f), vertexShaderBuffer(nullptr), vertexShader(nullptr), hullShader(nullptr), domainShader(nullptr), pixelShader(nullptr), matrixBuffer(nullptr), tessellationBuffer(nullptr), cameraBuffer(nullptr), renderModeBuffer(nullptr)
{
	const unsigned int numberOfElements = 0;
	ID3D10Blob* hullShaderBuffer = nullptr, * domainShaderBuffer = nullptr, * pixelShaderBuffer = nullptr, * errorMessage = nullptr;

	const auto hlslVertexFileName = vertexShaderFileName + ".hlsl";
	auto result = D3DCompileFromFile(CA2W(hlslVertexFileName.c_str()), nullptr, nullptr, vertexShaderFileName.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, /*D3DCOMPILE_DEBUG*/ 0, &vertexShaderBuffer, &errorMessage);
	if(!FAILED(result)){
		const auto hlslHullFileName = hullShaderFileName + ".hlsl";
		result = D3DCompileFromFile(CA2W(hlslHullFileName.c_str()), nullptr, nullptr, hullShaderFileName.c_str(), "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage);
	}
	if (!FAILED(result)) {
		const auto hlslDomainFileName = domainShaderFileName + ".hlsl";
		result = D3DCompileFromFile(CA2W(hlslDomainFileName.c_str()), nullptr, nullptr, domainShaderFileName.c_str(), "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage);
	}
	if (!FAILED(result)) {
		const auto hlslPixelFileName = pixelShaderFileName + ".hlsl";
		result = D3DCompileFromFile(CA2W(hlslPixelFileName.c_str()), nullptr, nullptr, pixelShaderFileName.c_str(), "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	}
	if (FAILED(result))
	{
		SetInitializationState(true);
		if (errorMessage)OutputShaderErrorMessage(errorMessage, hwnd, "Error");
		else MessageBox(hwnd, "Error", "Missing Shader File", MB_OK);
		return;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);
	if (!FAILED(result))result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), nullptr, &hullShader);
	if (!FAILED(result))result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), nullptr, &domainShader);
	if (!FAILED(result))result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(result))
	{
		initFailed = true;
		return;
	}

	//Release shader buffer resources as we don't need them anymore
	hullShaderBuffer->Release();
	hullShaderBuffer = nullptr;

	domainShaderBuffer->Release();
	domainShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	if (!CreateConstantBuffer<MatrixBufferType>(device, matrixBuffer, sizeof(MatrixBufferType)) ||
		!CreateConstantBuffer<TessellationBufferType>(device, tessellationBuffer, sizeof(TessellationBufferType)) ||
		!CreateConstantBuffer<CameraBufferType>(device, cameraBuffer, sizeof(CameraBufferType)) ||
		!CreateConstantBuffer<DisplacementBuffer>(device, displacementBuffer, sizeof(DisplacementBuffer)) ||
		!CreateConstantBuffer<RenderModeBufferType>(device, renderModeBuffer, sizeof(RenderModeBufferType))) {
		initFailed = true;
		return;
	}
}

Shader::Shader(const Shader& other) = default;

Shader::Shader(Shader&& other) noexcept = default;

Shader::~Shader() {
	try
	{
		if (renderModeBuffer)renderModeBuffer->Release(),renderModeBuffer = nullptr;
		if (cameraBuffer)cameraBuffer->Release(),cameraBuffer = nullptr;
		if (tessellationBuffer)tessellationBuffer->Release(),tessellationBuffer = nullptr;
		if (matrixBuffer)matrixBuffer->Release(),matrixBuffer = nullptr;
		if (pixelShader)pixelShader->Release(),pixelShader = nullptr;
		if (domainShader)domainShader->Release(),domainShader = nullptr;
		if (hullShader)hullShader->Release(),hullShader = nullptr;
		if (vertexShader)vertexShader->Release(),vertexShader = nullptr;
		if (vertexShaderBuffer)vertexShaderBuffer->Release(),vertexShaderBuffer = nullptr;
	}
	catch (exception& e){}
}

Shader& Shader::operator=(const Shader& other) = default;

Shader& Shader::operator=(Shader&& other) noexcept = default;

bool Shader::GetInitializationState() const {
	return initFailed;
}

int Shader::GetVertexBufferResourceCount() const {
	return vertexBufferResourceCount;
}

int Shader::GetHullBufferResourceCount() const {
	return hullBufferResourceCount;
}

int Shader::GetDomainBufferResourceCount() const {
	return domainBufferResourceCount;
}

int Shader::GetPixelBufferResourceCount() const {
	return pixelBufferResourceCount;
}

void Shader::IncrementVertexBufferResourceCount() {
	vertexBufferResourceCount++;
}

void Shader::IncrementHullBufferResourceCount() {
	hullBufferResourceCount++;
}

void Shader::IncrementDomainBufferResourceCount() {
	domainBufferResourceCount++;
}

void Shader::IncrementPixelBufferResourceCount() {
	pixelBufferResourceCount++;
}

ID3D10Blob* Shader::GetVertexShaderBuffer() const
{
	return vertexShaderBuffer;
}

ID3D11Buffer* Shader::GetMatrixBuffer() const
{
	return matrixBuffer;
}

ID3D11Buffer* Shader::GetCameraBuffer() const {
	return cameraBuffer;
}

const D3D11_MAPPED_SUBRESOURCE& Shader::GetMappedSubResource() const
{
	return mappedResource;
}

void Shader::SetRenderModeStates(const int nonTextured, const int texturedDiffuse, const int displacementEnabled)
{
	nonTextureRenderMode = nonTextured;
	textureDiffuseRenderMode = texturedDiffuse;
	displacementRenderMode = displacementEnabled;
}

void Shader::GetTessellationVariables(float& maxTessellationDistance, float& minTessellationDistance, float& maxTessellationFactor, float& minTessellationFactor) const
{
	maxTessellationDistance = maxTessellationDistance;
	minTessellationDistance = minTessellationDistance;
	maxTessellationFactor = maxTessellationFactor;
	minTessellationFactor = minTessellationFactor;
}

void Shader::SetTessellationVariables(const float& mxTessellationDistance, const float& mnTessellationDistance, const float& mxTessellationFactor, const float& mnTessellationFactor)
{
	maxTessellationDistance = mxTessellationDistance;
	minTessellationDistance = mnTessellationDistance;
	maxTessellationFactor = mxTessellationFactor;
	minTessellationFactor = mnTessellationFactor;
}

void Shader::GetDisplacementVariables(float& mipInterval, float& mipClampMinimum, float& mipClampMaximum, float& displacementPower) const
{
	mipInterval = mipInterval;
	mipClampMinimum = mipClampMinimum;
	mipClampMaximum = mipClampMaximum;
	displacementPower = displacementPower;
}

void Shader::SetDisplacementVariables(const float& mipInterv, const float& mipClampMin, const float& mipClampMax, const float& displacementPow)
{
	mipInterval = mipInterv;
	mipClampMinimum = mipClampMin;
	mipClampMaximum = mipClampMax;
	displacementPower = displacementPow;
}

void Shader::SetInitializationState(const bool st)
{
	initFailed = st;
}

void Shader::SetVertexShaderBuffer(ID3D10Blob* const vtShBuffer)
{
	vertexShaderBuffer = vtShBuffer;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* const deviceContext, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMFLOAT3& cameraPosition)
{
	vertexBufferResourceCount = 0;
	hullBufferResourceCount = 0;
	domainBufferResourceCount = 0;
	pixelBufferResourceCount = 0;
	if (!UpdateMatrixBuffer(deviceContext, matrixBuffer, XMMatrixTranspose(XMMATRIX()), XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 0.0f))))return false;
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount++, 1, &matrixBuffer);
	if (!UpdateTessellationBuffer(deviceContext, tessellationBuffer, maxTessellationDistance, minTessellationDistance, maxTessellationFactor, minTessellationFactor))return false;
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount++, 1, &tessellationBuffer);
	if (!UpdateCameraBuffer(deviceContext, cameraBuffer, cameraPosition))return false;
	deviceContext->VSSetConstantBuffers(vertexBufferResourceCount++, 1, &cameraBuffer);
	if (!UpdateMatrixBuffer(deviceContext, matrixBuffer, XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix)))return false;
	deviceContext->DSSetConstantBuffers(domainBufferResourceCount++, 1, &matrixBuffer);
	if (!UpdateDisplacementBuffer(deviceContext, displacementBuffer, mipInterval, mipClampMinimum, mipClampMaximum, displacementPower, displacementRenderMode))return false;
	deviceContext->DSSetConstantBuffers(domainBufferResourceCount++, 1, &displacementBuffer);
	if (!UpdateRenderModeBuffer(deviceContext, renderModeBuffer, nonTextureRenderMode, textureDiffuseRenderMode))return false;
	deviceContext->PSSetConstantBuffers(pixelBufferResourceCount++, 1, &renderModeBuffer);
	return true;
}

bool Shader::UpdateMatrixBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))return false;
	auto* matrixBufferDataPointer = static_cast<MatrixBufferType*>(mappedResource.pData);
	matrixBufferDataPointer->viewMatrix = viewMatrix;
	matrixBufferDataPointer->projectionMatrix = projectionMatrix;
	deviceContext->Unmap(buffer, 0);
	return true;
}

bool Shader::UpdateTessellationBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, float maxTessDistance, float minTessDistance, float maxTessFactor, float minTessFactor)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))return false;
	auto* tessellationBufferDataPointer = static_cast<TessellationBufferType*>(mappedResource.pData);
	tessellationBufferDataPointer->maxTessellationDistance = maxTessDistance;
	tessellationBufferDataPointer->minTessellationDistance = minTessDistance;
	tessellationBufferDataPointer->maxTessellationFactor = maxTessFactor;
	tessellationBufferDataPointer->minTessellationFactor = minTessFactor;
	deviceContext->Unmap(buffer, 0);
	return true;
}

bool Shader::UpdateDisplacementBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, float mipInterval, float mipClampMinimum, float mipClampMaximum, float displacementPower, bool displacementRenderMode)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))return false;
	auto* displacementBufferDataPointer = static_cast<DisplacementBuffer*>(mappedResource.pData);
	displacementBufferDataPointer->mipInterval = mipInterval;
	displacementBufferDataPointer->mipMinimum = mipClampMinimum;
	displacementBufferDataPointer->mipMaximum = mipClampMaximum;
	displacementBufferDataPointer->displacementPower = displacementPower;
	displacementBufferDataPointer->displacementEnabled = displacementRenderMode;
	displacementBufferDataPointer->padding = XMFLOAT3();
	deviceContext->Unmap(buffer, 0);
	return true;
}

bool Shader::UpdateRenderModeBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, bool nonTextureRenderMode, bool textureDiffuseRenderMode)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))return false;
	auto* renderModeBufferDataPointer = static_cast<RenderModeBufferType*>(mappedResource.pData);
	renderModeBufferDataPointer->nonTexture = nonTextureRenderMode;
	renderModeBufferDataPointer->textureDiffuse = textureDiffuseRenderMode;
	renderModeBufferDataPointer->padding = XMFLOAT2();
	deviceContext->Unmap(buffer, 0);
	return true;
}

bool Shader::UpdateCameraBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer, XMFLOAT3 cameraPosition)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))return false;
	auto* cameraBufferDataPointer = static_cast<CameraBufferType*>(mappedResource.pData);
	cameraBufferDataPointer->cameraPosition = cameraPosition;
	cameraBufferDataPointer->padding = 0.0f;
	deviceContext->Unmap(buffer, 0);
	return true;
}

void Shader::SetShader(ID3D11DeviceContext* const deviceContext) const {
	if (!deviceContext) return;
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->HSSetShader(hullShader, nullptr, 0);
	deviceContext->DSSetShader(domainShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const LPCSTR& shaderFileName) const
{
	std::ofstream outFile("error.txt");
	outFile.write(static_cast<char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize());
	errorMessage->Release();
	MessageBox(hwnd, "Error compiling shader. Check shader-error.txt for details.", shaderFileName, MB_OK);
}