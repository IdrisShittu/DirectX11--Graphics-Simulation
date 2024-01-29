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

//Type definitions
struct PatchConstantOutput
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct DomainInput
{
	float3 position : POSITION;
	float3 tex : TEXCOORD0;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float3 tex : TEXCOORD0;
};

[domain("tri")]
PixelInput TextureCubeDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
	PixelInput output;
	output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
	output.tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex + uvwCoord.z * patch[2].tex;
	matrix viewProjection = mul(viewMatrix, projectionMatrix);
	output.tex = output.positionW;
	output.positionH = mul(float4(output.positionW, 1.0f), viewProjection).xyww;
	return output;
}