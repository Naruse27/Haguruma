#include "SkinnedMesh.hlsli"
//#include "Function.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 boneWeights : WEIGHTS, uint4 boneIndices : BONES)
{
    VS_OUT vout;

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };

    for (int  i = 0;  i < 4 ; i++)
    {     // mul Š|‚¯ŽZ
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }
    position = float4(p, 1.0f);
    normal = float4(n, 0.0f);

    vout.position = mul(position, worldViewProjection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));  // –@ü
    float4 L = normalize(-lightDirection);            // ƒ‰ƒCƒg‚Ì•ûŒü

    vout.color = materialColor * max(1, dot(L, N));
    vout.color.a = materialColor.a;

    vout.texcoord = texcoord;

    return vout;
}