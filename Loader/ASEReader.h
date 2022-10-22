#pragma once

namespace DataLoader
{
	constexpr uint32 BUFFER_SIZE = 512;

	constexpr uint32 RESERVEDWORD_END = 65536;

	constexpr uint32 RESERVEDWORD_COUNT = 233;

	/// <summary>
	/// ASE Data Parsing �� ������ ������� �����鼭������ �о� ���̴� ������ �Ѵ�.
	/// </summary>
	class ASEReader
	{
	public:
		ASEReader();

		~ASEReader();

	private:
		FILE* _file;

		char _lineBuffer[BUFFER_SIZE] = { 0 };

		uint32 _bufferIndex;

		char _token[BUFFER_SIZE] = { 0 };

		uint32 _tokenIndex;

	public:
		void Release();

		// ������ �����Ѵ�.
		bool Open(const tstring& path);

		// ���� �ٷ� �Ѿ��.
		void ReadNextLine();

		bool IsEOF();

		void Close();

		// ���� (��� �Ǵ� ����) ������ �ܾ �д´�.
		uint32 ParsingInt();

		// �Ҽ� (��� �Ǵ� ����) ������ �ܾ �д´�.
		float ParsingFloat();

		// ���ڿ��� �Ľ� �� "std::string" ���·� ��ȯ�Ѵ�.
		std::string ParsingString();

		// Float Vector�� �Ľ���. (y, z �ٲ�� ���� �ʾƿ�)
		Vector3 ParsingVector3();

		// ����� ��ū�� �Ľ��ϰ� �迭�� �̸� ����Ʈ�س��� �� �߿��� � ��ɾ����� ��ȯ�Ѵ�.
		uint32 ParsingReserveWord();

		// ASE ���� ������ �ʿ���� ���ڿ��� ������� ��ŵ�Ѵ�.
		void TapJump();
	};
}