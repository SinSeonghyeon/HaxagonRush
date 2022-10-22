#pragma once

/// <summary>
/// �� ģ���� ���ӸŴ������ ��������!
/// ���� �Ŵ��� Ŭ������ ������� �ߴµ� ���� ���μ��� ������ �ʹ� ������ ������...
/// </summary>
class ISoundManager;

class ResourceManager;

class UIManager;

class GameProcess
{
public:
	GameProcess() = default;

	~GameProcess();

private:
	std::shared_ptr<Muscle::IGameEngine> m_GameEngine;

	ISoundManager* m_SoundManager;

	std::shared_ptr<ResourceManager> _resourceManager;

	// �Ͻ� ���� �г��� Pop-Up �Ǿ����ϱ� ..?
	// �̰� ���ؼ� ��� Script Component �� Scene�� Update�� �����ϰڽ��ϴ�.
	bool _isPausePanelUp = false;

public:
	// �̱���ȭ
	static std::shared_ptr<GameProcess> Get();

	static std::shared_ptr<GameProcess> _gameProcess;
	
public:
	inline bool GetIsPausePanelUp() { return _isPausePanelUp; };

	inline void SetIsPausePanelUp(bool value) { _isPausePanelUp = value; }

	void Initialize(HWND _Hwnd, int WindowSizeX, int WindowSizeY);

	void Update();

	void Render();

	// �޽��� �ڵ鷯 (���� �ݹ�)
	int MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
