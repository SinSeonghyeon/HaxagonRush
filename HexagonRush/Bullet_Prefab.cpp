#include "pch.h"
#include "Bullet_Prefab.h"

#include "BulletController.h"

Bullet_Prefab::Bullet_Prefab() : IPrefab(Muscle::CreateGameObject())
{
	// �������� ������Ʈ�� �ٿ��ָ� �Ǵ°ǰ� ..?
	std::shared_ptr<Muscle::GameObject> go1 = Muscle::CreateGameObject();

	// Radius ���� ���ư��鼭 ������ ���� ���� �༮
	go1->AddComponent<Muscle::ParticleRenderer>();
	
	go1->SetTag("Burn");

	std::shared_ptr<Muscle::GameObject> go2 = Muscle::CreateGameObject();
	
	// ��� ������ źȯ �ֺ��� ���׸��ϰ� ���ƴٴϴ� �� ������ �Ѹ� �༮
	go2->AddComponent<Muscle::ParticleRenderer>();

	go2->SetTag("AfterBurn");

	std::shared_ptr<Muscle::GameObject> go3 = Muscle::CreateGameObject();

	// Bullet Object�� �������� �����ϸ� �� ������Ʈ�� Play�Ǹ� ���İ��� �ֺ����� �ٷ��� ��ƼŬ�� �����ϴ�.
	go3->AddComponent<Muscle::ParticleRenderer>();

	go3->SetTag("Boom");

	// �� �༮���� �±׷� �з� �� ���� ������Ʈ �� ���� ������ �����̴�.
	GetGameObject()->AddComponent<BulletController>();
	// => Boom�� �Ǹ� Pool�� �ǵ����ô�.

	GetGameObject()->SetChild(go1);
	GetGameObject()->SetChild(go2);
	GetGameObject()->SetChild(go3);
}

Bullet_Prefab::~Bullet_Prefab()
{
}