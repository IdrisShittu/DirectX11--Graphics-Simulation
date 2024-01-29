cbuffer MatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float cameraPadding;
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

struct PixelInput {
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float3 posWorld : TEXCOORD1;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD0;
};

[domain("tri")]
PixelInput ReflectionDomainShader(in PatchConstantOutput input, in const float3 uvwCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch) {
    PixelInput output;
    output.positionW = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    output.posWorld = uvwCoord.x * patch[0].posWorld + uvwCoord.y * patch[1].posWorld + uvwCoord.z * patch[2].posWorld;
    output.normal = normalize(uvwCoord.x * patch[0].normal + uvwCoord.y * patch[1].normal + uvwCoord.z * patch[2].normal);
    output.positionH = mul(float4(output.positionW, 1.0f), viewMatrix * projectionMatrix);
    output.viewDirection = cameraPosition.xyz;
    return output;
}
