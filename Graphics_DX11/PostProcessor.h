#pragma once
#include "Graphics_PerFrameData.h"

#include "Graphics_RenderingData_UI.h"
#include "Graphics_TextData.h"

namespace GraphicsEngine
{
	class RenderTarget;

	// Post Processing Pass
	class Pass_CameraMotionBlur;

	class Pass_GaussianBlur;

	class Pass_Bloom;

	class Pass_Vignetting;

	class Pass_FXAA;

	class Pass_Final;
	// Post Processing Pass

	// Source Pass
	class Pass_Lighting;

	class Pass_Deferred;
	// Source Pass

	/// <summary>
	/// ����Ʈ ���μ����� �����ϰ� �������� ����ü������ �ø��α��� ����ϴ� �༮
	/// </summary>
	class PostProcessor
	{
	public:
		PostProcessor() = default;

		~PostProcessor() = default;

		PostProcessor(const PostProcessor& other) = delete;

		PostProcessor& operator= (const PostProcessor& other) = delete;

		PostProcessor(PostProcessor&& other) = delete;

	private:
		// Post Processing Pass
		std::shared_ptr<Pass_CameraMotionBlur> _cameraMotionBlurPass;

		std::shared_ptr<Pass_GaussianBlur> _gaussianBlurPass;

		std::shared_ptr<Pass_Bloom> _bloomPass;

		std::shared_ptr<Pass_Vignetting> _vignettingPass;

		std::shared_ptr<Pass_FXAA> _fxaaPass;

		std::shared_ptr<Pass_Final> _finalPass;
		// Post Processing Pass

		// Source Pass
		std::shared_ptr<Pass_Lighting> _lightingPass;			// Source : Scene Output

		std::shared_ptr<Pass_Deferred> _deferredPass;			// Source : G-Buffers
		// Source Pass

		std::shared_ptr<RenderTarget> _prevPassOutput;

	public:
		// �������� ���� ����Ʈ ���μ��̿� ���õ� �н����� �ʱ�ȭ�ϰ� ������ �ִ´�.
		void Initialize(std::shared_ptr<Pass_Lighting> lightingPass, std::shared_ptr<Pass_Deferred> deferredPass,
			const uint32& screenWidth, const uint32& screenHeight);

		void OnResize(const uint32& screenWidth, const uint32& screenHeight);

		// ���� ����Ʈ ���μ��� �ɼǿ� ���� �����ϰ� ���� ü���� �ø����Ѵ�.
		void Render(POSTPROCESS_OPTION& postProcessingOption,
			std::vector<std::shared_ptr<::TextData>>& textDataQueue, 
			std::vector<std::shared_ptr<::RenderingData_UI>>& renderQueueUI);

		void Release();

		// ���� �������� ����� �� �� �ִ�. ������Ʈ ���̵� ���� ��, ��� �� ..

		friend class Renderer;
	};
}