#include "pch.h"
#include "Controller.h"

// Z��ư
#define TRIGGER_BUTTON 0x005A

// SPACE BAR
#define EVASION_BUTTON VK_SPACE

#define EVASION_COOL_TIME 3.f

Controller::Controller(std::shared_ptr<Muscle::GameObject> gameObject) :IComponents(gameObject), _evasionCoolTiem(0.f)
{

}

Controller::~Controller()
{

}

void Controller::Start()
{
	_player = GetComponent<Player>();

	_playerTransform = _player->GetComponent<Muscle::Transform>();

	_evasionCoolTiem = 0.f;
}

// �÷��̾� Ŭ������ ���η��� �ߴµ� ������ ��� ��Ʈ�ѷ��� ������ �; ���⿡ ��ġ��!
void Controller::OnStayCollision(std::shared_ptr<Muscle::Collider> _Other)
{
	const Player::Status& playerStatus = _player->GetCheckStatus();

	// ������ �� �� ���� ��Ȳ��
	if ((playerStatus == Player::Status::ATTACK) || (playerStatus == Player::Status::DIE) || (playerStatus == Player::Status::HIT)
		|| (playerStatus == Player::Status::EVASION) || (playerStatus == Player::Status::RUSH))
		return;

	if (CameraMovementManager::Get()->GetisMove())
		return;

	// �浹 ���̸鼭 ���� Ʈ���� ��ư�� ������ �ִٸ� ..!
	if (Muscle::KeyBoard::Get()->KeyPress(TRIGGER_BUTTON) || Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonX, Muscle::XPadCommand::KeepPress))
	{
		std::shared_ptr<Platform> temp = _Other->GetComponent<Platform>();

		if (temp)
			TileManager::Get()->SelectTile(temp);
	}
}

void Controller::AttackUpdate()
{
	const Player::Status& playerStatus = _player->GetCheckStatus();

	// ������ �� �� ���� ��Ȳ�� !
	if ((playerStatus == Player::Status::ATTACK) || (playerStatus == Player::Status::DIE) || (playerStatus == Player::Status::HIT)
		|| playerStatus == Player::Status::EVASION)
		return;

	// Z ��ư�� ������ �ִٰ� ���ٸ� ..?
	if (Muscle::KeyBoard::Get()->KeyUp(TRIGGER_BUTTON) || Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonX, Muscle::XPadCommand::Release))
	{
		// ���� �õ� ! => ������ ���� ������ ������ �� ���� (���ο��� ���� ����)
		_player->Attack();
	}
}

void Controller::MoveUpdate()
{
	const Player::Status& playerStatus = _player->GetCheckStatus();

	// ���긦 �� �� ���� ��Ȳ�� !
	if ((playerStatus == Player::Status::ATTACK) || (playerStatus == Player::Status::DIE) || (playerStatus == Player::Status::HIT)
		|| playerStatus == Player::Status::EVASION || playerStatus == Player::Status::RUSH)
		return;

	if (CameraMovementManager::Get()->GetisMove()) return; // ī�޶� �������̸� �� �� ����..!

	Vector3 look = Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform()->GetXMLook();
	look.y = 0;
	look.Normalize();

	Vector3 right = Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform()->GetRight();
	right.y = 0;
	right.Normalize();

#pragma region 8 Way Move
	Vector3 dir;

#pragma region USE_XPAD
	if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = (-right) + (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right)+(-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down))
	{
		dir = (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right)+(look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = (-right) + (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up))
	{
		dir = (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if(Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = -(right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
#pragma endregion

#pragma region USE_KEYBOARD
	if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN) && Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = (-right) + (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN) && Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right)+(-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN))
	{
		dir = (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP) && Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right)+(look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);

		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP) && Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = (-right) + (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);

		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP))
	{
		dir = (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = -(right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
#pragma endregion

#pragma endregion
}

void Controller::PanicMoveUpdate()
{
	const Player::Status& playerStatus = _player->GetCheckStatus();

	// ���긦 �� �� ���� ��Ȳ�� !
	if ((playerStatus == Player::Status::ATTACK) || (playerStatus == Player::Status::DIE) || (playerStatus == Player::Status::HIT)
		|| playerStatus == Player::Status::EVASION || playerStatus == Player::Status::RUSH)
		return;

	if (CameraMovementManager::Get()->GetisMove()) return; // ī�޶� �������̸� �� �� ����..!

	Vector3 look = Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform()->GetXMLook();
	look.y = 0;
	look.Normalize();

	look = -look;

	Vector3 right = Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform()->GetRight();
	right.y = 0;
	right.Normalize();

	right = -right;

#pragma region 8 Way Move
	Vector3 dir;

#pragma region USE_XPAD
	if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = (-right) + (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right)+(-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down))
	{
		dir = (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right)+(look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up) && Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = (-right) + (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up))
	{
		dir = (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right))
	{
		dir = (right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left))
	{
		dir = -(right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
#pragma endregion

#pragma region USE_KEYBOARD
	if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN) && Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = (-right) + (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN) && Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right)+(-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_DOWN))
	{
		dir = (-look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP) && Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right)+(look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);

		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP) && Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = (-right) + (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);

		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_UP))
	{
		dir = (look);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);

		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT))
	{
		dir = (right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
	else if (Muscle::KeyBoard::Get()->KeyPress(VK_LEFT))
	{
		dir = -(right);

		dir.Normalize();

		GetGameObject()->GetTransform()->SetLook(dir);
		GetGameObject()->GetTransform()->RotateY(3.14f);


		_player->Move(dir);
	}
#pragma endregion

#pragma endregion
}

void Controller::EvasionUpdate()
{
	// CheckStatus�� PrevStatus�� �����̴�.
	const Player::Status& playerStatus = _player->GetCheckStatus();

	// ȸ�Ǹ� �� �� ���� ��Ȳ�� !
	if (/*(playerStatus == Player::Status::ATTACK) ||*/ (playerStatus == Player::Status::DIE) || (playerStatus == Player::Status::HIT)
		|| playerStatus == Player::Status::EVASION || playerStatus == Player::Status::IDLE)
		return;

	if ((Muscle::KeyBoard::Get()->KeyDown(EVASION_BUTTON) || Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonA, Muscle::XPadCommand::Push)) && (_evasionCoolTiem >= EVASION_COOL_TIME))
	{
		// �� Ű�� ���� �������� DeltaTime���� ȸ�� �̵� �ӵ���ŭ �̵��ؾ��Ѵ�. (������ ���� ?
		// �ӵ��� �Ƹ� �ִϸ��̼ǿ� ���缭 ���� ���ҽ�ų �ʿ䰡 ���� ���� ..?
		_evasionCoolTiem = 0.f;

		// �̵��� PlayerState -> OnEnter���� ������.
		_player->Evasion();
	}
}

void Controller::Update()
{
	if (_player->GetIsStun())
		return;

	if (GameProcess::Get()->GetIsPausePanelUp())
		return;

	_evasionCoolTiem += Muscle::CTime::GetGameDeltaTime();

	EvasionUpdate();

	if (!_player->GetIsPanic())
		MoveUpdate();
	else
		PanicMoveUpdate();

	AttackUpdate();
}