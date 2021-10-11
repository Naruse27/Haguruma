#include "ShadowMap.hlsli"

//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------
PSInputShadow main(VSInput input)
{
    PSInputShadow output = (PSInputShadow) 0;
    float4 P = float4(input.Position, 1.0);
	// WVP�ϊ�.
    output.Position = mul(matWVP, P);
	// �o�͒l�ݒ�.
    output.Depth = output.Position;
    return output;
}
