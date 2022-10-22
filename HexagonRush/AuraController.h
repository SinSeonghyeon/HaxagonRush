#pragma once

// Ÿ�� ����� �� ������ ������ ��Ʈ���ϴ� �༮
class AuraController : public Muscle::IComponents
{
public:
	AuraController(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~AuraController();

private:
	std::shared_ptr<Muscle::ParticleRenderer> _particleRenderer;

	std::shared_ptr<RenderingData_Particle> _renderingData;

	std::shared_ptr<TileManager> _tileManager;

	bool _isFirstRun;

public:
	virtual void Start() override;

	virtual void Update() override;
};