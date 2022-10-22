#include "pch.h"
#include "Pass_Shadow.h"

#include "ViewPort.h"

#include "ResourceManager.h"
#include "ShaderManager.h"

#include "RasterizerState.h"

#include "CB_Structure.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "Mesh.h"

#include "DepthStencilBufferArray.h"

namespace GraphicsEngine
{
	void Pass_Shadow::Initialize(const uint32& screenWidth, const uint32& screenHeight)
	{
		__super::Initialize(screenWidth, screenHeight);

		// Light View Projection Matrix �������
		_cbLightViewProjs = std::make_shared<CB_LIGHTVIEWPROJ>();

		// Per Object Info �������
		_cbPerObject = std::make_shared<CB_PEROBJECT>();

		_screenHalfViewPort = std::make_shared<ViewPort>();

		_screenHalfViewPort->Initialize(Vector2::Zero, screenWidth, screenHeight);

		_shadowDepth = std::make_shared<DepthStencilBuffer>();

		// ������ ���� ���� Depth Stencil Buffer
		_shadowDepth->Initialize(_device, screenWidth, screenHeight, false, true);

		_rasterizerState = Renderer::Get()->GetRasterizerState();

		_vsShadowStatic = ShaderManager::Get()->GetVS(TEXT("VS_Shadow"));

		_vsShadowSkinned = ShaderManager::Get()->GetVS(TEXT("VS_ShadowSkinning"));

		// TEST : Cascades Shadow Mapping
		_cbDirLightViewProj = std::make_shared<CB_DIRLIGHTVIEWPROJ>();

		_cascadeShadowMap = std::make_shared<DepthStencilBufferArray>();
		_cascadeShadowMap->Initialize(_device, screenWidth, screenHeight);

		_vsShadowCascadeStatic = ShaderManager::Get()->GetVS(TEXT("VS_CascadeShadow"));
		_vsShadowCascadeSkinned = ShaderManager::Get()->GetVS(TEXT("VS_CascadeShadowSkinning"));
		_gsShadowCascadeTriangle = ShaderManager::Get()->GetGS(TEXT("GS_CascadeShadow"));
		_gsShadowCascadeLine = ShaderManager::Get()->GetGS(TEXT("GS_CascadeShadowLine"));
	}

	void Pass_Shadow::OnResize(const uint32& screenWidth, const uint32& screenHeight)
	{
		__super::OnResize(screenWidth, screenHeight);

		_screenHalfViewPort->OnResize(screenWidth, screenHeight);

		_shadowDepth->OnResize(_device, screenWidth, screenHeight);

		// TEST : Cascades Shadow Mapping
		_cascadeShadowMap->OnResize(_device, screenWidth, screenHeight);
	}

	void Pass_Shadow::Release()
	{
		_vsShadowCascadeStatic.reset();

		_vsShadowCascadeSkinned.reset();

		_gsShadowCascadeTriangle.reset();

		_gsShadowCascadeLine.reset();

		_cbDirLightViewProj.reset();

		_cbLightViewProjs.reset();

		_cbPerObject.reset();

		_rasterizerState.reset();

		_vsShadowStatic.reset();

		_vsShadowSkinned.reset();

		_shadowDepth->Release();

		// TEST : Cascades Shadow Mapping
		_cascadeShadowMap->Release();

		__super::Release();
	}

	void Pass_Shadow::Render(std::shared_ptr<PerFrameConstantBuffer> perframeCB, std::vector<std::shared_ptr<RenderingData_3D>>& renderQueue)
	{
		BeginRender(perframeCB);

		CreateCascadeFrustums(perframeCB);

		// ���� ����Ʈ ����Ʈ�� ������ŭ �׷��� �Ѵ�.
		DrawShadowMap(renderQueue);

		// ���� �𷺼ų� ����Ʈ�� ������ŭ �׷��� �Ѵ�.
		DrawAtCascadeShadowMap(renderQueue);

		EndRender();
	}

	void Pass_Shadow::BeginRender(std::shared_ptr<PerFrameConstantBuffer> perframeCB)
	{
		// renderQueue�� ���鼭 Shadow Shader�� ����ؼ� �ϴ� Spot Light 1���� ������ ���� ������.
		for (uint32 i = 0; i < perframeCB->_cbPerFrameCPU->_spotLightCount ; i++)
		{
			// Light View Projection Matrix �����.
			SpotLightInfo spotInfo = perframeCB->_cbPerFrameCPU->_spotLightInfos[i];

			_cbLightViewProjs->_lightViewProj = Matrix::CreateLookAt(spotInfo._position, spotInfo._direction + spotInfo._position, Vector3::Up) *
				Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4, static_cast<float>(_screenWidth) / _screenHeight, 1.f, 200.f);
		}
	}

	void Pass_Shadow::DrawShadowMap(std::vector<std::shared_ptr<RenderingData_3D>>& renderQueue)
	{
		_screenHalfViewPort->SetViewPort(_deviceContext);

		_shadowDepth->ClearDepthStencilView(_deviceContext);

		// GPU�� ���� ���� ����ϴ� Doublg Speed Z ��忡�� �ξ� ���� �ӵ��� ������ �����Ѵ�.
		_deviceContext->OMSetRenderTargets(0, nullptr, _shadowDepth->GetDSV().Get());

		for (auto& renderingData : renderQueue)
		{
			// ���� ĳ�������� �ʴ� �༮�̶�� �Ѿ�� ..!
			if (!renderingData->_objectInfo->_usingShadow)
				continue;

			if (renderingData->_shaderInfo->_rasterizerState == ShaderInfo::RASTERIZER_STATE::SOLID)
				_rasterizerState->SetShadowSolidRenderState(_deviceContext);
			else if (renderingData->_shaderInfo->_rasterizerState == ShaderInfo::RASTERIZER_STATE::WIREFRAME)
				_rasterizerState->SetShadowWireFrameRenderState(_deviceContext);

			switch (renderingData->_dataType)
			{
			case RENDERINGDATA_TYPE::STATIC_MESH_OBJECT:
			{
				ExecuteShadowForStaticMeshObject(renderingData);

				break;
			}

			case RENDERINGDATA_TYPE::SKINNED_MESH_OBJECT:
			{
				ExecuteShadowForSkinnedMeshObject(renderingData);

				break;
			}

			default:
				break;
			}
		}
	}

	void Pass_Shadow::DrawAtCascadeShadowMap(std::vector<std::shared_ptr<RenderingData_3D>>& renderQueue)
	{
		_screenHalfViewPort->SetViewPort(_deviceContext);

		_cascadeShadowMap->ClearDepthStencilView(_deviceContext);

		// GPU�� ���� ���� ����ϴ� Doublg Speed Z ��忡�� �ξ� ���� �ӵ��� ������ �����Ѵ�.
		_deviceContext->OMSetRenderTargets(0, nullptr, _cascadeShadowMap->GetDSV().Get());

		// ����̽� ���ؽ�Ʈ�� ���� ���̴� ������Ʈ ���۸� ������Ʈ�ϰ� �ø���.
		_gsShadowCascadeTriangle->UpdateConstantBuffer(_cbDirLightViewProj.get(), CBV_REGISTER::b1);

		_gsShadowCascadeTriangle->Update();

		_gsShadowCascadeLine->UpdateConstantBuffer(_cbDirLightViewProj.get(), CBV_REGISTER::b1);

		for (auto& renderingData : renderQueue)
		{
			// ���� ĳ�������� �ʴ� �༮�̶�� �Ѿ�� ..!
			if (!renderingData->_objectInfo->_usingShadow)
				continue;

			if (renderingData->_shaderInfo->_rasterizerState == ShaderInfo::RASTERIZER_STATE::SOLID)
				_rasterizerState->SetShadowSolidRenderState(_deviceContext);
			else if (renderingData->_shaderInfo->_rasterizerState == ShaderInfo::RASTERIZER_STATE::WIREFRAME)
				_rasterizerState->SetShadowWireFrameRenderState(_deviceContext);

			switch (renderingData->_dataType)
			{
			case RENDERINGDATA_TYPE::STATIC_MESH_OBJECT:
			{
				ExecuteCSForStaticMeshObject(renderingData);

				break;
			}

			case RENDERINGDATA_TYPE::SKINNED_MESH_OBJECT:
			{
				ExecuteCSForSkinnedMeshObject(renderingData);

				break;
			}

			default:
				break;
			}
		}

		// ���� ���̴� ���ֱ�.
		_deviceContext->GSSetShader(nullptr, nullptr, NULL);
	}

	void Pass_Shadow::EndRender()
	{

	}

	void Pass_Shadow::ExecuteShadowForStaticMeshObject(std::shared_ptr<RenderingData_3D> renderingData)
	{
		std::shared_ptr<ObjectInfo> objectInfo = renderingData->_objectInfo;
		std::shared_ptr<GeometryInfo> geoInfo = renderingData->_geoInfo;
		std::shared_ptr<ShaderInfo> shaderInfo = renderingData->_shaderInfo;
		std::shared_ptr<MaterialInfo> materialInfo = renderingData->_materialInfo;

		CB_PEROBJECT* cbPerObject = _cbPerObject.get();

		// ������Ʈ�� ���� ����
		memcpy(cbPerObject, geoInfo.get(), sizeof(GeometryInfo));

		_vsShadowStatic->UpdateConstantBuffer(reinterpret_cast<CB_PEROBJECT*>(cbPerObject), CBV_REGISTER::b1);

		_vsShadowStatic->UpdateConstantBuffer(_cbLightViewProjs.get(), CBV_REGISTER::b2);

		_vsShadowStatic->Update();

		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		ResourceManager::Get()->GetMesh(objectInfo->_meshID)->Update();
	}

	void Pass_Shadow::ExecuteShadowForSkinnedMeshObject(std::shared_ptr<RenderingData_3D> renderingData)
	{
		std::shared_ptr<ObjectInfo> objectInfo = renderingData->_objectInfo;
		std::shared_ptr<GeometryInfo> geoInfo = renderingData->_geoInfo;
		std::shared_ptr<AnimationInfo> animInfo = renderingData->_animInfo;

		CB_PEROBJECT* cbPerObject = _cbPerObject.get();

		// ������Ʈ�� ���� ����
		memcpy(cbPerObject, geoInfo.get(), sizeof(GeometryInfo));

		// �ִϸ��̼� ���� ����
		memcpy(reinterpret_cast<char*>(cbPerObject->_boneMatrix), animInfo.get(), sizeof(AnimationInfo));

		_vsShadowSkinned->UpdateConstantBuffer(reinterpret_cast<CB_PEROBJECT*>(cbPerObject), CBV_REGISTER::b1);
		_vsShadowSkinned->UpdateConstantBuffer(_cbLightViewProjs.get(), CBV_REGISTER::b2);
		_vsShadowSkinned->Update();

		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		ResourceManager::Get()->GetMesh(objectInfo->_meshID)->Update();
	}

	void Pass_Shadow::ExecuteCSForStaticMeshObject(std::shared_ptr<RenderingData_3D> renderingData)
	{
		std::shared_ptr<ObjectInfo> objectInfo = renderingData->_objectInfo;
		std::shared_ptr<GeometryInfo> geoInfo = renderingData->_geoInfo;

		CB_PEROBJECT* cbPerObject = _cbPerObject.get();

		// ������Ʈ�� ���� ����
		memcpy(cbPerObject, geoInfo.get(), sizeof(GeometryInfo));

		_vsShadowCascadeStatic->UpdateConstantBuffer(reinterpret_cast<CB_PEROBJECT*>(cbPerObject), CBV_REGISTER::b1);

		_vsShadowCascadeStatic->Update();

		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		std::shared_ptr<Mesh> mesh = ResourceManager::Get()->GetMesh(objectInfo->_meshID);

		if (mesh->_topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			_gsShadowCascadeLine->Update();

		ResourceManager::Get()->GetMesh(objectInfo->_meshID)->Update();

		_gsShadowCascadeTriangle->Update();
	}

	void Pass_Shadow::ExecuteCSForSkinnedMeshObject(std::shared_ptr<RenderingData_3D> renderingData)
	{
		std::shared_ptr<ObjectInfo> objectInfo = renderingData->_objectInfo;
		std::shared_ptr<GeometryInfo> geoInfo = renderingData->_geoInfo;
		std::shared_ptr<AnimationInfo> animInfo = renderingData->_animInfo;

		CB_PEROBJECT* cbPerObject = _cbPerObject.get();

		// ������Ʈ�� ���� ����
		memcpy(cbPerObject, geoInfo.get(), sizeof(GeometryInfo));

		// �ִϸ��̼� ���� ����
		memcpy(reinterpret_cast<char*>(cbPerObject->_boneMatrix), animInfo.get(), sizeof(AnimationInfo));

		_vsShadowCascadeSkinned->UpdateConstantBuffer(reinterpret_cast<CB_PEROBJECT*>(cbPerObject), CBV_REGISTER::b1);
		_vsShadowCascadeSkinned->Update();

		_deviceContext->PSSetShader(nullptr, NULL, NULL);

		ResourceManager::Get()->GetMesh(objectInfo->_meshID)->Update();
	}

	void Pass_Shadow::CreateCascadeFrustums(std::shared_ptr<PerFrameConstantBuffer> perframeCB)
	{
		Matrix invProj = perframeCB->_cbPerFrameCPU->_cameraInfo._projMatrix.Invert();

		Matrix invView = perframeCB->_cbPerFrameCPU->_cameraInfo._viewMatrix.Invert();

		float nearZ = Vector3::Transform(Vector3(1.f, 1.f, 0.f), invProj).z;

		float farZ = Vector3::Transform(Vector3(1.f, 1.f, 1.f), invProj).z;

		// �� �����̽� ĿƮ����
		float cascadeEnds[FRUSTUM_CASCADE_COUNT + 1] = { nearZ,
			30.f,
			60.f,
			farZ };

		// View �����̽������� �ɽ����̵� �� Far Z�� (�̰� ���� ���� �� ȯ�濡 ���� �ٸ��� ����ϱ� ������
		// �ϵ� �ڵ��� �� ����.)
		_cbDirLightViewProj->_cascadeEnds[0] = 20.f;
		_cbDirLightViewProj->_cascadeEnds[1] = 40.f;
		_cbDirLightViewProj->_cascadeEnds[2] = farZ;

		std::vector<std::vector<Vector3>> cascadeCorners(FRUSTUM_CASCADE_COUNT, std::vector<Vector3>(8));

		// ���� �þ߰� ������ ź��Ʈ
		float tanHalfFovV = 1 / perframeCB->_cbPerFrameCPU->_cameraInfo._projMatrix.m[1][1];

		// ���� �þ߰� ������ ź��Ʈ
		float tanHalfFovH = 1 / perframeCB->_cbPerFrameCPU->_cameraInfo._projMatrix.m[0][0];

		// �� Casecade�� Directional Light���� �����ϴ� ����Ʈ �� �������� ��Ʈ������ ������.
		for (uint32 i = 0; i < FRUSTUM_CASCADE_COUNT; i++)
		{
			float nearX = cascadeEnds[i] * tanHalfFovH;
			float nearY = cascadeEnds[i] * tanHalfFovV;

			float farX = cascadeEnds[i + 1] * tanHalfFovH;
			float farY = cascadeEnds[i + 1] * tanHalfFovV;

			// View Space
			cascadeCorners[i] = 
			{
				{ nearX, nearY, cascadeEnds[i]},
				{ -nearX, nearY, cascadeEnds[i]},
				{ nearX, -nearY, cascadeEnds[i]},
				{ -nearX, -nearY, cascadeEnds[i]},
				{ farX, farY, cascadeEnds[i + 1]},
				{ -farX, farY, cascadeEnds[i + 1]},
				{ farX, -farY, cascadeEnds[i + 1]},
				{ -farX, -farY, cascadeEnds[i + 1]},
			};

			Vector3 centerPos = Vector3::Zero;

			// �ɽ����̵��� ���� �����̽������� ����ü �߽��� ���Ѵ�.
			for (uint32 j = 0; j < 8; j++)
			{
				cascadeCorners[i][j] = Vector3::Transform(cascadeCorners[i][j], invView);

				centerPos += cascadeCorners[i][j];
			}

			centerPos /= 8.0f;

			float radius = 0.f;

			for (uint32 j = 0; j < 8; j++)
			{
				float distance = (cascadeCorners[i][j] - centerPos).Length();

				radius = std::max<float>(distance, radius);
			}

			radius = std::ceil(radius * 16.f) / 16.f;

			// radius�� �̿��ؼ� �� ����ü�� ���δ� AABB �ڽ��� �����. (���� �����ڽ�)
			Vector3 maxExtents = Vector3(radius, radius, radius);

			Vector3 minExtents = -maxExtents;

			// TEST : ù ��° �� ������ ���ؼ� ����غ��� ..
			Vector3 lightDirection = perframeCB->_cbPerFrameCPU->_directionalLightInfos[0]._direction;

			lightDirection.Normalize();

			// �𷺼ų� ����Ʈ�� ��ġ�� ����� �ش�����ش�. 
			Vector3 shadowCamPos = centerPos - lightDirection * std::abs(minExtents.z);

			Vector3 cascadeExtents = maxExtents - minExtents;

			// World => Light NDC�� ��ȯ�ϴ� ����� �����.
			_cbDirLightViewProj->_lightViewProjList[i] = Matrix::CreateLookAt(shadowCamPos, centerPos, Vector3::Up) *
				Matrix::CreateOrthographic(cascadeExtents.x, cascadeExtents.y, 0.f, cascadeExtents.z);
		}
	}
}