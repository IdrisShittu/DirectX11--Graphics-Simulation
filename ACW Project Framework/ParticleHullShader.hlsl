//Type definitions
struct HullInput
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float tessellationFactor : TESS;
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
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	PatchConstantOutput output;

	output.edges[0] = 0.5f * (inputPatch[1].tessellationFactor + inputPatch[2].tessellationFactor);
	output.edges[1] = 0.5f * (inputPatch[2].tessellationFactor + inputPatch[0].tessellationFactor);
	output.edges[2] = 0.5f * (inputPatch[0].tessellationFactor + inputPatch[1].tessellationFactor);

	output.inside = output.edges[0];

	return output;
}

[domain("tri")]
//[partitioning("integer")]
[partitioning("fractional_odd")]
//[outputtopology("point")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)]
DomainInput ParticleHullShader(InputPatch<HullInput, 3> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	DomainInput output;
	output.position = inputPatch[outputControlPointID].position;
	output.tex = inputPatch[outputControlPointID].tex;

	return output;
}