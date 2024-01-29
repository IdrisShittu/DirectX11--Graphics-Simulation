TextureCube textureCube : register(t0);
SamplerState sampleTypeWrap : register(s0);

struct PixelInput {
    float4 positionH : SV_POSITION;
    float3 posWorld : TEXCOORD1; 
    float3 normal : NORMAL;     
    float3 viewDirection : TEXCOORD0; 
};

float4 ReflectionPixelShader(PixelInput input) : SV_TARGET{
    float3 reflectionVector = reflect(-input.viewDirection, input.normal);
    return textureCube.Sample(sampleTypeWrap, reflectionVector);
}
