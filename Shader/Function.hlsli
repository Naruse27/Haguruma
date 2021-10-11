float4 ScreenFilter(float4 color, float bright, float contrast, float saturate, float4 screenColor)
{
    float4 inputColor = color;
    	// ���x����
    inputColor.rgb += bright;

	// �R���g���X�g����
    inputColor.rgb = ((color.rgb - 0.5f) * contrast) + 0.5f;

	// �ʓx����
    float avr = (inputColor.r + inputColor.g + inputColor.b) / 3;
    inputColor.rgb = (inputColor.rgb - avr) * saturate + avr;

	// �J���[�o�����X����
    inputColor.rgb += screenColor.rgb;

    return inputColor;
}


//--------------------------------------------
// �g�U���ˊ֐��i�����o�[�g�V�F�[�f�B���O�j
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D);
    return K * C * D;
}

//--------------------------------------------
// ���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)
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
    float3 diffuse_radiance = diffuse_exitance / PI; // �g�U����

    float smoothness = shininess;
    float3 specular_exitance = specular_reflection * irradiance * reflectance; // ���ʔ���
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