#include "ShadowMap.hlsli"

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInputShadow input) : SV_Target
{
    float4 color = input.Depth.z / input.Depth.w;
    color.a = 1.0f;
    return color;
}