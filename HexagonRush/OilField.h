#pragma once

class Player;

// ��� ������ �̼��� ���ҽ�Ų��. ���� ���������� !
class OilField : public Muscle::IComponents
{
public:
	OilField(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~OilField();

private:
	// �÷��̾�
	std::shared_ptr<Player> _currentPlayer;

	float _timer;

	float _isSetUp;

public:
	virtual void Start() override;

	virtual void Update() override;

	virtual void OnStayCollision(std::shared_ptr<Muscle::Collider> _Other) override;
	
	virtual void OnExitCollision(std::shared_ptr<Muscle::Collider> _Other) override;
};