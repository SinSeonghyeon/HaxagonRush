#pragma once

#include "BulletController.h"

/// <summary>
/// �÷��̾� ������Ʈ
/// </summary>
class Platform;

class Rush_Bullet_Prefab;

// �ƽ� �޺� ī��Ʈ�� �Ǹ� Bullet Time �� Rush 
constexpr uint32 MAX_COMBO_COUNT = 5;

class Player : public FSM<Player>, public Character
{
public:
	enum class SHOOTING_BULLET_INFO
	{
		ONE_RED = 0,
		ONE_GREEN = 1,
		ONE_BLUE = 2,

		TWO_RED = 3,
		TWO_GREEN = 4,
		TWO_BLUE = 5,

		THREE_RED = 6,
		THREE_GREEN = 7,
		THREE_BLUE = 8
	};

	enum Status
	{
		IDLE = 0,
		MOVE = 1,
		ATTACK = 2,
		HIT = 3,
		DIE = 4,
		EVASION = 5,
		RUSH = 6,
		NONE = 7
	};

public:
	Player(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~Player();

private:
	Status _checkStatus; // �������ͽ��� ���¸� üũ�ϱ� ���� ����. // Enter Stay Exit�� ���� �ʿ���.

	float _hpRemaningCool;

	bool _isSpeedUpBuff;

	float _speedUpBuffTime;

	float _speedUpRemainingCool;

	bool _isPowerUpBuff;

	float _powerUpBuffTime;

	float _powerUpRemainingCool;

	// �޺� ī��Ʈ�� 5�� �Ǹ� State Rush !
	uint32 _comboCount;

	// �� �Ѿ��� �������� ����ϰ� �ִٰ� ������ ��꿡 ����Ѵ�.
	std::queue<SHOOTING_BULLET_INFO> _shootingBulletInfo;

	std::shared_ptr<Muscle::UIRenderer> _rushButtonUI;

private:
	std::vector<std::shared_ptr<Muscle::GameObject>> _rushBulletPrefabPool;

	int _rushBulletPrefabIndex = 0;

	float _rushRemainingTime = 0.f;

	bool _isInvincibility;

public:
	inline std::shared_ptr<Muscle::GameObject> GetRushBullet()
	{
		_rushBulletPrefabPool[_rushBulletPrefabIndex]->SetIsEnable(true);

		return _rushBulletPrefabPool[_rushBulletPrefabIndex++];
	}

	inline void StoreRushBullet(std::shared_ptr<Muscle::GameObject> rushBullet) { rushBullet->SetIsEnable(false); }

	inline float GetRushRemainingTime() { return _rushRemainingTime; }

	inline void SetRushRemainingTime(const float& time) { _rushRemainingTime = time; }

	inline void ResetRushBulletPrefabIndex() { _rushBulletPrefabIndex = 0; }

private:
	bool _isStun;

	bool _isPanic;

public:
	std::shared_ptr<Muscle::UIRenderer> GetRushButtonUI() { return _rushButtonUI; }

	const int& GetCurrentHP() { return _hp; }

	void SetHPUpBuff();

	void SetSpeedUpBuff();

	void SetPowerUpBuff();

	// ���ǵ� ���� ����
	void SetSpeedDown();

	// ����ȭ
	void SetSpeedNormal();

	// ���� ����
	void SetStun();

	// ���� ����
	void SetNotStun();

	// �д� ����
	void SetPanic();

	// �д� ����
	void SetNotPanic();

	inline bool GetIsStun() { return _isStun; }

	inline bool GetIsPanic() { return _isPanic; }

	inline void AddComboCount() { _comboCount++; }

private:
	// ������ ���� �������� ����մϴ�. (�� �Ӽ�, �� źȯ �Ӽ� �� ..)
	int CalcDamage(std::shared_ptr<Character> enemy);

	void LookEnermy();

public:
	void SetState(Status status);

	// ĳ���͸� �������� �����.
	void SetInvincible();

	// ���� ����
	void SetNotInvincible();

	virtual void Attack(std::shared_ptr<Character> enermy) override;

	virtual void Attack() override;

	virtual void Hit(int damage) override;

	virtual void Move(const Vector3& directional) override;

	virtual void Die() override;

	virtual void Evasion() override;

	virtual void Finalize() override;

	virtual StateColor GetColor() {return StateColor(); };

public:
	void PostShootingBulletInfo(Player::SHOOTING_BULLET_INFO info);

	inline const Status& GetCheckStatus() { return _checkStatus; }

	virtual void Start();

	virtual void Update();

	friend class Controller;

	friend class Inventory;

	friend class Player_Prefab;

	friend class ItemUIPanel;
};