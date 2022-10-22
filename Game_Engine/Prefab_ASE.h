#pragma once
#include <memory>
#include "AnimationClip.h"
/// <summary>
/// 2022.07.07
/// �� MetaData�� �ſ� �����!
/// �̸��� �ٸ���
/// </summary>
#include "ASEParsingData.h"

namespace Muscle
{
	class GameObject;

	struct Prefab_ASE;

	struct NodeMaterial
	{
		Vector4 _ambient;
		Vector4 _diffuse;
		Vector4 _specular;
		Vector4 _reflect;

		uint64 _diffuseMapID;
		uint64 _normalMapID;
	};

	struct NodeObjectBase : public std::enable_shared_from_this<NodeObjectBase>
	{
		virtual ~NodeObjectBase();

		std::shared_ptr<GameObject> MakeGameObject(Prefab_ASE* prefab,
			std::map<std::shared_ptr<NodeObjectBase>, std::shared_ptr<GameObject>>& refGameObjectByNodeMap);

		tstring _nodeName = TEXT("");

		tstring _nodeParent = TEXT("");

		ASEParsingData::NodeTM _nodeTM;

		Matrix _localTM;

		std::vector<ASEParsingData::TMAnimation> _tmAnimations;

		std::shared_ptr<NodeObjectBase> _parentNode;

		std::vector<std::shared_ptr<NodeObjectBase>> _childNodes;

		bool _isBone;

	private:
		virtual void SettingGameObject(Prefab_ASE* prefab, std::shared_ptr<GameObject> gameObject);
	};

	struct NodeGeoObject : NodeObjectBase
	{
		~NodeGeoObject() override;

		uint64 _meshID;

		uint32 _materialRef;
		std::vector<tstring> _boneNameList;
		std::vector<ASEParsingData::WVertex> _wVertexs;

	private:
		virtual void SettingGameObject(Prefab_ASE* prefab, std::shared_ptr<GameObject> gameObject) override;
	};

	struct NodeHelperObject : NodeObjectBase
	{
	};

	struct NodeShapeObject : NodeObjectBase
	{
	};

	struct Prefab_ASE
	{
		Prefab_ASE(std::vector<std::shared_ptr<NodeObjectBase>> rootNodes,
			std::map<tstring, std::shared_ptr<NodeObjectBase>> nodeMap,
			std::vector<std::shared_ptr<NodeMaterial>> materialList)
		{
			_rootNodes = std::move(rootNodes);
			_nodeMap = std::move(nodeMap);
			_materialList = std::move(materialList);
		}

		~Prefab_ASE();

		std::shared_ptr<GameObject> MakeGameObject();

		// ���� �θ� �����ϴ� �༮��. �� ģ������ �ϳ��� ��ƼƼ�� ��� ������� ���� ������Ʈ�� �����Ѵ�.
		std::vector<std::shared_ptr<NodeObjectBase>> _rootNodes;
		std::map<tstring, std::shared_ptr<NodeObjectBase>> _nodeMap;
		std::vector<std::shared_ptr<NodeMaterial>> _materialList;

		// �� �ִϸ��̼����� �� ���� ���� �������� �Űܵ� �Ǵ� ����
		// ������ ���� ������Ʈ�� �̵�, �ǰ� ���� ���� �θ� ������Ʈ�� Ʈ�������� ������ �����̴�.
		void InitializeAnimTrackList(std::shared_ptr<NodeObjectBase> currentNode, std::vector<tstring> currentNodeHierarchy);

		std::vector<AnimationKey> _animkey;

		// ��Ÿ ����
		tstring _prefabName;
	};

}