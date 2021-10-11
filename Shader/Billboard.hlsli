
struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register( b0 )
{
	row_major float4x4 worldViewProjection;
	float4 color;
};