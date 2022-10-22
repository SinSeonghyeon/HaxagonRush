#include "ConstantBuffer.hlsli"

#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define MAX_SEARCH_ITERATIONS 12
#define SUBPIXEL_QUALITY 0.75

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 world : POSITION;
};

Texture2D Texture_Target : register(t0);

SamplerState Sam_Clamp : register(s0);    

float RGBToLuma(float4 rgb)
{
   return sqrt(dot(rgb.xyz, float3(0.299, 0.587, 0.114)));
}

float4 PS_FXAA(VS_OUT input) : SV_TARGET
{
    float QUALITY[MAX_SEARCH_ITERATIONS] =
    {
        1.f, 1.f, 1.5f, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f
    };
    
    float4 colorCenter = Texture_Target.Sample(Sam_Clamp, input.uv);
    
    float lumaCenter = RGBToLuma(colorCenter);

    float2 up = float2(0.f, -_textureInfo.w);
    float2 down = float2(0.f, _textureInfo.w);
    float2 left = float2(-_textureInfo.z, 0.f);
    float2 right = float2(_textureInfo.z, 0.f);
    
    float lumaDown = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + down));
    float lumaUp = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + up));
    float lumaLeft = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + left));
    float lumaRight = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + right));
    
    float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
    float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));
    
    float lumaRange = lumaMax - lumaMin;
    
    // Luma�� ���̰� �Ӱ谪���� ũ�ٸ� ����� ��ȭ�� ū �� => �����ڸ� O
    // ���̰� �Ӱ谪���� �۴ٸ�, �Ǵ� ���� ��ο� ������ �ָ� �������� �ʴ´�. => �����ڸ� X
    if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
        return colorCenter;
    
    float lumaDownLeft = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + down + left));
    float lumaUpRight = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + up + right));
    float lumaUpLeft = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + up + left));
    float lumaDownRight = RGBToLuma(Texture_Target.Sample(Sam_Clamp, input.uv + down + right));
    
    // 4���� �����ڸ��� lumas�� ���� (���� ������ ���� ����� ���� �߰� ������ ���)
    float lumaDownUp = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;

    // �ڳʵ� �����ϴ� ..!
    float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
    float lumaDownCorners = lumaDownLeft + lumaDownRight;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaUpCorners = lumaUpRight + lumaUpLeft;
    
    // ����� ���� ���� ������ ��ȭ���� ����ġ ���
    // ����: | (upleft - left) - (left - downleft) | +2 *  | (up - center) - (center - down) | + | (upright - right) - (right - downright) |
    float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners) + abs(-2.0 * lumaCenter + lumaDownUp) * 2.0 + abs(-2.0 * lumaRight + lumaRightCorners);
    
    // ����: |(upright - up) - (up - upleft)| + 2 * |(right - center) - (center - left)| + |(downright - down) - (down - downleft)|
    float edgeVertical = abs(-2.0 * lumaUp + lumaUpCorners) + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0 + abs(-2.0 * lumaDown + lumaDownCorners);
    
    // ���� �����ڸ��� �����ΰ�? �����ΰ�?
    bool isHorizontal = (edgeHorizontal >= edgeVertical);
    
    // ���� �����ڸ��� �ݴ�������� 2���� �̿� �ؼ��� ������
    float luma1 = isHorizontal ? lumaDown : lumaRight;          // UV ����
    float luma2 = isHorizontal ? lumaUp : lumaLeft;             // UV ����

    // �� �������� ��ȭ���� ���
    float gradient1 = luma1 - lumaCenter;   // UV ���� ���������� ��ȭ��
    float gradient2 = luma2 - lumaCenter;   // UV ���� ���������� ��ȭ��

    // ��� ������ ��ȭ�� �� ���ĸ���?
    // ���� �������� �� ���ĸ��� (�Ʒ��� �Ǵ� ������) true, ���� �������� �� ���ĸ��� (���� �Ǵ� ����) false
    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    // �ش� ������ ��ȭ��, ����ȭ
    float gradientScaled = 0.25f * max(abs(gradient1), abs(gradient2));
    
    // �����ڸ��� ���⿡ ���� ���� ũ��(�� �ȼ�)�� ����
    // �����ڸ��� �����̸� �翬�� ���� �������� �̵��ؼ� ������ ���̴�.
    float stepLength = isHorizontal ? _textureInfo.w : _textureInfo.x;

    // �ùٸ� ������ ��� luma
    float lumaLocalAverage = 0.0;

    // ���� �������� �� ���ĸ���.
    if (is1Steepest)
    {
        lumaLocalAverage = 0.5f * (luma1 + lumaCenter);
    }
    // ���� �������� �� ���ĸ���.
    else
    {
        // ���� ������ �ٲ� (Step�� UV�� ���ҵǴ� �������� �̵��ؾ��Ѵ�.)
        stepLength = -stepLength;
        lumaLocalAverage = 0.5f * (luma2 + lumaCenter);
    }

    // UV �� �ùٸ� �������� ���ȼ� �̵���Ŵ
    float2 currentUv = input.uv;
    
    // ���� �����ڸ��� ������ �����սô� ..!
    if (isHorizontal)
    {
        currentUv.y += stepLength * 0.5f;
    }
    else
    {
        currentUv.x += stepLength * 0.5f;
    }
    
  // ������ �������� offset (�� �ݺ� ���ǿ� ����) �� ���.
    float2 offset = isHorizontal ? float2(_textureInfo.z, 0.0f) : float2(0.0, _textureInfo.w);

    // �����ڸ��� �� ������ �����Ͽ� Ž���ϱ� ���ؼ� UV�� ���. QUALITY �� ������ �� ������ ��.
    float2 uv1 = currentUv - offset;
    float2 uv2 = currentUv + offset;

    // Ž���ϴ� �ȼ��� ���� ������ lumas�� �а�, delta �� ����ϰ� ���� ��� luma�� ���
    float lumaEnd1 = RGBToLuma(Texture_Target.Sample(Sam_Clamp, uv1));
    float lumaEnd2 = RGBToLuma(Texture_Target.Sample(Sam_Clamp, uv2));
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;

    // ���� �������� luma delta�� ���� ��ȭ�� ���� ũ��, �츮�� �����ڸ��� ���鿡 ������ ����
    bool reached1 = abs(lumaEnd1) >= gradientScaled;
    bool reached2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reached1 && reached2;

    // ���鿡 �������� ���ߴٸ�, �츮�� ����ؼ� �� �������� Ž����.
    if (!reached1)
    {
        uv1 -= offset;
    }
    
    if (!reached2)
    {
        uv2 += offset;
    }
    
    if (!reachedBoth)
    {
        // ���� ���鿡 �������� �ʾҴٸ�, �츮�� �� �������� ��� Ž��, 
        for (int i = 1; i < MAX_SEARCH_ITERATIONS; i++)
        {
            // ���� ǰ���� ���� (���� : ������ �ݺ��� ���� �����ϰڴٴ� �ǹ�)
            // �ʿ��ϴٸ�, ù���� ������ luma�� ����, delta ���
            if (!reached1)
            {
                lumaEnd1 = RGBToLuma(Texture_Target.Sample(Sam_Clamp, uv1));
                lumaEnd1 -= lumaLocalAverage;
            }
        
            // �ʿ��ϴٸ�, �ݴ� ������ luma�� ����, delta ���
            if (!reached2)
            {
                lumaEnd2 = RGBToLuma(Texture_Target.Sample(Sam_Clamp, uv2));
                lumaEnd2 -= lumaLocalAverage;
            }
                
            // ���� ���� ������ luma delta�� ���� ��ȭ�� ���� ũ�ٸ�, �츮�� �����ڸ� ���鿡 ������ ����
            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reached1 && reached2;

            if (!reached1)
                uv1 -= offset * QUALITY[i];
            
            if (!reached2)
                uv2 += offset * QUALITY[i];
            
            // �� ���鿡 �����ߴٸ�, Ž���� �ߴ�
            if (reachedBoth)
            {
                break;
            }
        }
    }
    
    // �� �� �����ڸ������� �Ÿ��� ���
    float distance1 = isHorizontal ? (input.uv.x - uv1.x) : (input.uv.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - input.uv.x) : (uv2.y - input.uv.y);
    
    // � ������ �����ڸ��� ���� �� ������?
    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);

    // �����ڸ��� ����
    float edgeThickness = (distance1 + distance2);

    // UV offset: �����ڸ��� ������� ���� ����� �������� ����
    float pixelOffset = (-distanceFinal / edgeThickness) + 0.5f;
    
    // �߾� luma�� ���� ��պ��� �� �۳�?
    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

    // ���� �߾� luma�� �װ��� �̿����� �� �۴ٸ�, �� ���� delta luma�� ������� �մϴ�. (���� ����)
    // (�����ڸ��� ����� �� ����� ��������)
    bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

    // ���� luma ���̰� �߸��� ���, offset �� �������� ����
    float finalOffset = correctVariation ? pixelOffset : 0.0;
    
    // Sub-pixel shifting
    // 3x3 �̿��� ���� luma�� ��ü ���� ���
    float lumaAverage = (1.0f / 12.0f) * (2.0f * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

    // 3x3 �̿��� �縶 ������ ����, ���� ��հ� �߾� luma ���� delta�� ����
    float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0f, 1.0f);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

    // �� delta�� ����� sub-pixel�� offset ���
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    // �� offset �� �� ū���� ��
    finalOffset = max(finalOffset, subPixelOffsetFinal);
    
    float2 finalUv = input.uv;
    
    if (isHorizontal)
    {
        finalUv.y += finalOffset * stepLength;
    }
    else
    {
        finalUv.x += finalOffset * stepLength;
    }

    return float4(Texture_Target.Sample(Sam_Clamp, finalUv).xyz, 1.f);
}