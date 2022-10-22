#pragma once
/// <summary>
/// ����� �ؽ�Ʈ ���õ� ������ �ϵ��� ó���ϴ� ģ��! ������ ¥���� ����!
/// ������Ʈ�� ��ӹ��� ������ ���� ���� ���۵ǰ� ���� ������Ʈ�� �����ϱ� ���ؼ�..!!
/// </summary>


class DebugObjectManager : public Muscle::IComponents, public std::enable_shared_from_this<DebugObjectManager>
{
public:
	DebugObjectManager(std::shared_ptr<Muscle::GameObject> gameObj);

	virtual ~DebugObjectManager();

private:
	static std::shared_ptr<DebugObjectManager> _instance;

	std::shared_ptr<Muscle::TextRenderer> _timeText;

	std::shared_ptr<Muscle::TextRenderer> _cameraInfo;

	std::shared_ptr<Muscle::TextRenderer> _objectInfo;

	std::shared_ptr<Muscle::TextRenderer> _objectCountInfo;

	std::shared_ptr<Muscle::RendererBase> _renderbase;

	std::shared_ptr<Muscle::TextRenderer> _bossData;

	std::shared_ptr<Character> _boss; // ������ ������ ����ϱ� ���� ĳ��..

	int _index;

	std::shared_ptr<Muscle::GameObject> _gameobject; // ��ŷ ������Ʈ!

private:
	void CameraInfoUpdate();

	void ObjectInfoUpdate();

	void ObjectCountUpdate();

	void BossInfoUpdate();

public:
	static std::shared_ptr<DebugObjectManager> Get();

	void PickGameObject(std::shared_ptr<Muscle::GameObject> gameobj);

	virtual void Start() override;

	virtual void Update() override;

	virtual void LateUpdate() override; // ��� Object Udpate���� �� (ObjectManager) ������Ʈ�� Components�� ������ ������ ���� ������Ʈ ���ֱ�.

	virtual void Finalize() override;

};

