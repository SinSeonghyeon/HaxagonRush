#pragma once
#include "Pass_Interface.h"

namespace GraphicsEngine
{
	class RenderTarget;

	class DepthStencilBuffer;

	class VertexShader;

	class PixelShader;

	class Mesh;

	class ViewPort;

	// �ٵ� �� �༮���� ��쿡�� ���� �Ǵ� ���� �ܿ��� .. ��ȭ���� �������ϴµ� ?
	// ���� �ٿ� ���ø��� ���� �ʰڽ��ϴ�.
	class Pass_CameraMotionBlur : public Pass_Interface
	{
	public:
		void Initialize(const uint32& screenWidth, const uint32& screenHeight) override;

		void OnResize(const uint32& screenWidth, const uint32& screenHeight) override;

		void Release() override;

	public:
		// Position Map, Target Map (���� ���� ������ �����)
		void Render(std::shared_ptr<RenderTarget> position, std::shared_ptr<RenderTarget> target);

		inline std::shared_ptr<RenderTarget> GetOutput() { return _blurOutput; }

	private:
		void EndRender();

		std::shared_ptr<ViewPort> _screenSizeViewPort;

		std::shared_ptr<RenderTarget> _blurOutput;

		std::shared_ptr<Mesh> _meshScreen;

		std::shared_ptr<VertexShader> _vsQuad;
		
		std::shared_ptr<PixelShader> _psCameraMotionBlur;

		friend class Renderer;
	};
}