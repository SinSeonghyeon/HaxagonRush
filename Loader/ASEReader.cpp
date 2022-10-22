#include "pch.h"
#include "ASEReader.h"

#include "ASEToken.h"
#include "StringHelper.h"

namespace DataLoader
{
	ASEReader::ASEReader() :
		_file(nullptr), _bufferIndex(0), _tokenIndex(0)
	{
		
	}

	ASEReader::~ASEReader()
	{
		if (_file != nullptr)
			Close();
	}

	void ASEReader::Release()
	{
		Close();
	}

	bool ASEReader::Open(const tstring& path)
	{
		fopen_s(&_file, StringHelper::ToString(path).c_str(), "r");

		_bufferIndex = 0;

		_tokenIndex = 0;

		return _file != nullptr ? true : false;
	}

	void ASEReader::ReadNextLine()
	{
		fgets(_lineBuffer, BUFFER_SIZE, _file);
	}

	bool ASEReader::IsEOF()
	{
		return feof(_file);
	}

	void ASEReader::Close()
	{
		if (_file != nullptr)
			fclose(_file);

		_file = nullptr;
	}

	uint32 ASEReader::ParsingInt()
	{
		char character = _lineBuffer[_bufferIndex++];

		// ȭ��Ʈ �����̽��� �������
		while ((character == ' ') || (character == '\t') || (character == '\n') || (character == '\r'))
		{
			character = _lineBuffer[_bufferIndex++];
		}

		_token[0] = character;

		_tokenIndex = 1;

		while ((character >= '0') && (character <= '9') || (character == '-'))
		{
			character = _lineBuffer[_bufferIndex++];

			_token[_tokenIndex++] = character;
		}

		// �׻� �������� ���ڿ��� ���� �� ����
		_token[_tokenIndex] = NULL;

		return atoi(_token);
	}

	float ASEReader::ParsingFloat()
	{
		char character = _lineBuffer[_bufferIndex++];

		// ȭ��Ʈ �����̽��� �������
		while ((character == ' ') || (character == '\t') || (character == '\n') || (character == '\r'))
		{
			character = _lineBuffer[_bufferIndex++];
		}

		_token[0] = character;

		_tokenIndex = 1;

		while ((character >= '0') && (character <= '9') || (character == '-') || (character == '.'))
		{
			character = _lineBuffer[_bufferIndex++];
			_token[_tokenIndex++] = character;
		}

		// �׻� �������� ���ڿ��� ���� �� ����
		_token[_tokenIndex] = NULL;

		return static_cast<float>(atof(_token));
	}

	std::string ASEReader::ParsingString()
	{
		char character = _lineBuffer[_bufferIndex++];

		// ȭ��Ʈ �����̽��� �������
		// ��ĭ�� �־��ְ� ������ ..
		while ((character == ' ') || (character == '\t') || (character == '\n') || (character == '\r'))
		{
			character = _lineBuffer[_bufferIndex++];
		}

		// "�� ���� �������.
		_token[0] = _lineBuffer[_bufferIndex++];

		_tokenIndex = 1;

		character = _lineBuffer[_bufferIndex++];

		// ���� ������ �������. "�� ����.
		while ((character != '"'))
		{
			_token[_tokenIndex++] = character;

			character = _lineBuffer[_bufferIndex++];
		}

		// ���ڿ��� �������� NULL����.
		_token[_tokenIndex] = NULL;

		return std::string(_token);
	}

	uint32 ASEReader::ParsingReserveWord()
	{
		// ���ڿ� �б� ���� �۾�
		_tokenIndex = 0;

		// ���ڿ� �б� ���� �۾�
		_bufferIndex = 0;

		char character = '1';

		// 1. ���� �����鼭 *�� ������ *�������� ������ ���ڸ� ������ ���� ����Ѵ�.
		while (character != '\0')
		{
			character = _lineBuffer[_bufferIndex++];

			// ȭ��Ʈ �����̽��� �����Ѵ�.
			if ((character == ' ') || (character == '\t') || (character == '\r') || (character == '\n'))
				continue;
			// '*' ã�Ҵ� ..!
			else if ((character == '}') || (character == '\0'))
			{
				return RESERVEDWORD_END;
			}
			else
			{
				_token[_tokenIndex++] = '*';
				break;
			}
		}

		//// 2. ���� �� ���� ������ �հ� ���� ������ �ƴ� ���� ���۵Ǿ��ٴ� ��. ��, ��ū�� �˻��ϸ� �ȴ� ..!
		//// ���� : �׷���, '}' �̶�� �׳� �ش� ���� ���� ����� �������� ���̹Ƿ� �������Ѵ�.
		//if ((_lineBuffer[_bufferIndex] == '}') || (character == '\0'))
		//	return RESERVEDWORD_END;

		while ((_lineBuffer[_bufferIndex] != ' ') && (_lineBuffer[_bufferIndex] != '\t'))
		{
			_token[_tokenIndex++] = _lineBuffer[_bufferIndex++];
		}

		// ���ڿ��� �� �������� �� ���� ..!
		_token[_tokenIndex] = NULL;

		unsigned int reservedIndex = 0;

		// 3. Reserved Word �˻�
		for (reservedIndex = 0; reservedIndex < RESERVEDWORD_COUNT; reservedIndex++)
		{
			if (!strcmp(_token, ReservedTokens[reservedIndex]))
				break;
		}

		return reservedIndex;
	}

	DirectX::SimpleMath::Vector3 ASEReader::ParsingVector3()
	{
		Vector3 ret;

		// y, z�� �ٲ㼭 �Ľ��Ѵ�. (��ǥ�谡 �ٸ��� �����̴�.)
		ret.x = ParsingFloat();
		ret.z = ParsingFloat();
		ret.y = ParsingFloat();

		return ret;
	}

	void ASEReader::TapJump()
	{
		char character = _lineBuffer[_bufferIndex++];

		// ȭ��Ʈ �����̽��� �������
		while ((character == ' ') || (character == '\t') || (character == '\n') || (character == '\r'))
			character = _lineBuffer[_bufferIndex++];

		_bufferIndex++;
	
		character = _lineBuffer[_bufferIndex++];

		// ���� ������ �������. "�� ����.
		while ((character != '"') && (character != ' '))
			character = _lineBuffer[_bufferIndex++];
	}
}