Texture2D shaderTexture;
SamplerState sampleType;

cbuffer PixelParameters : register(b1) {
    float3 colourTint;
    float transparency;
};

struct PixelInput {
    float4 positionH : SV_POSITION;
    float2 tex : TEXCOORD0; 
};

float4 ParticlePixelShader(PixelInput input) : SV_TARGET{
    float radialGradient = 0.5f * 0.5f - dot(input.tex - 0.5f, input.tex - 0.5f);
    float shade = 2.0f / (1.0f + exp(12.0f * radialGradient));
    float3 textureColour = shaderTexture.Sample(sampleType, input.tex).rgb;
    float greyScale = dot(textureColour, float3(0.3333f, 0.3333f, 0.3333f));
    return float4(saturate(colourTint * greyScale * (1 - shade)), transparency);
}
