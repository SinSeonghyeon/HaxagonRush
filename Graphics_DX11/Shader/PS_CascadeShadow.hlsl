#include "ConstantBuffer.hlsli"

struct PS_OUT_TEX_ARRAY
{
    float4 pos : SV_POSITION;
    
    uint RTIndex : SV_RenderTargetArrayIndex;
};

float4 PS_CascadeShadow(PS_OUT_TEX_ARRAY input)
{   
    // ����. ���߿� ���� ���� ������ ������ ����� �Ӵϴ�.
}