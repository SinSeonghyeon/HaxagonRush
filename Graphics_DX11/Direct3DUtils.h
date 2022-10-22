#pragma once

#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <d3d11_1.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>

// Direct SDK�� Windows SDK�� �������鼭 DXTrace�� �����.
// �׷��� å���� �ٸ� ������� HR ���� �޼����� ��� ����� �����Ѵ�.
#if defined(DEBUG) || defined(_DEBUG)
#ifndef HR
#define HR(x)                                            \
    {                                                        \
        HRESULT hr = (x);                                    \
        if(FAILED(hr))                                       \
        {                                                    \
            LPWSTR output;                                   \
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |       \
                FORMAT_MESSAGE_IGNORE_INSERTS      |         \
                FORMAT_MESSAGE_ALLOCATE_BUFFER,              \
                NULL,                                        \
                hr,                                          \
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   \
                (LPTSTR) &output,                            \
                0,                                           \
                NULL);                                       \
            MessageBox(NULL, output, L"Error", MB_OK);       \
        }                                                    \
    }
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

#ifndef GRAPHICS_DEBUGGER_OBJECT_NAME
#define GRAPHICS_DEBUGGER_OBJECT_NAME (1)
#endif

// COM ��ü�� ���� Release ��ũ��
#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

//
// ����� ���� ���� ���
//

// ------------------------------
// D3D11SetDebugObjectName �Լ�
// ------------------------------
// Direct3D Device�� ���� ������ ��ü�� ���� �׷��� ����ſ����� ��ü �̸��� ����
// [In]resource				D3D11 ����̽����� ������ ��ü
// [In]name					��ü �̸�
template<UINT TNameLength>
inline void D3D11SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_ const char(&name)[TNameLength])
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#else
	UNREFERENCED_PARAMETER(resource);
	UNREFERENCED_PARAMETER(name);
#endif
}

// [In]resource				D3D11 ����̽����� ������ ��ü
// [In]name					��ü �̸�
// [In]length				���ڿ� ����
inline void D3D11SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_ LPCSTR name, _In_ UINT length)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, length, name);
#else
	UNREFERENCED_PARAMETER(resource);
	UNREFERENCED_PARAMETER(name);
	UNREFERENCED_PARAMETER(length);
#endif
}

// [In]resource				D3D11 ����̽����� ������ ��ü
// [In]name					��ü �̸�
inline void D3D11SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_ const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.length(), name.c_str());
#else
	UNREFERENCED_PARAMETER(resource);
	UNREFERENCED_PARAMETER(name);
#endif
}

// �׷��� ����ſ��� Direct3D Device�� ���� ������ ��ü�� �̸��� ����� �Լ�.
// [In]resource				D3D11 ����̽����� ������ ��ü
inline void D3D11SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_ std::nullptr_t)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
#else
	UNREFERENCED_PARAMETER(resource);
#endif
}

// ------------------------------
// DXGISetDebugObjectName �Լ�
// ------------------------------
// DXGI ��ü�� ���� �׷��� ����ſ����� ��ü �̸��� �����մϴ�.
// [In]object				DXGI ��ü
// [In]name					��ü �̸�
template<UINT TNameLength>
inline void DXGISetDebugObjectName(_In_ IDXGIObject* object, _In_ const char(&name)[TNameLength])
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	object->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#else
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(name);
#endif
}

// [In]object				DXGI ��ü
// [In]name					��ü �̸�
// [In]length				���ڿ� ����
inline void DXGISetDebugObjectName(_In_ IDXGIObject* object, _In_ LPCSTR name, _In_ UINT length)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	object->SetPrivateData(WKPDID_D3DDebugObjectName, length, name);
#else
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(name);
	UNREFERENCED_PARAMETER(length);
#endif
}

// [In]object				DXGI ��ü
// [In]name					��ü �̸�
inline void DXGISetDebugObjectName(_In_ IDXGIObject* object, _In_ const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	object->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.length(), name.c_str());
#else
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(name);
#endif
}

// �׷��� ����ſ��� DXGI ��ü�� �̸��� ����� �Լ�.
// [In]object				DXGI ��ü
inline void DXGISetDebugObjectName(_In_ IDXGIObject* object, _In_ std::nullptr_t)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	object->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
#else
	UNREFERENCED_PARAMETER(object);
#endif
}

#endif