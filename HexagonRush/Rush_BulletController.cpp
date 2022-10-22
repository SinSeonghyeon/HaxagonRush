#include "pch.h"
#include "Rush_BulletController.h"

#define RUSH_BULLET_DAMAGE 100

#define RUSH_BULLET_TIME 1.5f

#define EPSILON 1.5f

Rush_BulletController::Rush_BulletController(std::shared_ptr<Muscle::GameObject> gameObj) : 
	IComponents(gameObj), _isCollided(false), _deleteTimer(0.f), _rushBulletState(RUSH_BULLET_STATE::STORED), _destination(Vector3::Zero),
	_bulletLifeTimer(0.f)
{

}

Rush_BulletController::~Rush_BulletController()
{

}

float Rush_BulletController::CheckDistance()
{
	return (_destination - GetGameObject()->GetTransform()->GetWorldPosition()).Length();
}

void Rush_BulletController::SettingParticleOfBurn()
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

	initInfo->_maxParticleCount = 500;

	shapeInfo->_emitVelocity = Vector3::Zero;

	shapeInfo->_radius = 0.f;

	shapeInfo->_emitTime = 0.005f;

	shapeInfo->_period = 0.2f;

	shapeInfo->_particleAverageSize = Vector2(0.6f, 0.6f);

	shapeInfo->_color = Vector4(0.8f, 0.8f, 0.8f, 1.f);

	shapeInfo->_acceleration = Vector3::Zero;		// ������ ���� �ʰ� ���� ������Ʈ�� �ִ� �ʿ��� �׳� �����ȴ� !

	shapeInfo->_lifeSpan = 0.5f;
}

void Rush_BulletController::SettingParticleOfAfterBurn()
{
	std::shared_ptr<RenderingData_Particle> renderingData = _afterBurnRenderer->GetParticleData();

	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;

	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;

	// Init Info
	initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::CIRCLE;

	initInfo->_firstRun = true;

	initInfo->_isLoop = true;

	initInfo->_maxParticleCount = 100;

	// Shape Info
	shapeInfo->_emitVelocity = Vector3::Zero;

	shapeInfo->_radius = 2.5f;

	shapeInfo->_emitTime = 0.005f;

	shapeInfo->_period = 0.4f;

	// �ֺ� ���� ���� ���� ����
	shapeInfo->_color = _color;

	shapeInfo->_particleAverageSize = Vector2(0.3f, 0.3f);

	shapeInfo->_acceleration = Vector3::Zero;			// �߷� �� �� �غ���

	shapeInfo->_lifeSpan = 0.3f;
}

void Rush_BulletController::SettingParticleOfBoom()
{
	std::shared_ptr<RenderingData_Particle> renderingData = _boomRenderer->GetParticleData();

	std::shared_ptr<Particle_InitInfo> initInfo = renderingData->_initInfo;

	std::shared_ptr<Particle_ShapeInfo> shapeInfo = renderingData->_shapeInfo;

	// Init Info
	initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::SPHERE;

	initInfo->_firstRun = true;

	initInfo->_isLoop = false;

	initInfo->_duration = 2.0f;

	initInfo->_maxParticleCount = 1200;

	// Shape Info
	shapeInfo->_emitVelocity = Vector3::Up;		// ������ �� ����� ���̴��� Speed Zero�̸� ���ڸ��� �ִ´�

	shapeInfo->_radius = 1.5f;

	shapeInfo->_emitTime = 0.005f;

	shapeInfo->_period = 0.2f;

	// �����°� AfterBurn�̶� Boom�� ���� �̷��� !
	shapeInfo->_color = _color;

	shapeInfo->_particleAverageSize = Vector2(0.25f, 0.25f);

	shapeInfo->_acceleration = Vector3::Down;

	shapeInfo->_lifeSpan = 1.f;
}

void Rush_BulletController::LaunchTo(const Vector3& dest)
{
	// Ÿ�̸� �ǵ�����
	_bulletLifeTimer = 0.f;

	_isCollided = false;

	_rushBulletState = RUSH_BULLET_STATE::LAUNCHED;

	_destination = dest;

	_deleteTimer = 0.f;

	_burnRenderer->Play();

	_afterBurnRenderer->Play();

	// ������ �������� / ���� �ֱ⼺
	static std::random_device rd;

	static std::mt19937 gen(rd());

	static std::uniform_real_distribution<float> angleDist(0.f, 3.141592f * 2);

	static std::uniform_real_distribution<float> radiusDist(3.f, 6.f);

	float angle = angleDist(gen);

	float radius = radiusDist(gen);

	// ���� 8�� 23�� ���� �÷��̾� ���� �躤�Ͱ� Up��.
	Vector3 playerLook = _playerTransform->GetUp();

	Vector3 playerRight = _playerTransform->GetRight();

	Matrix bulletRot = Matrix::CreateFromAxisAngle(playerLook, angle);

	Vector3 circleOffset = Vector3::Transform(playerRight * radius, bulletRot);

	Vector3 bulletStartPoint = _playerTransform->GetWorldPosition() + playerLook * 3.f + circleOffset;

	float distantce = (_destination - bulletStartPoint).Length();

	// �̰� ���߿� �� ������ �ϵ��ڵ����� ���� ��ġ�� ���� ! => �ƴϸ� �ѱ� ��ġ
	// ���� �÷��̾�� Up�� Look�̴� ! (8�� 21�� ����)
	// ĳ���� ������ ���� ������ ��ġ�� ã��.
	GetGameObject()->GetTransform()->SetPosition(bulletStartPoint);

	ObjectMover::Get()->MoveFromTo(GetGameObject()->GetTransform(), GetGameObject()->GetTransform()->GetPosition(), _destination, 1.f, 0,
		MoveInfo::Bezier, playerLook * distantce * 0.3f + circleOffset * 2.f, playerLook * distantce * 0.6f + circleOffset,
		MoveInfo::Look, false, _destination);
}

void Rush_BulletController::SetColor(const Vector4& color)
{
	_color = color;
}

void Rush_BulletController::Start()
{
	std::vector<std::shared_ptr<Muscle::GameObject>> sceneObjects = Muscle::IGameEngine::Get()->GetObjManager()->m_vectorObjects;

	for (auto& obj : sceneObjects)
	{
		if (obj->GetName() == "Boss")
		{
			_boss = obj->GetComponent<Character>();
		}
		else if (obj->GetName() == "Player")
		{
			_playerTransform = obj->GetTransform();

			_player = obj->GetComponent<Player>();
		}
	}

	std::vector<std::shared_ptr<Muscle::GameObject>> childs = GetGameObject()->GetChildrens();

	// ��ǰ ĳ��
	for (auto& child : childs)
	{
		if (child->GetTag() == "Burn")
		{
			_burn = child;

			_burnRenderer = _burn->GetComponent<Muscle::ParticleRenderer>();
		}
		else if (child->GetTag() == "AfterBurn")
		{
			_afterBurn = child;

			_afterBurnRenderer = _afterBurn->GetComponent<Muscle::ParticleRenderer>();
		}
		else if (child->GetTag() == "Boom")
		{
			_boom = child;

			_boomRenderer = _boom->GetComponent<Muscle::ParticleRenderer>();
		}
	}

	// ��ƼŬ ����Ʈ ���� ����
	SettingParticleOfBurn();

	SettingParticleOfAfterBurn();

	SettingParticleOfBoom();
}

void Rush_BulletController::Update()
{
	// ���� ��Ȳ�̸� ������Ʈ ���
	if (_rushBulletState == RUSH_BULLET_STATE::STORED)
		return;

	_bulletLifeTimer += Muscle::CTime::GetGameDeltaTime();

	if (!_isCollided)
	{
		if (_bulletLifeTimer > RUSH_BULLET_TIME)
			_player->StoreRushBullet(GetGameObject());

		// �������� �Ÿ� ����
		float dist = CheckDistance();

		if (dist < EPSILON)
		{
			_isCollided = true;

			// ���� �͵� �� ���� ���� ��ƼŬ�� ����
			_burnRenderer->Stop();

			_afterBurnRenderer->Stop();

			_boomRenderer->Play();

			_boss->Hit(RUSH_BULLET_DAMAGE);
		}

		return;
	}

	// �浹�� �ߴٸ� �ð��� ���. (��� ����Ʈ ������ �����ο� ������)
	_deleteTimer += Muscle::CTime::GetGameDeltaTime();

	if (_deleteTimer >= 5.f)
	{
		// ������Ʈ Ǯ�� �ǵ�����.
		_boomRenderer->Stop();

		_player->StoreRushBullet(GetGameObject());
	}
}