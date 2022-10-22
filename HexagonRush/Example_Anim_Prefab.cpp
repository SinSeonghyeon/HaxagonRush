#include "pch.h"
#include "Example_Anim_Prefab.h"


void TestFuction(std::shared_ptr<Muscle::GameObject> gameObject)
{
	gameObject->GetTransform()->SetPosition(0, 0, 0);
	// �׽�Ʈ�� �Լ�.
}

Example_Anim_Prefab::Example_Anim_Prefab() :IPrefab(Muscle::CreateFromPrefab(TEXT("��ũ��")))
{
	GetGameObject()->GetComponent<Muscle::Transform>()->SetScale(0.02f, 0.02f, 0.02f);
	std::shared_ptr<Muscle::CapsuleCollider> col = GetGameObject()->AddComponent<Muscle::CapsuleCollider>();
	col->SetCapsule(60.0f, 60.0f);
	col->SetLocalPostion(Vector3(0, 1.0f, 0));
	GetGameObject()->AddComponent<Player>();
	std::shared_ptr<Muscle::Animater> anim = GetGameObject()->GetComponent<Muscle::Animater>();
	anim->Play("Armature|mixamo.com|Layer0.002");




	Muscle::AnimEventKey animEventKey;
	animEventKey._eventfunc = TestFuction; // �Լ� ������ �ȳѰܵ� �۵��� �ȴ�. 
	// �ȳѱ�� bool ���� üũ�ؼ� ����ϴ� ������Ʈ�� ���� ���� �ؾ���.
	// �Ű������� ���ӿ�����Ʈ�� �޾ƾ� �Ѵ�..

	animEventKey._eventKey = false; // �ʱ�ȭ ���ص� ������/�⺻���� false;

	animEventKey._name = "test"; // �̺�Ʈ Ű�� �̸��� �ִ� ��.

	animEventKey._targetFream = 5.0f * 160; //�ż��� �ͽ����ʹ� TICKSPERFRAME 160 �����̴�. ��ǥ ������ * 160 �� ���� ������ ��!

	animEventKey._targetGameObject = GetGameObject(); // �Լ� �������� �Ű������� ���� ����. ���� �ڱ� �ڽ��� ������?

	anim->AddAnimEvent("Armature|mixamo.com|Layer0.002", animEventKey); // �̺�Ʈ ��� �Ϸ�.



	bool _test = anim->GetEventKey("test"); // �̺�Ʈ key�� ���� ���¸� Ȯ�� �ϴ� �Լ�.
	//Components�� Update���� �������� ����ϸ� ������..?
	//�ʱ�ȭ�� ���� ��� ���ص� ������.


}
