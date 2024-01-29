Texture2D displacementTexture;
SamplerState sampleType;

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
};

cbuffer CameraBuffer : register(b2)
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
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

[domain("tri")]
PixelInput DepthDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
	PixelInput output;
	output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
	output.tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;
	output.normal = uvwCoord.x * patch[0].normal + uvwCoord.y * patch[1].normal + uvwCoord.z * patch[2].normal;
	output.normal = normalize(output.normal);

	if (displacementPower)
	{
		float mipLevel = clamp((distance(output.positionW, cameraPosition) - mipInterval) / mipInterval, mipMinimum, mipMaximum);
		float height = displacementTexture.SampleLevel(sampleType, output.tex, mipLevel).r;
		output.positionW += (displacementPower * (height - 1.0f)) * output.normal;
	}
	output.positionH = mul(float4(output.positionW, 1.0f), viewMatrix);
	output.positionH = mul(output.positionH, projectionMatrix);
	return output;
}