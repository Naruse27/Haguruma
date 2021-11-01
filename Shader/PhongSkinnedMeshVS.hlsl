#include "PhongSkinnedMesh.hlsli"
#include "Function.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;

    float sigma = vin.tangent.w;
    vin.tangent.w = 0;


    float4 blendedPosition = { 0, 0, 0, 1 };
    float4 blendedNormal = { 0, 0, 0, 0 };
    float4 blendedTangent = { 0, 0, 0, 0 };
    //float4 blendedBinormal = { 0, 0, 0, 0 };
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += vin.boneWeights[boneIndex] *
           mul(vin.position, boneTransforms[vin.boneIndices[boneIndex]]);
        blendedNormal += vin.boneWeights[boneIndex] *
           mul(vin.normal, boneTransforms[vin.boneIndices[boneIndex]]);
        blendedTangent += vin.boneWeights[boneIndex] *
           mul(vin.tangent, boneTransforms[vin.boneIndices[boneIndex]]);
       // blendedBinormal += vin.boneWeights[boneIndex] *
       //    mul(vin.binormal, boneTransforms[vin.boneIndices[boneIndex]]);
    }
    vin.position = float4(blendedPosition.xyz, 1.0f);
    vin.normal = float4(blendedNormal.xyz, 0.0f);
    vin.tangent = float4(blendedTangent.xyz, 0.0f);
    //vin.binormal = float4(blendedBinormal.xyz, 0.0f);

    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, viewProjection));

    vout.worldPosition = mul(vin.position, world);
    vout.worldNormal = normalize(mul(vin.normal, world));
    vout.worldTangent = normalize(mul(vin.tangent, world));
    //vout.worldBinormal = normalize(mul(vin.binormal, world));
    vout.worldTangent.w = sigma;
 
    vout.texcoord = vin.texcoord;
    vout.color = materialColor;

<<<<<<< HEAD
    //// シャドウマップ用のパラメーター計算
	//{
	//	// ライトから見たNDC座標を算出
    //    float4 wvpPos = mul(vin.position, mul(world, lightViewProjection));
	//	// NDC座標からUV座標を算出する
    //    wvpPos /= wvpPos.w;
    //    wvpPos.y = -wvpPos.y;
    //    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
    //    vout.shadowTexcoord = float4(wvpPos.xyz, 0);
    //}
=======
    // シャドウマップ用のパラメーター計算
	{
		// ライトから見たNDC座標を算出
        float4 wvpPos = mul(vin.position, mul(world, lightViewProjection));
		// NDC座標からUV座標を算出する
        wvpPos /= wvpPos.w;
        wvpPos.y = -wvpPos.y;
        wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
        vout.shadowTexcoord = float4(wvpPos.xyz, 0);
    }
>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
    
    return vout;
}