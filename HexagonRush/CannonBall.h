#pragma once

class CannonBall : public Muscle::IComponents
{
public:
	CannonBall(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~CannonBall();

public:
	// ���� ��ġ�� �߻��Ѵ�. �׸��� ���鿡 ������ ������
	void Launch();

public:
	virtual void Start() override;

	virtual void Update() override;
};