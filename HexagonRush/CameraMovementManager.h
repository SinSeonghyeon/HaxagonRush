#pragma once


class CameraMovementManager : public Muscle::IComponents
{
public:
	CameraMovementManager(std::shared_ptr<Muscle::GameObject> gameObject);

	virtual ~CameraMovementManager();

private:
	static std::shared_ptr<CameraMovementManager> _instance;

	std::shared_ptr<Muscle::Camera> _camera; // ī�޶� ĳ���ؼ� ���.

	std::shared_ptr<Muscle::GameObject> _player; // �÷��̾� ĳ���ؼ� ���.

	std::shared_ptr<Muscle::GameObject> _boss; // ���� ĳ���ؼ� ���.

	bool _isMove; // �÷��̾��� �������� ���� ����.. ���� ī�޶� �̵����ΰ�?

	float _timer; // ī�޶��� ���� �̵��ð��� ����. 0 �����̸� ī�޶� �̵� ����.

public:
	void Stage1Start();

	void Stage2Start();

	void BossDieMove();

	void BossPhaseChangeMove();

	void PlayerDieMove();

	void PlayerRushMove();

public:
	bool GetisMove();

	static std::shared_ptr<CameraMovementManager> Get();

	virtual void Update() override;

	virtual void LateUpdate() override;

	virtual void Start() override;

	virtual void Finalize() override;
};