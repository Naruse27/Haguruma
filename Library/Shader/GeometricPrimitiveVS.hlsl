#include "GeometricPrimitive.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)
{
    VS_OUT vout;
    vout.position = mul(position, worldViewProjection);
    normal.w = 0;
    float4 N = normalize(mul(normal, world)); // mul �|���Z
    float4 L = normalize(-lightDirection);

    vout.color = materialColor * max(0.25f, dot(L, N));
    vout.color.a = materialColor.a;
    return vout;
}