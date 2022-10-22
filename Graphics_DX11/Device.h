#pragma once
#include "WindowInfo.h"

namespace GraphicsEngine
{
	class Device
	{
	public:
		void Initialize(WindowInfo& info);

		void Release();

		ComPtr<ID3D11Device> GetDevice() { return _device; }

		ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

		ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }

		ComPtr<IDXGIAdapter> GetAdapter() { return _dxgiAdapter; }

	private:
		// COM(Component Object Model)
		// - DX�� ���α׷��� ��� �������� ���� ȣȯ���� �����ϰ� �ϴ� ���
		// - COM ��ü(COM �������̽�)�� ���. ���λ����� �츮���� ������
		// - ComPtr ������ ����Ʈ ������
		ComPtr<IDXGIFactory1>	_dxgi;				// ȭ�� ���� ��ɵ�

		ComPtr<ID3D11Device>	_device;			// ���� ��ü ����

		ComPtr<ID3D11Device1>	_device1;

		ComPtr<ID3D11DeviceContext> _deviceContext;

		ComPtr<ID3D11DeviceContext1> _deviceContext1;

		ComPtr<IDXGIAdapter>	_dxgiAdapter;
	};
}