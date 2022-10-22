#pragma once
#include "Pass_Interface.h"

namespace GraphicsEngine
{
	class ViewPort;

	class RenderTarget;

	class DepthStencilBuffer;

	class VertexShader;

	class PixelShader;

	class Mesh;

	class Pass_Sampling;

	class Pass_GaussianBlur;

	// ������ ��ǲ�� ���� ȭ���̶�� ���� �����Ͽ� ����.
	// �ƴ϶�� ������ ��ũ�� ����� �°� OnResize �������.
	class Pass_Bloom : public Pass_Interface
	{
	public:
		void Initialize(const uint32& screenWidth, const uint32& screenHeight) override;

		void OnResize(const uint32& screenWidth, const uint32& screenHeight) override;

		void Release() override;

		void Render(std::shared_ptr<RenderTarget> targetScreen);

		inline std::shared_ptr<RenderTarget> GetOutput() { return _bloomOutput; }

	private:
		// ��� Ŀ�긦 Ȱ���ؼ� ���� �κ��� ã���ϴ�.
		void BloomSampling(std::shared_ptr<RenderTarget> targetScreen);

		void Blur16XAddUp8X();

		void Blur8XAddUp4X();

		void Blur4XAddUp2X();

		void Blur2XAddUpOrigin(std::shared_ptr<RenderTarget> targetScreen);

		void EndRender();

	private:
		std::shared_ptr<Pass_Sampling> _samplingPass;

		std::shared_ptr<Pass_GaussianBlur> _blurPass;

		std::shared_ptr<ViewPort> _screenSizeViewPort;

		std::shared_ptr<RenderTarget> _bloomSampled;

		std::shared_ptr<RenderTarget> _bloomOutput;

		std::shared_ptr<VertexShader> _vsQuad;

		std::shared_ptr<PixelShader> _psBloomSampling1;

		std::shared_ptr<PixelShader> _psBloomSampling2;

		std::shared_ptr<PixelShader> _psBloomSampling3;

		std::shared_ptr<PixelShader> _psBloomAddUp;

		std::shared_ptr<Mesh> _meshScreen;

		// �ռ��� ���ؼ� 8X, 4X, 2X, 1X RenderTarget Depth ViewPort �ʿ���
		std::shared_ptr<ViewPort> _octarViewPort;

		std::shared_ptr<RenderTarget> _octarTarget;

		std::shared_ptr<ViewPort> _quarterViewPort;

		std::shared_ptr<RenderTarget> _quarterTarget;

		std::shared_ptr<ViewPort> _halfViewPort;

		std::shared_ptr<RenderTarget> _halfTarget;

		friend class Renderer;

		friend class PostProcessor;
	};
}

// 1. Bloom Target�� �޴´�.
// 2. Bloom Target���� ������� �ؾ��� �κ��� ã�´�. (�ٿ� ���� �ϰ� �ص� �ȴ�.)
// 3. 2�� ������� ������ Down Sampling�� �����Ѵ�. (2X, 4X, 8X, 16X)
// 4. 3�� ��������� 16X�� Blur �� 8X�� �ռ��Ѵ�.
// 5. �ռ��� 8X�� ���� �� 4X�� �ռ��Ѵ�.
// 6. �ռ��� 4X�� ���� �� 2X�� �ռ��Ѵ�.
// 7. ���������� 2X�� ���ϰ�, ���� Ÿ�ٰ� �ռ��Ѵ�.
// 8. ��������� ������