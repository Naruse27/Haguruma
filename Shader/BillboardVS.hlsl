#include "Billboard.hlsli" 

VS_OUT main( float4 position : POSITION, float2 texcoord : TEXCOORD )
{
	VS_OUT vout;

	vout.position = mul( position, worldViewProjection );
    vout.texcoord = texcoord;

	return vout;
}