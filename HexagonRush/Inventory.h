#pragma once

class BuffController;

// �κ��丮 Ŭ����.
// �������� ���� 3�� 
class Inventory : public Muscle::IComponents
{
public:
	Inventory(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~Inventory();

private:
	std::shared_ptr<Player> _player;

	std::shared_ptr<BuffController> _buffController;

	// ������ ����Ʈ
	uint32 _hpItemCount;

	uint32 _powerUpItemCount;

	uint32 _speedUpItemCount;
	
private:
	// HP Up Item �� �� �ִ��� üũ
	bool CheckCanUseHPItem();

	// Speed Up �� �� �ִ��� üũ
	bool CheckCanUseSpeedItem();

	// Power Up �� �� �ִ��� üũ
	bool CheckCanUsePowerItem();

public:
	uint32 GetHPItemCount() {return _hpItemCount;}

	uint32 GetPowerItemCount() { return _powerUpItemCount; }

	uint32 GetSpeedItemCount() { return _speedUpItemCount; }

	virtual void Start() override;

	virtual void Update() override;
};