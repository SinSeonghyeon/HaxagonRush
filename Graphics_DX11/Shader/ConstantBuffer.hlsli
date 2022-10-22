#include "Light.hlsli"
#include "Struct.hlsli"

// 2�� 31���� ����ũ�� ������ �� ������ ���� �ʰڴٴ� ��
static const uint g_LightCullMask = 1 << 31;

static const uint g_LightInfoMax = 100;

cbuffer CB_PERFRAME : register(b0)
{
    CameraInfo _cameraInfo;
    
    uint _dirLightCount;
    
    float3 _pad1;
    
    DirectionalLightInfo _directionalLightInfo[g_LightInfoMax];
    
    uint _spotLightCount;
    
    float3 _pad2;
    
    SpotLightInfo _spotLightInfo[g_LightInfoMax];
    
    uint _pointLightCount;
    
    float3 _pad3;
    
    PointLightInfo _pointLightInfo[g_LightInfoMax];
    
    SSAOInfo _ssaoInfo;
}

cbuffer CB_PEROBJECT : register(b1)
{
    float4x4 _world;
    float4x4 _worldInvTranspose;
    float4x4 _worldViewProj;
    float4x4 _texTransform;
    
    float4 _ambient;
    float4 _diffuse;
    float4 _specular;
    float4 _reflect;
    
    float4x4 _boneMatrix[30];
    
    uint _objectID;
    
    // �� �߰��Ϸ��� �� Pad �߰��� �� 16����Ʈ ������ ���Ͽ�
}

cbuffer CB_PERTEXTURE : register(b1)
{
    // x : Width - y : Height - z : 1 / Width - w : 1 / Height
    float4 _screenInfo;
    
    // x : Width - y : Height - z : 1 / Width - w : 1 / Height
    float4 _textureInfo;
}

cbuffer CB_TONEMAPPING : register(b1)
{
    float _exposure;
    
    float3 _pad4;
}

cbuffer CB_BLOOM : register(b1)
{
    float _threshold;
    
    float3 _pad5;
}

// TODO : �پ��� ���� ���ؼ��� ��� ó���غ��� ..
cbuffer CB_LIGHTVIEWPROJ : register(b2)
{    
    float4x4 _lightViewProj;
}

#define MAX_CASCADE_COUNT 3

cbuffer CB_DIRLIGHTVIEWPROJ : register(b3)
{
    float4x4 _lightViewProjList[MAX_CASCADE_COUNT];
    
    float4 _cascadeEnds;
};