#include "SkinnedMesh.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;

    float4 blendedPosition = { 0, 0, 0, 1 };
    float4 blendedNormal = { 0, 0, 0, 0 };

    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += vin.boneWeights[boneIndex] *
           mul(vin.position, boneTransforms[vin.boneIndices[boneIndex]]);
        blendedNormal += vin.boneWeights[boneIndex] *
           mul(vin.normal, boneTransforms[vin.boneIndices[boneIndex]]);
    }
    vin.position = float4(blendedPosition.xyz, 1.0f);

    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, viewProjection));

    vout.worldPosition = mul(vin.position, world);
    vout.worldNormal = normalize(mul(vin.normal, world));
 
    vout.texcoord = vin.texcoord;
    vout.color = materialColor;

    return vout;
}