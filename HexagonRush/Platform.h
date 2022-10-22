#pragma once

/// <summary>
/// ����ü ���� ģ��..
/// </summary>
/// 
/// 
enum class PlatStatus
{
	Idle,
	Select,
	Delete,
	Respawn,
	Drop,
	PermanentlyDelete
};
class Platform : public Muscle::IComponents
{
public:
	Platform(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~Platform();

private:
	StateColor _color;

	PlatStatus _status;

	PlatStatus _preStatus; // ���� ����.

	std::shared_ptr<Muscle::Collider> _collider; // ĳ���ؼ� ���..!

	Vector3 _initialPosition; // �ʱ� ��ġ.

	std::shared_ptr<Platform> _dropPostion; // Ÿ���� ������ ��ġ.

	std::shared_ptr<RenderingData_3D> _renderingData; //ĳ���ؼ� ���..!

	std::shared_ptr<Muscle::ParticleRenderer> _particleRenderer;	// ��ƼŬ ������

	std::shared_ptr<RenderingData_Particle> _particleRenderingData;	

public:
	const StateColor& GetColor() { return _color; } // �ٴ� ���� ����

	void SetColor(StateColor color); // �ٴ� ���� ����

	void SetPosition(Vector3 initialPosition); // �ٴ� �ʱ� ��ġ ����

	void DeletePlatform(); // �÷��� ���� �ı�.

	void CreatePlatform(); // �÷��� �ı� �� �����.

	void SelectPlatform(); // �÷��̾ �÷����� ���� ���� ��.

	void SelectCanclePlatform(); // �÷��̾ �÷����� ������ ��� ���� ��.

	void PermanentlyDelete(); // �÷��̾ �÷����� ������ ��� ���� ��.

	void Drop(std::shared_ptr<Platform> platform); // Ÿ���� ������ ����Ʈ����. //Ǯ�� �ִ� ģ������ ����� �Լ�..

	void DropReset(); // Ÿ���� �ٽ� ���ͽ�Ų��. //Ǯ�� �ִ� ģ������ ����� �Լ�..

	Vector3 GetInitialPosition() { return _initialPosition; }

	PlatStatus GetState() { return _status; }

public:
	virtual void Start();

	virtual void Update(); // ��� Object Udpate���� �� (ObjectManager) ������Ʈ�� Components�� ������ ������ ���� ������Ʈ ���ֱ�.
};