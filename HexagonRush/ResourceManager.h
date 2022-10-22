#pragma once
/// <summary>
/// ���� �뵵�� ����ұ�?
/// �ϴ��� ASE������ �ε��ϴ� ������ �ұ�?
/// ���嵵 ���⼭ �ұ�?
/// </summary>
/// 
/// 
class Muscle::ResourceManager;

class ResourceManager : public std::enable_shared_from_this<ResourceManager>
{
public:
	ResourceManager() = default;

	~ResourceManager() = default;

private:
	std::shared_ptr<Muscle::ResourceManager> _graphicsResourceManager; //ĳ���ؼ� ����� ��.

	static std::shared_ptr<ResourceManager> _instance;

	std::queue<std::tuple<tstring, tstring, bool,bool>> _aseLoadDataQueue;

	std::queue<std::tuple<tstring, tstring>> _textureLoadDataQueue;

	std::queue<std::tuple<tstring, tstring>> _environmentPanoramaMapLoadDataQueue;

	std::queue<std::tuple<std::string, std::string,unsigned int>> _soundLoadDataQueue;

	int _resourceAllCount;

	bool _isComplete;

	void DataQueuePopToThead(); // �񵿱� ����~ ������ ������ ������ ~

public:
	const uint64 GetTextureID(const tstring& textureName);

	const uint64 GetMeshID(const tstring& meshName);

	const uint64 GetFontID(const tstring& fontName);

	static std::shared_ptr<ResourceManager> Get();

	void LoadASE(const tstring& name, const tstring& path, bool isSmooth, bool isStatic);

	void LoadTexture(const tstring& name, const tstring& path);

	void LoadEnvironmentPanoramaMap(const tstring& name, const tstring& path);

	void LoadSoundFile(const std::string& name, const std::string& filePath, unsigned int mode = 0);

	void StartDataLoadToTread();

	bool GetisComplete();

	int GetAllResourceCount(); // ��� ���ҽ� ī��Ʈ

	int GetResourcesRemainingCount(); // ���� ���ҽ� ī��Ʈ

public:
	void Initialize();

	void Finalize();

	void Update();

	void LoadAllASE();

	void ResetTexture();
	// �ΰ��� ���ҽ� �ε� �ڵ� .. �˾��� ��� ��

public:
	// Stage 1�� �޽�, �ؽ�ó�� �ε��Ѵ�.
	void LoadStage1();

	// Stage 2�� �޽�, �ؽ�ó�� �ε��Ѵ�.
	void LoadStage2();

	// UI�� �޽�, �ؽ�ó�� �ε��Ѵ�. Ÿ��Ʋ, ����, Ŭ����� �̰͸� �ε��ϸ� �� ��.
	void LoadUIScene();

private:
	void LoadInGameUI();
	
	// Stage 1
	void LoadStage1MapTexture();
	void LoadStage1MapASE();

	// Stage 2
	void LoadStage2MapTexture();
	void LoadStage2MapASE();

	// Ʃ�丮�� ������ �޽� ,�ؽ��ĸ� �ε��Ѵ�..
	void LoadTu_BossTexture();
	void LoadTu_BossASE();

	// Tile�� �޽�, �ؽ�ó�� �ε��Ѵ�.
	void LoadTileTexture();
	void LoadTileASE();

	// Sound�� �ε��Ѵ�.
	void LoadSound();

	// Player TExture�� �ε��Ѵ�.
	void LoadPlayerTexture();

	// UITexture�� �ε��Ѵ�.
	void LoadUITexture();

	// Loading Scene ���ҽ��� �ε��Ѵ�.
	void LoadLoadingScene();

	// 2�� ������ �޽� ,�ؽ��ĸ� �ε��Ѵ�..
	void Boss2_BossTexture();

	void Boss2_BossASE();
};