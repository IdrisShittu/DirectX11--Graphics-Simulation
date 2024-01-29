struct HullInput {
    float3 position : POSITION;
    float3 posWorld : TEXCOORD1;
    float3 normal : NORMAL;
    float tessellationFactor : TESS;
};

struct PatchConstantOutput {
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct DomainInput {
    float3 position : POSITION;
    float3 posWorld : TEXCOORD1;
    float3 normal : NORMAL;
};

PatchConstantOutput PatchConstantFunction(InputPatch<HullInput, 3> inputPatch, uint patchId : SV_PrimitiveID) {
    PatchConstantOutput output;
    output.edges[0] = 0.5f * (inputPatch[1].tessellationFactor + inputPatch[2].tessellationFactor);
    output.edges[1] = 0.5f * (inputPatch[2].tessellationFactor + inputPatch[0].tessellationFactor);
    output.edges[2] = 0.5f * (inputPatch[0].tessellationFactor + inputPatch[1].tessellationFactor);
    output.inside = (output.edges[0] + output.edges[1] + output.edges[2]) / 3.0f;
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)]
DomainInput ReflectionHullShader(InputPatch<HullInput, 3> inputPatch, uint outputControlPointID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID) {
    DomainInput output;
    output.position = inputPatch[outputControlPointID].position;
    output.posWorld = inputPatch[outputControlPointID].posWorld;
    output.normal = inputPatch[outputControlPointID].normal;

    return output;
}
