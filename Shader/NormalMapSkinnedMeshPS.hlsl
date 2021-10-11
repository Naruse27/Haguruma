#include "NormalMapSkinnedMesh.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    //float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    //float3 N = normalize(pin.worldNormal.xyz);
    //float3 L = normalize(-lightDirection.xyz);
    //float3 diffuse = color.rgb * max(0, dot(N, L));
    //return float4(diffuse, color.a) * pin.color;
    
    float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    
    float3 N = normalize(pin.worldNormal.xyz); // ñ@ê¸
    float3 T = normalize(pin.worldTangent.xyz); // ê≥ê⁄
    float sigma = pin.worldTangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma); // è]ñ@ê¸
    float4 normal = textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
    float3 L = normalize(-lightDirection.xyz); // ÉâÉCÉg
    float3 diffuse = color.rgb * max(0.5f, dot(N, L)); // ägéUåı
    float3 V = normalize(cameraPosition.xyz - pin.worldPosition.xyz);
    float3 specular = pow(max(0, dot(N, normalize(V + L))), 128); // ãæñ îΩéÀ   pow(íl, ó›èÊ)
    return float4(diffuse + specular, alpha) * pin.color;
}