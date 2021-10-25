#include "PhongSkinnedMesh.hlsli" 
#include "Function.hlsli"

Texture2D myTexture : register(t0); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[
Texture2D normalMap : register(t1);
Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = myTexture.Sample(mySampler, pin.texcoord); // �g�U���ˌ�
    
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz); // �ʂ���J�����ւ̐��K���x�N�g��
    float3 L = normalize(lightDirection.xyz); // �ʂ�������ւ̐��K���ς݃x�N�g��
    float3 N = normalMap.Sample(mySampler, pin.texcoord).rgb; //	�m�[�}���e�N�X�`���@�������[���h�֕ϊ�
    
    //float3 ambient = ambient_color.rgb * ka.rgb; // ���� ambient�̐F�Ƌ���
    //float3 directionalDiffuse = ClacHalfLambert(N, L, lightDirection.rgb, )
    
    //  {
    //     // �V���h�E�}�b�v����[�x�l�擾
    //    float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
    //  // �[�x�l���r���ĉe���ǂ����𔻒肷��
    //    if (pin.shadow_texcoord.z - depth > shadow_bias)
    //    {
    //        color.rgb *= shadow_color.rgb;
    //    }
    //}
    //return color;
    return float4(0, 0, 0, 0);

}