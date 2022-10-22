#pragma once
#include "Graphics_RenderingData_Particle.h"
#include "CB_Structure.h"

namespace GraphicsEngine
{
	constexpr uint32 PARTICLE_EMITTER = 0;

	constexpr uint32 PARTICLE_FLARE = 1;

	class Pass_Deferred;

	class RenderTarget;

	class DepthStencilState;

	class BlendState;

	class VertexShader;

	class PixelShader;
	
	class GeometryShader;
	
	class Texture;

	constexpr uint32 MAX_PARTICLE_COUNT = 1000;

	// ȭ�鿡 ��µǰ� �ִ� �ϳ��� ��ƼŬ �ý���
	struct ParticleSystem						// TODO : System Pool ������
	{
		static uint64 _nextID;

		ParticleSystem() : _effectType(PARTICLE_EFFECT_TYPE::CIRCLE), _systemPlayID(0), _totalTime(0.f), 
			_duration(0.f), _initVB(nullptr), _drawVB(nullptr), _streamOutVB(nullptr), _isFinalPhaseOfSystem(false)
		{}

		void Initialize(std::shared_ptr<RenderingData_Particle> targetData);

		void InitVBs(std::shared_ptr<RenderingData_Particle> targetData);

		void Release();

		PARTICLE_EFFECT_TYPE _effectType;

		// ���� �������� ������ ���� ID.
		uint64 _systemPlayID;

		// ��ƼŬ �ý����� ���ư��� �ɸ� �ð�. �̰ɷ� ���� �ð��� �缭 �ڵ� ���ᵵ ������.
		float _totalTime;

		// ���� �ð�
		float _duration;

		// �����ΰ��� ?
		bool _isLoop;

		// Loop Particle System�ε� .. �̹� �����ӿ� ������Ʈ ��ȣ�� �Դ°� ?
		bool _isThisFrameUpdated;

		// XY ǥ�� ������ �Ȱ��� ����ϴ°� ? => �̹��� ���� ����
		bool _useXYSameDeviation;

		bool _isFinalPhaseOfSystem;

		// ������ ���� Particle Size Variation�� �ֱ� ����
		std::normal_distribution<float> _norDistX;

		std::normal_distribution<float> _norDistY;
		
		ComPtr<ID3D11Buffer> _initVB;

		ComPtr<ID3D11Buffer> _drawVB;

		ComPtr<ID3D11Buffer> _streamOutVB;

		CB_PARTICLE_TIMER _cbTimer;

		CB_PARTICLE_STREAMOUT _cbStreamOut;

		CB_PARTICLE_DRAW _cbDraw;

		CB_PARTICLE_COLOR _cbColor;

		uint64 _refTextureID;
	};

	// ������ ����� ����Ѵ�.
	// ������ �н� �� ������ �н� ������� ���� Ÿ������
	// ��ƼŬ���� ������ �����̶�� ����� ������. ����, ���� �� ������ ��ü�� ��ƼŬ�� ����� �� ����.
	// ���� ������ ���� �Ӹ� �ӿ� �������� �� ���͸� ���� �ڵ带 ������ ..
	class ParticleProcessor
	{
	public:
		void Initialize();

		void Release();

		// Pass_Lighting�� Target Screen�� �޾Ƽ� �װ��� ���� Ÿ������ ���� �� ��ƼŬ �׸��� �� �� ..? �� ��
		void Render(std::shared_ptr<RenderTarget> targetScreen, 
			std::vector<std::shared_ptr<RenderingData_Particle>> renderQueueParticle);

	private:
		void CreateRandomTextureSRV();

	public:
		void SetCurrentFrameDeltaTime(float deltaTime) { _currFramedeltaTime = deltaTime; }

	private:
		float _currFramedeltaTime;

		std::shared_ptr<Pass_Deferred> _deferredPassRef;

		ComPtr<ID3D11DeviceContext> _deviceContext;

		// ��ƼŬ �ý��� ������ Ư�� �� Depth ���¸� ���� Ű�� ��찡 ����. (Stream Out)
		std::shared_ptr<DepthStencilState> _depthStencilState;

		// ��ƼŬ �ý��� ������ Ư�� �� Blend State�� ������ ����.
		std::shared_ptr<BlendState> _blendState;

		// ���������� ���� ���� ���۸� ������.
		std::shared_ptr<GeometryShader> _gsStreamOutParticleCircle;				// ���� (2D) ��ƼŬ

		std::shared_ptr<GeometryShader> _gsStreamOutParticleSphere;				// ���� (3D) ��ƼŬ

		std::shared_ptr<GeometryShader> _gsStreamOutOnlyDelete;					// ���� X, ����⸸ �Ѵ�.

		// ���� ���� ���۸� �ؽ�ó�� ���� �� �ִ� ����� ���� ���� ����
		std::shared_ptr<GeometryShader> _gsDraw;
		
		std::shared_ptr<VertexShader> _vsStreamOut;

		std::shared_ptr<VertexShader> _vsDraw;

		std::shared_ptr<PixelShader> _psDraw;

		std::map<uint64, std::shared_ptr<ParticleSystem>> _systemsIDMap;

	private:
		ComPtr<ID3D11ShaderResourceView> _randVecTexSRV;
		// ���� �߻��� ����ϰ� ���� �� ���̴����� ����Ѵ�.

	private:
		std::shared_ptr<ParticleSystem> GetParticleSystem(const uint64& particleDataID);

		void UpdateConstantBufferOfSystem(std::shared_ptr<RenderingData_Particle>& renderingData, std::shared_ptr<ParticleSystem> system);

		void UpdateShaderResourceOfSystem(std::shared_ptr<RenderingData_Particle>& renderingData, std::shared_ptr<ParticleSystem> system);

		// System�� ��� ������� ���̴��� ������Ʈ !
		void UpdateBufferOfShader(std::shared_ptr<ParticleSystem>& system);

		// First Run
		void FirstRunOfSystem(std::shared_ptr<ParticleSystem>& system);

		// Continuous
		void ContinuousRunOfSystem(std::shared_ptr<ParticleSystem>& system);

		// Only Delete And Draw Run => Loop Particle Last Phase
		void OnlyDeleteAndDrawRunOfSystem(std::shared_ptr<ParticleSystem>& system);
	};
}