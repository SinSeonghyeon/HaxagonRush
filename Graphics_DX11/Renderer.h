#pragma once
#include "Graphics_RenderingData_UI.h"
#include "Graphics_RenderingData_3D.h"
#include "Graphics_RenderingData_Particle.h"
#include "CB_Structure.h"

#include "Device.h"
#include "SwapChain.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "SpriteBatcher.h"
#include "RenderTarget.h"
#include "DepthStencilBuffer.h"
#include "BlendState.h"
#include "WindowInfo.h"

namespace GraphicsEngine
{
	class Pass_Shadow;

	class Pass_Deferred;

	class Pass_SSAO;

	class Pass_Bloom;

	class Pass_Final;

	class Pass_Sampling;

	class Pass_GaussianBlur;

	class Pass_EdgePreserveBlur;

	class Pass_Lighting;

	class Pass_FXAA;

	class Pass_CameraMotionBlur;

	class ParticleProcessor;

	class PostProcessor;

	struct PerFrameConstantBuffer final
	{
		void Release()
		{
			_cbPerFrameGPU.ReleaseAndGetAddressOf();

			_cbPerFrameCPU.reset();
		}

		POSTPROCESS_OPTION _currPostProcessOption;

		ComPtr<ID3D11Buffer> _cbPerFrameGPU;

		std::shared_ptr<CB_PERFRAME> _cbPerFrameCPU;

		void Initialize(ComPtr<ID3D11Device> device);

		void UpdatePerFrameConstantBuffer(ComPtr<ID3D11DeviceContext> deviceContext,
			std::shared_ptr<::PerFrameData> perframeData);
	};

	// ��ü - ���������� �׸��� �׸��� ������ �����Ѵ�.
	// �� ������ ��� �˾��� ������ ���� Ŭ����
	class Renderer
	{
	public:
		Renderer() = default;

		~Renderer() = default;

		Renderer(const Renderer& other) = delete;

		Renderer& operator= (const Renderer& other) = delete;

		Renderer(Renderer&& other) = delete;

	private:
		static std::shared_ptr<Renderer> _renderer;

	public:
		static std::shared_ptr<Renderer> Get();

	public:
		void Initialize(HWND hWnd, uint32 screenWidth, uint32 screenHeight);

		void InitializeForPass();

		void Release();

	private:
		WindowInfo _info;

		std::shared_ptr<Device> _device;

		std::shared_ptr<SwapChain> _swapChain;

		std::shared_ptr<DepthStencilState> _depthStencilState;

		std::shared_ptr<RasterizerState> _rasterizerState;

		std::shared_ptr<SamplerState> _samplerState;

		std::shared_ptr<BlendState> _blendState;

		std::shared_ptr<SpriteBatcher> _spriteBatcher;

	public:
		// TEST
		uint32 _currCubeMapID;

	private:
		// �� �� ���� ��� (�����δ� �׸���, ���� ��ü �� .. ��������.
		// ���� �Ŵ��� �ϳ��� RenderQueue Class�� ������.
		std::vector<std::shared_ptr<::RenderingData_UI>> _renderQueueUI;

		std::vector<std::shared_ptr<::RenderingData_3D>> _renderQueue;

		std::vector<std::shared_ptr<::RenderingData_Particle>> _renderQueueParticle;

		std::vector<std::shared_ptr<::TextData>> _textDataQueue;

	public:
		void OnResize(const uint32& screenWidth, const uint32& screenHeight);

		ComPtr<ID3D11Device> GetDevice() { return _device->GetDevice(); }

		ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _device->GetDeviceContext(); }

		std::shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }

		std::shared_ptr<DepthStencilState> GetDepthStencilState() { return _depthStencilState; }

		std::shared_ptr<RasterizerState> GetRasterizerState() { return _rasterizerState; }

		std::shared_ptr<SamplerState> GetSamplerState() { return _samplerState; }

		std::shared_ptr<BlendState> GetBlendState() { return _blendState; }

		std::shared_ptr<SpriteBatcher> GetSpriteBatcher() { return _spriteBatcher; }

		bool GetIsEnable4xMSAA() { return _info._enable4xMSAA; }

		void PostRenderingData_UI(std::shared_ptr<::RenderingData_UI>& renderingData);

		// 3D ������ �����͸� ������ ���ο� �������� �׽��ϴ�.
		void PostRenderingData_3D(std::shared_ptr<::RenderingData_3D>& renderingData);

		void PostRenderingData_Particle(std::shared_ptr<::RenderingData_Particle>& renderingData);

		void PostTextData(std::shared_ptr<::TextData> textData);

		// ���� ������ �����͸� ��� ó���ϰ� �Ϸ��� �������� ȭ�� �� ����մϴ�.
		void ExecuteRenderQueue();

		// Per Frame Data�� 0�� �������Ϳ� �Ⱦ� �����ϴ�.
		void ExecuteForPerFrameData(std::shared_ptr<::PerFrameData> perframeData);

	private:
		std::shared_ptr<Pass_Shadow> _shadowPass;

		std::shared_ptr<Pass_Deferred> _deferredPass;

		std::shared_ptr<Pass_Lighting> _lightingPass;

		std::shared_ptr<Pass_Sampling> _samplingPass;

		std::shared_ptr<ParticleProcessor> _particleProcessor;

		std::shared_ptr<PostProcessor> _postProcessor;

		std::shared_ptr<PerFrameConstantBuffer> _cbPerFrame;






		// SSAO ����
		// std::shared_ptr<Pass_SSAO> _ssaoPass;

		// std::shared_ptr<Pass_EdgePreserveBlur> _edgePreserveBlurPass;

		friend class Pass_Bloom;

	public:
		// G-Buffer�� �����ϴ� ��찡 ���Ƽ� �׳� ����.
		std::shared_ptr<Pass_Deferred> GetDeferredPass() { return _deferredPass; }
	};
}