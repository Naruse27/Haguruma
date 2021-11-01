#include "PhongSkinnedMesh.hlsli" 
#include "Function.hlsli"

//Texture2D myTexture : register(t0); //テクスチャー
//SamplerState mySampler : register(s0); //サンプラー
//Texture2D normalMap : register(t1);
//Texture2D shadow_map : register(t4);
//SamplerState shadow_sampler_state : register(s4);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
   
    float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    float3 ambient = ka.xyz;
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz); // 面からカメラへの正規化べクトル
    float3 N = normalize(pin.worldNormal.xyz); // 法線
    float3 T = normalize(pin.worldTangent.xyz); // 正接
    float sigma = pin.worldTangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma); // 従法線
    float4 normal = textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
    float3 L = normalize(-lightDirection.xyz); // ライト
    float3 diffuse = color.rgb * max(0.5f, dot(N, L)); // 拡散光
    float3 specular = CalcPhongSpecular(N, L, E, lightDirection.rgb, ks.rgb); //pow(max(0, dot(N, normalize(V + L))), 64); // 鏡面反射   pow(値, 累乗)
    
<<<<<<< HEAD
    color = float4(ambient, color.a);
    color.rgb += diffuse.rgb;// * (diffuse + point_diffuse + spot_diffuse);
    color.rgb += specular;
    //float4 c = float4(diffuse + specular, alpha) * pin.color;
    color = CalcFog(color, fogColor, fogRange.xy, length(pin.worldPosition.xyz - cameraPosition.xyz));
    return color;
=======
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

>>>>>>> 070702319ab3ccce4f6a49339f966afc094a1dd4
}