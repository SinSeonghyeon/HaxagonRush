#pragma once

// �������� Ŭ�����ϸ� ������ ��. ��ư �� ���� �ϴ� ������ ..
// 8�� 19�� ���� 12�� 5�� .. ��� ���� �� �ν� ~
class StageClearScene : public Muscle::IScene
{
public:
	StageClearScene();

	virtual ~StageClearScene();

	static float GetPrevScenePlayTime();

private:
	static std::string _nextSceneName;

	static float _playTimePrevScene;


	bool _isAnyBottonClicked;

	std::shared_ptr<SceneFader> _sceneFader;

	std::shared_ptr<ButtonBase> _nextButton;

	std::shared_ptr<Muscle::UIRenderer> _nextButtonRenderer;

public:
	static void SetNextSceneName(std::string sceneName);

	static void SetPrevScenePlayTime(float time);

	virtual void Start() override;

	virtual void RapidUpdate() override;
	
	virtual void Update() override;
};