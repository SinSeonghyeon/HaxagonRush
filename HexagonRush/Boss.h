#pragma once

// �������� 1 (Ʃ�丮���̿��µ� ��¼�ٺ��� �׳� �������� 1 ���� ������)
class Boss : public Character, public FSM<Boss>
{
public:
	enum Status
	{
		// ������ 1����
		ATTACK1,
		ATTACK2,
		ATTACK9,
		ATTACK10,
		ATTACK7,

		// ������ 2����
		ATTACK4,
		ATTACK3,

		// ������ 3����
		ATTACK6,
		ATTACK8,


		IDLE,
		HIT,
		DIE,
		NONE
	};

	enum Phase
	{
		Phase1,
		Phase2,
		Phase3
	};

public:
	Boss(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~Boss();

private:
	Phase _phase; // ���� ü�¿� ���� ������, ������ 1�ܰ�

	Status _checkStatus; // �������ͽ��� ���¸� üũ�ϱ� ���� ����. // Enter Stay Exit�� ���� �ʿ���.

	StateColor _currentColor; // ���� �Ӽ��� ����,

	std::weak_ptr<Muscle::GameObject> _hand1; // �� �߻� ����.

	std::weak_ptr<Muscle::GameObject> _hand2; // �� �߻� ����.

	std::list<std::shared_ptr<Muscle::SkinnedMeshRenderer>> _coreRenderer; // ���� ��ȯ�ϴ� 

	std::vector<std::shared_ptr<NumberRenderer>> _numberRenderers;

	Vector3 _initPosition; // ����ũ�� ��ġ�� �ٲ��.. �ʱ��������� ��������..!

	float _time = 0;

	float _colorTimer = 0;

	float _attackSpeed = 3.0f;

public:
	virtual StateColor GetColor() { return _currentColor; };

	Vector3 GetInitialPosition() { return _initPosition; }

	void SetColor(StateColor color);

	void SetState(Status status);

	void ChangePhase(Phase phase);

	virtual void Attack(std::shared_ptr<Character> enermy) override; // �����ϴ�!

	virtual void Attack() override; // 

	virtual void Hit(int damage) override; // ������ ���ϴ�!

	virtual void Die() override; // �״�!

	virtual void Finalize() override;

	std::shared_ptr<Muscle::GameObject> GetHand1();

	std::shared_ptr<Muscle::GameObject> GetHand2();

public:
	virtual void Start() override;

	virtual void Update() override;
};