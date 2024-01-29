Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInput {
    float4 positionH : SV_POSITION;
    float2 tex : TEXCOORD0; 
};

float4 Texture2DPixelShader(PixelInput input) : SV_TARGET{
    return shaderTexture.Sample(sampleType, input.tex);
}
