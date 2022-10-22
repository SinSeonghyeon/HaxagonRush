#include "pch.h"
#include "SwapChain.h"

#include "Renderer.h"

namespace GraphicsEngine
{
	void SwapChain::Initialize(WindowInfo& info, std::shared_ptr<Device> device, int screenWidth, int screenHeight)
	{
		_backBuffer = std::make_shared<RenderTarget>();
		_depthStencilBuffer = std::make_shared<DepthStencilBuffer>();

		ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;

		HRESULT hr = device->GetDXGI().As(&dxgiFactory2);

		// Direct 11.1�� �����Ѵ�.
		if (dxgiFactory2 != nullptr)
		{
			// ����ü�� Descriptor ����ü�� ������ �°� �����ϰ� ���� ü���� �����Ѵ�.
			DXGI_SWAP_CHAIN_DESC1 _sd;
			ZeroMemory(&_sd, sizeof(_sd));
			_sd.BufferCount = 1;
			_sd.Width = info._screenWidth;
			_sd.Height = info._screenHeight;
			_sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			// 4X MSAA ���� ���ο� ���� ����
			if (false)
			{
				_sd.SampleDesc.Count = 4;
				_sd.SampleDesc.Quality = info._4xMSAAQuality - 1;
			}
			else
			{
				_sd.SampleDesc.Count = 1;
				_sd.SampleDesc.Quality = 0;
			}

			_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			_sd.Flags = 0;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC _fd;
			_fd.RefreshRate.Numerator = 60;
			_fd.RefreshRate.Denominator = 1;
			_fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			_fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			_fd.Windowed = TRUE;

			// ������ ���� ü�� Descriptor�� �����Ͽ� ������ â�� ���� ���� ü���� �����Ѵ�.
			HR(dxgiFactory2->CreateSwapChainForHwnd(device->GetDevice().Get(), info._hWnd, &_sd, &_fd, nullptr, _swapChain1.GetAddressOf()));

			HR(_swapChain1.As(&_swapChain));
		}
		// Direct3D '11.0' �� �����Ѵ�.
		else
		{
			// ����ü�� Descriptor ����ü�� ������ �°� �����ϰ� ���� ü���� �����Ѵ�.
			DXGI_SWAP_CHAIN_DESC _sd;
			ZeroMemory(&_sd, sizeof(_sd));
			_sd.BufferCount = 1;
			_sd.BufferDesc.Width = info._screenWidth;
			_sd.BufferDesc.Height = info._screenHeight;
			_sd.BufferDesc.RefreshRate.Numerator = 60;
			_sd.BufferDesc.RefreshRate.Denominator = 1;
			_sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			// 4X MSAA ���� ���ο� ���� ����
			if (false)
			{
				_sd.SampleDesc.Count = 4;
				_sd.SampleDesc.Quality = info._4xMSAAQuality - 1;
			}
			else
			{
				_sd.SampleDesc.Count = 1;
				_sd.SampleDesc.Quality = 0;
			}

			_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			_sd.OutputWindow = info._hWnd;
			_sd.Windowed = TRUE;
			_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			_sd.Flags = 0;

			HR(device->GetDXGI()->CreateSwapChain(device->GetDevice().Get(), &_sd, _swapChain.GetAddressOf()));
		}

		dxgiFactory2.ReleaseAndGetAddressOf();

		_depthStencilBuffer->Initialize(device->GetDevice(), screenWidth, screenHeight);

		OnResize(device->GetDevice(), device->GetDeviceContext(), screenWidth, screenHeight);
	}

	void SwapChain::BeginRenderToBackBuffer(ComPtr<ID3D11DeviceContext> deviceContext)
	{
		assert(deviceContext);
		assert(_swapChain);

		_backBuffer->ClearRenderTargetView(deviceContext);

		_depthStencilBuffer->ClearDepthStencilView(deviceContext);

		deviceContext->OMSetRenderTargets(1, _backBuffer->GetRTV().GetAddressOf(), _depthStencilBuffer->GetDSV().Get());
	}

	void SwapChain::Release()
	{
		_swapChain.ReleaseAndGetAddressOf();

		_swapChain1.ReleaseAndGetAddressOf();

		_backBuffer->Release();
		
		_depthStencilBuffer->Release();
	}

	void SwapChain::OnResize(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext, int screenWidth, int screenHeight)
	{
		assert(device);
		assert(deviceContext);
		assert(_swapChain);

		// �� ���ۿ� ���õ� ���ҽ� ��ü
		_backBuffer->Release();

		// Only Depth Stencil Buffer & View Initialize
		_backBuffer->Initialize(device, screenWidth, screenHeight);

		ComPtr<ID3D11Texture2D> backBuffer;

		HR(_swapChain->ResizeBuffers(1, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		HR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));

		ComPtr<ID3D11RenderTargetView> _rtv;

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, _rtv.GetAddressOf());

		_backBuffer->GetRTV() = _rtv;

		// ����� ��ü �̸��� �����մϴ�.
		D3D11SetDebugObjectName(backBuffer.Get(), "BackBuffer[0]");

		backBuffer.Reset();

		_rtv.Reset();

		_depthStencilBuffer->Release();

		_depthStencilBuffer->Initialize(device, screenWidth, screenHeight);

		// ����� ��ü �̸��� �����մϴ�.
		D3D11SetDebugObjectName(_depthStencilBuffer->GetDepthStencilBuffer().Get(), "BackBuffer_DepthStencilBuffer");
		D3D11SetDebugObjectName(_depthStencilBuffer->GetDSV().Get(), "BackBuffer_DepthStencilView");
		D3D11SetDebugObjectName(_backBuffer->GetRTV().Get(), "BackBuffer_RTV");
	}

	void SwapChain::Present()
	{
		HR(_swapChain->Present(0, 0));
	}
}