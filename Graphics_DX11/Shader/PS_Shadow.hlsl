struct VS_OUT
{
    float4 pos : SV_POSITION;
};

float4 PS_Shadow(VS_OUT input) : SV_Target
{   
    // ���̸� �� ���� Texture�� ��� (���������� �� ���� ���� ����. ���̰��� ���� ���� Ÿ���� �� �Ἥ ..)
    return float4(input.pos.zzz, 1.f);
}