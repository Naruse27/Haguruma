//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
    matrix World;
    matrix matWVP;
};

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR; //���_�J���[����
};

//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct PSInputShadow
{
    float4 Position : SV_POSITION;
    float4 Depth : TEXCOORD1;
};