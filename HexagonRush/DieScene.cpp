#include "pch.h"
#include "DieScene.h"

#include "SceneFader.h"
#include "ButtonBase.h"

// 'Z'
#define BUTTON_CLICK_KEY 0x005A

#define DIE_BUTTON_COUNT 2

std::string DieScene::_retrySceneName = "TitleScene";

DieScene::DieScene() : IScene("DieScene")
{

}

DieScene::~DieScene()
{

}

void DieScene::SetRetrySceneName(std::string sceneName)
{
	_retrySceneName = sceneName;
}

void DieScene::Start()
{
	GetSoundManager()->Play("Ending", IPlayMode::BGM);

	_isAnyBottonClicked = false;

	_sceneFader.reset();

	_retryButton.reset();

	_retryButtonRenderer.reset();

	_titleButton.reset();

	_titleButtonRenderer.reset();

	// Scene Fader
	std::shared_ptr<Muscle::GameObject> sceneFader = Muscle::CreateGameObject();

	sceneFader->AddComponent<Muscle::UIRenderer>();

	_sceneFader = sceneFader->AddComponent<SceneFader>();

	// 0. ��� �� Back Ground Image
	std::shared_ptr<Muscle::GameObject> backGround = Muscle::CreateGameObject();

	backGround->AddComponent<Muscle::UIRenderer>()->SetSortOrder(100);
	backGround->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-1.f, 1.f), Vector2(1.f, -1.f));
	backGround->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("DieScene_BackGround")));

	// Cursor
	std::shared_ptr<Muscle::GameObject> cursor = Muscle::CreateGameObject();

	_cursorRenderer = cursor->AddComponent<Muscle::UIRenderer>();
	_cursorRenderer->SetSortOrder(6);
	_cursorRenderer->SetDrawNDCPosition(Vector2(-0.4f, -0.3f), Vector2(0.4f, -0.5f));
	_cursorRenderer->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Cursor")));

	// 1. Retry (�ش� ���������� ����Ѵ�.)
	std::shared_ptr<Muscle::GameObject> retry = Muscle::CreateGameObject();

	retry->AddComponent<Muscle::UIRenderer>()->SetSortOrder(5);
	retry->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-0.2f, -0.3f), Vector2(0.2f, -0.5f));
	retry->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("DieScene_Retry")));

	retry->AddComponent<ButtonBase>();

	// Retry => 5�� ���̵� �ƿ� �� ����� �� ���� !
	auto retryFunc = [&](void)
	{
		_sceneFader->FadeOut(1.f, [](void)
			{
				Muscle::MuscleEngine::GetInstance()->GetSceneManager()->LoadScene(_retrySceneName);
			});
	};

	retry->GetComponent<ButtonBase>()->RegistCallBack(retryFunc);

	_retryButton = retry->GetComponent<ButtonBase>();
	_retryButtonRenderer = retry->GetComponent<Muscle::UIRenderer>();

	// 2. Title (Ÿ��Ʋ ȭ������ ���)
	std::shared_ptr<Muscle::GameObject> title = Muscle::CreateGameObject();

	title->AddComponent<Muscle::UIRenderer>()->SetSortOrder(5);
	title->GetComponent<Muscle::UIRenderer>()->SetDrawNDCPosition(Vector2(-0.2f, -0.6f), Vector2(0.2f, -0.8f));
	title->GetComponent<Muscle::UIRenderer>()->SetTextureID(
		ResourceManager::Get()->GetTextureID(TEXT("DieScene_Title")));

	title->AddComponent<ButtonBase>();

	// Title => 5�� ���̵� �ƿ� �� Ÿ��Ʋ ������ ��� !
	auto titleFunc = [&](void)
	{
		_sceneFader->FadeOut(1.f, [](void)
			{
				Muscle::MuscleEngine::GetInstance()->GetSceneManager()->LoadScene("TitleScene");
			});
	};

	title->GetComponent<ButtonBase>()->RegistCallBack(titleFunc);

	_titleButton = title->GetComponent<ButtonBase>();
	_titleButtonRenderer = title->GetComponent<Muscle::UIRenderer>();
}

void DieScene::RapidUpdate()
{

}

void DieScene::Update()
{
	if (_isAnyBottonClicked)
		return;

	static uint32 selectedButton = 0;

	if (Muscle::KeyBoard::Get()->KeyDown(VK_UP) || 
		Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::Up, Muscle::XPadCommand::Push))
		selectedButton = (selectedButton + 1) % DIE_BUTTON_COUNT;
	else if (Muscle::KeyBoard::Get()->KeyDown(VK_DOWN) || 
		Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::Down, Muscle::XPadCommand::Push))
		selectedButton = (selectedButton - 1) % DIE_BUTTON_COUNT;

	// 0�� ��ŸƮ ��ư�� ���õ� ����
	if (selectedButton == 0)
	{
		_cursorRenderer->SetDrawNDCPosition(Vector2(-0.4f, -0.3f), Vector2(0.4f, -0.5f));

		_retryButton->SetButtonState(ButtonBase::BUTTON_STATE::SELECTED);

		_titleButton->SetButtonState(ButtonBase::BUTTON_STATE::NONE);

	}
	else if (selectedButton == 1)
	{
		_cursorRenderer->SetDrawNDCPosition(Vector2(-0.4f, -0.6f), Vector2(0.4f, -0.8f));

		_retryButton->SetButtonState(ButtonBase::BUTTON_STATE::NONE);

		_titleButton->SetButtonState(ButtonBase::BUTTON_STATE::SELECTED);
	}

	// Ű���带 ���� ��ư ���� �Ϸ� !
	if (Muscle::KeyBoard::Get()->KeyPress(BUTTON_CLICK_KEY) ||
		Muscle::XPad::Get()->IsPadCommand(Muscle::XPadInput::ButtonX, Muscle::XPadCommand::Push))
	{
		// 0�̸� ���� ��ư ����, �ƴϸ� ������ ��ư ����
		selectedButton == 0 ? _retryButton->SetButtonState(ButtonBase::BUTTON_STATE::ON_CLICKED) :
			_titleButton->SetButtonState(ButtonBase::BUTTON_STATE::ON_CLICKED);

		_isAnyBottonClicked = true;
	}
}