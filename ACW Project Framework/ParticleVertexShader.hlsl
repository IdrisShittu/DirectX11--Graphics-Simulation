cbuffer MatrixBuffer : register(b0)
{
	matrix worldInverseMatrix;
	matrix texTransform;
};

cbuffer TessellationBuffer : register(b1)
{
	float maxTessellationDistance;
	float minTessellationDistance;
	float maxTessellationFactor;
	float minTessellationFactor;
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float padding;
};

cbuffer InverseViewBuffer : register(b3)
{
	matrix inverseView;
};

//Type definitions
struct VertexInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	matrix instanceWorldMatrix : INSTANCEMATRIX;
};

struct HullInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float tessellationFactor : TESS;
};

HullInput ParticleVertexShader(VertexInput input)
{
	HullInput output;

	output.position.xyz = (input.position.x * inverseView[0] + input.position.y * inverseView[1]).xyz;

	output.position = mul(float4(output.position.xyz, 1.0f), input.instanceWorldMatrix).xyz;

	//Pass colour as is to pixel shader
	output.tex = input.tex;

	float distanceToCamera = distance(output.position, cameraPosition);

	float tess = saturate((minTessellationDistance - distanceToCamera) / (minTessellationDistance - maxTessellationDistance));

	output.tessellationFactor = minTessellationFactor + tess * (maxTessellationFactor - minTessellationFactor);

	return output;
}