#include "ConstantBuffer.hlsli"

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 world : POSITION;
};

// �� �ܰ� (���ϱ� �� �༮)
Texture2D Texture_Curr : register(t0);

// �� �ܰ� (�� �Ϸ�Ȱ�)
Texture2D Texture_Prev : register(t1);

SamplerState Sam_Clamp : register(s0);    

// �ռ� !
float4 PS_BloomAddUp(VS_OUT input) : SV_Target
{
    float4 color = (float4) 0.f;
    
    color = Texture_Curr.Sample(Sam_Clamp, input.uv) + Texture_Prev.Sample(Sam_Clamp, input.uv);
    
    return color;
}