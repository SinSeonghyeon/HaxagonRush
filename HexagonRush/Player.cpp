#include "pch.h"
#include "Player.h"

#include "SceneFader.h"

#define PLAYER_DEFAULT_HP 3

#define PLAYER_DEFAULT_SPEED 7.f

#define PLAYER_DEFAULT_DAMAGE 30

// ITEM 
#define HP_UP_COOL_TIME 5.f

#define SPEED_UP_BUFF_TIME 5.f

#define SPEED_UP_COOL_TIME 5.f

#define POWER_UP_BUFF_TIME 3.f

#define POWER_UP_COOL_TIME 5.f

#define SPEED_UP_MULTIPLY 2.f

#define POWER_UP_MULTIPLY 2.f
// ITEM

#define PER_PLATFORM_DAMAGE 20

// X Button
#define RUSH_START_KEY 0x58

// ���� ���� �ð�
#define STUN_TIME 3.f

// �д� ���� �ð�
#define PANIC_TIME 5.f

// ������ ���� �޺� ī��Ʈ ����. 6 !
#define MAX_COMBO_COUNT 1

Player::Player(std::shared_ptr<Muscle::GameObject> gameObj) :Character(gameObj), _isInvincibility(false),
_hpRemaningCool(0.f), _speedUpRemainingCool(0.f), _powerUpRemainingCool(0.f),
_isSpeedUpBuff(false), _isPowerUpBuff(false), _isStun(false), _isPanic(false)
{
	_checkStatus = Status::NONE;

	_speed = PLAYER_DEFAULT_SPEED;

	_hp = PLAYER_DEFAULT_HP;

	_damage = PLAYER_DEFAULT_DAMAGE;

	_comboCount = 0;

	_rushButtonUI = Muscle::CreateGameObject()->AddComponent<Muscle::UIRenderer>();
	_rushButtonUI->GetGameObject()->SetIsEnable(false);
	_rushButtonUI->SetDrawNDCPosition(Vector2(-0.15f, -0.5f), Vector2(0.15f, -0.8f));
}

Player::~Player()
{
}

void Player::SetHPUpBuff()
{
	_hp++;

	_hpRemaningCool = HP_UP_COOL_TIME;
}

void Player::SetSpeedUpBuff()
{
	_isSpeedUpBuff = true;

	_speedUpBuffTime = 0.f;

	_speedUpRemainingCool = SPEED_UP_COOL_TIME;

	_speed = PLAYER_DEFAULT_SPEED * SPEED_UP_MULTIPLY;
}

void Player::SetPowerUpBuff()
{
	_isPowerUpBuff = true;

	_powerUpBuffTime = 0.f;

	_powerUpRemainingCool = POWER_UP_COOL_TIME;

	// �̰� ������ �ǹ̾��� .. Attack���� Power Up Buff�� ��, ���� ���� �������� ����Ѵ�.
	_damage = static_cast<int>(PLAYER_DEFAULT_DAMAGE * POWER_UP_MULTIPLY);
}

void Player::SetSpeedDown()
{
	_speed = 0.5f * PLAYER_DEFAULT_SPEED;
}

void Player::SetSpeedNormal()
{
	_speed = PLAYER_DEFAULT_SPEED;
}

void Player::SetNotStun()
{
	_isStun = false;
}

void Player::SetPanic()
{
	_isPanic = true;

	std::function<void(std::shared_ptr<Muscle::GameObject>)> func = [](std::shared_ptr<Muscle::GameObject> gameObj)
	{
		std::shared_ptr<Player> player = gameObj->GetComponent<Player>();

		player->SetNotPanic();
	};

	Muscle::CTime::Invoke(func, PANIC_TIME, GetGameObject());
}

void Player::SetNotPanic()
{
	_isPanic = false;
}

void Player::SetStun()
{
	_isStun = true;


	std::function<void(std::shared_ptr<Muscle::GameObject>)> func = [](std::shared_ptr<Muscle::GameObject> gameObj)
	{
		std::shared_ptr<Player> player = gameObj->GetComponent<Player>();

		player->SetNotStun();
	};

	Muscle::CTime::Invoke(func, STUN_TIME, GetGameObject());
}

int Player::CalcDamage(std::shared_ptr<Character> enemy)
{
	int retDamage = 0.f;

	if (enemy != nullptr)
	{
		StateColor curBossColor = enemy->GetColor();

		uint32 info = static_cast<uint32>(_shootingBulletInfo.front()); _shootingBulletInfo.pop();

		if (info < 3)
			retDamage = PER_PLATFORM_DAMAGE * 3;
		else if (info < 6)
			retDamage = PER_PLATFORM_DAMAGE * 4;
		else if (info < 9)
			retDamage = PER_PLATFORM_DAMAGE * 5;

		info %= 3;

		switch (curBossColor)
		{
		case StateColor::Red:
		{
			// Ǯ�� ����
			if (info == 1)
				retDamage *= 0.7f;
			// ���� ����
			else if (info == 2)
				retDamage *= 1.5f;

			break;
		}

		case StateColor::Green:
		{
			// ���� ����
			if (info == 2)
				retDamage *= 0.7f;
			// �ҷ� ����
			else if (info == 0)
				retDamage *= 1.5f;

			break;
		}

		case StateColor::Blue:
		{
			// �ҷ� ����
			if (info == 0)
				retDamage *= 0.7f;
			// Ǯ�� ����
			else if (info == 1)
				retDamage *= 1.5f;

			break;
		}
		}
	}

	// �Ŀ��� ���� ���̸� 2���
	return _isPowerUpBuff ? retDamage * POWER_UP_MULTIPLY : retDamage;
}

void Player::LookEnermy()
{
	// ���� �ٶ󺸵��� ȸ����
	std::shared_ptr<Character> target = _target.lock();

	Vector3 bossPos = target->GetGameObject()->GetTransform()->GetWorldPosition();
	bossPos.y = GetGameObject()->GetTransform()->GetPosition().y;
	GetGameObject()->GetTransform()->LookAt(bossPos);
	GetGameObject()->GetTransform()->RotateY(3.14f);

}

void Player::SetState(Status status)
{
	if (_checkStatus == status)
		return;

	switch (status)
	{
	case Player::IDLE:
	{
		FSM<Player>::SetState(std::make_shared<PlayerState::Idle>());
		break;
	}

	case Player::MOVE:
	{
		// ����, ���� ��Ȳ, ���� ��Ȳ���� �������� �����Ѵ�.
		if (_checkStatus == Player::Status::ATTACK ||
			_checkStatus == Player::Status::HIT ||
			_checkStatus == Player::Status::RUSH)
			return;

		FSM<Player>::SetState(std::make_shared<PlayerState::Move>());

		break;
	}

	case Player::ATTACK:
	{
		if (_checkStatus == Player::HIT)
		{
			// �´� �߿� ������ �� �� ���� ..? => �� ������ ���Դٴ� ���� .. TileManager����
			// ���� ���� Ÿ���� �ѹ� ��ҽ�ų �ʿ䰡 �ֵ��� �� ..?

			return;
		}

		FSM<Player>::SetState(std::make_shared<PlayerState::Attack>());

		break;
	}

	case Player::EVASION:
	{
		FSM<Player>::SetState(std::make_shared<PlayerState::Evasion>());

		break;
	}

	case Player::HIT:
	{
		FSM<Player>::SetState(std::make_shared<PlayerState::Hit>());

		break;
	}

	case Player::RUSH:
	{
		// ���� ������Ʈ�� ���ϴ�. �̺�Ʈ�� �������ֽ��ϴ�.
		FSM<Player>::SetState(std::make_shared<PlayerState::Rush>());

		break;
	}

	case Player::DIE:
	{
		FSM<Player>::SetState(std::make_shared<PlayerState::Die>());

		CameraMovementManager::Get()->PlayerDieMove();

		break;
	}

	default:
		break;
	}

	_checkStatus = status;
}

void Player::SetInvincible()
{
	_isInvincibility = true;
}

void Player::SetNotInvincible()
{
	_isInvincibility = false;
}

// ź�� �浹���� �߻��ϴ� �༮
void Player::Attack(std::shared_ptr<Character> enermy)
{
	if (enermy != nullptr)
	{
		// ������ ���� �������� ����Ѵ�.
		int damage = CalcDamage(enermy);

		// �ϴ� ���⼭ �־��ݽô� .. �� �� ���� ������ �޺�ī��Ʈ ����
		_comboCount++;

		enermy->Hit(damage);

		switch (_comboCount)
		{
		case 1:
			break;
		case 2:
			GetSoundManager()->Play("Combo_1", IPlayMode::Effect);
			break;
		case 3:
			GetSoundManager()->Play("Combo_2", IPlayMode::Effect);
			break;
		case 4:
			GetSoundManager()->Play("Combo_3", IPlayMode::Effect);
			break;
		case 5:
			GetSoundManager()->Play("Combo_4", IPlayMode::Effect);
			break;
		case 6:
			GetSoundManager()->Play("Combo_5", IPlayMode::Effect);
			break;
		default:
			break;
		}
	}
}

void Player::Attack()
{
	// True => ���� ���� (3�� �̻�, ���� ���� Complete !)
	if (TileManager::Get()->CheckPlatforms())
	{


		TileManager::Get()->SelectDelete(); // ����!

		LookEnermy();

		SetState(Player::ATTACK);
	}
	else // ����
	{
		TileManager::Get()->SelectCancel();
	}
}

void Player::Hit(int damage)
{
	// �̹� �¾Ұų�, ���� �����̰ų�, ���� ���� (�׳� �´°ɷ� �ߴ�. 8�� 22��), �����̸� �̸� ���� �ʴ´� !
	if ((_checkStatus == Player::HIT) || (_checkStatus == Player::DIE) /*|| (_checkStatus == Player::RUSH)*/ ||
		_isInvincibility)
		return;

	SetState(Player::HIT);

	if (_checkStatus == Player::HIT)
	{
		Muscle::CTime::SetGameSpeed(0.1f, 0.1f);

		_hp -= damage;

		if (_hp <= 0)
			SetState(Player::DIE);
	}
}

void Player::Move(const Vector3& directional)
{
	// �����̸� �� ��������
	if (_isStun)
		return;

	SetState(Player::MOVE);

	if (_checkStatus == Player::MOVE)
		_Trasform->SetPosition(_Trasform->GetPosition() + directional * _speed * Muscle::CTime::GetGameDeltaTime());
}

void Player::Die()
{
	SetState(Player::DIE);
}

void Player::Evasion()
{
	SetState(Player::EVASION);

	if (_checkStatus != Player::EVASION)
	{
	}
}

void Player::Finalize()
{
	Character::Finalize();

	FSM<Player>::Finalize();
}

void Player::PostShootingBulletInfo(SHOOTING_BULLET_INFO info)
{
	_shootingBulletInfo.push(info);
}

void Player::Start()
{
	Character::Start();

	SetState(Player::IDLE);

	// �ʱ�ȭ ���־���� !
	_isSpeedUpBuff = false;

	_speedUpBuffTime = SPEED_UP_BUFF_TIME;

	_isPowerUpBuff = false;

	_powerUpBuffTime = POWER_UP_BUFF_TIME;


}

void Player::Update()
{
	if (GameProcess::Get()->GetIsPausePanelUp())
		return;

	// ������ �̵� �Ǵ� ���̵� ������ ���� ����� �� �ִ�.
	if ((Muscle::KeyBoard::Get()->KeyDown(RUSH_START_KEY) || Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonB, Muscle::XPadCommand::Push))
		&& (_checkStatus == Status::IDLE || _checkStatus == Status::MOVE)
		&& (_checkStatus != Player::DIE)
		&& !CameraMovementManager::Get()->GetisMove())		// ī�޶� �������� �ʰ� ���� ��
	{
		if (_comboCount >= MAX_COMBO_COUNT)
		{
			LookEnermy();

			SetState(Player::RUSH);

			_comboCount = 0;
		}
	}

	//#ifdef _DEBUG
		// ������ ���δ� ��û~ ����� ���� ���Ѵ޶�~
	if (Muscle::KeyBoard::Get()->KeyDown(VK_F9))
	{
		_isInvincibility = !_isInvincibility;
	}
	//#endif

	float gameDelta = Muscle::CTime::GetGameDeltaTime();

	if (_isSpeedUpBuff)
	{
		_speedUpBuffTime += gameDelta;

		if (_speedUpBuffTime > SPEED_UP_BUFF_TIME)
		{
			_speed = PLAYER_DEFAULT_SPEED;

			_isSpeedUpBuff = false;
		}
	}

	if (_isPowerUpBuff)
	{
		_powerUpBuffTime += gameDelta;

		if (_powerUpBuffTime > POWER_UP_BUFF_TIME)
		{
			_damage = PLAYER_DEFAULT_DAMAGE;

			_isPowerUpBuff = false;
		}
	}

	_hpRemaningCool -= gameDelta;
	_speedUpRemainingCool -= gameDelta;
	_powerUpRemainingCool -= gameDelta;

	_currentStatus->OnStay();
}