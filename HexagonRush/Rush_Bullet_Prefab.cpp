#include "pch.h"
#include "Rush_Bullet_Prefab.h"

#include "Rush_BulletController.h"

Rush_Bullet_Prefab::Rush_Bullet_Prefab() : IPrefab(Muscle::CreateGameObject())
{
	GetGameObject()->AddComponent<Rush_BulletController>();

	std::shared_ptr<Muscle::GameObject> go1 = Muscle::CreateGameObject();

	// Radius ���� ���ư��鼭 ������ ���� ���� �༮
	go1->AddComponent<Muscle::ParticleRenderer>()->SetParticleTextureID(ResourceManager::Get()->GetTextureID(TEXT("Particle")));
	
	go1->SetTag("Burn");

	std::shared_ptr<Muscle::GameObject> go2 = Muscle::CreateGameObject();

	// ��� ������ źȯ �ֺ��� ���׸��ϰ� ���ƴٴϴ� �� ������ �Ѹ� �༮
	go2->AddComponent<Muscle::ParticleRenderer>()->SetParticleTextureID(ResourceManager::Get()->GetTextureID(TEXT("Particle")));

	go2->SetTag("AfterBurn");

	std::shared_ptr<Muscle::GameObject> go3 = Muscle::CreateGameObject();

	// Bullet Object�� �������� �����ϸ� �� ������Ʈ�� Play�Ǹ� ���İ��� �ֺ����� �ٷ��� ��ƼŬ�� �����ϴ�.
	go3->AddComponent<Muscle::ParticleRenderer>()->SetParticleTextureID(ResourceManager::Get()->GetTextureID(TEXT("Particle")));

	go3->SetTag("Boom");

	GetGameObject()->SetChild(go1);
	GetGameObject()->SetChild(go2);
	GetGameObject()->SetChild(go3);
}

Rush_Bullet_Prefab::~Rush_Bullet_Prefab()
{

}