#include "PhongSkinnedMesh.hlsli" 
#include "Function.hlsli"

Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー
Texture2D normalMap : register(t1);
Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = myTexture.Sample(mySampler, pin.texcoord); // 拡散反射光
    
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz); // 面からカメラへの正規化ベクトル
    float3 L = normalize(lightDirection.xyz); // 面から光源への正規化済みベクトル
    float3 N = normalMap.Sample(mySampler, pin.texcoord).rgb; //	ノーマルテクスチャ法線をワールドへ変換
    
    float3 ambient = ka.rgb; //ambient_color.rgb * ka.rgb; // 環境光 ambientの色と強さ
    float3 directionalDiffuse = ClacHalfLambert(N, L, lightColor.rgb, kd.rgb);
    float3 directionalSpecular = CalcPhongSpecular(N, L, E, lightColor.rgb, ks.rgb);

    float4 color = float4(ambient, diffuseColor.a); // ambientと不透明度
    color.rgb += diffuseColor.rgb * directionalDiffuse; // ambient  +=  拡散反射光 * 指定方向拡散光
    color.rgb += directionalSpecular; // ambient += 鏡面反射
    {
         // シャドウマップから深度値取得
        float depth = shadow_map.Sample(shadow_sampler_state, pin.shadowTexcoord.xy).r;
      // 深度値を比較して影かどうかを判定する
        if (pin.shadowTexcoord.z - depth > shadowBias)
        {
            color.rgb *= shadowColor.rgb;
        }
    }
    return color;
    //return float4(0, 0, 0, 0);

}