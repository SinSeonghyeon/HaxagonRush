#include "pch.h"
#include "EndingScene.h"
#include "SceneFader.h"
EndingScene::EndingScene() : IScene("EndingScene")
{

}

EndingScene::~EndingScene()
{

}

void EndingScene::Start()
{
	GetSoundManager()->Play("Ending", IPlayMode::BGM);
	Muscle::CTime::SetGameSpeed(1.0f,0);

	_textRenderer.reset();

	_BackgroundRenderer.reset();

	_textPosition = Vector2(0.0f, -2.0f);
	std::shared_ptr<Muscle::GameObject> UiObject = Muscle::CreateGameObject();

	_textRenderer = UiObject->AddComponent<Muscle::UIRenderer>();
	_BackgroundRenderer = UiObject->AddComponent<Muscle::UIRenderer>();

	_textRenderer->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Hex_Ending_text")));
	_textRenderer->SetDrawNDCPosition(Vector2(-1.0f, 1.0f), Vector2(1.0f, -1.4f));

	_BackgroundRenderer->SetTextureID(ResourceManager::Get()->GetTextureID(TEXT("Hex_Ending_background")));
	_BackgroundRenderer->SetDrawNDCPosition(Vector2(-1.f, 1.f), Vector2(1.f, -1.f));
	_BackgroundRenderer->SetSortOrder(100);
}

void EndingScene::RapidUpdate()
{

}

void EndingScene::Update()
{
	if (_textPosition.y < 2.4f)
	{
		_textRenderer->SetDrawNDCPosition(Vector2(-1.0f, 1.0f) + _textPosition, Vector2(1.0f, -1.4f) + _textPosition);
		_textPosition.y += 0.1f * Muscle::CTime::GetGameDeltaTime();
	}
	else
	{
		Muscle::IGameEngine::Get()->GetSceneManager()->LoadScene("TitleScene");
	}
}
