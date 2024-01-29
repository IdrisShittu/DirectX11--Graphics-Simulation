#define MAX_LIGHTS 16

Texture2D textures[3] : register(t0);
Texture2D depthMapTexture[MAX_LIGHTS] : register(t3);

SamplerState sampleTypeWrap : register(s0);
SamplerState sampleTypeClamp : register(s1);

cbuffer RenderModes : register(b0)
{
	int nonTexture;
	int textureDiffuse;
	float2 paddingRenderMode;
}

struct Lights
{
	float4 ambientColour;
	float4 diffuseColour;
	float4 specularColour;
	float3 lightPositions;
	float specularPower;
	int isDirectionalLight;
	float3 padding;
};

cbuffer LightBuffer : register(b1)
{
	Lights lights[MAX_LIGHTS];
	int lightCount;
	float3 padding;
};

struct PixelInput
{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
	float4 lightViewPosition[MAX_LIGHTS] : TEXCOORD2;
};

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	float3 bumpedNormalW = mul(normalMapSample, TBN);
	return bumpedNormalW;
}

float CalculateShadow(Texture2D depthMap, float4 position, float nDotL)
{
	float2 projectedTextureCoords;
	projectedTextureCoords.x = position.x / position.w / 2.0f + 0.5f;
	projectedTextureCoords.y = -position.y / position.w / 2.0f + 0.5f;
	float lightDepthValue = position.z / position.w;
	float bias = max(0.01f * (1.0f - nDotL), 0.012f);
	float shadow = 0.0f;
	if ((saturate(projectedTextureCoords.x) == projectedTextureCoords.x) && (saturate(projectedTextureCoords.y) == projectedTextureCoords.y))
	{
		float2 textureSize;
		depthMap.GetDimensions(textureSize.x, textureSize.y);
		textureSize = 1.0f / textureSize;
		float depthValue = 0.0f;
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				float depthValue = depthMap.Sample(sampleTypeClamp, projectedTextureCoords.xy + float2(x, y) * textureSize).r;
				shadow += lightDepthValue - bias > depthValue ? 1.0f : 0.0f;
			}
		}
	}
	return shadow / 9.0f;
}

float4 TextureDisplacementPS(PixelInput input) : SV_TARGET{
    float4 bumpMap = textures[1].Sample(sampleTypeWrap, input.tex) * 2.0f - 1.0f;
    float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));
    float4 baseColour = textures[0].Sample(sampleTypeWrap, input.tex);
    float4 totalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < lightCount; i++) {
        float3 lightDir = normalize(lights[i].lightPositions - input.positionW);
        float nDotL = max(dot(bumpNormal, lightDir), 0.0f);
        float3 reflection = normalize(((2.0f * bumpNormal) * nDotL) - lightDir);
        float rDotV = max(dot(reflection, input.viewDirection), 0.0f);
        totalAmbient += lights[i].ambientColour * baseColour;
        totalDiffuse += saturate(lights[i].diffuseColour * nDotL * baseColour);
        float4 specularIntensity = textures[2].Sample(sampleTypeWrap, input.tex);
        totalSpecular += saturate(lights[i].specularColour * pow(rDotV, lights[i].specularPower) * specularIntensity);
        if (lights[i].isDirectionalLight) {
            totalDiffuse *= (1.0f - CalculateShadow(depthMapTexture[i], input.lightViewPosition[i], nDotL));
        }
    }

    return saturate(totalAmbient + totalDiffuse + totalSpecular);
}
