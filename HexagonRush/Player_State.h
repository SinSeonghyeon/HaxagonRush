#pragma once

#include "DieScene.h"
#include "Rush_BulletController.h"
#include "ResourceManager.h"

// Z��ư
#define RUSH_SHOOTING_KEY_KEABOARD 0x005A

#define RUSH_SHOOTING_KEY_KEABOARD2 0x43

#define PLAYER_AnimSpeed 2
/// <summary>
/// Ch_Attack
/// Ch_Attack_Rush
/// Ch_Dead
/// Ch_Hurt
/// Ch_idle
/// Ch_Run
/// </summary>
namespace PlayerState
{
	class Idle : public IState<Player>
	{
	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(0.5f, 10000.0f);
			this->_character = player;
			this->_character.lock()->GetAnimater()->Play("Ch_idle");
		}

		virtual void OnStay()
		{
			if (!this->_character.lock()->GetAnimater()->GetisPlay())
				this->_character.lock()->GetAnimater()->Play("Ch_idle");
		}
		 
		virtual void OnExit()
		{
			this->_character.lock()->GetAnimater()->Stop();
		}
	};

	class Move : public IState<Player>
	{
	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed, 10000.0f);
			this->_character = player;
			this->_character.lock()->GetAnimater()->Play("Ch_Run");
		}

		virtual void OnStay()
		{
			if (!this->_character.lock()->GetAnimater()->GetisPlay())
				this->_character.lock()->GetAnimater()->Play("Ch_Run");

			if (!Muscle::KeyBoard::Get()->KeyPress(VK_UP) &&
				!Muscle::KeyBoard::Get()->KeyPress(VK_RIGHT) &&
				!Muscle::KeyBoard::Get()->KeyPress(VK_LEFT) &&
				!Muscle::KeyBoard::Get()->KeyPress(VK_DOWN) &&
				!Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Up) &&
				!Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Right) &&
				!Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Left) &&
				!Muscle::XPad::Get()->IsPadStickMove(Muscle::XPadInput::Down))
			{
				this->_character.lock()->SetState(Player::IDLE);
			}
		}

		virtual void OnExit()
		{
			this->_character.lock()->GetAnimater()->Stop();
		}
	};

	class Attack : public IState<Player>
	{
	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed, 10000.0f);
			GetSoundManager()->Play("Attack", IPlayMode::Effect);
			this->_character = player;
			this->_character.lock()->GetAnimater()->Play("Ch_Attack");
		}

		virtual void OnStay()
		{
			if (!this->_character.lock()->GetAnimater()->GetisPlay())
				this->_character.lock()->SetState(Player::IDLE);
		}

		virtual void OnExit()
		{
			this->_character.lock()->GetAnimater()->Stop();
		}
	};

	class Hit : public IState<Player>
	{
		std::shared_ptr<Player> _player;

	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			_player = player;

			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed, 10000.0f);
			this->_character = player;
			this->_character.lock()->GetAnimater()->Play("Ch_Hurt");

			GetSoundManager()->Play("Hit", IPlayMode::Effect);

			// ���� ���� ������ ����Ѵ�.
			TileManager::Get()->SelectCancel();

			// �¾��� �� ���� ��������� ..
			player->SetInvincible();
		}

		virtual void OnStay()
		{
			if (!this->_character.lock()->GetAnimater()->GetisPlay())
				this->_character.lock()->SetState(Player::IDLE);
		}

		virtual void OnExit()
		{
			this->_character.lock()->GetAnimater()->Stop();

			// 1.5�� �� ������ Ǯ������.
			std::function<void(std::shared_ptr<Muscle::GameObject>)> func1 = [](std::shared_ptr<Muscle::GameObject> gameObj) {
				gameObj->GetComponent<Player>()->SetNotInvincible();
			};

			Muscle::CTime::Invoke(func1, 1.5f, _player->GetGameObject());
		}
	};

	class Evasion : public IState<Player>
	{
	private:
		Vector3 _currEvasionWay;

		std::shared_ptr<Player> _currPlayer;

	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed, 10000.0f);

			GetSoundManager()->Play("Evade", IPlayMode::Effect);

			_currPlayer = player;

			this->_character = player;

			this->_character.lock()->GetAnimater()->Play("Ch_evasion");

			std::shared_ptr<Muscle::Transform> transform = this->_character.lock()->GetGameObject()->GetTransform();

			// �ǰ� ������ �ݶ��̴� OFF ..!
			this->_character.lock()->GetComponent<Muscle::Collider>()->SetIsEnable(false);

			// ���� �ͻ�� ���� �������� �־ Up�� Look�̴�.
			_currEvasionWay = -transform->GetLook();

			_currEvasionWay.y = 0.f;

			// �ִϸ��̼� Ÿ�� ���� �̵� => ������Ʈ ������ �̵��ϸ� �վ����
			// ObjectMover::Get()->MoveBy(transform, moveWay * player->GetSpeed(), 1.5f);

			// ȸ���� ���� ������ ����Ѵ�.
			TileManager::Get()->SelectCancel();
		}

		virtual void OnStay()
		{
			if (!this->_character.lock()->GetAnimater()->GetisPlay())
			{
				this->_character.lock()->SetState(Player::IDLE);

				return;
			}

			std::shared_ptr<Muscle::Transform> transform = this->_character.lock()->GetGameObject()->GetTransform();

			// ���� �ͻ�� ���� �������� �־ Up�� Look�̴�.
			Vector3 currPos = transform->GetPosition();

			transform->SetPosition(currPos + _currEvasionWay * Muscle::CTime::GetGameDeltaTime() * _currPlayer->GetSpeed() * 2);
		}

		virtual void OnExit()
		{
			// �ǰ� ������ �ݶ��̴� ON !
			this->_character.lock()->GetComponent<Muscle::Collider>()->SetIsEnable(true);

			this->_character.lock()->GetAnimater()->Stop();
		}
	};

	constexpr float RUSH_FULL_TIME = 5.f;

	constexpr float RUSH_GAME_SPEED = 0.2f;

	/// <summary>
/// Ch_Attack
/// Ch_Attack_Rush
/// Ch_Dead
/// Ch_Hurt
/// Ch_idle
/// Ch_Run
/// </summary>

	class Rush : public IState<Player>
	{
	private:
		float _rushTime;

		// �׳� ĳ���ϰ� ����ҷ� !
		std::shared_ptr<Player> _player;

	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed * 10, 10000.0f); // ������ �ٸ� �ִϸ��̼Ǻ��� 5�� ������ ����,

			_player = player;

			this->_character = player;

			// Ȥ�� �𸣴� Offset 2 sec
			Muscle::CTime::SetGameSpeed(RUSH_GAME_SPEED, RUSH_FULL_TIME + 2.f);

			_rushTime = 0.f;

			// ���� �ִϸ��̼� ������ �ǽ�
			this->_character.lock()->GetAnimater()->Play("Ch_Attack_Rush");

			// ī�޶� ����
			CameraMovementManager::Get()->PlayerRushMove();

			GetSoundManager()->SetSoundSpeed(Muscle::CTime::GetGameSpeed(), IPlayMode::Effect);
			GetSoundManager()->SetSoundSpeed(Muscle::CTime::GetGameSpeed(), IPlayMode::BGM);

			this->_character.lock()->GetRushButtonUI()->GetGameObject()->SetIsEnable(true);
			auto uiData = this->_character.lock()->GetRushButtonUI()->GetUIData();

			uiData->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(TEXT("ButtonUp"));

			Muscle::XPad::Get()->StartVibration();
		}

		virtual void OnStay()
		{
			// �ð��ʸ� ���鼭 ���� Ÿ���� ���� ������ ������ �����Ѵ�.
			// ���� Ÿ���� ������ ..
			_rushTime += Muscle::CTime::GetDeltaTime();
			auto uiData = this->_character.lock()->GetRushButtonUI()->GetUIData();

			uiData->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(TEXT("ButtonUp"));

			// �ܿ� ���� �ð�
			_player->SetRushRemainingTime(RUSH_FULL_TIME - _rushTime);

			if (_rushTime >= RUSH_FULL_TIME)
			{
				this->_character.lock()->SetState(Player::IDLE);

				return;
			}


			auto CreateBullet = [&]()
			{
				// źȯ ������Ʈ ����
				std::shared_ptr<Muscle::GameObject> rushBullet = _player->GetRushBullet();

				// Stage 1 ���� �ھ� ������ ���ư� �� �ִ� �ϵ� �ڵ�
				Vector3 dest = _player->GetGameObject()->GetTransform()->GetWorldPosition() + (_player->GetTarget()->GetGameObject()->GetTransform()->GetWorldPosition() - _player->GetGameObject()->GetTransform()->GetWorldPosition())
					+ _player->GetTarget()->GetGameObject()->GetTransform()->GetUp() * 8;

				// �߻� !
				rushBullet->GetComponent<Rush_BulletController>()->LaunchTo(dest);

				uiData->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(TEXT("ButtonDown"));
				ObjectMover::Get()->Shake(Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform(), 0.01f);

				GetSoundManager()->Play("Rush", IPlayMode::Effect);

				this->_character.lock()->GetAnimater()->RePlay("Ch_Attack_Rush");
			};


#pragma region USE_XPAD
			//  ������ ���� ���������� �ϴ� ������ ��� �׸��� ���ư��� źȯ ������Ʈ �߻� (Rush_BulletController)
			if (Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::LTRIGGER, Muscle::XPadCommand::Push) ||
				Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::RTRIGGER, Muscle::XPadCommand::Push))
			{
				CreateBullet();
			}
#pragma endregion

#pragma region USE_KEYBOARD
			// ������ ���� ���������� �ϴ� ������ ��� �׸��� ���ư��� źȯ ������Ʈ �߻� (Rush_BulletController)
			if (Muscle::KeyBoard::Get()->KeyDown(RUSH_SHOOTING_KEY_KEABOARD) || Muscle::KeyBoard::Get()->KeyDown(RUSH_SHOOTING_KEY_KEABOARD2))
			{
				CreateBullet();
			}

#pragma endregion
		}

		virtual void OnExit()
		{
			// �޸� Ǯ ���� Reset
			_player->ResetRushBulletPrefabIndex();

			Muscle::CTime::SetGameSpeed(1.f, 0.f);

			this->_character.lock()->GetRushButtonUI()->GetGameObject()->SetIsEnable(false);

			// ������ �ӵ� ����
			GetSoundManager()->SetSoundSpeed(Muscle::CTime::GetGameSpeed(), IPlayMode::Effect);
			GetSoundManager()->SetSoundSpeed(Muscle::CTime::GetGameSpeed(), IPlayMode::BGM);

			// ���� ����
			Muscle::XPad::Get()->StopVibration();
		}
	};

	class Die : public IState<Player>
	{
	public:
		virtual void OnEnter(std::shared_ptr<Player> player)
		{
			player->GetAnimater()->SetAnimSpeed(PLAYER_AnimSpeed, 10000.0f);

			this->_character = player;
			this->_character.lock()->GetAnimater()->Play("Ch_Dead");

			// ���� ���� ������ ����Ѵ�.
			TileManager::Get()->SelectCancel();

			// �ױ� ������ �� �̸��� ���� (For Retry)
			DieScene::SetRetrySceneName(Muscle::IGameEngine::Get()->GetSceneManager()->GetCurrentSceneName());
		}

		virtual void OnStay()
		{
		}

		virtual void OnExit()
		{
			this->_character.lock()->GetAnimater()->Stop();
		}
	};
}