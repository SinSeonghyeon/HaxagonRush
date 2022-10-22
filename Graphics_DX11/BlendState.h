#pragma once

namespace GraphicsEngine
{
	class BlendState
	{
	public:
		void Initialize(ComPtr<ID3D11Device> device);

		void Release();

		void SetBlendStateOff(ComPtr<ID3D11DeviceContext> deviceContext);

		void SetSrcDestAlphaBlendState(ComPtr<ID3D11DeviceContext> deviceContext);

		void SetSrcAdditiveBlendState(ComPtr<ID3D11DeviceContext> deviceContext);
		
		void SetUIBlendState(ComPtr<ID3D11DeviceContext> deviceContext);

	public:
		// ������ ���Ŀ� �´� ���� ȥ��
		ComPtr<ID3D11BlendState> _srcDestAlphaBlendState;

		// Dest�� Source�� �ش� Alpha�� �°� ĥ�ϴ� ����ȥ��
		ComPtr<ID3D11BlendState> _srcAdditiveBlendState;

		// UI Blend State
		ComPtr<ID3D11BlendState> _uiBlendState;
	};
}