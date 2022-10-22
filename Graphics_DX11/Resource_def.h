#pragma once

enum class RESOURCE_TYPE
{
	// Load�� �Ϲ� �޽�
	MESH,

	// Load�� �Ϲ� �ؽ�ó
	TEXTURE,
	
	// Load�� �Ϲ� ��Ʈ
	FONT,

	// Load�� �Ϲ� Shader
	SHADER,

	// ���� Ÿ������ ����� �׸��� ���� �׷��� ����ϴ� �ؽ�ó .. �̰��� ���� Ÿ�� �������� ����
	RENDER_TARGET_TEXTURE,
};

enum class TEXTURE_TYPE
{
	DEFAULT,

	ENVIRONMENT,

	USE_MIPMAP
};