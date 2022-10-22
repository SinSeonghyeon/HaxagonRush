#pragma once
#include "Resource_def.h"

namespace GraphicsEngine
{
	class ResourceIDGenerator
	{
		inline static uint64 _nextID = 0;

	public:
		// �ִ� 42�� ���� ������Ʈ�� �ٷ� �� �ִ�.
		inline static uint64 Generate() { return _nextID++; }
	};

	/// <summary>
	/// ���ҽ����� ���� ������ ���� �߻� �������̽� Ŭ����
	/// </summary>
	template <RESOURCE_TYPE ResourceType>
	interface ResourceBase
	{
	public:
		ResourceBase();

		virtual ~ResourceBase();

		virtual void Release() abstract;

		void Reset() { _refCount = max(0, _refCount--); }

	public:
		inline uint32 GetRefCount() { return _refCount; }

		inline RESOURCE_TYPE GetResourceType() { return _resourceType; }

		// ���۷��� ī��Ʈ�� 0�̵� ���� ������ �ð� => ������ �ξ Ư�� �ð���ŭ �귶�ٸ� ���ҽ� �ٿ�
		inline float GetTimeFromRefZero() { return _timeFromRefZero; }

		inline const tstring& GetName() { return _resourceName; }

		void SetName(const tstring& name) { _resourceName = name; }

		inline const uint64& GetID() { return _resourceID; }

	protected:
		RESOURCE_TYPE _resourceType;

		uint32 _refCount;

		float _timeFromRefZero;

		// Hashing, Searching � ���Ǵ� ���ҽ��� �̸�
		tstring _resourceName;

		// ���ҽ� ���� �ÿ� �߱޵Ǵ� ID
		uint64 _resourceID;
	};

	template <RESOURCE_TYPE ResourceType>
	ResourceBase<ResourceType>::ResourceBase() :
		_resourceType(ResourceType), _refCount(0), _timeFromRefZero(0.f)
	{
		_resourceID = ResourceIDGenerator::Generate();
	}

	template <RESOURCE_TYPE ResourceType>
	ResourceBase<ResourceType>::~ResourceBase()
	{
	}
}