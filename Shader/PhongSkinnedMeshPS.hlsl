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
    
    color = float4(ambient, color.a);
    color.rgb += diffuse.rgb;// * (diffuse + point_diffuse + spot_diffuse);
    color.rgb += specular;
    //float4 c = float4(diffuse + specular, alpha) * pin.color;
    color = CalcFog(color, fogColor, fogRange.xy, length(pin.worldPosition.xyz - cameraPosition.xyz));
    return color;
}