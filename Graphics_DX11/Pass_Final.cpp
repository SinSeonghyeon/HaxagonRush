#include "pch.h"
#include "Pass_Final.h"

#include "Pass_Debug.h"

#include "CB_Structure.h"

#include "Renderer.h"
#include "ShaderManager.h"
#include "ResourceManager.h"

#include "ViewPort.h"
#include "Mesh.h"
#include "Font.h"
#include "VertexShader.h"
#include "PixelShader.h"

namespace GraphicsEngine
{
	void Pass_Final::Initialize(const uint32& screenWidth, const uint32& screenHeight)
	{
		__super::Initialize(screenWidth, screenHeight);

		_cbColor = std::make_shared<CB_COLOR>();

		_cbToneMapping = std::make_shared<CB_TONEMAPPING>();

		_cbToneMapping->_exposure = 1.5f;

		_screenViewPort = std::make_shared<ViewPort>();

		_screenViewPort->Initialize(Vector2::Zero, screenWidth, screenHeight);

		_vsQuad = ShaderManager::Get()->GetVS(TEXT("VS_Quad"));

		_psSample = ShaderManager::Get()->GetPS(TEXT("PS_Sample"));

		_psSampleColor = ShaderManager::Get()->GetPS(TEXT("PS_Sample_Color"));

		_psFinalHDRReinhard = ShaderManager::Get()->GetPS(TEXT("PS_ToneMapping_Reinhard"));

		_psFinalHDRFilmic = ShaderManager::Get()->GetPS(TEXT("PS_ToneMapping_Filmic"));

		_meshScreen = ResourceManager::Get()->GetMesh(STATIC_QUAD_MESH);

		_meshDynamicQuad = ResourceManager::Get()->GetMesh(DYNAMIC_QUAD_MESH);

		_swapChain = Renderer::Get()->GetSwapChain();

		_depthStencilState = Renderer::Get()->GetDepthStencilState();

		_blendState = Renderer::Get()->GetBlendState();

		_spriteBatcher = Renderer::Get()->GetSpriteBatcher();

		// Final Pass, �� ���ۿ� �״�� ��� �༮���� ����ϴ� �н����� ���⿡ ���ӵ�.
		_debugPass = std::make_shared<Pass_Debug>();

		_debugPass->Initialize(screenWidth, screenHeight);
	}

	void Pass_Final::OnResize(const uint32& screenWidth, const uint32& screenHeight)
	{
		__super::OnResize(screenWidth, screenHeight);

		_screenViewPort->OnResize(screenWidth, screenHeight);

		_swapChain->OnResize(_device, _deviceContext, screenWidth, screenHeight);

		// Pass OnResize
		_debugPass->OnResize(screenWidth, screenHeight);
	}

	void Pass_Final::Release()
	{
		_screenViewPort->Release();

		_vsQuad.reset();

		_psSample.reset();

		_meshScreen.reset();

		_meshDynamicQuad.reset();

		_swapChain.reset();

		_depthStencilState.reset();

		_blendState.reset();

		_spriteBatcher.reset();

		_cbToneMapping.reset();

		_cbColor.reset();

		// Pass Release
		_debugPass->Release();

		__super::Release();
	}

	void Pass_Final::Render(std::shared_ptr<RenderTarget> finalScreen, std::vector<std::shared_ptr<TextData>>& textDataQueue, 
		std::vector<std::shared_ptr<RenderingData_UI>>& renderQueueUI, POSTPROCESS_OPTION& postProcessingOption)
	{
		BeginRender();

		_vsQuad->Update();

		/*_psSample->UpdateShaderResource(finalScreen->GetSRV().Get(), SRV_REGISTER::t0);

		_psSample->Update();*/

		// Tone Mapping
		_psFinalHDRReinhard->UpdateConstantBuffer(_cbToneMapping.get(), CBV_REGISTER::b1);

		_psFinalHDRReinhard->UpdateShaderResource(finalScreen->GetSRV().Get(), SRV_REGISTER::t0);

		_psFinalHDRReinhard->Update();

		/*_psFinalHDRFilmic->UpdateConstantBuffer(_cbToneMapping.get(), CBV_REGISTER::b1);

		_psFinalHDRFilmic->UpdateShaderResource(finalScreen->GetSRV().Get(), SRV_REGISTER::t0);

		_psFinalHDRFilmic->Update();*/
		
		_meshScreen->Update();

		// UI �����͸� �Ѹ�. �Ѹ��� �� ��Ʈ ������ ���� ���� �� �׸��� �׷��ش�.
		ExecuteRenderQueueUI(renderQueueUI);

		// Text Data
		ExecuteTextDataQueue(textDataQueue);

		// Debug Data
		if ((static_cast<uint32>(postProcessingOption) & static_cast<uint32>(POSTPROCESS_OPTION::ON_DEBUG_PANEL))
			== static_cast<uint32>(POSTPROCESS_OPTION::ON_DEBUG_PANEL))
			_debugPass->Render();

		EndRender();
	}

	void Pass_Final::BeginRender()
	{
		_screenViewPort->SetViewPort(_deviceContext);

		_swapChain->BeginRenderToBackBuffer(_deviceContext);
	}

	void Pass_Final::ExecuteRenderQueueUI(std::vector<std::shared_ptr<RenderingData_UI>>& renderQueueUI)
	{
		// ���� ť ���Ŀ� => ��Ʈ ������ ������ ���� �ʰ� �׸��� => ���� �տ� ���δ�
		static auto SortRenderQueueUI = [](const std::shared_ptr<RenderingData_UI>& first, const std::shared_ptr<RenderingData_UI>& second)
		{
			return first->_drawInfo->_sortOrder > second->_drawInfo->_sortOrder;
		};

		// �Ѹ��� �� ť�� ���� ������ �ǽ��Ѵ�.
		// 2D �� �̹����� �������ϴ� ���̶� ������ �׸� ������ �߿��ϴ�.
		std::sort(renderQueueUI.begin(), renderQueueUI.end(), SortRenderQueueUI);

		for (auto& ui : renderQueueUI)
		{
			std::shared_ptr<DrawInfo> drawInfo = ui->_drawInfo;

			std::shared_ptr<NDCInfo> posInfo = ui->_ndcInfo;

			// ���� ���带 ����ϴ� UI��� ���� ������Ʈ�� �������ش�.
			if (ui->_drawInfo->_useAlphaBlend)
				_blendState->SetUIBlendState(_deviceContext);

			Vector2 leftTop = posInfo->_leftTop;

			Vector2 rightBottom = posInfo->_rightBottom;

			// �ش� NDC ��ǥ�� �°� Dynamic Quad�� ������Ʈ
			static std::vector<StaticMeshVertex> vertices(6);

			// �� �ϴ�
			vertices[0]._position = Vector3(leftTop.x, rightBottom.y, 0.f);
			vertices[0]._uv = Vector2(0.f, 1.f);

			// �� ���
			vertices[1]._position = Vector3(leftTop.x, leftTop.y, 0.f);
			vertices[1]._uv = Vector2(0.f, 0.f);

			// �� ���
			vertices[2]._position = Vector3(rightBottom.x, leftTop.y, 0.f);
			vertices[2]._uv = Vector2(1.f, 0.f);

			// �� �ϴ�
			vertices[3]._position = Vector3(leftTop.x, rightBottom.y, 0.f);
			vertices[3]._uv = Vector2(0.f, 1.f);

			// �� ���
			vertices[4]._position = Vector3(rightBottom.x, leftTop.y, 0.f);
			vertices[4]._uv = Vector2(1.f, 0.f);

			// �� �ϴ�
			vertices[5]._position = Vector3(rightBottom.x, rightBottom.y, 0.f);
			vertices[5]._uv = Vector2(1.f, 1.f);

			_meshDynamicQuad->ResetVertexBuffer(vertices);

			_vsQuad->Update();

			_cbColor->_color = drawInfo->_color;
			
			// ���̴� ���ҽ� ������Ʈ
			_psSampleColor->UpdateShaderResource(ResourceManager::Get()->GetTexture(drawInfo->_textureID)->GetSRV(), 
				SRV_REGISTER::t0);

			// ������Ʈ ���� ������Ʈ
			_psSampleColor->UpdateConstantBuffer(_cbColor.get(), CBV_REGISTER::b1);

			_psSampleColor->Update();

			_meshDynamicQuad->Update();

			// �� �׷����Ƿ� ���� ������Ʈ�� ���ش�. ���� Ű�°ſ��� ������尡 �� �ִ� ���̴ϱ�
			// ���带 ������� ��쿡�� �ٽ� ���ִ°ɷ� �Ѵ�.
			if (ui->_drawInfo->_useAlphaBlend)
				_blendState->SetBlendStateOff(_deviceContext);
		}
	}

	void Pass_Final::ExecuteTextDataQueue(std::vector<std::shared_ptr<TextData>>& textDataQueue)
	{
		_spriteBatcher->Begin();

		for (auto& textData : textDataQueue)
		{
			std::shared_ptr<Font> font = ResourceManager::Get()->GetFont(textData->_fontID);

			font->DrawTextColor(_spriteBatcher->GetSpriteBatch(), textData->_startCoord, textData->_color, textData->_outText);
		}

		_spriteBatcher->End();

		// SpriteBatch�� SpriteFont�� ����� �Ǹ� Blent State�� �������� ��. ���� ������ Default�� �������ش�.
		_deviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	}

	void Pass_Final::EndRender()
	{
		_depthStencilState->SetDepthEnableState(_deviceContext);

		_swapChain->Present();
	}
}