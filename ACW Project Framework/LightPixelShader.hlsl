#define MAX_LIGHTS 16

Texture2D shaderTexture;
SamplerState sampleType;

struct Lights {
    float4 ambientColour;
    float4 diffuseColour;
    float3 lightPosition; 
};

cbuffer LightBuffer : register(b0) {
    Lights lights[MAX_LIGHTS];
    int lightCount;
};

struct PixelInput {
    float4 positionH : SV_POSITION;
    float3 positionW : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 LightPixelShader(PixelInput input) : SV_TARGET{
    input.normal = normalize(input.normal);
    float4 baseColour = shaderTexture.Sample(sampleType, input.tex);
    float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < lightCount; i++) {
        totalAmbient += lights[i].ambientColour * baseColour;
        float3 lightDirection = normalize(lights[i].lightPosition - input.positionW);
        float nDotL = max(dot(input.normal, lightDirection), 0.0f);
        totalDiffuse += lights[i].diffuseColour * baseColour * nDotL;
    }
    return saturate(totalAmbient + totalDiffuse);
}
