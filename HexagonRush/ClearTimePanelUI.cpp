#include "pch.h"
#include "ClearTimePanelUI.h"

ClearTimePanelUI::ClearTimePanelUI(std::shared_ptr<Muscle::GameObject> gameObj) :
	IComponents(gameObj)
{

}

ClearTimePanelUI::~ClearTimePanelUI()
{

}

void ClearTimePanelUI::Start()
{
	std::vector<std::shared_ptr<Muscle::GameObject>> childs = GetGameObject()->GetChildrens();

	for (int i = 0 ; i < childs.size() ; i++)
	{
		_renderer[i] = childs[i]->GetComponent<Muscle::UIRenderer>();
	}

	// ���� ������ �ɸ� ���� �÷��� �ð�
	float playTime = StageClearScene::GetPrevScenePlayTime();

	// �÷��� �ð�, ����. �ʴ���
	int playTimeint = static_cast<int>(playTime);

	int minute = playTimeint / 60;

	int sec = playTimeint % 60;

	// �÷��� �ð�, ��Ʈ��
	std::string playMinute = std::to_string(minute);

	std::string playSec = std::to_string(sec);

	// ���̰� 2, 2�� �� �� ���� ������ݽô�.
	while (playMinute.length() < 2)
		playMinute = '0' + playMinute;

	while (playSec.length() < 2)
		playSec = '0' + playSec;

	for (int i = 0; i < 2 ; i++)
	{
		// ���ڿ� ���� �б⾲
		_renderer[i]->GetUIData()->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(std::to_wstring(playMinute[i]));
	}

	// �����ݷ�
	_renderer[2]->GetUIData()->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(TEXT("Colne"));

	for (int i = 3 ; i < 5 ; i++)
		_renderer[i]->GetUIData()->_drawInfo->_textureID = ResourceManager::Get()->GetTextureID(std::to_wstring(playSec[i]));
}

void ClearTimePanelUI::Update()
{

}