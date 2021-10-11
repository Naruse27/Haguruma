float4 ScreenFilter(float4 color, float bright, float contrast, float saturate, float4 screenColor)
{
    float4 inputColor = color;
    	// 明度調整
    inputColor.rgb += bright;

	// コントラスト調整
    inputColor.rgb = ((color.rgb - 0.5f) * contrast) + 0.5f;

	// 彩度調整
    float avr = (inputColor.r + inputColor.g + inputColor.b) / 3;
    inputColor.rgb = (inputColor.rgb - avr) * saturate + avr;

	// カラーバランス調整
    inputColor.rgb += screenColor.rgb;

    return inputColor;
}


//--------------------------------------------
// 拡散反射関数（ランバートシェーディング）
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率(0～1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D);
    return K * C * D;
}

//--------------------------------------------
// 鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)
float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power)
{
    float3 H = normalize(-L + E);

    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

#define PI 3.141592653

float3 fresnelSchlick(float NoL, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - NoL, 5.0);
}

float3 renderingEquation(float3 L, float3 V, float3 N, float3 irradiance, float3 diffuse_reflection, float3 specular_reflection, float shininess)
{
#if 0
	float tones = 4;
	//irradiance = ceil(irradiance * tones) / tones;
	irradiance = floor(irradiance * tones) / tones;
#endif
	
#if 1
    float3 reflectance = 0.2;
#else
    float3 reflectance = saturate(fresnelSchlick(max(0, dot(N, L)), float3(0.02, 0.02, 0.02)));
#endif

    float3 diffuse_exitance = diffuse_reflection * irradiance * (1.0 - reflectance);
    float3 diffuse_radiance = diffuse_exitance / PI; // 拡散反射

    float smoothness = shininess;
    float3 specular_exitance = specular_reflection * irradiance * reflectance; // 鏡面反射
#if 0
	float3 R = reflect(-L, N);
	float3 specular_radiance = pow(max(0, dot(V, R)), smoothness) * specular_exitance;
#else
    float3 H = normalize(L + V);
    float3 specular_radiance = (smoothness + 8) / (8 * PI) * pow(max(0, dot(N, H)), smoothness) * specular_exitance;
#endif
	
    float3 radiance = diffuse_radiance + specular_radiance;
	
    return radiance;
}