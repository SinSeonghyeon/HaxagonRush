#include "ConstantBuffer.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 posH : SV_Position;
    float3 posL : POSITON;
    float4 posW : POSITION2;
    
    uint objectID : OBJECTID;
};

VS_OUT VS_SkyBox(VS_IN input)
{
    VS_OUT output;
    
    // ��ī�� �ڽ�, ť�갡 ���� �� ���̿� �ֵ��� ����
    output.posH = mul(float4(input.pos, 1.0f), _worldViewProj).xyww;
    
    output.posL = input.pos;
    
    output.posW = mul(float4(input.pos, 1.0f), _world);
    
    output.objectID = _objectID;
    
    return output;
}