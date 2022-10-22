#pragma once
#include "Graphics_RenderingData_3D.h"

namespace GraphicsEngine
{	
	class VertexShader;

	class PixelShader;

	class GeometryShader;

	class ComputeShader;

	/// <summary>
	/// �̰� ������ ū�� �´� .. �� ģ�� �ȿ��� ������ �۾��� �������ش�. 
	/// 
	/// ���ҽ� �Ŵ��� �ȿ��� Ȱ����.
	/// </summary>
	class ShaderManager
	{
	public:
		ShaderManager() = default;

		~ShaderManager() = default;

		ShaderManager(const ShaderManager& other) = delete;

		ShaderManager& operator= (const ShaderManager& other) = delete;

		ShaderManager(ShaderManager&& other) = delete;

	private:
		static std::shared_ptr<ShaderManager> _shaderManager;

	public:
		static std::shared_ptr<ShaderManager> Get();

	public:
		void Initialize();

		void Release();

	public:
		std::shared_ptr<VertexShader> GetVS(const tstring& name);

		std::shared_ptr<PixelShader> GetPS(const tstring& name);

		std::shared_ptr<GeometryShader> GetGS(const tstring& name);

		std::shared_ptr<ComputeShader> GetCS(const tstring& name);

	private:
		std::map<tstring, std::shared_ptr<VertexShader>> _vertexShaders;

		std::map<tstring, std::shared_ptr<PixelShader>> _pixelShaders;

		std::map<tstring, std::shared_ptr<GeometryShader>> _geometryShaders;

		std::map<tstring, std::shared_ptr<ComputeShader>> _computeShaders;
	};
}