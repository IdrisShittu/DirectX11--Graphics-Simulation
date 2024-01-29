#define MAX_LIGHTS 16

Texture2D textures[2];
SamplerState sampleType;

struct Lights
{
	float4 ambientColour;
	float4 diffuseColour;
	float4 lightPositions;
};

cbuffer LightBuffer : register(b0)
{
	Lights lights[MAX_LIGHTS];
	int lightCount;
	float3 padding;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 TextureNormalPixelShader(PixelInput input) : SV_TARGET
{
	float4 baseColour = textures[0].Sample(sampleType, input.tex);
	float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	input.normal = normalize(input.normal);
	float4 bumpMap = textures[1].Sample(sampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));
	for (int i = 0; i < lightCount; i++)
	{
		totalAmbient += saturate(lights[i].ambientColour * baseColour);
		float3 lightDirection = normalize(lights[i].lightPositions.xyz - input.positionW);
		float nDotL = dot(bumpNormal, lightDirection);
		totalDiffuse += saturate(lights[i].diffuseColour * nDotL * baseColour);
	}
	return totalAmbient + totalDiffuse;
}