#include "pch.h"
#include "TestTile_Prefab.h"

TestTile_Prefab::TestTile_Prefab() : IPrefab(Muscle::CreateFromPrefab(TEXT("Map_Tile")))
{
	// �̸� Ž���� �����ϱ� ������ Ÿ�ϸʸ� ���� �Ľ����ָ� ���ڴ�...!
	// 
	std::function<void(const std::vector<std::shared_ptr<Muscle::GameObject>>&)>
		func = [&](const std::vector<std::shared_ptr<Muscle::GameObject>>& childrens)
	{
		for (auto& iter : childrens)
		{
			Vector3 curPos = iter->GetComponent<Muscle::Transform>()->GetWorldPosition();

			std::shared_ptr<Muscle::SphereCollider> col = iter->AddComponent<Muscle::SphereCollider>();
			col->SetLocalPostion(Vector3(0, 0.5f, 0));
			col->SetRadius(1.0f);

			auto platform = iter->AddComponent<Platform>();

			// �ϴ� ��ƼŬ �������� �� �� �ھƺ���.
			iter->AddComponent<Muscle::ParticleRenderer>();

			platform->SetPosition(iter->GetTransform()->GetPosition());
			func(iter->GetChildrens());
			TileManager::Get()->PushBackTile(platform);
		}
	};

	func(GetGameObject()->GetChildrens());

	GetGameObject()->SetTag("Tile");
}