#include "PhongSkinnedMesh.hlsli"
#include "Function.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 bone_weights : WEIGHTS, uint4 bone_indices : BONES)
{
    VS_OUT vout;

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (bone_weights[i] * mul(position, bone_transforms[bone_indices[i]])).xyz;
        n += (bone_weights[i] * mul(float4(normal.xyz, 0), bone_transforms[bone_indices[i]])).xyz;
    }
    position = float4(p, 1.0f);
    normal = float4(n, 0.0f);

    vout.position = mul(position, worldViewProjection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-lightDirection);

    vout.color = materialColor;
    vout.texcoord = texcoord;

    vout.worldPos = mul(world, position).xyz;
    vout.worldNormal = N;

    return vout;
}