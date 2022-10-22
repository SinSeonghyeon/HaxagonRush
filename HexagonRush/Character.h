#pragma once

/// <summary>
/// �������̽� Ŭ�����̴�..!
/// ������ �ְ� �ޱ����ؼ� ĳ���� Ŭ������ ��ӽ��� �����!
/// </summary>

class Character : public Muscle::IComponents
{
public:
	Character(std::shared_ptr<Muscle::GameObject> gameObj) :IComponents(gameObj), _speed(8.0f), _hp(5), _damage(1), _target()
	{
	}

	virtual ~Character() = default;

protected:
	std::weak_ptr<Character> _target; // ���� ���! 

	std::shared_ptr<Muscle::Transform> _Trasform;

	std::shared_ptr<Muscle::Animater> _animater;

	float _speed;

	int _hp;

	int _Maxhp;

	int _damage;

public:
	virtual void Start()
	{
		_Trasform = GetGameObject()->GetTransform();

		_animater = GetComponent<Muscle::Animater>();
	}; // Transfrom ĳ�� ��������

	virtual void Attack(std::shared_ptr<Character> enermy) abstract; // �����ϴ�!

	virtual void Attack() abstract; // �����ϴ�!

	virtual void Hit(int damage) abstract; // ������ ���ϴ�!

	virtual void Die() abstract; // ������ ���ϴ�!

	virtual void Move(const Vector3& directional) {};

	virtual void Evasion() {};

	virtual StateColor GetColor() abstract;

	int GetHp() { return _hp; };

	int GetMaxHp() { return _Maxhp; };

	virtual void Finalize() override
	{
		_target.reset();
		_Trasform.reset();
		_animater.reset();
	}

	std::shared_ptr<Character> GetTarget() { return _target.lock(); }

	void SetTarget(std::shared_ptr<Character> teraget)
	{
		_target = teraget;
	};

	std::shared_ptr<Muscle::Animater> GetAnimater()
	{
		return _animater;
	};


public:
	const float& GetSpeed() { return _speed; }

};

