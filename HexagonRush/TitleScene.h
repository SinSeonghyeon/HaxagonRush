#pragma once

class ButtonBase;

class SceneFader;

class Muscle::UIRenderer;

// ������ ������
class TitleScene : public Muscle::IScene
{
public:
	TitleScene();

	~TitleScene();

private:
	bool _isAnyBottonClicked;

	// Ÿ��Ʋ ���� �� �� �� �ۿ� ����.
	std::shared_ptr<SceneFader> _sceneFader;

	std::shared_ptr<ButtonBase> _startButton;

	std::shared_ptr<Muscle::UIRenderer> _startButtonRenderer;

	std::shared_ptr<ButtonBase> _exitButton;

	std::shared_ptr<Muscle::UIRenderer> _exitButtonRenderer;

	std::shared_ptr<Muscle::UIRenderer> _cursor;

private:
	virtual void Start() override;

	virtual void RapidUpdate() override;

	virtual void Update() override;
};