cbuffer MatrixBuffer : register(b0) {
    matrix viewProjectionMatrix;
};

cbuffer DisplacementBuffer : register(b1) {
    float mipInterval;
    float mipMinimum;
    float mipMaximum;
    float displacementPower;
};

struct PatchConstantOutput {
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct DomainInput {
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct PixelInput {
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float4 colour : COLOR;
};

[domain("tri")]
PixelInput ColourDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch) {
    PixelInput output;
    output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    output.colour = uvwCoord.x * patch[0].colour + uvwCoord.y * patch[1].colour + uvwCoord.z * patch[2].colour;
    output.positionH = mul(float4(output.positionW, 1.0f), viewProjectionMatrix);
    return output;
}
