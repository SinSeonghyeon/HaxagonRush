#include "pch.h"
#include "AuraController.h"

AuraController::AuraController(std::shared_ptr<Muscle::GameObject> gameObj) :
	IComponents(gameObj), _isFirstRun(true)
{

}

AuraController::~AuraController()
{
	if (_particleRenderer != nullptr)
		_particleRenderer.reset();

	if (_tileManager != nullptr)
		_tileManager.reset();
}

void AuraController::Start()
{
	_particleRenderer = GetGameObject()->GetComponent<Muscle::ParticleRenderer>();

	_renderingData = _particleRenderer->GetParticleData();

	// Init Info
	_renderingData->_initInfo->_particleEffectType = PARTICLE_EFFECT_TYPE::CIRCLE;
	_renderingData->_initInfo->_firstRun = true;
	_renderingData->_initInfo->_isLoop = true;

	// Shape Info
	_renderingData->_shapeInfo->_refTextureID = ResourceManager::Get()->GetTextureID(TEXT("Particle"));

	_isFirstRun = true;

	// Tile Manager Caching
	_tileManager = TileManager::Get();

	// OffSet (Import�� �𵨰��� ���̸� �޲ߴϴ�.)
	// _particleRenderer->SetOffsetMatrix(Matrix::CreateRotationZ(3.14f / 2));
	_particleRenderer->SetOffsetMatrix(Matrix::CreateRotationX(3.14f / 2));
}

void AuraController::Update()
{
	// ���� ���õ� Ÿ���� ������ �޾ƿɴϴ�.
	const TileManager::SELECTED_PLATFORM_INFO& info = _tileManager->GetCurrentSelectedInfo();

	// �ϳ��� �������� ������ ������ ������� �ʽ��ϴ�.
	if (info._count == 0)
	{
		_particleRenderer->Stop();

		_isFirstRun = true;

		return;
	}

	// ù �������� �˷��ݽô�.
	if (_isFirstRun)
	{
		_particleRenderer->Play();

		_isFirstRun = false;
	}

	uint32 var = info._count;

	float invVar = 1.f / var;

	Vector4 color = Vector4::Zero;

	for (auto& colors : info._platColors)
	{
		if (colors == StateColor::Red)
			color += Vector4(1.f, 0.f, 0.f, 1.f);
		else if (colors == StateColor::Green)
			color += Vector4(0.f, 1.f, 0.f, 1.f);
		else if (colors == StateColor::Blue)
			color += Vector4(0.f, 0.1f, 0.9f, 1.f);
	}

	// ������ ���� ��� !
	color /= info._count;

	_renderingData->_initInfo->_maxParticleCount = 400 * var;

	_renderingData->_shapeInfo->_color = color;
	_renderingData->_shapeInfo->_emitVelocity = Vector3::Up * var * 0.8f;
	_renderingData->_shapeInfo->_emitTime = 0.005f; /* * invVar;*/
	_renderingData->_shapeInfo->_radius = 0.7f * var;

	_renderingData->_shapeInfo->_deviation = Vector2(0.1f, 0.1f);			// �����󿡴� ������ �� ���� !
	_renderingData->_shapeInfo->_useXYSameDeviation = true;
	_renderingData->_shapeInfo->_particleAverageSize = Vector2(0.1f, 0.1f) * var;
	_renderingData->_shapeInfo->_period = 0.2f;

	_renderingData->_shapeInfo->_acceleration = Vector3(0.f, 1.5f, 0.f) * var;
	_renderingData->_shapeInfo->_lifeSpan = 1.5f; /** var;*/
} 