#pragma once

// �÷��̾��� ���۰� ���õ� Ŭ����
class Controller : public Muscle::IComponents
{
public:
	Controller(std::shared_ptr<Muscle::GameObject> gameObject);

	virtual ~Controller();

private:
	std::shared_ptr<Muscle::Transform> _playerTransform;

	std::shared_ptr<Player> _player;
	
	float _evasionCoolTiem;

public:
	virtual void OnStayCollision(std::shared_ptr<Muscle::Collider> _Other);

private:
	void AttackUpdate();

	void MoveUpdate();

	void PanicMoveUpdate();

	void EvasionUpdate();

public:
	virtual void Start();

	virtual void Update(); // ��� Object Udpate���� �� (ObjectManager) ������Ʈ�� Components�� ������ ������ ���� ������Ʈ ���ֱ�.
};