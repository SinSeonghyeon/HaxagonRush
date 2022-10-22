#include "pch.h"
#include "BulletController.h"

#define EPSILON 1.5f
#define BULLET_SPEED 45.f

BulletController::BulletController(std::shared_ptr<Muscle::GameObject> gameObj) :
	IComponents(gameObj), _currBulletState(BULLET_STATE::LOAD), _burn(nullptr), _burnRenderer(nullptr),
	_afterBurn(nullptr), _afterBurnRenderer(nullptr), _boom(nullptr), _boomRenderer(nullptr), _destination(Vector3::Zero)
{

}

BulletController::~BulletController()
{
	_topTransform.reset();

	_burn.reset();

	_burnRenderer.reset();

	_afterBurn.reset();

	_afterBurnRenderer.reset();

	_boom.reset();

	_boomRenderer.reset();
}

void BulletController::SettingParticleOfBurn()
{
#pragma region EZREAL_ULT
//	std::shared_ptr<RenderingData_Particle> renderingData = _burnRenderer->GetParticleData();
//
//	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;
//
//	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;
//
//	// Init Info
//	initInfo->_firstRun = true;
//
//	initInfo->_isLoop = true;
//
//	initInfo->_duration = 1.5f;
//
//	initInfo->_maxParticleCount = static_cast<uint32>(_currBulletLevel) * 200;
//
//	// Shape Info
//	shapeInfo->_emitVelocity = _destination - _topTransform->GetWorldPosition();
//
//	shapeInfo->_emitVelocity.Normalize();
//
//	shapeInfo->_emitVelocity *= static_cast<uint32>(_currBulletLevel) * 1.5f;
//
//	shapeInfo->_radius = static_cast<uint32>(_currBulletLevel) * 1.f;
//
//	shapeInfo->_emitTime =  0.01f / static_cast<uint32>(_currBulletLevel);
//
//	shapeInfo->_period = 0.2f;
//
//	shapeInfo->_particleAverageSize = Vector2(1.f, 1.f);
//
//	shapeInfo->_acceleration = Vector3::Zero;		// ������ ���� �ʰ� ���� ������Ʈ�� �ִ� �ʿ��� �׳� �����ȴ� !
//
//	shapeInfo->_lifeSpan = 1.f;
#pragma endregion

	std::shared_ptr<RenderingData_Particle> renderingData = _burnRenderer->GetParticleData();

	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;

	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;

	// Init Info
	initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::CIRCLE;

	initInfo->_firstRun = true;

	initInfo->_isLoop = true;

	initInfo->_maxParticleCount = static_cast<uint32>(_currBulletLevel) * 200;
	
	shapeInfo->_emitVelocity = Vector3::Zero;

	shapeInfo->_radius = 0.f;

	shapeInfo->_emitTime = 0.005f;

	shapeInfo->_period = 0.2f;

	shapeInfo->_particleAverageSize = Vector2(0.4f, 0.4f) * static_cast<uint32>(_currBulletLevel);

	if (_currBulletColor == BULLET_COLOR::RED)
		shapeInfo->_color = Vector4(1.f, 0.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::GREEN)
		shapeInfo->_color = Vector4(0.f, 1.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::BLUE)
		shapeInfo->_color = Vector4(0.f, 0.1f, 0.9f, 1.f);

	shapeInfo->_acceleration = Vector3::Zero;		// ������ ���� �ʰ� ���� ������Ʈ�� �ִ� �ʿ��� �׳� �����ȴ� !

	shapeInfo->_lifeSpan = 0.5f;
}

void BulletController::SettingParticleOfAfterBurn()
{
	std::shared_ptr<RenderingData_Particle> renderingData = _afterBurnRenderer->GetParticleData();

	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;

	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;

	// Init Info
	initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::CIRCLE;

	initInfo->_firstRun = true;

	initInfo->_isLoop = true;

	initInfo->_maxParticleCount = static_cast<uint32>(_currBulletLevel) * 40;

	// Shape Info
	shapeInfo->_emitVelocity = Vector3::Zero;

	shapeInfo->_radius = static_cast<uint32>(_currBulletLevel) * 0.5f;

	shapeInfo->_emitTime = 0.01f / static_cast<uint32>(_currBulletLevel);

	shapeInfo->_period = 0.4f;

	if (_currBulletColor == BULLET_COLOR::RED)
		shapeInfo->_color = Vector4(1.f, 0.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::GREEN)
		shapeInfo->_color = Vector4(0.f, 1.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::BLUE)
		shapeInfo->_color = Vector4(0.f, 0.f, 1.f, 1.f);

	shapeInfo->_particleAverageSize = Vector2(0.15f, 0.15f) * static_cast<uint32>(_currBulletLevel);

	shapeInfo->_acceleration = Vector3::Zero;			// �߷� �� �� �غ���

	shapeInfo->_lifeSpan = 0.3f;
}

void BulletController::SettingParticleOfBoom()
{
	std::shared_ptr<RenderingData_Particle> renderingData = _boomRenderer->GetParticleData();

	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;

	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;

	// Init Info
	initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::SPHERE;

	initInfo->_firstRun = true;

	initInfo->_isLoop = false;

	initInfo->_duration = 2.0f;

	initInfo->_maxParticleCount = static_cast<uint32>(_currBulletLevel) * 600;

	// Shape Info
	shapeInfo->_emitVelocity = Vector3::Up;		// ������ �� ����� ���̴��� Speed Zero�̸� ���ڸ��� �ִ´�

	shapeInfo->_radius = static_cast<uint32>(_currBulletLevel) * 0.5f;

	shapeInfo->_emitTime = 0.005f;

	shapeInfo->_period = 0.2f;

	if (_currBulletColor == BULLET_COLOR::RED)
		shapeInfo->_color = Vector4(1.f, 0.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::GREEN)
		shapeInfo->_color = Vector4(0.f, 1.f, 0.f, 1.f);
	else if (_currBulletColor == BULLET_COLOR::BLUE)
		shapeInfo->_color = Vector4(0.f, 0.1f, 0.9f, 1.f);

	shapeInfo->_particleAverageSize = Vector2(0.1f, 0.1f) * static_cast<uint32>(_currBulletLevel);

	shapeInfo->_acceleration = Vector3::Down;

	shapeInfo->_lifeSpan = 1.f;
}

// ���� �ʱ�ȭ�� �� ȣ��Ǵ� �Լ����� .. �� ���� ���Ҹ� �� �� �ְڴ�.
void BulletController::Start()
{
	auto&& childrens = GetGameObject()->GetChildrens();

	_topTransform = GetGameObject()->GetTransform();

	// ���� ������ �´� ������Ʈ���� ĳ�� �� ���� �� 
	for (auto& children : childrens)
	{
		auto&& tag = children->GetTag();

		if (tag == "Burn")
		{
			_burn = children;
			
			_burnRenderer = children->GetComponent<Muscle::ParticleRenderer>();
		}
		else if (tag == "AfterBurn")
		{
			_afterBurn = children;

			_afterBurnRenderer = children->GetComponent<Muscle::ParticleRenderer>();
		}
		else if (tag == "Boom")
		{
			_boom = children;

			_boomRenderer = children->GetComponent<Muscle::ParticleRenderer>();
		}
	}
}

void BulletController::Update()
{
	if (GameProcess::Get()->GetIsPausePanelUp())
		return;
	
	switch (_currBulletState)
	{
		case BULLET_STATE::LOAD:
		{
			// �Ѿ��� ���ư��� ������ Destination�� �ٶ󺸵��� ���� !
			_topTransform->LookAt(_destination, _topTransform->GetWorldPosition());

			// ��ƼŬ ������ �������ְ� ..!
			SettingParticleOfBurn();

			SettingParticleOfAfterBurn();

			SettingParticleOfBoom();

			// �� �Ѿ��� �߻��� ������Ʈ���� �ʰ� ���� �Ѿ��� ������ ����.

			_currBulletState = BULLET_STATE::FLY;

			_burnRenderer->Play();

			_afterBurnRenderer->Play();

			break;
		}

		case BULLET_STATE::FLY:
		{
			// �������� �����ߴ��� üũ
			if ((_destination - _topTransform->GetWorldPosition()).Length() < EPSILON)
			{
				// �����ߴٸ� ���� ���·� ����
				_currBulletState = BULLET_STATE::BOOM;

				// �ٷ� ��ž���� ���� 1�� ���� �� ��ž�ϴ°� ���� �� ������ ..
				_burnRenderer->Stop();

				_afterBurnRenderer->Stop();

				// ���濡�� ���� ���Ѵ�. �̰��� �Ѿ��� ���� ����
				_agent->Attack(_agent->GetTarget());

				// ������ �����̴�
				_boomRenderer->Play();

				_timeFromBoom = 0.f;
			}
			// ���� �� �ߴٸ� �������� Ʈ������ !
			else
			{
				float delta = Muscle::CTime::GetGameDeltaTime();

				Vector3 currPos = _topTransform->GetWorldPosition();

				_topTransform->SetPosition(currPos + _topTransform->GetLook() * BULLET_SPEED * delta);
			}

			break;
		}

		case BULLET_STATE::BOOM:
		{
			// ������ ����� �߻� �ӵ�
			float delta = Muscle::CTime::GetGameDeltaTime();

			Vector3 currPos = _topTransform->GetWorldPosition();

			_timeFromBoom += Muscle::CTime::GetGameDeltaTime();

			_topTransform->SetPosition(currPos + _topTransform->GetLook() * static_cast<uint32>(_currBulletLevel) * BULLET_SPEED * delta);

			if (_timeFromBoom >= _boomRenderer->GetParticleData()->_initInfo->_duration)
				_currBulletState = BULLET_STATE::END;

			break;
		}

		case BULLET_STATE::END:
		{
			_burnRenderer->Stop();

			_afterBurnRenderer->Stop();

			_boomRenderer->Stop();

			// ���� ! �߻� �󵵰� ������ Ǯ�� ���� �ʴ´�.
			Muscle::DeleteGameObject(GetGameObject());

			break;
		}
	}
}

// �÷��̾� ��Ʈ�ѷ����� ���� ���߸� Pool���� �Ѿ� ������ �߻��Ŵ !

// ó�� �߰��� ���� Update�� LOAD ���·� �����Ѵ�. (Ǯ���� STATE::LOAD�� ������ ��)

// LOAD ���¿����� ������ �ܰ迡 ���� ��ƼŬ �������� ���¸� ������Ʈ�Ѵ�.

// FLY ���¿����� �׳� ���ư��� ..? ������ �� �� ������.

// BOOM ���¿����� �� ������� ��ƼŬ�� �ѹ濡 �Ѹ���. �ܰ躰 Duration ! IsLoop Of => ������ ���� �������� EmitVelocity ����� �� �� ..