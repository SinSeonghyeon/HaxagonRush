#include "pch.h"
#include "IntroScene.h"
#include "Stage1Scene.h"


IntroScene::IntroScene() : IScene("IntroScene")
{

}

IntroScene::~IntroScene()
{

}


void IntroScene::RapidUpdate()
{

}

void IntroScene::Start()
{
	ResourceManager::Get()->ResetTexture(); // ���ҽ� ������ �ٽ� �ε��Ѵ�.
	_loadingImageIDs.clear();

	static int i = 0;
	if (i++ == 0)
		ResourceManager::Get()->LoadAllASE(); // ase ������ �ѹ��� �ε�.

	_nextSceneName = (char*)_inputData;
	delete _inputData;

	if (_nextSceneName == "Stage1Scene") // �������� 1�� �������� 2�� ������ ������ ���� UI Resource�� �ε��ص� ������.
		ResourceManager::Get()->LoadStage1();
	else if (_nextSceneName == "Stage2Scene")
		ResourceManager::Get()->LoadStage2();
	else
		ResourceManager::Get()->LoadUIScene();
	ResourceManager::Get()->StartDataLoadToTread();

	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_01")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_02")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_03")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_04")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_05")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_06")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_07")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_08")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_09")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_10")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_11")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_12")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_13")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_14")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_15")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_16")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_17")));
	_loadingImageIDs.push_back(ResourceManager::Get()->GetTextureID(TEXT("Hex_loading_18")));


	_uiRenderer.reset();

	_uiRenderer = Muscle::CreateGameObject()->AddComponent<Muscle::UIRenderer>();

	_uiRenderer->SetDrawNDCPosition(Vector2(-1.f, 1.f), Vector2(1.f, -1.f));

	_uiRenderer->SetTextureID(_loadingImageIDs[0]);


}

void IntroScene::Update()
{
	float processRate = static_cast<float>(ResourceManager::Get()->GetResourcesRemainingCount()) / ResourceManager::Get()->GetAllResourceCount();

	uint32 imageIndex = (1.f - processRate) * (_loadingImageIDs.size() - 1);

	_uiRenderer->SetTextureID(_loadingImageIDs[imageIndex]);

	if (ResourceManager::Get()->GetisComplete())
		Muscle::IGameEngine::Get()->GetSceneManager()->LoadScene(_nextSceneName);
}