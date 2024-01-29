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

cbuffer InverseViewBuffer : register(b2)
{
	matrix inverseView;
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
	float2 tex : TEXCOORD0;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
};

[domain("tri")]
PixelInput ParticleDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
	PixelInput output;

	//Calculate new vertex position
	output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
	output.tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;

	//float3 planeNormal = output.positionW - cameraPosition;
	//planeNormal.y = 0.0f;
	//planeNormal = normalize(planeNormal);

	//float3 right;
	//float3 up;

	//right.x = viewMatrix[0][0];  // 0
	//right.y = viewMatrix[0][1];  // 4
	//right.z = viewMatrix[0][2];  // 8

	//up.x = viewMatrix[1][0];  // 1
	//up.y = viewMatrix[1][1];  // 5
	//up.z = viewMatrix[1][2];  // 9

	////bbPos.xyz = (bbPos.x * Vx + bbPos.y * Vy).xyz;

	//output.positionW = (output.positionW.x * inverseView[0] + output.positionW.y * inverseView[1]).xyz;

	//output.positionW += right * (output.tex.x - 0.5f) * 1.0f;
	//output.positionW += up * (output.tex.y - 0.5f) * 1.0f;

	//input.position = (input.position.x * inverseView[0] + input.position.y * inverseView[1]).xyz;


	matrix viewProjection = mul(viewMatrix, projectionMatrix);

	output.positionH = mul(float4(output.positionW, 1.0f), viewProjection);

	return output;
}