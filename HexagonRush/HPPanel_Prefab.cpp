#include "pch.h"
#include "HPPanel_Prefab.h"

#include "HPPanelController.h"

HPPanel_Prefab::HPPanel_Prefab() : IPrefab(Muscle::CreateGameObject())
{
	// HP Panel Controller => �÷��̾��� ���� ������ ��� ������Ʈ�ϸ�
	// HP�� ���� Left, Middle, Right�� Texture ID�� ��Ʈ���Ѵ�. (�ִ°�, ���°�)
	GetGameObject()->AddComponent<HPPanelController>();

	// HP�� ǥ���� �̹��� 3��
	std::shared_ptr<Muscle::GameObject> left = Muscle::CreateGameObject();

	left->AddComponent<Muscle::UIRenderer>();

	left->SetTag("Left");

	std::shared_ptr<Muscle::GameObject> middle = Muscle::CreateGameObject();

	middle->AddComponent<Muscle::UIRenderer>();

	middle->SetTag("Mid");

	std::shared_ptr<Muscle::GameObject> right = Muscle::CreateGameObject();

	right->AddComponent<Muscle::UIRenderer>();

	right->SetTag("Right");

	GetGameObject()->SetChild(left);
	GetGameObject()->SetChild(middle);
	GetGameObject()->SetChild(right);
}

HPPanel_Prefab::~HPPanel_Prefab()
{

}