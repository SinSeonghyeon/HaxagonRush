#include "ConstantBuffer.hlsli"

#define CAMERA_MOTION_BLUR_SAMPLE_COUNT 9

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 world : POSITION;
};

Texture2D Texture_Position : register(t0);

Texture2D Texture_Target : register(t1);

SamplerState Sam_Clamp : register(s0);

float4 PS_CameraMotionBlur(VS_OUT input) : SV_TARGET
{
    float4 currentPosition = float4(Texture_Position.Sample(Sam_Clamp, input.uv).xyz, 1.f);
    
    float4 currView = mul(currentPosition, _cameraInfo._viewMatrix);
    
    float4 currProj = mul(currView, _cameraInfo._projMatrix);
    
    currProj /= currProj.w;
    
    float4 prevView = mul(currentPosition, _cameraInfo._prevViewMatrix);
    
    float4 prevProj = mul(prevView, _cameraInfo._prevProjMatrix);
    
    prevProj /= prevProj.w;
    
    float f = 1.0f / CAMERA_MOTION_BLUR_SAMPLE_COUNT;
    
    // '���� �ȼ� -> ���� �ȼ�' ���� ����
    float2 velocity = (float2(prevProj.x - currProj.x, prevProj.y - currProj.y) / 2) * f;
    
    float4 color = 0;
    
    for (int i = 0; i < CAMERA_MOTION_BLUR_SAMPLE_COUNT; i++)
    {
        float2 newUV = saturate(input.uv + velocity.xy * i);
        
        // �ӵ� �������� �̵��ϸ鼭 ���ø� (�ٵ� �̰� �ֺ� ���� ��ȭ�� ���� ���� ��ġ�� �ٸ��� �ϴ°� ���� �� ������ ..)
        float4 currentColor = Texture_Target.Sample(Sam_Clamp, newUV);
        
        color += currentColor;
    }
    
    return color / CAMERA_MOTION_BLUR_SAMPLE_COUNT;
}