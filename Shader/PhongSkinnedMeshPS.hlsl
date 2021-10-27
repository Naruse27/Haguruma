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
    
    float3 ambient = ka.rgb; //ambient_color.rgb * ka.rgb; // ���� ambient�̐F�Ƌ���
    float3 directionalDiffuse = ClacHalfLambert(N, L, lightColor.rgb, kd.rgb);
    float3 directionalSpecular = CalcPhongSpecular(N, L, E, lightColor.rgb, ks.rgb);

    float4 color = float4(ambient, diffuseColor.a); // ambient�ƕs�����x
    color.rgb += diffuseColor.rgb * directionalDiffuse; // ambient  +=  �g�U���ˌ� * �w������g�U��
    color.rgb += directionalSpecular; // ambient += ���ʔ���
    {
         // �V���h�E�}�b�v����[�x�l�擾
        float depth = shadow_map.Sample(shadow_sampler_state, pin.shadowTexcoord.xy).r;
      // �[�x�l���r���ĉe���ǂ����𔻒肷��
        if (pin.shadowTexcoord.z - depth > shadowBias)
        {
            color.rgb *= shadowColor.rgb;
        }
    }
    return color;
    //return float4(0, 0, 0, 0);

}