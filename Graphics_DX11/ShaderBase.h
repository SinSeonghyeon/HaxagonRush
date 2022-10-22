#pragma once
#include "Renderer.h"
#include "ResourceBase.h"
#include "CB_Structure.h"

namespace GraphicsEngine
{
	// ������ ����ؼ� ������Ʈ ���۵��� ��� �������� �� �ֵ��� ��������
	struct ConstantBuffer
	{
		ComPtr<ID3D11Buffer> _cbGPU;

		CBV_REGISTER _slot;
	};

	struct ShaderResource
	{
		ComPtr<ID3D11ShaderResourceView> _srv;

		SRV_REGISTER _slot;
	};

	struct Sampler
	{
		ComPtr<ID3D11SamplerState> _samplerState;

		SAM_REGISTER _slot;
	};

	// ����ϴ� Constant Buffer�� Ÿ���� �޴´�.
	template <SHADER_TYPE ShaderType>
	class ShaderBase
	{
	public:
		virtual void Initialize(const tstring& path, const tstring& shaderName, const tstring& entryPoint,
			const tstring& version, const D3D_SHADER_MACRO* defines) abstract;

		virtual void Update() abstract;

		virtual void Release();

	public:
		inline tstring GetName() { return _shaderName; }

		inline SHADER_TYPE GetType() { return _shaderType; }

		// �� ���̴� ���Ͽ� �ʿ��� ������Ʈ ���۸� �߰��մϴ�.
		template <typename TConstantBuffer>
		void CreateConstantBuffer(CBV_REGISTER slot)
		{
			std::shared_ptr<ConstantBuffer> cb = std::make_shared<ConstantBuffer>();

			cb->_slot = slot;

			std::shared_ptr<TConstantBuffer> temp = std::make_shared<TConstantBuffer>();

			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth = sizeof(TConstantBuffer);
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = NULL;
			cbDesc.StructureByteStride = NULL;

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = temp.get();
			initData.SysMemPitch = NULL;
			initData.SysMemSlicePitch = NULL;

			Renderer::Get()->GetDevice()->CreateBuffer(&cbDesc, &initData, cb->_cbGPU.GetAddressOf());

			temp.reset();

			_constantBuffers.push_back(cb);
		}

		// �� ���̴� ���Ͽ� �ִ� ������Ʈ ������ ������ ������Ʈ�մϴ�.
		template <typename TConstantBuffer>
		void UpdateConstantBuffer(TConstantBuffer* srcbuffer, CBV_REGISTER slot)
		{
			// 0�� �������ʹ� Per-Frame ���۷� ���� => ���� ���̴����� ���������� �ʴ´�.
			std::shared_ptr<ConstantBuffer> cb = _constantBuffers[static_cast<uint32>(slot) - 1];

			D3D11_MAPPED_SUBRESOURCE mappedResource;

			Renderer::Get()->GetDeviceContext()->Map(cb->_cbGPU.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			TConstantBuffer* dataPtr = (TConstantBuffer*)mappedResource.pData;

			memcpy(dataPtr, srcbuffer, sizeof(TConstantBuffer));

			Renderer::Get()->GetDeviceContext()->Unmap(cb->_cbGPU.Get(), 0);
		}

		// ���̴� ���ҽ��� ������ �� �ֵ��� ������ �����մϴ�. (���� ���)
		void CreateShaderResource(SRV_REGISTER slot)
		{
			std::shared_ptr<ShaderResource> newSR = std::make_shared<ShaderResource>();

			newSR->_slot = slot;

			_shaderResources.push_back(newSR);
		}

		// �� ���̴� �ڵ忡�� ����� ���̴� ���ҽ� �並 �ش� ���Կ� ������Ʈ�� �� �ֵ��� �����մϴ�.
		void UpdateShaderResource(ComPtr<ID3D11ShaderResourceView> shaderResourceView, SRV_REGISTER slot)
		{
			// ���� ������ �༮�� ������Ʈ�մϴ�.
			for (auto& sr : _shaderResources)
				if (static_cast<uint32>(sr->_slot) == static_cast<uint32>(slot))
					sr->_srv = shaderResourceView;
		}

		void CreateSamplerState(SAM_REGISTER slot)
		{
			std::shared_ptr<Sampler> newSS = std::make_shared<Sampler>();

			newSS->_slot = slot;

			_samplerStates.push_back(newSS);
		}

		// �ٵ� �̰� Ư���� ��� �ƴϸ� ���̴� �ε� �� �����ϴ� �� �ܿ��� ����� ���� ������
		void UpdateSamplerState(ComPtr<ID3D11SamplerState> samplerState, SAM_REGISTER slot)
		{
			for (auto& sam: _samplerStates)
				if (static_cast<uint32>(sam->_slot) == static_cast<uint32>(slot))
					sam->_samplerState = samplerState;
		}

	protected:
		std::vector<std::shared_ptr<ConstantBuffer>> _constantBuffers;

		std::vector<std::shared_ptr<ShaderResource>> _shaderResources;

		std::vector<std::shared_ptr<Sampler>> _samplerStates;

		SHADER_TYPE _shaderType;

		tstring _shaderName;
	};

	template <SHADER_TYPE ShaderType>
	void GraphicsEngine::ShaderBase<ShaderType>::Release()
	{
		for (auto& constantBuffer : _constantBuffers)
		{
			constantBuffer->_cbGPU.Reset();
		}
		
		for (auto& shaderResource : _shaderResources)
		{
			shaderResource->_srv.Reset();
		}

		for (auto& samplerState : _samplerStates)
		{
			samplerState->_samplerState.Reset();
		}
	}
}