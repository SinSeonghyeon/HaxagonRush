#pragma once
#include "Graphics_DX11_typedef.h"

struct DrawInfo
{
	DrawInfo() : _textureID(ULLONG_MAX), _sortOrder(5),
		_color(Vector4(1.f, 1.f, 1.f, 1.f)), _useAlphaBlend(true)
	{
	}

	uint64 _textureID;

	uint32 _sortOrder;

	Vector4 _color;

	bool _useAlphaBlend;
};

// NDC ���������� LeftTop, Right Bottom
// (-1, 1)
struct NDCInfo
{
	Vector2 _leftTop;

	Vector2 _rightBottom;
};

// ȭ�� �ػ� ��ȭ�� ���� ��ǥ�� ������ �����ϱ� ���� NDC ���� ���
struct RenderingData_UI
{
	RenderingData_UI() : _drawInfo(std::make_shared<DrawInfo>()), _ndcInfo(std::make_shared<NDCInfo>())
	{}

	std::shared_ptr<DrawInfo> _drawInfo;

	std::shared_ptr<NDCInfo> _ndcInfo;
};

// ȭ�� �ػ󵵿� ���缭 ��Ŀ�� ���� �θ� �þ�� ���� �þ���Ѵ�.

// ��ƼŬ ���̴� �������ϴϱ� ���� ���� ��Ÿ�� ����.