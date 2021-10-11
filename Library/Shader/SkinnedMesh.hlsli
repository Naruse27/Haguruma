struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

#define MAX_BONES 128
cbuffer CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 worldViewProjection;
    row_major float4x4 world;
    float4 materialColor;
    float4 lightDirection;
    row_major float4x4 boneTransforms[MAX_BONES];
};