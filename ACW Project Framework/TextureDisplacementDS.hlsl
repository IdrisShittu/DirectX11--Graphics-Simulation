Texture2D displacementTexture;
SamplerState sampleType;

#define MAX_LIGHTS 16

cbuffer MatrixBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer DisplacementBuffer : register(b1)
{
	float mipInterval;
	float mipMinimum;
	float mipMaximum;
	float displacementPower;
	int displacementEnabled;
	float3 padding;
};

struct Lights
{
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightMatrixBuffer : register(b2)
{
	Lights lights[MAX_LIGHTS];
	int lightCount;

	float3 lightPadding;
};

cbuffer CameraBuffer : register(b3)
{
	float3 cameraPosition;
	float cameraPadding;
};

struct PatchConstantOutput
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct DomainInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
	float4 lightViewPosition[MAX_LIGHTS] : TEXCOORD2;
};

[domain("tri")]
PixelInput TextureDisplacementDS(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
	PixelInput output;
	output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
	output.tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;
	output.normal = uvwCoord.x * patch[0].normal + uvwCoord.y * patch[1].normal + uvwCoord.z * patch[2].normal;
	output.tangent = uvwCoord.x * patch[0].tangent + uvwCoord.y * patch[1].tangent + uvwCoord.z * patch[2].tangent;
	output.binormal = uvwCoord.x * patch[0].binormal + uvwCoord.y * patch[1].binormal + uvwCoord.z * patch[2].binormal;
	output.normal = normalize(output.normal);
	output.tangent = normalize(output.tangent);
	output.binormal = normalize(output.binormal);
	float mipLevel = clamp((distance(output.positionW, cameraPosition) - mipInterval) / mipInterval, mipMinimum, mipMaximum);
	float height = displacementTexture.SampleLevel(sampleType, output.tex, mipLevel).r;
	if (!displacementEnabled)output.positionW += (displacementPower * (height - 1.0f)) * output.normal;
	output.positionH = mul(float4(output.positionW, 1.0f), viewMatrix);
	output.positionH = mul(output.positionH, projectionMatrix);
	output.viewDirection = normalize(cameraPosition.xyz - output.positionW);
	for (int i = 0; i < lightCount; i++)
	{
		matrix lightViewProjection = mul(lights[i].lightViewMatrix, lights[i].lightProjectionMatrix);
		output.lightViewPosition[i] = mul(float4(output.positionW, 1.0f), lightViewProjection);
	}
	return output;
}