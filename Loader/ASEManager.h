#pragma once

#include "ASEReader.h"
#include "ASEParsingData.h"

namespace DataLoader
{
	/// <summary>
	/// �ϴ� �Ľ��ϰ� �ϼ��� (���� �������� �迭, �ִϸ��̼� ������ �� ..) �� �ѱ�� �ͺ��� ��������.
	/// </summary>
	class ASEManager
	{
	public:
		ASEManager() = default;

		~ASEManager() = default;

		ASEManager(const ASEManager& other) = delete;

		ASEManager& operator= (const ASEManager& other) = delete;

		ASEManager(ASEManager&& other) = delete;
		
	public:
		void Initialize();

		void Release();

		ASEParsingData Parse(const tstring& path);

	private:
		// ---------- Scene ----------
		void ParseScene();
		// ---------- Scene ----------


		// ---------- Material ----------
		void ParseMaterialList();

		void ParsingMaterial();

		void ParsingMaterialMapDiffuse(ASEParsingData::MaterialList::Material& parsingMat);

		void ParsingMaterialMapNormal(ASEParsingData::MaterialList::Material& parsingMat);
		// ---------- Material ----------


		// ---------- Object ----------
		void ParseGeomObject();

		void ParseMesh(ASEParsingData::GeoObject& parsingGeo);

		void ParseMeshVertexList(ASEParsingData::Mesh& parsingMesh);

		void ParseMeshFaceList(ASEParsingData::Mesh& parsingMesh);

		void ParseMeshNormals(ASEParsingData::Mesh& parsingMesh);

		void ParseMeshTVertexList(ASEParsingData::Mesh& parsingMesh);

		void ParseMeshTFaceList(ASEParsingData::Mesh& parsingMesh);

		ASEParsingData::TMAnimation ParseTMAnimation();

		void ParseTMAnimationControlPosTrack(ASEParsingData::TMAnimation& parsingAnim);

		void ParseTMAnimationControlRotTrack(ASEParsingData::TMAnimation& parsingAnim);

		ASEParsingData::NodeTM ParseNodeTM();

		void ParseBoneList(ASEParsingData::GeoObject& parsingGeo);

		void ParseWVertexs(ASEParsingData::GeoObject& parsingGeo);

		void ParseWVertexsMeshWeight(ASEParsingData::WVertex& wVertex);

		void ParseHelperObject();

		void ParseShapeObject();
		// ---------- Object ----------

	private:
		// ASE ������ �Ľ� �� ������� ��� ����ü�̴�.
		ASEParsingData _aseParsingData;

		// ASE ������ ������ �о��ش�. �� �̰� Lexer ..? ���� �༮���� �ٲ�� �� ..
		ASEReader _aseReader;

	private:
		static std::shared_ptr<ASEManager> _aseManager;

	public:
		static std::shared_ptr<ASEManager> Get();
	};
}