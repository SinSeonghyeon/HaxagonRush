#include "pch.h"
#include "Device.h"

#include "Graphics_DX11.h"

namespace GraphicsEngine
{
	void Device::Initialize(WindowInfo& info)
	{
		HRESULT hr = S_FALSE;

		UINT createDeviceFlag = 0;

		// Debug�� Ȱ��ȭ�Ǿ� ������ ����׿� �÷��� �߰����ֱ� ..!
		// �̷����ϸ� ����� ��� ���忡�� ����� ������ Ȱ��ȭ�� �� �ִ�.
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		uint32 numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		uint32 numFeatureLevels = ARRAYSIZE(featureLevels);

		D3D_DRIVER_TYPE d3dDriverType;
		D3D_FEATURE_LEVEL featureLevel;

		// 1. Direct3D�� ����̽��� ����̽� ���ؽ�Ʈ�� �����Ѵ�.
		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			d3dDriverType = driverTypes[driverTypeIndex];

			// ���̷�Ʈ 11�� ImediateContext�� ����̽��� ���ÿ� �����.
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlag, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, _device.GetAddressOf(), &featureLevel, _deviceContext.GetAddressOf());

			// Direct 11.1
			HR(_device.As(&_device1));
			HR(_deviceContext.As(&_deviceContext1));

			if (hr == E_INVALIDARG)
			{
				// Direct3D 11.0 API�� D3D_FEATURE_LEVEL_11_1�� �ν����� ���ϹǷ� ��� ���� 11.0 ���ϸ� �õ��ؾ� �մϴ�.
				hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlag, &featureLevels[1], numFeatureLevels - 1,
					D3D11_SDK_VERSION, _device.GetAddressOf(), &featureLevel, _deviceContext.GetAddressOf());
			}

			if (SUCCEEDED(hr))
				break;
		}

		// ������ �����Ͽ��ٸ� �޽��� �ڽ� ����
		if (FAILED(hr))
		{
			MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);

			return;
		}

		// Direct3D 11�� Feature Level�� �������� �ʴ´ٸ� �޽��� �ڽ� ����
		if ((featureLevel != D3D_FEATURE_LEVEL_11_0) && (featureLevel != D3D_FEATURE_LEVEL_11_1))
		{
			MessageBox(0, L"Direct3D Feature Level 11 Unsupported.", 0, 0);

			return;
		}

		HR(_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &info._4xMSAAQuality));

		// 4X MSAA�� �׻� �����ǹǷ� ��ȯ�� ǰ�� ������ ���� �ݵ�� 0���� Ŀ�� �Ѵ�. ����, 0���� �۰ų� ������ assert !
		assert(info._4xMSAAQuality > 0);

		ComPtr<IDXGIDevice> dxgiDevice = nullptr;

		// Adapter �� Factory ����.
		HR(_device.As(&dxgiDevice));

		HR(dxgiDevice->GetAdapter(_dxgiAdapter.GetAddressOf()));

		HR(_dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(_dxgi.GetAddressOf())));

		// Direct3D ���� ���α׷��� �⺻ ���� ���� Alt + Enter ��ü ȭ�������� ��ȯ�� ��Ȱ��ȭ�մϴ�.
		_dxgi->MakeWindowAssociation(info._hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

		dxgiDevice.ReleaseAndGetAddressOf();
	}

	void Device::Release()
	{
		_dxgi.ReleaseAndGetAddressOf();

		_dxgiAdapter.ReleaseAndGetAddressOf();

		_device.Reset();

		_deviceContext.Reset();

		_device1.ReleaseAndGetAddressOf();

		_deviceContext1.ReleaseAndGetAddressOf();
	}
}