#include "Sprite.hlsli"

Texture2D diffuseMap : register(t0); // �摜
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}