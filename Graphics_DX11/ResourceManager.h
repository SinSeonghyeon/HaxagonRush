#pragma once
#include "Resource_def.h"
#include "Mesh.h"
#include "Texture.h"

namespace GraphicsEngine
{
	class Texture;

	class Mesh;
	
	class Font;

	// 2D Draw Quad
	constexpr uint64 STATIC_QUAD_MESH = 0;

	constexpr uint64 STATIC_DEBUG_MESH = 1;

	constexpr uint64 DYNAMIC_QUAD_MESH = 2;
	

	struct IBLSource
	{
		void Initialize(std::shared_ptr<Texture> Irradiance, std::shared_ptr<Texture> SpecularPre, std::shared_ptr<Texture> brdfLUT)
		{
			_iblSources.push_back(Irradiance);

			_iblSources.push_back(SpecularPre);

			_iblSources.push_back(brdfLUT);
		}

		void Release()
		{
			for (auto& source : _iblSources)
				source->Release();
		}

		std::vector<std::shared_ptr<Texture>> _iblSources;
	};

	class ResourceManager
	{
	public:
		ResourceManager() = default;

		~ResourceManager() = default;

		ResourceManager(const ResourceManager& other) = delete;

		ResourceManager& operator= (const ResourceManager& other) = delete;

		ResourceManager(ResourceManager&& other) = delete;

	private:
		static std::shared_ptr<ResourceManager> _resourceManager;

	public:
		static std::shared_ptr<ResourceManager> Get();

	public:
		void Initialize();

		void Update();

		void Release();

	private:
		std::unordered_map<uint64, std::shared_ptr<Texture>> _textures;

		std::unordered_map<uint64, std::shared_ptr<Mesh>> _meshes;

		std::unordered_map<uint64, std::shared_ptr<Font>> _fonts;

	public:
		// TEST : Create Irradiance, Pre-Filtered Specular, Specular BRDF LUT �ڵ� �и��� ���ҽ� �Ŵ����� �Űܳ����� ���� �� ����.
		// Key : Environment Map ID => Value : Irradiance, Specular Pre-Filtered, Specular BRDF LUT
		std::map<uint64, std::shared_ptr<IBLSource>> _IBLSourceForEnvMap;

	public:
		const uint64& LoadTexture(const tstring& textureName, const tstring& path);

		// IBL Source�� ����� Map�� ���� ���� _IBLSourceForEnvMap�� �����Ѵ�.
		void CreateIBLSource(const uint64& envMapID, bool fullBuild = false, const uint64& HDRIMapID = ULLONG_MAX);

		void RegisterTexture(std::shared_ptr<Texture> texture);

		std::shared_ptr<Texture> GetTexture(const uint64& resourceID);

		template <typename TVertex = StaticMeshVertex>
		const uint64& LoadMesh(const tstring& meshName, const std::vector<TVertex>& vertexVector, const std::vector<uint32>& indexVector, 
			bool isDynamic = false, bool isLineList = false);

		std::shared_ptr<Mesh> GetMesh(const uint64& resourceID);

		const uint64& LoadFont(const tstring& fontName, const tstring& path);

		std::shared_ptr<Font> GetFont(const uint64& resourceID);

		const uint64 GetIDByResourceName(const tstring& name);

		void ResetTexture();
	private:
		// ---------------- �⺻���� �������� ���� �ʿ��� �޽� ���� ----------------
		void CreateBasicMeshes();

		// ȭ�� ũ�⸦ ���� �޽�
		void CreateStaticQuadMesh();

		// ����� �г� ��¿�
		void CreateStaticDebugMesh();

		void CreateDynamicQuadMesh();
	};

	template <typename TVertex /*= StaticMeshVertex*/>
	const uint64& GraphicsEngine::ResourceManager::LoadMesh(const tstring& meshName, const std::vector<TVertex>& vertexVector,
		const std::vector<uint32>& indexVector, bool isDynamic /*= false*/, bool isLineList)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		if (!isLineList)
			mesh->Initialize<TVertex>(meshName, vertexVector, indexVector, isDynamic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		else
			mesh->Initialize<TVertex>(meshName, vertexVector, indexVector, isDynamic, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		_meshes.insert({ mesh->GetID(), mesh });

		return mesh->GetID();
	}
}