struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

typedef VS_OUT GS_IN;
typedef GS_IN GS_OUT;
typedef GS_OUT PS_IN;

static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 materialColor;
    row_major float4x4 boneTransforms[MAX_BONES];
};

// cbuffer 定数バッファ
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
};

cbuffer DESTRUCTION_CONSTANT_BUFFER : register(b3)
{
    float _ScaleFactor;
    float _RotationFactor;
    float _PositionFactor;
    float _Destruction;
};