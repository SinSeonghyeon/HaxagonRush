#include "pch.h"
#include "ParticleProcessor.h"

#include "BlendState.h"
#include "DepthStencilState.h"

#include "ResourceManager.h"
#include "ShaderManager.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"

#include "Texture.h"

#include "Pass_Deferred.h"

namespace GraphicsEngine
{
	uint64 GraphicsEngine::ParticleSystem::_nextID = 0;

	void ParticleProcessor::Initialize()
	{
		_deviceContext = Renderer::Get()->GetDeviceContext();

		_depthStencilState = Renderer::Get()->GetDepthStencilState();

		_blendState = Renderer::Get()->GetBlendState();

		_gsStreamOutParticleCircle = ShaderManager::Get()->GetGS(TEXT("GS_Particle_StreamOut_Circle"));

		_gsStreamOutParticleSphere = ShaderManager::Get()->GetGS(TEXT("GS_Particle_StreamOut_Sphere"));

		_gsStreamOutOnlyDelete = ShaderManager::Get()->GetGS(TEXT("GS_Particle_StreamOut_OnlyDelete"));

		_gsDraw = ShaderManager::Get()->GetGS(TEXT("GS_Particle_Draw"));

		_vsStreamOut = ShaderManager::Get()->GetVS(TEXT("VS_Particle_StreamOut"));

		_vsDraw = ShaderManager::Get()->GetVS(TEXT("VS_Particle_Draw"));

		_psDraw = ShaderManager::Get()->GetPS(TEXT("PS_Particle_Draw"));
	
		// ������ Pass
		_deferredPassRef = Renderer::Get()->GetDeferredPass();

		// Key : ID => Value : �׷��Ƚ� �������� ȸ����Ű�� ParticleSyatem ��ü
		_systemsIDMap = std::map<uint64, std::shared_ptr<ParticleSystem>>();

		// ���̴� �ڵ� ������ ������ ����� �ؽ���
		CreateRandomTextureSRV();
	}

	void ParticleProcessor::Release()
	{
		_deviceContext.Reset();

		_gsStreamOutParticleCircle.reset();

		_gsStreamOutParticleSphere.reset();

		_gsStreamOutOnlyDelete.reset();

		_gsDraw.reset();

		_vsStreamOut.reset();

		_vsDraw.reset();

		_psDraw.reset();

		// Pass Ref Reset
		_deferredPassRef.reset();

		_randVecTexSRV.ReleaseAndGetAddressOf();

		// �����ִ� �ý����� �������Ѵ�.
		for (auto& system : _systemsIDMap)
			system.second->Release();
	}

	void ParticleProcessor::Render(std::shared_ptr<RenderTarget> targetScreen,
		std::vector<std::shared_ptr<RenderingData_Particle>> renderQueueParticles)
	{
		// 1. ���� ������ Particle�鿡 ���Ͽ� �׸���.
		// (������Ʈ�� �����ϴ� �� ��� �߻��ϴ� Loop�̰ų�, ó�� �� ������)
		for (auto& particleData : renderQueueParticles)
		{
			// �̹� �����ӿ� ������ �༮�̴�.
			if (particleData->_initInfo->_firstRun)
			{
				std::shared_ptr<ParticleSystem> newSys = std::make_shared<ParticleSystem>();

				// IsLoop, Duration, TotalTime �� .. 
				newSys->Initialize(particleData);

				UpdateConstantBufferOfSystem(particleData, newSys);

				UpdateShaderResourceOfSystem(particleData, newSys);

				UpdateBufferOfShader(newSys);

				FirstRunOfSystem(newSys);

				particleData->_initInfo->_firstRun = false;

				particleData->_initInfo->_particlePlayID = newSys->_systemPlayID;

				_systemsIDMap.insert({ newSys->_systemPlayID , newSys });

				newSys->_totalTime += _currFramedeltaTime;

				newSys->_isThisFrameUpdated = true;
			}
			else
			{
				std::shared_ptr<ParticleSystem> system = GetParticleSystem(particleData->_initInfo->_particlePlayID);

				if (system == nullptr)
					continue;

				UpdateConstantBufferOfSystem(particleData, system);

				UpdateShaderResourceOfSystem(particleData, system);

				UpdateBufferOfShader(system);

				ContinuousRunOfSystem(system);

				system->_totalTime += _currFramedeltaTime;

				system->_isThisFrameUpdated = true;
			}
		}

#pragma region ������
		//// 2. System �� Loop�� �ƴϸ� ���� ������ ����. Duration�� Total Time���� ��Ʈ���ؼ� �Ѿ����� ����
		//// 3. System �� Loop�ε� ���� �̹� �����ӿ� ������ �ȿԴٸ� ? => Stop ȣ���ߴٴ� ��. ����������.
		//for (auto iter = _systemsIDMap.begin() ;  iter != _systemsIDMap.end() ;)
		//{
		//	std::shared_ptr<ParticleSystem> system = iter->second;

		//	// ��������
		//	if ((!system->_isLoop) && (system->_duration <= system->_totalTime))
		//	{
		//		system->Release();

		//		_systemsIDMap.erase(iter++);
		//	}
		//	// ���� �ƴѵ� �ð��� ���� �� ��. => ������Ʈ
		//	else if (!system->_isLoop)
		//	{
		//		UpdateBufferOfShader(system);

		//		ContinuousRunOfSystem(system);

		//		system->_totalTime += _currFramedeltaTime;

		//		system->_isThisFrameUpdated = true;

		//		++iter;
		//	}
		//	// ������ ���� �ý����̿����� 1������ ���� ���� �༮
		//	/*else if ((!system->_isLoop) && (system->_isLoopedSystem))
		//	{
		//		system->_totalTime += _currFramedeltaTime;

		//		system->_isThisFrameUpdated = true;

		//		++iter;
		//	}*/
		//	// Loop System�ε� �̹� �����ӿ� ������Ʈ ���� �ʾҴٸ� ? => Stop => ����
		//	// �� �ƴ� .. LifeSpan��ŭ ������ ���� ������ .. �� ������ ���� ����. (����Ƽ ��� �Ǿ��ִ����� ��-��)
		//	else if ((system->_isLoop) && (!system->_isThisFrameUpdated))
		//	{
		//		/*system->_isLoop = false;

		//		system->_duration = system->_totalTime + system->_cbStreamOut._lifeSpan;
		//		
		//		++iter;*/

		//		// system->_isLoopedSystem = true;

		//		// => ���� ����� ���ϴ� üũ �ɼ��� �ʿ��� �� ����.

		//		system->Release();

		//		_systemsIDMap.erase(iter++);
		//	}
		//	// �ƹ� ���� ���ٸ� ��� ������ ������Ʈ ���� �ʱ�ȭ���ݽô�.
		//	else
		//	{
		//		system->_isThisFrameUpdated = false;

		//		++iter;
		//	}
		//}
#pragma endregion

#pragma region �Ź���
		// 2. System �� Loop�� �ƴϸ� ���� ������ ����. Duration�� Total Time���� ��Ʈ���ؼ� �Ѿ����� ����
		// 3. System �� Loop�ε� ���� �̹� �����ӿ� ������ �ȿԴٸ� ? => Stop ȣ���ߴٴ� ��. ����������.
		for (auto iter = _systemsIDMap.begin(); iter != _systemsIDMap.end();)
		{
			std::shared_ptr<ParticleSystem> system = iter->second;

			if ((system->_isFinalPhaseOfSystem) && (system->_duration > system->_totalTime))
			{
				UpdateBufferOfShader(system);

				OnlyDeleteAndDrawRunOfSystem(system);

				system->_totalTime += _currFramedeltaTime;

				system->_isThisFrameUpdated = true;

				++iter;

				continue;
			}
			else if ((system->_isFinalPhaseOfSystem) && (system->_duration <= system->_totalTime))
			{
				// ����� 
				system->Release();

				_systemsIDMap.erase(iter++);

				continue;
			}

			// ���� �ý��� �ƴѵ� �ð��� �� �Ǿ��� ! => ���̳� ������� ��������. (������ ���Ҹ�ŭ�� �� �ùķ��̼�, �� ����)
			if ((!system->_isLoop) && (system->_duration <= system->_totalTime))
			{
				system->_isFinalPhaseOfSystem = true;

				// ��ƼŬ�� ������ �� �� ������Ʈ�� ������.
				system->_duration = system->_totalTime + system->_cbStreamOut._lifeSpan;

				++iter;
			}
			// ���� �ƴѵ� �ð��� ���� �� ��. => ������Ʈ
			else if (!system->_isLoop)
			{

				UpdateBufferOfShader(system);

				ContinuousRunOfSystem(system);

				system->_totalTime += _currFramedeltaTime;

				system->_isThisFrameUpdated = true;

				++iter;
			}
			
			// �����ε� ���� �޽����� �ȿ�. �ߴ� => Final Phase�� ����.
			else if ((system->_isLoop) && (!system->_isThisFrameUpdated))
			{
				UpdateBufferOfShader(system);

				OnlyDeleteAndDrawRunOfSystem(system);

				system->_isFinalPhaseOfSystem = true;

				system->_duration = system->_totalTime + system->_cbStreamOut._lifeSpan;

				++iter;

				// => ���� ����� ���ϴ� üũ �ɼ��� �ʿ��� �� ����.
			}
			// �ƹ� ���� ���ٸ� ��� ������ ������Ʈ ���� �ʱ�ȭ���ݽô�.
			else
			{
				system->_isThisFrameUpdated = false;

				++iter;
			}
		}
#pragma endregion
		// NULL To Un-Binding
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

		_deviceContext->GSSetShaderResources(0, 1, nullSRV);
	}

	void ParticleProcessor::CreateRandomTextureSRV()
	{
		ComPtr<ID3D11Device> device = Renderer::Get()->GetDevice();

		std::random_device rd;

		std::mt19937 gen(rd());

		std::uniform_real_distribution<float> dist(-1.f, 1.f);

		// 
		// Create the random data.
		//
		Vector4 randomValues[1024];

		for (int i = 0; i < 1024; ++i)
		{
			randomValues[i].x = dist(gen);
			randomValues[i].y = dist(gen);
			randomValues[i].z = dist(gen);
			randomValues[i].w = dist(gen);
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024 * sizeof(Vector4);
		initData.SysMemSlicePitch = 0;

		//
		// Create the texture.
		//
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ComPtr<ID3D11Texture1D> randomTex = 0;
		HR(device->CreateTexture1D(&texDesc, &initData, randomTex.GetAddressOf()));

		//
		// Create the resource view.
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* randomTexSRV = 0;
		HR(device->CreateShaderResourceView(randomTex.Get(), &viewDesc, &_randVecTexSRV));

		randomTex.ReleaseAndGetAddressOf();
	}

	std::shared_ptr<GraphicsEngine::ParticleSystem> ParticleProcessor::GetParticleSystem(const uint64& particleDataID)
	{
		return _systemsIDMap.find(particleDataID) != _systemsIDMap.end() ? _systemsIDMap.at(particleDataID) : nullptr;
	}

	void ParticleProcessor::UpdateConstantBufferOfSystem(std::shared_ptr<RenderingData_Particle>& renderingData, std::shared_ptr<ParticleSystem> system)
	{
		// TEST CODE .. ������� ���� �ѹ��� �ʱ�ȭ���ش�.
		static std::random_device rd;

		static std::mt19937 gen(rd());

		static std::uniform_real_distribution<float> dist(0.f, 1.f);

		// CB_Stream Output Update
		system->_cbStreamOut._emitVelocity = renderingData->_shapeInfo->_emitVelocity;
		system->_cbStreamOut._emitTime = renderingData->_shapeInfo->_emitTime;
		system->_cbStreamOut._transformMatrix = renderingData->_shapeInfo->_transformMatrix;
		system->_cbStreamOut._radius = renderingData->_shapeInfo->_radius;

		if (system->_useXYSameDeviation)
		{
			float dev = std::max<float>(system->_norDistX(gen), system->_norDistY(gen));

			system->_cbStreamOut._averageSize = renderingData->_shapeInfo->_particleAverageSize + Vector2(dev, dev);
		}
		else
			system->_cbStreamOut._averageSize = renderingData->_shapeInfo->_particleAverageSize + Vector2(system->_norDistX(gen), system->_norDistY(gen));

		system->_cbStreamOut._lifeSpan = renderingData->_shapeInfo->_lifeSpan;
		system->_cbStreamOut._period = renderingData->_shapeInfo->_period;
		system->_cbStreamOut._randNumber = dist(gen);							// ���̴����� ���� �߻��� ���� ����ϴ� ��

		// CB_Draw Update
		system->_cbDraw._acceleration = renderingData->_shapeInfo->_acceleration;
		system->_cbDraw._lifeSpan = renderingData->_shapeInfo->_lifeSpan;

		// CB_Color Update
		system->_cbColor._color = renderingData->_shapeInfo->_color;
	}

	void ParticleProcessor::UpdateShaderResourceOfSystem(std::shared_ptr<RenderingData_Particle>& renderingData, std::shared_ptr<ParticleSystem> system)
	{
		system->_refTextureID = renderingData->_shapeInfo->_refTextureID;
	}

	void ParticleProcessor::UpdateBufferOfShader(std::shared_ptr<ParticleSystem>& system)
	{
		// --------------------------------- Constant Buffer Resource ---------------------------------
		// ���̴��鿡�� ������Ʈ ���۸� ������Ʈ ��ų �غ� ���ش�.

		// CB_Timer Update (������ �ƴ� ģ�����Ե� �ð��� �ǽð� ������Ʈ�� �ʿ��ϴ�.)
		system->_cbTimer._deltaTime = _currFramedeltaTime;
		system->_cbTimer._totalTime += _currFramedeltaTime;						// �̰� ��¥ �� ��Ż Ÿ������ �����ص� ���� ����.

		_vsDraw->UpdateConstantBuffer(&system->_cbDraw, CBV_REGISTER::b1);
		_psDraw->UpdateConstantBuffer(&system->_cbTimer, CBV_REGISTER::b1);
		_psDraw->UpdateConstantBuffer(&system->_cbColor, CBV_REGISTER::b2);

		_gsStreamOutParticleCircle->UpdateConstantBuffer(&system->_cbStreamOut, CBV_REGISTER::b1);
		_gsStreamOutParticleCircle->UpdateConstantBuffer(&system->_cbTimer, CBV_REGISTER::b2);

		_gsStreamOutParticleSphere->UpdateConstantBuffer(&system->_cbStreamOut, CBV_REGISTER::b1);
		_gsStreamOutParticleSphere->UpdateConstantBuffer(&system->_cbTimer, CBV_REGISTER::b2);

		_gsStreamOutOnlyDelete->UpdateConstantBuffer(&system->_cbStreamOut, CBV_REGISTER::b1);
		_gsStreamOutOnlyDelete->UpdateConstantBuffer(&system->_cbTimer, CBV_REGISTER::b2);


		// --------------------------------- Shader Resource ---------------------------------
		// ��ƼŬ�� �׸��µ� ����ϴ� ���۷��� �ؽ���
		std::shared_ptr<Texture> tex = ResourceManager::Get()->GetTexture(system->_refTextureID);

		if (tex != nullptr)
			_psDraw->UpdateShaderResource(tex->GetSRV(), SRV_REGISTER::t0);

		// ���� �߻�
		_gsStreamOutParticleCircle->UpdateShaderResource(_randVecTexSRV, SRV_REGISTER::t0);
		_gsStreamOutParticleSphere->UpdateShaderResource(_randVecTexSRV, SRV_REGISTER::t0);
		_gsStreamOutOnlyDelete->UpdateShaderResource(_randVecTexSRV, SRV_REGISTER::t0);

		// Z - Culling
		_gsDraw->UpdateShaderResource(_deferredPassRef->GetDepth()->GetSRV(), SRV_REGISTER::t0);
	}

	void ParticleProcessor::FirstRunOfSystem(std::shared_ptr<ParticleSystem>& system)
	{
		// 1. Stream - Out Pass
		// �̹� ���꿡�� �ٷ�� Primitive Topology�� PointList ..!
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		uint32 stride = sizeof(Particle);
		uint32 offset = 0;

		_deviceContext->IASetVertexBuffers(0, 1, system->_initVB.GetAddressOf(), &stride, &offset);

		// ���� ���� ����� ��Ʈ�� ��� ���� ������� �׷��� ���ڵ��� �����Ѵ�.
		// ���ŵ� ���ڵ��� ��Ʈ�� ����� ���ؼ� Target���� ������ ���ۿ� ��ϵȴ�.
		_deviceContext->SOSetTargets(1, system->_streamOutVB.GetAddressOf(), &offset);

		// Pixel Shader ����.
		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		// Depth�� ����.
		_depthStencilState->SetDepthDisableState(_deviceContext);

		_vsStreamOut->Update();

		if (system->_effectType == PARTICLE_EFFECT_TYPE::CIRCLE)
			_gsStreamOutParticleCircle->Update();
		else if (system->_effectType == PARTICLE_EFFECT_TYPE::SPHERE)
			_gsStreamOutParticleSphere->Update();
			
		_deviceContext->Draw(1, 0);

		// Stream Out Target���� ����� VB UnBinding
		ID3D11Buffer* bufferArray[1] = { nullptr };

		_deviceContext->SOSetTargets(1, bufferArray, &offset);

		// ���ؽ� ���� ���� (�׸� �׸��� ��� ��Ʈ�� �ƿ��� ������� ����� �༮�� ������ ������
		// �ٲ㰡�鼭 ����Ѵ�. ������ ���� ���۸�)
		// �׷� ���� ��Ʈ�� �� �༮�� Draw !
		system->_streamOutVB.Swap(system->_drawVB);

		// 2. Draw Particles Pass

		// ������ �ٽ� ���ش�.
		_depthStencilState->SetDepthEnableState(_deviceContext);

		// Blend State On
		_blendState->SetSrcAdditiveBlendState(_deviceContext);

		_deviceContext->IASetVertexBuffers(0, 1, system->_drawVB.GetAddressOf(), &stride, &offset);

		// ���̴� ����
		_vsDraw->Update();
		_gsDraw->Update();
		_psDraw->Update();

		// �׷� !
		_deviceContext->DrawAuto();

		// 3. ������ �۾� (���� ���̴� ����, ���� ������Ʈ ����)
		_deviceContext->GSSetShader(nullptr, NULL, NULL);

		_blendState->SetBlendStateOff(_deviceContext);
	}

	void ParticleProcessor::ContinuousRunOfSystem(std::shared_ptr<ParticleSystem>& system)
	{
		// 1. Stream - Out Pass
		// �̹� ���꿡�� �ٷ�� Primitive Topology�� PointList ..!
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		uint32 stride = sizeof(Particle);
		uint32 offset = 0;

		_deviceContext->IASetVertexBuffers(0, 1, system->_drawVB.GetAddressOf(), &stride, &offset);

		// ���� ���� ����� ��Ʈ�� ��� ���� ������� �׷��� ���ڵ��� �����Ѵ�.
		// ���ŵ� ���ڵ��� ��Ʈ�� ����� ���ؼ� Target���� ������ ���ۿ� ��ϵȴ�.
		_deviceContext->SOSetTargets(1, system->_streamOutVB.GetAddressOf(), &offset);

		// Pixel Shader ����.
		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		// Depth�� ����.
		_depthStencilState->SetDepthDisableState(_deviceContext);

		_vsStreamOut->Update();

		if (system->_effectType == PARTICLE_EFFECT_TYPE::CIRCLE)
			_gsStreamOutParticleCircle->Update();
		else if (system->_effectType == PARTICLE_EFFECT_TYPE::SPHERE)
			_gsStreamOutParticleSphere->Update();

		_deviceContext->DrawAuto();

		// Stream Out Target���� ����� VB UnBinding
		ID3D11Buffer* bufferArray[1] = { nullptr };

		_deviceContext->SOSetTargets(1, bufferArray, &offset);

		// ���ؽ� ���� ���� (�׸� �׸��� ��� ��Ʈ�� �ƿ��� ������� ����� �༮�� ������ ������
		// �ٲ㰡�鼭 ����Ѵ�. ������ ���� ���۸�)
		// �׷� ���� ��Ʈ�� �� �༮�� Draw !
		system->_streamOutVB.Swap(system->_drawVB);

		// 2. Draw Particles Pass
		// ������ �ٽ� ���ش�.
		_depthStencilState->SetDepthEnableState(_deviceContext);

		// Blend State On
		_blendState->SetSrcAdditiveBlendState(_deviceContext);

		_deviceContext->IASetVertexBuffers(0, 1, system->_drawVB.GetAddressOf(), &stride, &offset);

		// ���̴� ����
		_vsDraw->Update();
		_gsDraw->Update();
		_psDraw->Update();

		// �׷� !
		_deviceContext->DrawAuto();

		// 3. ������ �۾� (���� ���̴� ����, ���� ������Ʈ ����)
		_deviceContext->GSSetShader(nullptr, NULL, NULL);

		_blendState->SetBlendStateOff(_deviceContext);
	}

	void ParticleProcessor::OnlyDeleteAndDrawRunOfSystem(std::shared_ptr<ParticleSystem>& system)
	{
		// 1. Stream - Out Pass
		// �̹� ���꿡�� �ٷ�� Primitive Topology�� PointList ..!
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		uint32 stride = sizeof(Particle);
		uint32 offset = 0;

		_deviceContext->IASetVertexBuffers(0, 1, system->_drawVB.GetAddressOf(), &stride, &offset);

		// ���� ���� ����� ��Ʈ�� ��� ���� ������� �׷��� ���ڵ��� �����Ѵ�.
		// ���ŵ� ���ڵ��� ��Ʈ�� ����� ���ؼ� Target���� ������ ���ۿ� ��ϵȴ�.
		_deviceContext->SOSetTargets(1, system->_streamOutVB.GetAddressOf(), &offset);

		// Pixel Shader ����.
		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		// Depth�� ����.
		_depthStencilState->SetDepthDisableState(_deviceContext);

		_vsStreamOut->Update();

		// �� Phase������ ���ؽ��� Delete Time Update�� ����⸸ �Ѵ�.
		_gsStreamOutOnlyDelete->Update();

		_deviceContext->DrawAuto();

		// Stream Out Target���� ����� VB UnBinding
		ID3D11Buffer* bufferArray[1] = { nullptr };

		_deviceContext->SOSetTargets(1, bufferArray, &offset);

		// ���ؽ� ���� ���� (�׸� �׸��� ��� ��Ʈ�� �ƿ��� ������� ����� �༮�� ������ ������
		// �ٲ㰡�鼭 ����Ѵ�. ������ ���� ���۸�)
		// �׷� ���� ��Ʈ�� �� �༮�� Draw !
		system->_streamOutVB.Swap(system->_drawVB);

		// 2. Draw Particles Pass
		// ������ �ٽ� ���ش�.
		_depthStencilState->SetDepthEnableState(_deviceContext);

		// Blend State On
		_blendState->SetSrcAdditiveBlendState(_deviceContext);

		_deviceContext->IASetVertexBuffers(0, 1, system->_drawVB.GetAddressOf(), &stride, &offset);

		// ���̴� ����
		_vsDraw->Update();
		_gsDraw->Update();
		_psDraw->Update();

		// �׷� !
		_deviceContext->DrawAuto();

		// 3. ������ �۾� (���� ���̴� ����, ���� ������Ʈ ����)
		_deviceContext->GSSetShader(nullptr, NULL, NULL);

		_blendState->SetBlendStateOff(_deviceContext);
	}

	// --------------------- Particle System In Graphics Engine ---------------------
	void ParticleSystem::Initialize(std::shared_ptr<RenderingData_Particle> targetData)
	{
		// ����� �پ缺�� �ֱ� ���� ���� ����
		_norDistX = std::normal_distribution<float>(targetData->_shapeInfo->_particleAverageSize.x, targetData->_shapeInfo->_deviation.x);

		_norDistY = std::normal_distribution<float>(targetData->_shapeInfo->_particleAverageSize.y, targetData->_shapeInfo->_deviation.y);

		_useXYSameDeviation = targetData->_shapeInfo->_useXYSameDeviation;

		_systemPlayID = _nextID++;

		_totalTime = 0.f;

		_duration = targetData->_initInfo->_duration;

		_isLoop = targetData->_initInfo->_isLoop;

		_effectType = targetData->_initInfo->_particleEffectType;

		InitVBs(targetData);
	}

	void ParticleSystem::InitVBs(std::shared_ptr<RenderingData_Particle> targetData)
	{
		ComPtr<ID3D11Device> device = Renderer::Get()->GetDevice();

		D3D11_BUFFER_DESC vbDesc;

		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = sizeof(Particle);
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		Particle initParticle;
		ZeroMemory(&initParticle, sizeof(Particle) * 1);

		initParticle._age = 0.f;

		// Emitting Particle Type
		initParticle._type = PARTICLE_EMITTER;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &initParticle;

		HRESULT hr = device->CreateBuffer(&vbDesc, &vinitData, _initVB.GetAddressOf());

		vbDesc.ByteWidth = sizeof(Particle) * targetData->_initInfo->_maxParticleCount;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

		hr = device->CreateBuffer(&vbDesc, NULL, _streamOutVB.GetAddressOf());
		hr = device->CreateBuffer(&vbDesc, NULL, _drawVB.GetAddressOf());
	}

	void ParticleSystem::Release()
	{
		_initVB.ReleaseAndGetAddressOf();

		_drawVB.ReleaseAndGetAddressOf();

		_streamOutVB.ReleaseAndGetAddressOf();
	}
}