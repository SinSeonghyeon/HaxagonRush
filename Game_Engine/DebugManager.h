#pragma once

#include "QuadTree.h"

namespace Muscle
{

	enum class DEBUG_OPTION
	{
		NONE = 0,
		ON_BOUNDING_VOLUME = 1,
		ON_COLLIDER = 2,
		ON_QUAD = 4,
		ON_ALL = ON_BOUNDING_VOLUME | ON_COLLIDER | ON_QUAD

		///	���� 2�� n�� �÷� �ö󰡾��Ѵ�.
		/// ��Ʈ������ &�� ����ؼ� ��󳻱� ����!
	};

	class SphereCollider;

	class BoxCollider;

	class CapsuleCollider;

	class GraphicsManager;
	
	constexpr uint32 MAX_DEBUG_REDERNING_DATA = 1000;

	constexpr uint32 DEBUG_OPTION_COUNT = 3;

	class DebugManager final
	{
	public:
		DebugManager() = default;

		~DebugManager();

		DebugManager(const DebugManager& other) = delete;

		DebugManager& operator= (const DebugManager& other) = delete;

		DebugManager(DebugManager&& other) = delete;

		DEBUG_OPTION _debugOption;

	private:
		std::shared_ptr<GraphicsManager> _graphicsManager; // ĳ��
			
		std::shared_ptr<PerFrameData> _perframeData; // ���������� ĳ���ϰ� ����� ��.

		std::queue<std::shared_ptr<RenderingData_3D>> _culledRenderQueue; // ������

		std::vector<std::shared_ptr<RenderingData_3D>> _debugRenderingDataPools; // �̸� �����س��� ��������.

		std::vector<std::function<void(DebugManager&)>> _workList; // �Լ� ������.

		uint32 _dataIndex;

		void DrawBoundingVolumes();

		void DrawColliders();

		void DrawQuadTree();

		void DrawQuadTreeNode(std::shared_ptr<QuadTree::Node> node);

		void DrawSphereCollider(std::shared_ptr<SphereCollider> sphereCollider);

		void DrawBoxCollider(std::shared_ptr<BoxCollider> boxCollider);

		void DrawCapsuleCollider(std::shared_ptr<CapsuleCollider> CapsuleCollider);

	public:
		void Initialize();

		void Render();

		void Release();

	public:
		void PostPerFrameData(std::shared_ptr<::PerFrameData>& perframeData); // ĳ�� �ϴ� �뵵!

		void PostCulledRenderingData(std::shared_ptr<RenderingData_3D>& culledRenderingData); // �� �Լ��� ����� �׸� �༮�� ������.
	};
}


