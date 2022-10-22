#pragma once

// Player Header �߰� ����

class Character;

/// <summary>
/// Bullet_Prefab ������ ���� ������Ʈ���� �ٿ����� �������� �ش� �Ѿ��� ������ �ϰԵ� �༮
/// </summary>
class BulletController : public Muscle::IComponents
{
public:
	// �Ѿ��� ���� ����
	enum class BULLET_STATE
	{
		LOAD,
		FLY,
		BOOM,
		END
	};

	// �Ѿ��� ���� (Ʈ���Ű� 3����, 4����, 5���� ..)
	enum class BULLET_LEVEL
	{
		ONE = 1,
		TWO = 2,
		THREE = 3
	};

	enum class BULLET_COLOR
	{
		RED = 0,
		GREEN = 1,
		BLUE = 2
	};

public:
	BulletController(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~BulletController();

private:
	std::shared_ptr<Muscle::Transform> _topTransform;

	BULLET_STATE _currBulletState;

	BULLET_LEVEL _currBulletLevel;

	BULLET_COLOR _currBulletColor;

	std::shared_ptr<Character> _agent;

	Vector3 _destination;

	float _timeFromBoom;

	std::shared_ptr<Muscle::GameObject> _burn;

	std::shared_ptr<Muscle::ParticleRenderer> _burnRenderer;

	std::shared_ptr<Muscle::GameObject> _afterBurn;

	std::shared_ptr<Muscle::ParticleRenderer> _afterBurnRenderer;

	std::shared_ptr<Muscle::GameObject> _boom;

	std::shared_ptr<Muscle::ParticleRenderer> _boomRenderer;

private:
	void SettingParticleOfBurn();

	void SettingParticleOfAfterBurn();

	void SettingParticleOfBoom();

public:
	// �߻� �� �߻��� ���, ������, ����, �÷� �������ְ� �߻��� �� !
	void SettingAgent(std::shared_ptr<Character> agent) { _agent = agent; }

	void SetDestination(Vector3 dest) { _destination = dest; }

	void SetBulletLevel(BULLET_LEVEL level) { _currBulletLevel = level; }

	void SetBulletColor(BULLET_COLOR color) { _currBulletColor = color; }

	virtual void Start() override;

	virtual void Update() override;
};