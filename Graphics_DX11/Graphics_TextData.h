#pragma once

// �ؽ�Ʈ�� Ư���ϴϱ� ���� ������.
struct TextData
{
	TextData() : _fontID(7), _startCoord(Vector2(10.f, 10.f))
		, _color(Vector4(0.3f, 0.8f, 0.8f, 1.f)), _outText(TEXT("Text"))
	{ }

	uint64 _fontID;

	Vector2 _startCoord;

	Vector4 _color;

	tstring _outText;
};