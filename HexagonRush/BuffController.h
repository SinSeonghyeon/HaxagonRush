#pragma once

// ������ ����� �� ������ ����Ʈ ����� �༮
class BuffController : public Muscle::IComponents
{
public:
	BuffController(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~BuffController();

private:
	std::shared_ptr<Muscle::ParticleRenderer> _hpRenderer;

	std::shared_ptr<Muscle::ParticleRenderer> _speedUpRenderer;

	std::shared_ptr<Muscle::ParticleRenderer> _powerUpRenderer;

	bool _isPlayingHP;

	bool _isPlayingSpeedUp;

	bool _isPlayingPowerUp;

	float _hpBuffEffectTime;

	float _speedUpBuffEffectTime;

	float _powerUpBuffEffectTime;

public:
	void OnHPBuff();

	void OnSpeedUpBuff();

	void OnPowerUpBuff();

	virtual void Update() override;
};