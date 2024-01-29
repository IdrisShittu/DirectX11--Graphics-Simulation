TextureCube shaderTexture;
SamplerState sampleType;

struct PixelInput {
    float4 positionH : SV_POSITION;
    float3 tex : TEXCOORD0; 
};

float4 TextureCubePixelShader(PixelInput input) : SV_TARGET{
    return shaderTexture.Sample(sampleType, input.tex);
}
