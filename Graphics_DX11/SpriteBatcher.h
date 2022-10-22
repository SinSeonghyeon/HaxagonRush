#pragma once

namespace GraphicsEngine
{
	class SpriteBatcher final
	{
	public:
		SpriteBatcher();

		~SpriteBatcher();

		void Initialize(ComPtr<ID3D11DeviceContext> deviceContext);

		void Release();

		// Batch�ϱ� ���� Sprite Draw�� �����Ѵ�.
		void Begin(SpriteSortMode sorteMode = SpriteSortMode_Deferred,
			_In_opt_ ID3D11BlendState* blendState = nullptr,
			_In_opt_ ID3D11SamplerState* samplerState = nullptr,
			_In_opt_ ID3D11DepthStencilState* depthStencilState = nullptr,
			_In_opt_ ID3D11RasterizerState* rasterizerState = nullptr,
			_In_opt_ std::function<void __cdecl()> setCustomShaders = nullptr,
			Matrix transformMatrix = Matrix::Identity);

		// �����Ѵ�. => ���� ���� Ÿ�ٿ� �ϰ� �����Ѵ�.
		void End();

		inline std::shared_ptr<SpriteBatch> GetSpriteBatch() { return _spriteBatch; }

	private:
		std::shared_ptr<SpriteBatch> _spriteBatch;
	};
}