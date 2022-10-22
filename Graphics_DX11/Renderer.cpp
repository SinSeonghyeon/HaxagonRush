#include "pch.h"
#include "Renderer.h"

#include "Graphics_RenderingData_3D.h"
#include "CB_Structure.h"

#include "Device.h"
#include "SwapChain.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"

#include "ShaderManager.h"
#include "ResourceManager.h"

#include "Texture.h"
#include "Mesh.h"
#include "Font.h"

#include "VertexShader.h"
#include "PixelShader.h"

#include "Pass_Shadow.h"
#include "Pass_Deferred.h"
#include "Pass_SSAO.h"
#include "Pass_GaussianBlur.h"
#include "Pass_EdgePreserveBlur.h"
#include "Pass_Sampling.h"
#include "Pass_Lighting.h"
#include "Pass_CameraMotionBlur.h"
#include "Pass_Bloom.h"
#include "Pass_FXAA.h"
#include "Pass_Final.h"

#include "ParticleProcessor.h"
#include "PostProcessor.h"

std::shared_ptr<GraphicsEngine::Renderer> GraphicsEngine::Renderer::_renderer = nullptr;

namespace GraphicsEngine
{
	std::shared_ptr<GraphicsEngine::Renderer> Renderer::Get()
	{
		if (_renderer == nullptr)
			_renderer = std::make_shared<GraphicsEngine::Renderer>();

		return _renderer;
	}

	void Renderer::Initialize(HWND hWnd, uint32 screenWidth, uint32 screenHeight)
	{
		_info._screenWidth = screenWidth;
		_info._screenHeight = screenHeight;
		_info._hWnd = hWnd;
		_info._enable4xMSAA = true;

		_device = std::make_shared<Device>();
		_swapChain = std::make_shared<SwapChain>();
		_depthStencilState = std::make_shared<DepthStencilState>();
		_rasterizerState = std::make_shared<RasterizerState>();
		_samplerState = std::make_shared<SamplerState>();
		_blendState = std::make_shared<BlendState>();
		_spriteBatcher = std::make_shared<SpriteBatcher>();

		_device->Initialize(_info);
		_swapChain->Initialize(_info, _device, screenWidth, screenHeight);
		_depthStencilState->Initialize(_device->GetDevice());
		_rasterizerState->Initialize(_device->GetDevice());
		_samplerState->Initialize(_device->GetDevice());
		_blendState->Initialize(_device->GetDevice());
		_spriteBatcher->Initialize(_device->GetDeviceContext());

		// ����� ��ü �̸��� �����մϴ�.
		D3D11SetDebugObjectName(_device->GetDeviceContext().Get(), "ImmediateContext");
		DXGISetDebugObjectName(_swapChain->GetSwapChain().Get(), "SwapChain");

		// �׷��� ī�忡 �����͸� ���� �� �Ű�ü�� ����� �༮��
		_cbPerFrame = std::make_shared<PerFrameConstantBuffer>();
		_cbPerFrame->Initialize(_device->GetDevice());
	}

	void Renderer::InitializeForPass()
	{
		// ------------------ Pass ------------------
		_shadowPass = std::make_shared<Pass_Shadow>();
		_shadowPass->Initialize(_info._screenWidth, _info._screenHeight);

		_deferredPass = std::make_shared<Pass_Deferred>();
		_deferredPass->Initialize(_info._screenWidth, _info._screenHeight);

		_samplingPass = std::make_shared<Pass_Sampling>();
		_samplingPass->Initialize(_info._screenWidth, _info._screenHeight);		

		_lightingPass = std::make_shared<Pass_Lighting>();
		_lightingPass->Initialize(_info._screenWidth, _info._screenHeight);
	
		// Particle Processing
		_particleProcessor = std::make_shared<ParticleProcessor>();
		_particleProcessor->Initialize();

		// Post Processing
		_postProcessor = std::make_shared<PostProcessor>();
		_postProcessor->Initialize(_lightingPass, _deferredPass,
			_info._screenWidth, _info._screenHeight);



		// SSAO ����
		/*_edgePreserveBlurPass = std::make_shared<Pass_EdgePreserveBlur>();
		_edgePreserveBlurPass->Initialize(_info._screenWidth, _info._screenHeight);

		_ssaoPass = std::make_shared<Pass_SSAO>();
		_ssaoPass->Initialize(_info._screenWidth, _info._screenHeight);*/
	}

	void Renderer::Release()
	{
		_device->Release();

		_swapChain->Release();

		_depthStencilState->Release();

		_rasterizerState->Release();

		_samplerState->Release();

		_blendState->Release();

		_spriteBatcher->Release();

		for (int i = 0; i < _renderQueue.size() ; i++)
			_renderQueue.front().reset();

		_cbPerFrame->Release();

		// ------------------ Pass ------------------
		_shadowPass->Release();

		_deferredPass->Release();

		_samplingPass->Release();

		_lightingPass->Release();

		// ------------------ Processor ------------------
		_particleProcessor->Release();

		_postProcessor->Release();
		
		// SSAO ����
		/*_ssaoPass->Release();
		_edgePreserveBlurPass->Release();*/
	}

	void Renderer::OnResize(const uint32& screenWidth, const uint32& screenHeight)
	{
		_info._screenWidth = screenWidth;
		_info._screenHeight = screenHeight;

		// ------------------ Pass ------------------
		_shadowPass->OnResize(screenWidth * 4, screenHeight * 4);
		_deferredPass->OnResize(screenWidth, screenHeight);
		_samplingPass->OnResize(screenWidth, screenHeight);
		_lightingPass->OnResize(screenWidth, screenHeight);

		_postProcessor->OnResize(screenWidth, screenHeight);

		// SSAO ���� (�ƹ��� �ص� ���ڰ� �������� ���� ..)
		/*_ssaoPass->OnResize(screenWidth, screenHeight);
		_edgePreserveBlurPass->OnResize(screenWidth, screenHeight);*/
	}

	void Renderer::PostRenderingData_UI(std::shared_ptr<::RenderingData_UI>& renderingData)
	{
		_renderQueueUI.push_back(renderingData);
	}

	void Renderer::PostRenderingData_3D(std::shared_ptr<::RenderingData_3D>& renderingData)
	{
		_renderQueue.push_back(renderingData);
	}

	void Renderer::PostRenderingData_Particle(std::shared_ptr<::RenderingData_Particle>& renderingData)
	{
		_renderQueueParticle.push_back(renderingData);
	}

	void Renderer::PostTextData(std::shared_ptr<::TextData> textData)
	{
		_textDataQueue.push_back(textData);
	}

	void Renderer::ExecuteRenderQueue()
	{
		// �׸��� ���� �׸���.
		_shadowPass->Render(_cbPerFrame, _renderQueue);
		
		// ���� ���� G-Buffer �� ��Ÿ ���� �̾Ƴ���.
		_deferredPass->Render(_renderQueue);

#pragma region SSAO ���μ�
		//// SSAO : ȭ��� ������ ������� �׸��� .. �ִ��� ǰ���� ������ ..!
		//_ssaoPass->Render(_deferredPass->GetDepth(), _deferredPass->GetNormal());

		//// ���� SSAO Map�� ����� �������� ���ø� !!
		//_samplingPass->Render(_ssaoPass->_ssaoOutput, SAMPLING_OPTION::Half);

		//// ���� ������� Blur�ؼ� ������ ���� (4�� ������ �ؾ� ���ٸ� �ϴ� ..)
		//_edgePreserveBlurPass->Render(_deferredPass->GetDepth(), _deferredPass->GetNormal(), _samplingPass->_sampleHalf, BLUR_MAX_COUNT);
#pragma endregion

		// Emissive Effect !
		_postProcessor->_bloomPass->Render(_deferredPass->GetEmissive());

		// �� ������ �ǽ��Ѵ�.
		_lightingPass->Render(_deferredPass->_gBuffers, _shadowPass->_shadowDepth, _shadowPass->_cascadeShadowMap, nullptr,
			_shadowPass->_cbLightViewProjs.get(), _shadowPass->_cbDirLightViewProj.get(), _postProcessor->_bloomPass->GetOutput());

		// ��ƼŬ ������
		_particleProcessor->Render(_lightingPass->GetOutput(), _renderQueueParticle);

		// ������ ����Ʈ ���μ���, UI, Texture ������ �� ����ü�� �ø��α��� ..!
		_postProcessor->Render(_cbPerFrame->_currPostProcessOption, _textDataQueue, _renderQueueUI);

		// ���� ť �� UI ����ť, �ؽ�Ʈ ������ ť �ʱ�ȭ ! => CPU �ܿ��� �۾��� �� �� �ֵ��� �ѱ��.
		_renderQueue.clear();
		_renderQueueUI.clear();
		_renderQueueParticle.clear();
		_textDataQueue.clear();
	}

	void Renderer::ExecuteForPerFrameData(std::shared_ptr<::PerFrameData> perframeData)
	{
		// ���� ���ο��� �������� ����� PerFrame ConstantBuffer�� �ֽ�ȭ�մϴ�.
		_cbPerFrame->UpdatePerFrameConstantBuffer(_device->GetDeviceContext(), perframeData);

		// ��ƼŬ ���μ������� ���� �ùķ��̼��� �����ϱ� ���� DeltaTime�� �ǳ��ݴϴ�.
		_particleProcessor->SetCurrentFrameDeltaTime(perframeData->_deltaTime);
	}

	void PerFrameConstantBuffer::Initialize(ComPtr<ID3D11Device> device)
	{
		_cbPerFrameCPU = std::make_shared<CB_PERFRAME>();

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(CB_PERFRAME);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = NULL;
		cbDesc.StructureByteStride = NULL;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = _cbPerFrameCPU.get();
		initData.SysMemPitch = NULL;
		initData.SysMemSlicePitch = NULL;

		Renderer::Get()->GetDevice()->CreateBuffer(&cbDesc, &initData, _cbPerFrameGPU.GetAddressOf());
	}

	void PerFrameConstantBuffer::UpdatePerFrameConstantBuffer(ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<::PerFrameData> perframeData)
	{
		// Current Frame Post Processing Option
		memcpy(&_currPostProcessOption, &perframeData->_postProcessOption, sizeof(POSTPROCESS_OPTION));

		// --------------- Main Camera Data ---------------
		// �ϴ� ���� ��� ���� ���� �; ���⿡ ���д�. �� ģ���� ��� ��������, CB�� ���� �и������� .. ������ ������
		memcpy(&_cbPerFrameCPU->_cameraInfo, perframeData->_cameraInfo.get(), sizeof(CameraInfo));
		// --------------- Main Camera Data ---------------

		// Directional Light
		_cbPerFrameCPU->_directionalLightCount = static_cast<uint32>(perframeData->_directionalLightInfos.size());

		for (size_t i = 0; i < _cbPerFrameCPU->_directionalLightCount ; i++)
			memcpy(_cbPerFrameCPU->_directionalLightInfos + i, perframeData->_directionalLightInfos[i].get(), sizeof(DirectionalLightInfo));

		// Spot Light
		_cbPerFrameCPU->_spotLightCount = static_cast<uint32>(perframeData->_spotLightInfos.size());

		for (size_t i = 0; i < _cbPerFrameCPU->_spotLightCount; i++)
			memcpy(_cbPerFrameCPU->_spotLightInfos + i, perframeData->_spotLightInfos[i].get(), sizeof(SpotLightInfo));

		// Point Light
		_cbPerFrameCPU->_pointLightCount = static_cast<uint32>(perframeData->_pointLightInfos.size());

		for (size_t i = 0; i < _cbPerFrameCPU->_pointLightCount; i++)
			memcpy(_cbPerFrameCPU->_pointLightInfos + i, perframeData->_pointLightInfos[i].get(), sizeof(PointLightInfo));

		// SSAO
		memcpy(&_cbPerFrameCPU->_ssaoInfo, perframeData->_ssaoInfo.get(), sizeof(SSAOInfo));

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		deviceContext->Map(_cbPerFrameGPU.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		CB_PERFRAME* dataPtr = (CB_PERFRAME*)mappedResource.pData;

		memcpy(dataPtr, _cbPerFrameCPU.get(), sizeof(CB_PERFRAME));

		deviceContext->Unmap(_cbPerFrameGPU.Get(), 0);

		// b0�� �������ʹ� Per Frame Data�� Register�̴�.
		deviceContext->VSSetConstantBuffers(static_cast<uint32>(CBV_REGISTER::b0), 1, _cbPerFrameGPU.GetAddressOf());
		deviceContext->PSSetConstantBuffers(static_cast<uint32>(CBV_REGISTER::b0), 1, _cbPerFrameGPU.GetAddressOf());
		deviceContext->GSSetConstantBuffers(static_cast<uint32>(CBV_REGISTER::b0), 1, _cbPerFrameGPU.GetAddressOf());

		perframeData->_directionalLightInfos.clear();
		perframeData->_spotLightInfos.clear();
		perframeData->_pointLightInfos.clear();
	}
}