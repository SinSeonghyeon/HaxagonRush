#include "pch.h"
#include "StageClearScene.h"

#include "SceneFader.h"
#include "ButtonBase.h"

// 'Z'
#define BUTTON_CLICK_KEY 0x005A

std::string StageClearScene::_nextSceneName = "Stage2Scene";

float StageClearScene::_playTimePrevScene = 0.f;

float StageClearScene::GetPrevScenePlayTime()
{
	return _playTimePrevScene;
}

StageClearScene::StageClearScene() : IScene("StageClearScene")
{
	_isAnyBottonClicked = false;
}

StageClearScene::~StageClearScene()
{

}

void StageClearScene::SetNextSceneName(std::string sceneName)
{
	_nextSceneName = sceneName;
}

void StageClearScene::SetPrevScenePlayTime(float time)
{
	_playTimePrevScene = time;
}

void StageClearScene::Start()
{
	_isAnyBottonClicked = false;

	_sceneFader.reset();

	_nextButton.reset();

	_nextButtonRenderer.reset();

	// Scene Fader
	std::shared_ptr<Muscle::GameObject> sceneFader = Muscle::CreateGameObject();

	sceneFader->AddComponent<Muscle::UIRenderer>();

	_sceneFader = sceneFader->AddComponent<SceneFader>();

	// Back Ground
	std::shared_ptr<Muscle::GameObject> back = Muscle::CreateGameObject();

	back->AddComponent<Muscle::UIRenderer>()->SetSortOrder(100);

	back->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-1.f, 1.f), Vector2(1.f, -1.f));

	back->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("StageClear_BackGround")));

	// Cursor
	std::shared_ptr<Muscle::GameObject> cursor = Muscle::CreateGameObject();

	cursor->AddComponent<Muscle::UIRenderer>()->SetSortOrder(6);

	cursor->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-0.4f, -0.7f), Vector2(0.4f, -0.9f));

	cursor->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("Cursor")));

	// Next Stage Button
	std::shared_ptr<Muscle::GameObject> nextStage = Muscle::CreateGameObject();

	nextStage->AddComponent<Muscle::UIRenderer>();

	nextStage->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-0.2f, -0.7f), Vector2(0.2f, -0.9f));

	nextStage->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("StageClear_NextStage")));

	nextStage->AddComponent<ButtonBase>();

	// next => 5�� ���̵� �ƿ� �� Ÿ��Ʋ ������ ��� !
	auto nextFunc = [&](void)
	{
		_sceneFader->FadeOut(1.f, [](void)
			{
				Muscle::MuscleEngine::GetInstance()->GetSceneManager()->LoadScene(_nextSceneName);
			});
	};

	nextStage->GetComponent<ButtonBase>()->RegistCallBack(nextFunc);

	_nextButtonRenderer = nextStage->GetComponent<Muscle::UIRenderer>();

	_nextButton = nextStage->GetComponent<ButtonBase>();
	
	// Stage Clear Time
}

void StageClearScene::RapidUpdate()
{
}

void StageClearScene::Update()
{
	if (_isAnyBottonClicked)
		return;

	_nextButton->SetButtonState(ButtonBase::BUTTON_STATE::SELECTED);

	// Ű���带 ���� ��ư ���� �Ϸ� !
	if (Muscle::KeyBoard::Get()->KeyPress(BUTTON_CLICK_KEY) || Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonX, Muscle::XPadCommand::Push))
	{
		// ��ư �ϳ���.
		_nextButton->SetButtonState(ButtonBase::BUTTON_STATE::ON_CLICKED);

		_isAnyBottonClicked = true;
	}
}