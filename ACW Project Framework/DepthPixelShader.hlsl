struct PixelInput{
	float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
};

float4 DepthPixelShader(PixelInput input) : SV_TARGET
{
	float3 depth;

	depth = input.positionH.z / input.positionH.w;

	return float4(depth, 1.0f);
}