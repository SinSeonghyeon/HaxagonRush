#pragma once
#include "Graphics_RenderingData_3D.h"
#include "Graphics_PerFrameData.h"

namespace GraphicsEngine
{
	// 2�� 31��
	constexpr uint32 LIGHT_CULL_MASK = static_cast<uint32>(INT_MAX) + 1;

	// 1 ������ �� 1ȸ ������Ʈ�ϴ� Constant Buffer
	__declspec(align(16)) struct CB_PERFRAME
	{
		CameraInfo _cameraInfo;

		uint32 _directionalLightCount;

		Vector3 _pad1;

		DirectionalLightInfo _directionalLightInfos[LIGHT_INFO_MAX];

		uint32 _spotLightCount;

		Vector3 _pad2;

		SpotLightInfo _spotLightInfos[LIGHT_INFO_MAX];

		uint32 _pointLightCount;

		Vector3 _pad3;

		PointLightInfo _pointLightInfos[LIGHT_INFO_MAX];

		// Screen Space Ambient Occlusion
		SSAOInfo _ssaoInfo;
	};

#pragma region Legacy Lighting Model
	/*__declspec(align(16)) struct CB_PEROBJECT
	{
		Matrix _world;

		Matrix _worldInvTranspose;

		Matrix _worldViewProj;

		Matrix _texTransform;

		Vector4 _ambient;

		Vector4 _diffuse;

		Vector4 _specular;

		Vector4 _reflect;

		Matrix _boneMatrix[30];

		uint32 _objectID;
	};*/
#pragma endregion

#pragma region PBR Lighting Model
	__declspec(align(16)) struct CB_PEROBJECT
	{
		Matrix _world;

		Matrix _worldInvTranspose;

		Matrix _worldViewProj;

		Matrix _texTransform;

		float _metallic;

		float _roughtness;

		Vector2 _pad;

		Vector4 _emissive;

		Matrix _boneMatrix[70];

		uint32 _objectID;
	}; 
#pragma endregion

	__declspec(align(16)) struct CB_PERTEXTURE
	{
		// x : Width - y : Height - z : 1 / Width - w : 1 / Height
		Vector4 _screenInfo;

		Vector4 _textureInfo;
	};

	constexpr uint32 FRUSTUM_CASCADE_COUNT = 3;

	__declspec(align(16)) struct CB_DIRLIGHTVIEWPROJ
	{
		// �� �ɽ����̵� �� View Proj Mat
		Matrix _lightViewProjList[FRUSTUM_CASCADE_COUNT];

		// �� �ɽ����̵��� �� (Far) => �̰��� �������� ��� �ɽ����̵忡 ���ϴ���
		// ����Ѵ�. ���̴������� Float4�� �޾ƿ� !
		float _cascadeEnds[FRUSTUM_CASCADE_COUNT];

		float _pad;
	};

	__declspec(align(16)) struct CB_LIGHTVIEWPROJ
	{
		Matrix _lightViewProj;
	};

	__declspec(align(16)) struct CB_TONEMAPPING
	{
		float _exposure;

		Vector3 _pad;
	};

	// In IBL, Specular Pre-Filtered Map�� ���� �� ����ϴ� Constant Buffer�Դϴ�.
	// Roughness�� ���� ���� ���� �� ����ŧ�� �Ӹ��� ����� ���Ͽ�
	__declspec(align(16)) struct CB_SPECMAPFILTER
	{
		float _roughness;

		Vector3 _pad;
	};

	// ----------------------- Particle System -----------------------
	__declspec(align(16)) struct CB_PARTICLE_STREAMOUT			// ��Ʈ�� �ƿ� ������ �� �ʿ��� ����
	{
		Vector3 _emitVelocity;		// ���ڰ� ����Ǵ� �ӵ�
    
		float _radius;				// ������ 
    
		Vector2 _averageSize;		// ��� ������
    
		float _emitTime;			// ���ڸ� �����ϴ� �ð�
    
		float _lifeSpan;			// ���ڸ� ���̴� �ð�

		float _period;				// ���� �ൿ�� �ݺ��ϴ� �ֱ� (�� �ʿ� �� �� �ݺ�)

		float _randNumber;			// ���� �踮���

		Vector2 _pad;

		Matrix _transformMatrix;	// World Rotation Matrix => Look ���͸� �������� ��ƼŬ�� �����ǰ� �װ��� �� ��ķ� Transform �Ѵ�.
	};
	
	__declspec(align(16)) struct CB_PARTICLE_DRAW				// ���ڸ� �׸��� ������ �� �ʿ��� ����
	{
		// ��� ���ӵ�, ��� ���ӵ��� ������ �𵨸� �ùķ��̼����� ����غ���.
		Vector3 _acceleration;

		float _lifeSpan;			// ���ڸ� ���̴� �ð�
	};

	__declspec(align(16)) struct CB_PARTICLE_TIMER
	{
		CB_PARTICLE_TIMER(): _deltaTime(0.f), _totalTime(0.f)
		{

		}

		float _deltaTime;		// ���� �ùķ��̼��̴ٺ��� �ð��� �߿��ϴ�.

		float _totalTime;		// ��Ż Ÿ���� �ֱ⼺�� �̿��ϰ� ���� �� �� �� �� ���� �� ���Ƽ� �߹� ~

		Vector2 _pad;
	};

	__declspec(align(16)) struct CB_PARTICLE_COLOR
	{
		CB_PARTICLE_COLOR() : _color(Vector4(1.f, 1.f, 1.f, 1.f))
		{}

		Vector4 _color;
	};

	// ----------------------- UI -----------------------
	__declspec(align(16)) struct CB_COLOR
	{
		CB_COLOR() : _color(Vector4(1.f, 1.f, 1.f ,1.f))
		{}

		Vector4 _color;
	};
}