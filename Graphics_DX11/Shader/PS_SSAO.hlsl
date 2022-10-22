#include "ConstantBuffer.hlsli"

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD1;
    float3 toFarPlane : TEXCOORD0;
};

Texture2D Texture_Depth : register(t0);

Texture2D Texture_Normal : register(t1);

Texture2D Texture_RandomVectorMap : register(t2);

SamplerState Sam_Clamp : register(s0);      // ���÷� (Wide Wrap)

SamplerState Sam_Wrap : register(s1);

// ������ ǥ������ Ÿ�� �ȼ� ����Ʈ�� �󸶳� �������� ���� ���� Term�� ����Ѵ�.
float CalculateOcclusion(float distZ)
{
    // ���� ǥ������ Depth�� �� �ڿ� �ִٸ� ���� �� ����. (distZ �� ������ ���)
    // ����� ����� ��쿡�� ���� �� ����. => Epsilon �̻��� �տ� �־�� �����ٰ� �����Ѵ�.
    // ���� �ָ� �־ ���� �� ����.
    float occlusion = 0.f;
    
    if (distZ > _ssaoInfo._surfaceEpsilon)
    {
        float fadeLength = _ssaoInfo._occlusionFadeEnd - _ssaoInfo._occlusionFadeStart;
        
        // distZ�� Start�� ����� ������ ���󵵴� 1, End�� ����� ������ 0���� ���� �����Ѵ�.
        occlusion = saturate((_ssaoInfo._occlusionFadeEnd - distZ) / fadeLength);
    }
    
    return occlusion;
}

float4 PS_SSAO(VS_OUT input) : SV_TARGET
{
    // ���� �ȼ��� World Space Normal
    float3 normal = Texture_Normal.Sample(Sam_Clamp, input.uv).xyz;
    
    // View Space Normal�� ��ȯ
    normal = mul(normal, (float3x3) _cameraInfo._viewMatrix);
    
    // ���� �ȼ��� ����
    float depth = Texture_Depth.Sample(Sam_Clamp, input.uv).x;
        
    float viewPosZ = _cameraInfo._projMatrix._43 / (depth - _cameraInfo._projMatrix._33);
    
    // ���� �ȼ��� View-Space ��ġ�� �̾Ƴ���.
    // Origin - FarPlane Vector
    float3 targetPoint = (viewPosZ / input.toFarPlane.z) * input.toFarPlane;
    
    // (0, 1) => (-1, 1)
    float3 randomVector = 2.0f * Texture_RandomVectorMap.Sample(Sam_Wrap, input.uv).xyz - 1.f;
    
    float occlusionSum = 0.0f;
    
    [unroll]
    for (uint i = 0 ; i < _ssaoInfo._sampleCount ; i++)
    {
        // �̸� ������ ��� ������ ���͵��� �翬�� ���� �����Ǿ� ����.
        // �ϳ��� ���� ���Ϳ� ���ؼ� �ݻ��Ű�� ���� ���� ������ ������ ���͵��� ���������.        
        float3 offset = reflect(_ssaoInfo._offsetVectors[i].xyz, randomVector);
        
        // p �տ� �ִ� �༮�鿡 ���ؼ��� �Ű澴��.
        float flip = sign(dot(offset, normal));
        
        // ���� ������ �̳��� ������ �� q�� ��´�.
        float3 q = targetPoint + flip * _ssaoInfo._occlusionRadius * offset;
                
        // ������ �� q�� ���� �ؽ�ó ��ǥ�� ã��.
        float4 projTexCoord = mul(float4(q, 1.f), _cameraInfo._projMatrix);

        float2 projUV = projTexCoord.xy / projTexCoord.w;
        
        projUV.y = -projUV.y;
    
        projUV = projUV * 0.5f + 0.5f;
        
        // ������ ǥ���� q�κ��� �������� ������ r�� ã�ƶ�.
        float rDepth = Texture_Depth.Sample(Sam_Clamp, projUV.xy).x;
        
        float rViewPosZ = _cameraInfo._projMatrix._43 / (rDepth - _cameraInfo._projMatrix._33);
        
        float3 r = (rViewPosZ / q.z) * q;
        
        // �� r�� TargetPoint�� �����ϴ��� ã��.
        float distZ = targetPoint.z - r.z;
        float dp = max(dot(normal, normalize(r - targetPoint)), 0.0f);
        float occlusion = dp * CalculateOcclusion(distZ);
        
        occlusionSum += occlusion;
    }
    
    occlusionSum /= _ssaoInfo._sampleCount;
        
    return saturate(pow(1.f - occlusionSum, 8.f));
}