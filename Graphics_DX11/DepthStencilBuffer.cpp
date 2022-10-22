#include "pch.h"
#include "DepthStencilBuffer.h"

#include "Graphics_DX11.h"

namespace GraphicsEngine
{
	void DepthStencilBuffer::Initialize(ComPtr<ID3D11Device> device, uint32 textureWidth, uint32 textureHeight, 
		bool usingMSAA, bool usingShadowMap)
	{
		_usingShadowMap = usingShadowMap;

		_usingMSAA = usingMSAA;

		if (_usingShadowMap)
		{
			InitializeForShadowMap(device, textureWidth, textureHeight);

			return;
		}

		D3D11_TEXTURE2D_DESC _depthStencilDesc;

		_depthStencilDesc.Width = textureWidth;
		_depthStencilDesc.Height = textureHeight;
		_depthStencilDesc.MipLevels = 1;
		_depthStencilDesc.ArraySize = 1;

		// ���ٽ� ����� �ϴ� ���� ������� �ʴ°ɷ� ..
		_depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// 4X MSAA�� ����Ϸ��� ? => ���� ü�ο� ���� MSAA �Ű� ������ �������ݴϴ�.
		if (_usingMSAA)
		{
			_depthStencilDesc.SampleDesc.Count = 4;

			// ����Ƽ�� ������ 0���� �ؼ� ����ȭ.
			_depthStencilDesc.SampleDesc.Quality = 0;
		}
		else
		{
			_depthStencilDesc.SampleDesc.Count = 1;
			_depthStencilDesc.SampleDesc.Quality = 0;
		}

		_depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		_depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		_depthStencilDesc.CPUAccessFlags = 0;
		_depthStencilDesc.MiscFlags = 0;

		// Depth Buffer �� Depth Stencil View ����
		HR(device->CreateTexture2D(&_depthStencilDesc, nullptr, _depthStencilBuffer.GetAddressOf()));

		// DepthStencilView�� ���� �� ����� ���� ���ٽ� �� ����ũ���̺긦 �ۼ��ϰ� �Ű������� ������.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if (_usingMSAA)
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		depthStencilViewDesc.Texture2D.MipSlice = 0;

		HR(device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilViewDesc, _depthStencilView.GetAddressOf()));
	}

	void DepthStencilBuffer::InitializeForShadowMap(ComPtr<ID3D11Device> device, uint32 textureWidth, uint32 textureHeight)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = textureWidth;
		texDesc.Height = textureHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		HR(device->CreateTexture2D(&texDesc, nullptr, _depthStencilBuffer.GetAddressOf()));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		HR(device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _depthStencilView.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		HR(device->CreateShaderResourceView(_depthStencilBuffer.Get(), &srvDesc, _shaderResourceView.GetAddressOf()));
	}

	void DepthStencilBuffer::Release()
	{
		_depthStencilBuffer.ReleaseAndGetAddressOf();

		_depthStencilView.ReleaseAndGetAddressOf();

		_shaderResourceView.ReleaseAndGetAddressOf();
	}

	void DepthStencilBuffer::OnResize(ComPtr<ID3D11Device> device, uint32 textureWidth, uint32 textureHeight)
	{
		Release();

		
		!_usingShadowMap ? Initialize(device, textureWidth, textureHeight, _usingMSAA)
			: InitializeForShadowMap(device, textureWidth, textureHeight);
	}

	void DepthStencilBuffer::ClearDepthStencilView(ComPtr<ID3D11DeviceContext> deviceContext)
	{
		deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}