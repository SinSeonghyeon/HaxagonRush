#pragma once

/// <summary>
/// �������� Ÿ���� �ϴ����� ����..
/// �ϴÿ��� �������ų�,
/// ���� �ð��� �Ǹ� ���� ������ �پ��ų�,
/// Ÿ�� �Ŵ����� �ʿ��� �� �ϴ�.
/// </summary>
class TileManager : public Muscle::IComponents
{
public:
	struct SELECTED_PLATFORM_INFO
	{
		uint32 _count;

		std::vector<StateColor> _platColors;
	};

	struct ATTACK_PLATFORM_INFO
	{
		uint32 _count;

		StateColor _color;
	};

public:
	TileManager(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~TileManager();

private:
	std::vector<std::shared_ptr<Platform>> _tiles; // �ʵ忡 ����ִ� Ÿ�ϵ�.

	std::vector<std::shared_ptr<Platform>> _tilePool; // õ�忡 �������� Ÿ��. Ÿ�� Ǯ ������� �����ϸ� �� ��.

	std::list<std::shared_ptr<Platform>> _selectPlatforms;	// ���� �÷��̾ ������ Ÿ��

private:
	static std::shared_ptr<TileManager> _instance;

	SELECTED_PLATFORM_INFO _selectedPlatformInfo;

	ATTACK_PLATFORM_INFO _attackPlatformInfo;
	
	float _endTimer = 3.0f;

	float _timer = 0; // ��ü ���� �ð��� �����Ѵ�.

	float _deleteTimer = 0;

	int _deleteIndex = 0;

public:
	void RandDropTile();

	void PushBackTile(std::shared_ptr<Platform> _tile);

	static std::shared_ptr<TileManager> Get();

	virtual void Start() override;

	virtual void Update() override;

	virtual void Finalize() override;

	void SelectCancel(); // ���� ��� Ÿ�� ���� ���

	void SelectDelete(); // ���� ��� ���� ������ Ÿ�� ����!

	bool CheckPlatforms();

	void SelectTile(std::shared_ptr<Platform> tile);

	bool FindSelectTile(std::shared_ptr<Platform> tile);


	// ���� ���õ� �÷������� ������ �����ؼ� ��ȯ�Ѵ٤�.
	const SELECTED_PLATFORM_INFO& GetCurrentSelectedInfo();

	// �� �� �� ���ž��մϴ�. ���� ������Ʈ�� ���� ���� ..! ����� ��
	const ATTACK_PLATFORM_INFO& GetCurrentAttackInfo();
};