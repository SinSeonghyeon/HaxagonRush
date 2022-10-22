#include "pch.h"
#include "ASEManager.h"

#include <filesystem>
#include "ASEToken.h"

std::shared_ptr<DataLoader::ASEManager> DataLoader::ASEManager::_aseManager = nullptr;

namespace DataLoader
{
	std::shared_ptr<DataLoader::ASEManager> ASEManager::Get()
	{
		if (_aseManager == nullptr)
			_aseManager = std::make_shared<ASEManager>();

		return _aseManager;
	}

	void DataLoader::ASEManager::Initialize()
	{
		_aseParsingData = ASEParsingData();

		_aseReader = ASEReader();
	}

	ASEParsingData DataLoader::ASEManager::Parse(const tstring& path)
	{
		_aseParsingData = ASEParsingData();

		_aseReader.Open(path);

		while (!_aseReader.IsEOF())
		{
			_aseReader.ReadNextLine();

			switch (_aseReader.ParsingReserveWord())
			{
			case RESERVEDWORD_END:
			{
				// ���� ���� �Ľ��� ���� ���
				break;
			}

			case TOKENR_3DSMAX_ASCIIEXPORT:
			{
				// ���� �߿� ������ �ƴϴϱ� �Ľ� �� ��.
				break;
			}

			case TOKENR_SCENE:
			{
				ParseScene();

				break;
			}

			case TOKENR_MATERIAL_LIST:
			{
				ParseMaterialList();

				break;
			}

			case TOKENR_GEOMOBJECT:
			{
				ParseGeomObject();

				break;
			}

			case TOKENR_HELPEROBJECT:
			{
				ParseHelperObject();

				break;
			}

			case TOKENR_SHAPEOBJECT:
			{
				ParseShapeObject();

				break;
			}
			}
		}

		_aseReader.Close();

		return _aseParsingData;
	}

	void ASEManager::ParseScene()
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_SCENE_FILENAME:
			{
				_aseParsingData.scene.fileName = _aseReader.ParsingString();
				break;
			}

			case TOKENR_SCENE_FIRSTFRAME:
			{
				_aseParsingData.scene.firstFrame = _aseReader.ParsingInt();
				break;
			}

			case TOKENR_SCENE_LASTFRAME:
			{
				_aseParsingData.scene.lastFrame = _aseReader.ParsingInt();
				break;
			}

			case TOKENR_SCENE_FRAMESPEED:
			{
				_aseParsingData.scene.frameSpeed = _aseReader.ParsingInt();
				break;
			}

			case TOKENR_SCENE_TICKSPERFRAME:
			{
				_aseParsingData.scene.ticksPerFrame = _aseReader.ParsingInt();
				break;
			}

			case TOKENR_SCENE_BACKGROUND_STATIC:
			{
				_aseParsingData.scene.backgroundStatic = _aseReader.ParsingVector3();
				break;
			}
			case TOKENR_SCENE_AMBIENT_STATIC:
			{
				_aseParsingData.scene.ambientStatic = _aseReader.ParsingVector3();
				break;
			}
			}
		}
	}

	void ASEManager::ParseMaterialList()
	{
		uint32 reservedWord;

		uint32 parsedMatCount = 0;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			// '}' ��ū�� ���������� ��� ���͸����� �Ľ��� �Ϸ�Ǿ��ٸ�.
			if (reservedWord == RESERVEDWORD_END && parsedMatCount == _aseParsingData.materialList.materialCount)
				break;

			switch (reservedWord)
			{
			case TOKENR_MATERIAL_COUNT:
			{
				_aseParsingData.materialList.materialCount = _aseReader.ParsingInt();

				break;
			}

			case TOKENR_MATERIAL:
			{
				ParsingMaterial();

				parsedMatCount++;

				break;
			}
			}
		}
	}

	void ASEManager::ParsingMaterial()
	{
		ASEParsingData::MaterialList::Material material;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_MATERIAL_AMBIENT:
			{
				material.ambient = _aseReader.ParsingVector3();
				break;
			}

			case TOKENR_MATERIAL_SPECULAR:
			{
				material.specular = _aseReader.ParsingVector3();
				break;
			}

			case TOKENR_MATERIAL_DIFFUSE:
			{
				material.diffuse = _aseReader.ParsingVector3();
				break;
			}

			case TOKENR_MAP_DIFFUSE:
			{
				ParsingMaterialMapDiffuse(material);
				break;
			}
			case TOKENR_MAP_BUMP:
			{
				ParsingMaterialMapNormal(material);
				break;
			}
			}
		}

		_aseParsingData.materialList.materialList.push_back(material);
	}

	void ASEManager::ParsingMaterialMapDiffuse(ASEParsingData::MaterialList::Material& parsingMat)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_BITMAP:
			{
				// ���ϸ� �̾���
				parsingMat.mapDiffuse.bitmap =
					std::filesystem::path(_aseReader.ParsingString()).filename().string();

				break;
			}
			}
		}
	}

	void ASEManager::ParsingMaterialMapNormal(ASEParsingData::MaterialList::Material& parsingMat)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_BITMAP:
			{
				// ���ϸ� �̾���
				parsingMat.mapNormal.bitmap =
					std::filesystem::path(_aseReader.ParsingString()).filename().string();

				break;
			}
			}
		}
	}

	void ASEManager::ParseGeomObject()
	{
		ASEParsingData::GeoObject geoObject;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_NODE_NAME:
			{
				geoObject.nodeName = _aseReader.ParsingString();

				break;
			}

			case TOKENR_NODE_PARENT:
			{
				geoObject.nodeParent = _aseReader.ParsingString();

				break;
			}

			case TOKENR_NODE_TM:
			{
				geoObject.nodeTM = ParseNodeTM();

				break;
			}

			case TOKENR_MESH:
			{
				ParseMesh(geoObject);

				break;
			}

			case TOKENR_MATERIAL_REF:
			{
				geoObject.materialRef = _aseReader.ParsingInt();

				break;
			}

			case TOKENR_TM_ANIMATION:
			{
				geoObject.tmAnimation.push_back(ParseTMAnimation());

				break;
			}

			case TOKENR_MESH_NUMBONE:
			{
				geoObject.numBone = _aseReader.ParsingInt();

				geoObject.boneList.resize(geoObject.numBone);

				break;
			}

			case TOKENR_BONE_LIST:
			{
				ParseBoneList(geoObject);

				break;
			}

			case TOKENR_MESH_WVERTEXS:
			{
				ParseWVertexs(geoObject);

				break;
			}
			}
		}

		_aseParsingData.geoObjectList.push_back(geoObject);
	}

	void ASEManager::ParseMesh(ASEParsingData::GeoObject& parsingGeo)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			switch (reservedWord)
			{
			case TOKENR_TIMEVALUE:
			{
				parsingGeo.mesh.timeValue = _aseReader.ParsingInt();
				break;
			}

			case TOKENR_MESH_NUMVERTEX:
			{
				parsingGeo.mesh.numVertex = _aseReader.ParsingInt();
				parsingGeo.mesh.vertexList.resize(parsingGeo.mesh.numVertex);
				parsingGeo.wVertexs.resize(parsingGeo.mesh.numVertex);
				break;
			}

			case TOKENR_MESH_NUMFACES:
			{
				parsingGeo.mesh.numFaces = _aseReader.ParsingInt();
				parsingGeo.mesh.faceList.resize(parsingGeo.mesh.numFaces);
				parsingGeo.mesh.faceNormals.resize(parsingGeo.mesh.numFaces);
				break;
			}

			case TOKENR_MESH_NUMTVERTEX:
			{
				parsingGeo.mesh.numTVertex = _aseReader.ParsingInt();
				parsingGeo.mesh.tVertexList.resize(parsingGeo.mesh.numTVertex);
				break;
			}

			case TOKENR_MESH_NUMTVFACES:
			{
				parsingGeo.mesh.numTVFace = _aseReader.ParsingInt();
				parsingGeo.mesh.tFaceList.resize(parsingGeo.mesh.numTVFace);
				break;
			}

			case TOKENR_MESH_VERTEX_LIST:
			{
				ParseMeshVertexList(parsingGeo.mesh);
				break;
			}

			case TOKENR_MESH_FACE_LIST:
			{
				ParseMeshFaceList(parsingGeo.mesh);
				break;
			}

			case TOKENR_MESH_NORMALS:
			{
				ParseMeshNormals(parsingGeo.mesh);
				break;
			}

			case TOKENR_MESH_TVERTLIST:
			{
				ParseMeshTVertexList(parsingGeo.mesh);
				break;
			}

			case TOKENR_MESH_TFACELIST:
			{
				ParseMeshTFaceList(parsingGeo.mesh);
				break;
			}
			}
		}
	}

	void ASEManager::ParseMeshVertexList(ASEParsingData::Mesh& parsingMesh)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			const unsigned vertexIndex = _aseReader.ParsingInt();
			const Vector3 vertexPos = _aseReader.ParsingVector3();

			parsingMesh.vertexList[vertexIndex] = { vertexIndex, vertexPos };
		}
	}

	void ASEManager::ParseMeshFaceList(ASEParsingData::Mesh& parsingMesh)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			// �޽� ���̽� ��ū�� �ƴϸ� ���� ������ �Է����� �ʴ´�.
			if (reservedWord != TOKENR_MESH_FACE)
				continue;

			const unsigned faceIndex = _aseReader.ParsingInt();
			_aseReader.TapJump();

			const unsigned _vertIndexA = _aseReader.ParsingInt();
			_aseReader.TapJump();

			const unsigned _vertIndexB = _aseReader.ParsingInt();
			_aseReader.TapJump();

			const unsigned _vertIndexC = _aseReader.ParsingInt();

			// MAX������ CW => CCW�� �ٲ�����.
			parsingMesh.faceList[faceIndex] = { faceIndex, _vertIndexA, _vertIndexC, _vertIndexB };
		}
	}

	void ASEManager::ParseMeshNormals(ASEParsingData::Mesh& parsingMesh)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			const unsigned faceIndex = _aseReader.ParsingInt();
			const Vector3 faceNormal = _aseReader.ParsingVector3();

			parsingMesh.faceNormals[faceIndex].index = faceIndex;
			parsingMesh.faceNormals[faceIndex].faceNormal = faceNormal;

			// Max CW => CCW�� ����� ���� ���� ���ô� ~
			// A Vertex
			{
				_aseReader.ReadNextLine();

				_aseReader.ParsingReserveWord();

				const unsigned vertIndex = _aseReader.ParsingInt();

				const Vector3 vertNormal = _aseReader.ParsingVector3();

				parsingMesh.faceNormals[faceIndex].vertexNormal[0] = { vertIndex, vertNormal };
			}

			// C Vertex
			{
				_aseReader.ReadNextLine();

				_aseReader.ParsingReserveWord();

				const unsigned vertIndex = _aseReader.ParsingInt();

				const Vector3 vertNormal = _aseReader.ParsingVector3();

				parsingMesh.faceNormals[faceIndex].vertexNormal[2] = { vertIndex, vertNormal };
			}

			// B Vertex
			{
				_aseReader.ReadNextLine();

				_aseReader.ParsingReserveWord();

				const unsigned vertIndex = _aseReader.ParsingInt();

				const Vector3 vertNormal = _aseReader.ParsingVector3();

				parsingMesh.faceNormals[faceIndex].vertexNormal[1] = { vertIndex, vertNormal };
			}
		}
	}

	void ASEManager::ParseMeshTVertexList(ASEParsingData::Mesh& parsingMesh)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord != TOKENR_MESH_TVERT)
				continue;

			const unsigned tvertIndex = _aseReader.ParsingInt();

			parsingMesh.tVertexList[tvertIndex].index = tvertIndex;
			parsingMesh.tVertexList[tvertIndex].u = _aseReader.ParsingFloat();
			parsingMesh.tVertexList[tvertIndex].v = _aseReader.ParsingFloat();
		}
	}

	void ASEManager::ParseMeshTFaceList(ASEParsingData::Mesh& parsingMesh)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord != TOKENR_MESH_TFACE)
				continue;

			const unsigned tFaceIndex = _aseReader.ParsingInt();

			parsingMesh.tFaceList[tFaceIndex].faceIndex = tFaceIndex;

			// �ż��� ���⸦ �����ߴ��� �ȴ�...
			// CW => CCW
			parsingMesh.tFaceList[tFaceIndex].vertexIndex[0] = _aseReader.ParsingInt();
			parsingMesh.tFaceList[tFaceIndex].vertexIndex[1] = _aseReader.ParsingInt();
			parsingMesh.tFaceList[tFaceIndex].vertexIndex[2] = _aseReader.ParsingInt();
		}
	}

	ASEParsingData::TMAnimation ASEManager::ParseTMAnimation()
	{
		ASEParsingData::TMAnimation ret;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;
			if (reservedWord == TOKENR_ANIMATION_NAME)
				ret.AnimName = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_CONTROL_POS_TRACK)
				ParseTMAnimationControlPosTrack(ret);
			else if (reservedWord == TOKENR_CONTROL_ROT_TRACK)
				ParseTMAnimationControlRotTrack(ret);
		}

		return ret;
	}

	void ASEManager::ParseTMAnimationControlPosTrack(ASEParsingData::TMAnimation& parsingAnim)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_NODE_NAME)
				parsingAnim.nodeName = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_CONTROL_POS_SAMPLE)
			{
				const unsigned tick = _aseReader.ParsingInt();

				const Vector3 tickPos = _aseReader.ParsingVector3();

				// �� �ִϸ��̼� �����Ϳ��� ù �������� ƽ���� �۰ų� ������ �������� ƽ���� ū ��� �����Ѵ�.
				//const unsigned firstFrameTick = _aseParsingData.scene.firstFrame * _aseParsingData.scene.ticksPerFrame;

				//if (tick < firstFrameTick)
				//	continue;

				//const unsigned lastFrameTick = _aseParsingData.scene.lastFrame * _aseParsingData.scene.ticksPerFrame;

				//if (tick > lastFrameTick)
				//	continue;

				// �������̶�� ��ġ�� FirstFrameTick �������� ���ϱ� ���ؼ� ���ش�.
				parsingAnim.controlPosSamples.push_back({ tick /*- firstFrameTick*/, tickPos });
			}
		}
	}

	void ASEManager::ParseTMAnimationControlRotTrack(ASEParsingData::TMAnimation& parsingAnim)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_NODE_NAME)
				parsingAnim.nodeName = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_CONTROL_ROT_SAMPLE)
			{
				const unsigned tick = _aseReader.ParsingInt();

				const Vector3 tickAxis = _aseReader.ParsingVector3();

				const float tickAngle = _aseReader.ParsingFloat();

				Quaternion quat{ Quaternion::CreateFromAxisAngle(tickAxis, tickAngle) };

				//// �� �ִϸ��̼� �����Ϳ��� ù �������� ƽ���� �۰ų� ������ �������� ƽ���� ū ��� �����Ѵ�.
				//const unsigned firstFrameTick = _aseParsingData.scene.firstFrame * _aseParsingData.scene.ticksPerFrame;

				//if (tick < firstFrameTick)
				//	continue;

				//const unsigned lastFrameTick = _aseParsingData.scene.lastFrame * _aseParsingData.scene.ticksPerFrame;

				//if (tick > lastFrameTick)
				//	continue;

				// �������̶�� ��ġ�� FirstFrameTick �������� ���ϱ� ���ؼ� ���ش�.
				parsingAnim.controlRotSamples.push_back({ tick /*- firstFrameTick*/, quat });
			}
		}
	}

	ASEParsingData::NodeTM ASEManager::ParseNodeTM()
	{
		ASEParsingData::NodeTM ret;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			// MAx �� DirectX�� ��ǥ�谡 �ٸ��� => Y, Z �ٲ��ش� => 2�� �� 3�� / 2�� �� 3�� �ٲ��ش�.
			if (reservedWord == TOKENR_NODE_NAME)
			{
				ret.nodeName = _aseReader.ParsingString();
			}
			else if (reservedWord == TOKENR_TM_ROW0)
			{
				const Vector3 value = _aseReader.ParsingVector3();

				ret.tm.m[0][0] = value.x;
				ret.tm.m[0][1] = value.y;
				ret.tm.m[0][2] = value.z;
				ret.tm.m[0][3] = 0.f;
			}
			else if (reservedWord == TOKENR_TM_ROW1)
			{
				const Vector3 value = _aseReader.ParsingVector3();

				ret.tm.m[2][0] = value.x;
				ret.tm.m[2][1] = value.y;
				ret.tm.m[2][2] = value.z;
				ret.tm.m[2][3] = 0.f;
			}
			else if (reservedWord == TOKENR_TM_ROW2)
			{
				const Vector3 value = _aseReader.ParsingVector3();

				ret.tm.m[1][0] = value.x;
				ret.tm.m[1][1] = value.y;
				ret.tm.m[1][2] = value.z;
				ret.tm.m[1][3] = 0.f;
			}
			else if (reservedWord == TOKENR_TM_ROW3)
			{
				const Vector3 value = _aseReader.ParsingVector3();

				ret.tm.m[3][0] = value.x;
				ret.tm.m[3][1] = value.y;
				ret.tm.m[3][2] = value.z;
				ret.tm.m[3][3] = 1.f;

				// �װ�Ƽ�� ������ .. ���� �ƴ��� üũ����.
				const float det = ret.tm.Determinant();

				ret.bHasNegativeScale = det < 0;

				// �װ�Ƽ�� �������̸� NodeTM �װ�Ƽ�� ������ �����ϰ� ������
				if (ret.bHasNegativeScale)
				{
					Vector3 scale;
					Quaternion rotation;
					Vector3 translation;

					ret.tm.Decompose(scale, rotation, translation);

					scale = -1 * scale;

					ret.tm = Matrix::CreateScale(scale)
						* Matrix::CreateFromQuaternion(rotation)
						* Matrix::CreateTranslation(translation);
				}
			}
		}

		return ret;
	}

	void ASEManager::ParseBoneList(ASEParsingData::GeoObject& parsingGeo)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_BONE)
			{
				const unsigned boneIndex = _aseReader.ParsingInt();

				_aseReader.ReadNextLine();

				reservedWord = _aseReader.ParsingReserveWord();

				if (reservedWord == TOKENR_BONE_NAME)
				{
					parsingGeo.boneList[boneIndex] = _aseReader.ParsingString();
				}

				// �� �̸��� �ְ� �ݴ� '}'�� ��ŵ�ϱ� ���ؼ� �� �� �� ����ݴϴ�.
				_aseReader.ReadNextLine();
			}
		}
	}

	void ASEManager::ParseWVertexs(ASEParsingData::GeoObject& parsingGeo)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_MESH_WEIGHT)
			{
				ASEParsingData::WVertex wVert;
				wVert.index = _aseReader.ParsingInt();

				ParseWVertexsMeshWeight(wVert);

				parsingGeo.wVertexs[wVert.index] = wVert;
			}
		}
	}

	void ASEManager::ParseWVertexsMeshWeight(ASEParsingData::WVertex& wVertex)
	{
		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_BONE_BLENGING_WEIGHT)
			{
				const unsigned boneIndex = _aseReader.ParsingInt();
				const float boneWeight = _aseReader.ParsingFloat();
				wVertex.boneBlendingWeight.push_back({ boneIndex, boneWeight });
			}
		}
	}

	void ASEManager::ParseHelperObject()
	{
		ASEParsingData::HelperObject helperObject;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_NODE_NAME)
				helperObject.nodeName = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_NODE_PARENT)
				helperObject.nodeParent = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_NODE_TM)
				helperObject.nodeTM = ParseNodeTM();
			else if (reservedWord == TOKENR_TM_ANIMATION)
				helperObject._tmAnimations.push_back(ParseTMAnimation());
		}

		_aseParsingData.helperObjectList.push_back(helperObject);
	}

	void ASEManager::ParseShapeObject()
	{
		ASEParsingData::ShapeObject shapeObject;

		uint32 reservedWord;

		while (true)
		{
			_aseReader.ReadNextLine();

			reservedWord = _aseReader.ParsingReserveWord();

			if (reservedWord == RESERVEDWORD_END)
				break;

			if (reservedWord == TOKENR_NODE_NAME)
				shapeObject.nodeName = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_NODE_PARENT)
				shapeObject.nodeParent = _aseReader.ParsingString();
			else if (reservedWord == TOKENR_NODE_TM)
				shapeObject.nodeTM = ParseNodeTM();
			else if (reservedWord == TOKENR_TM_ANIMATION)
				shapeObject._tmAnimations.push_back(ParseTMAnimation());
		}

		_aseParsingData.shapeObjectList.push_back(shapeObject);
	}

	void ASEManager::Release()
	{
		// ���������� �� �� ����
		_aseReader.Release();
	}
}