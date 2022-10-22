#include "pch.h"
#include "Player_Prefab.h"
#include "Bullet_Prefab.h"
#include "BulletController.h"

#include "SceneFader.h"
#include "Inventory.h"
#include "AuraController.h"
#include "BuffController.h"

#include "Rush_Bullet_Prefab.h"

void Player_NormalAttack(std::shared_ptr<Muscle::GameObject> gameObject)
{
	std::shared_ptr<Controller> control = gameObject->GetComponent<Controller>();

	std::shared_ptr<Player> player = gameObject->GetComponent<Player>();

	if (player == nullptr)
		return;

	// ���� �Լ� ���� ! (���Ϳ��� ���� ���Ѵ�.) => �̰� Bullet �¾��� �� ǥ���ϱ� ���ؼ� �ű�
	// player->Attack(player->GetTarget());

	ObjectMover::Get()->Shake(Muscle::IGameEngine::Get()->GetMainCamera()->GetTransform(), 0.2f);

	std::shared_ptr<Bullet_Prefab> testBullet = std::make_shared<Bullet_Prefab>();

	testBullet->GetGameObject()->GetComponent<Muscle::Transform>()->SetPosition(gameObject->GetTransform()->GetWorldPosition() + Vector3(0, 3, 0));

	// ���� �������� �������� ���ٰ� .. �������� ��¦ ���� �ִ� �ھ ���� �� �ֵ��� �Ѿ��� ���� ��ġ�� �ϵ� �ڵ����� ���� !
	Vector3 dest = gameObject->GetTransform()->GetWorldPosition() + (player->GetTarget()->GetGameObject()->GetTransform()->GetWorldPosition() - gameObject->GetTransform()->GetWorldPosition())
		+ player->GetTarget()->GetGameObject()->GetTransform()->GetUp() * 8;

	// �����ϱ� ��, ����� �� ���� ��Ʈ������ ! Ȯ���ؾ���. => Ÿ�� �Ŵ������� �� �� �� �÷��̾�� �˷�����.
	const TileManager::ATTACK_PLATFORM_INFO& attackInfo = TileManager::Get()->GetCurrentAttackInfo();

	// �Ѿ� �߻��� ���� �̷��� ������ ���־�� �Ѵ�.
	// 
	// �÷��̿��� ���� ���� �� �Ѿ��� �˷�����.
	uint32 info = 0;

	if (attackInfo._color == StateColor::Red)
		info += 0;
	else if (attackInfo._color == StateColor::Green)
		info += 1;
	else if (attackInfo._color == StateColor::Blue)
		info += 2;

	// �ܰ迡 ����ؼ� ������ �־��ش�.
	info += (attackInfo._count - 3) * 3;

	// ���� ������ �����ݴϴ� !
	player->PostShootingBulletInfo(static_cast<Player::SHOOTING_BULLET_INFO>(info));

	// ��ü (���� ������)
	testBullet->GetGameObject()->GetComponent<BulletController>()->SettingAgent(player);

	// ����
	testBullet->GetGameObject()->GetComponent<BulletController>()->SetBulletColor(static_cast<BulletController::BULLET_COLOR>(attackInfo._color));

	// ���� (������ 3���� ���� 1���� ..!)
	testBullet->GetGameObject()->GetComponent<BulletController>()->SetBulletLevel(static_cast<BulletController::BULLET_LEVEL>(attackInfo._count - 2));

	// ������
	testBullet->GetGameObject()->GetComponent<BulletController>()->SetDestination(dest);
}

// �÷��̾� �� ! => ������ �޽� ������, ��Ų�� �޽� �������� ���� �ִ�.
Player_Prefab::Player_Prefab() :IPrefab(Muscle::CreateFromPrefab(TEXT("Player")))
{
	// Texture
	std::vector<std::shared_ptr<Muscle::GameObject>> allchilds;

	GetGameObject()->GetAllChildrens(allchilds);

	for (auto& object : allchilds)
	{
		std::shared_ptr<Muscle::SkinnedMeshRenderer> renderer = object->GetComponent<Muscle::SkinnedMeshRenderer>();

		if (renderer != nullptr)
		{
			std::shared_ptr<RenderingData_3D> renderingData = renderer->_renderingData;

			renderingData->_materialInfo->_diffuseMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_Albedo"));
			renderingData->_materialInfo->_normalMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_Normal"));
			renderingData->_materialInfo->_metallicMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_Metallic"));
			renderingData->_materialInfo->_roughnessMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_Roughness"));
			renderingData->_materialInfo->_ambientOcclusionMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_AO"));
			renderingData->_materialInfo->_emissiveMapID = ResourceManager::Get()->GetTextureID(TEXT("Player_Emissive"));

			//renderingData->_shaderInfo->_vsName = TEXT("VS_ANMRAOE_Skinning_Rim");
			//renderingData->_shaderInfo->_psName = TEXT("PS_ANMRAOE_Skinning_Rim");

			renderingData->_shaderInfo->_vsName = TEXT("VS_ANMRAOE_Skinning_Rim");
			renderingData->_shaderInfo->_psName = TEXT("PS_ANMRAOE_Skinning_Rim");
		}
	}

	// �÷��̾ ĳ���ϱ� ���ؼ� �̸��� �����ش�. (�ܺο��� ������Ʈ ã���)
	GetGameObject()->SetName("Player");

	// Work Man���� ���� �� ����� ����� ũ��, ��ǥ�谡 �ٸ���.
	// GetGameObject()->GetTransform()->SetScale(0.05f, 0.05f, 0.05f);

	GetGameObject()->GetTransform()->SetPosition(Vector3(0.f, 4.2f, 0.f));

	// �ݶ��̴� �߰� (�������� ��ȣ �ۿ� ����)
	std::shared_ptr<Muscle::CapsuleCollider> col = GetGameObject()->AddComponent<Muscle::CapsuleCollider>();

	col->SetCapsule(1.5f, 4.0f);

	col->SetLocalPostion(Vector3(0.f, 3.0f, 0.f));

	// �ݶ��̴� �߰� 22 (������ ��ȣ �ۿ� ����)

	std::shared_ptr<Muscle::GameObject> wallColObj = Muscle::CreateGameObject();

	wallColObj->SetName("Player_Wall");

	wallColObj->AddComponent<Muscle::CapsuleCollider>()->SetCapsule(1.5f, 4.0f);

	col->SetLocalPostion(Vector3(0.f, 3.0f, 0.f));

	GetGameObject()->SetChild(wallColObj);

	// �÷��̾� ������Ʈ
	std::shared_ptr<Player> player = GetGameObject()->AddComponent<Player>();

	// ��Ʈ�ѷ� ������Ʈ (������, ȸ�� ��)
	GetGameObject()->AddComponent<Controller>();

	// Aura Particle Renderer�� Controller
	GetGameObject()->AddComponent<Muscle::ParticleRenderer>();
	GetGameObject()->AddComponent<AuraController>();

	// �κ��丮 ������Ʈ (�������� ���۰� �־����� ������ 3��)
	GetGameObject()->AddComponent<Inventory>();

	// Buff Controller (������ ��� � ���� Particle�� ��Ʈ�����ݴϴ�.)
	std::shared_ptr<Muscle::GameObject> buffObj = Muscle::CreateGameObject();

	buffObj->SetName("BuffController");

	buffObj->AddComponent<Muscle::ParticleRenderer>();

	buffObj->AddComponent<BuffController>();

	// �̰� �ڽ����� ����.
	GetGameObject()->SetChild(buffObj);

	// ���� ����
	static std::random_device rd;

	static std::mt19937 gen(rd());

	static std::uniform_int_distribution<int> dist(0, 3);

	// ���� �ҷ� ������ Ǯ�� ä���ش�.
	for (int i = 0; i < 150; i++)
	{
		std::shared_ptr<Muscle::GameObject> rushBullet = std::make_shared<Rush_Bullet_Prefab>()->GetGameObject();

		rushBullet->SetIsEnable(false);

		std::shared_ptr<Rush_BulletController> rushBulletController = rushBullet->GetComponent<Rush_BulletController>();

		// �ֺ� ���� ���� ���� ����
		int color = dist(gen);

		if (color == 0)
			rushBulletController->SetColor(Vector4(1.f, 0.f, 0.f, 1.f));
		else if (color == 1)
			rushBulletController->SetColor(Vector4(0.f, 1.f, 0.f, 1.f));
		else
			rushBulletController->SetColor(Vector4(0.f, 0.1f, 0.9f, 1.f));

		player->_rushBulletPrefabPool.push_back(rushBullet);
	}

	{
		// �밡�ٰ� �� ����
		// �ִϸ��̼� Ű �̺�Ʈ ��� !
		Muscle::AnimEventKey animEventKey;
		animEventKey._eventfunc = Player_NormalAttack;
		animEventKey._eventKey = false;
		animEventKey._name = "Attack";
		animEventKey._targetFream = 32.0f * 160;
		animEventKey._targetGameObject = GetGameObject(); // �Լ� �������� �Ű������� ���� ����. ���� �ڱ� �ڽ��� ������?
		GetGameObject()->GetComponent<Muscle::Animater>()->AddAnimEvent("Ch_Attack", animEventKey); // �̺�Ʈ ��� �Ϸ�.
	}
	{ // WalkSound

		Muscle::AnimEventKey animEventKey;
		animEventKey._eventfunc = [](std::shared_ptr<Muscle::GameObject> gameObject) { GetSoundManager()->Play("Move_R", IPlayMode::Effect); };
		animEventKey._name = "WalkRight";
		animEventKey._targetFream = 15.0f * 160;
		animEventKey._targetGameObject = GetGameObject(); // �Լ� �������� �Ű������� ���� ����. ���� �ڱ� �ڽ��� ������?
		GetGameObject()->GetComponent<Muscle::Animater>()->AddAnimEvent("Ch_Run", animEventKey); // �̺�Ʈ ��� �Ϸ�.

		animEventKey._eventfunc = [](std::shared_ptr<Muscle::GameObject> gameObject) { GetSoundManager()->Play("Move_L", IPlayMode::Effect); };
		animEventKey._name = "WalkLeft";
		animEventKey._targetFream = 45.0f * 160;
		GetGameObject()->GetComponent<Muscle::Animater>()->AddAnimEvent("Ch_Run", animEventKey); // �̺�Ʈ ��� �Ϸ�.
	}
}

Player_Prefab::~Player_Prefab()
{
	_gameObject.reset();
}