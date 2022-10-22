#pragma once
#include "Graphics_DX11_typedef.h"

// �� �Ƿ��� �Ѱ�� ���� �Ϲ����� ���͵��� �̿��� ���� �ù����̼� ������ ����� �� ���� ..
// �׳� ���� Ÿ���� ������ �ش� Ÿ���� ���� ���Ǵ�� �ùķ��̼��� �ϵ��� ����.
enum class PARTICLE_EFFECT_TYPE
{
	CIRCLE,
	SPHERE,
};

struct Particle_InitInfo
{
	Particle_InitInfo() : _particleEffectType(PARTICLE_EFFECT_TYPE::CIRCLE), _firstRun(true), _isLoop(true),
		_duration(5.f) ,_particlePlayID(ULLONG_MAX), _maxParticleCount(500)
	{}

	PARTICLE_EFFECT_TYPE _particleEffectType;	// �׸��µ� ����ϴ� ���̴��� �ٸ� �� ���� ..
												// ���� �޶����� Particle ����ü ���� �޶��� �� �ִ�.

	bool _firstRun;					// �����ΰ��� ..?

	bool _isLoop;					// ��� �ݺ��ϳ��� ? => False�̸� 

	float _duration;				// ���� �ð� (isLoop False�̸� �� �ð���ŭ�� ���� �� ����

	uint32 _maxParticleCount;		// �ִ� ��ƼŬ�� ����

	uint64 _particlePlayID;			// First Run�� �� �� �ο�����. �� �༮���� Curr�� �����.
};

// �ʱ� �ӵ�, �ʱ� ��ġ, ��� ���ӵ� (�ð��� ���� �Լ��� ���������� .. ?!) �� ���ؼ� �����Ǵ� ���
struct Particle_ShapeInfo
{
	Particle_ShapeInfo() :  _transformMatrix(Matrix::Identity), _emitVelocity(Vector3::Up), _radius(5.f), _emitTime(0.005f), _period(0.2f), 
		_particleAverageSize(Vector2(0.5f, 0.5f)), _deviation(Vector2(0.001f, 0.001f)), _useXYSameDeviation(true), _refTextureID(ULLONG_MAX), _acceleration(Vector3(0.f, 3.f, 0.f)), _lifeSpan(1.5f), _color(Vector4(1.f, 1.f, 1.f, 1.f))
	{}

	// Stream - Output Info	
	Vector3 _emitVelocity;			// �����ϴ� �ӵ�

	Matrix _transformMatrix;		// ��ƼŬ�� ���� ��ġ �� ���� ����

	float _radius;					// ������ (_emitPosition���� _radius��ŭ ������ ������ ���ڰ� �����ȴ�.)

	float _emitTime;				// ���� ���⿡ �ɸ��� �ð� (�е� ������ �� ��)

	float _period;					// �ֱ� (�� ���� �ֱ�� ����� ������ ����� �׸���)

	Vector2 _particleAverageSize;	// Particle�� ��� ������.

	Vector2 _deviation;				// X�� Y ���� ����

	bool _useXYSameDeviation;		// X�� Y �Ȱ��� ������ ����ϳĴ� �ɼ� => ���� ũ��� ���;��ϴ� ��ƼŬ�� �ٸ� ������ ����ϸ� ��ƼŬ�� ������� ������ ���� �ֱ� ����.

	uint64 _refTextureID;			// ��ƼŬ ����Ʈ�� ����ϴ� ���̵� (���� ���� �����ϴ� ��쵵 �����ٵ� ..)

	Vector3 _acceleration;			// ��� ���ӵ�

	float _lifeSpan;				// ��ƼŬ ���� �� ����

	Vector4 _color;					// ��ƼŬ�� ���� ���� ����
};

// ��ƼŬ �ý����� ����ϱ� ���� �������̽�
// �� ���� ���� �ý����� �����Ѵ�.
struct RenderingData_Particle
{
	RenderingData_Particle() : _initInfo(std::make_shared<Particle_InitInfo>()), _shapeInfo(std::make_shared<Particle_ShapeInfo>())
	{}

	std::shared_ptr<Particle_InitInfo> _initInfo;

	std::shared_ptr<Particle_ShapeInfo> _shapeInfo;
};