struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
 //   float4 binormal : BINORMAL; // 最悪消す
    float2 texcoord : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
 //   float4 worldBinormal : BINORMAL; // 最悪消す
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
  //  float4 shadowTexcoord : TEXCOORD1; // シャドウマップ用のパラメーター変数
};

static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 materialColor;
    float4 ka;
    float4 kd; // difusse
    float4 ks;
    row_major float4x4 boneTransforms[MAX_BONES];
};

// cbuffer 定数バッファ
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 lightColor;
    float4 cameraPosition;
};

cbuffer FOG_CONSTANT_BUFFER : register(b5)
{
    float4 fogColor;
    float4 fogRange;
}

cbuffer SHADOWMAP_CONSTANT_BUFFER : register(b6)
{
    row_major float4x4 lightViewProjection;
    float3 shadowColor;
    float shadowBias;
};