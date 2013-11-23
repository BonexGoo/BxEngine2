#pragma once
#include <BxCore.hpp>
#include <BxMemory.hpp>

//! \brief ����ڸ���� ���� �ǽð��ε�����
template<autobuffer AUTOBUFFER_ID>
class BxAutoBuffer
{
private:
	uhuge LastTimeByUse;
	int LockCountByUse;
	bool IsAutoRelease;
	void* Buffer;

private:
	class List
	{
	public:
		BxAutoBuffer* Data;
		int Size;
		List* Next;
		List(BxAutoBuffer* data, int size)
		{
			Data = data;
			Size = size;
			Next = nullptr;
		}
		~List()
		{
			BxDelete(Next);
		}
	};
	global_func inline List* BufferPoolFirst()
	{
		global_data List PoolList(nullptr, 0);
		return &PoolList;
	}
	global_func inline int& BufferPoolUsed()
	{
		global_data int PoolUsed = 0;
		return PoolUsed;
	}

protected:
	global_func inline int& BufferPoolSize()
	{
		global_data int PoolSize = -1;
		return PoolSize;
	}

protected:
	BxAutoBuffer()
	{
		LastTimeByUse = 0;
		LockCountByUse = 0;
		IsAutoRelease = true;
		Buffer = nullptr;
	}
	~BxAutoBuffer()
	{
		if(Buffer) Free();
	}
	void* Alloc(int size, bool isAutoRelease, bool doLock)
	{
		if(doLock) ++LockCountByUse;
		while(SecureBuffer(size));
		BxAssert("BxAutoBuffer<�̹� Buffer�� �����մϴ�>", !Buffer);
		LastTimeByUse = BxCore::System::GetTimeMilliSecond();
		// Ǯ�� �߰�
		BufferPoolUsed() += size;
		List* AddPool = BxNew_Param(List, this, size);
		AddPool->Next = BufferPoolFirst()->Next;
		BufferPoolFirst()->Next = AddPool;
		// ����Ȯ��
		Buffer = BxAlloc(size);
		return Buffer;
	}
	void Free()
	{
		BxAssert("BxAutoBuffer<������ Buffer�� �����ϴ�>", Buffer);
		BxAssert("BxAutoBuffer<Buffer�� Lock�����Դϴ�>", LockCountByUse == 0);
		LastTimeByUse = 0;
		// Ǯ�˻�
		List* FindPool = BufferPoolFirst();
		while(FindPool->Next->Data != this)
			FindPool = FindPool->Next;
		// Ǯ���� ����
		BufferPoolUsed() -= FindPool->Next->Size;
		List* SubPool = FindPool->Next;
		FindPool->Next = SubPool->Next;
		SubPool->Next = nullptr;
		BxDelete(SubPool);
		// ��������
		BxFree(Buffer);
		Buffer = nullptr;
	}
	inline void* GetBuffer()
	{
		if(Buffer)
			LastTimeByUse = BxCore::System::GetTimeMilliSecond();
		return Buffer;
	}
	inline void Lock()
	{
		BxAssert("BxAutoBuffer<Lock��ų Buffer�� �����ϴ�>", Buffer);
		++LockCountByUse;
	}
	inline void Unlock()
	{
		BxAssert("BxAutoBuffer<Unlock��ų Buffer�� �����ϴ�>", Buffer);
		BxAssert("BxAutoBuffer<Lock�� ���� �Ѿ Unlock�Դϴ�>", 0 < LockCountByUse);
		--LockCountByUse;
	}
	inline void SetAutoRelease(bool isAutoRelease)
	{
		IsAutoRelease = isAutoRelease;
	}

private:
	global_func bool SecureBuffer(int size)
	{
		if(BufferPoolSize() == -1)
			return false;
		if(BufferPoolSize() < BufferPoolUsed() + size)
		{
			List* SubPool = nullptr;
			uhuge LastTimeMax = 0;
			List* FindPool = BufferPoolFirst();
			while(FindPool = FindPool->Next)
			{
				if(0 < FindPool->Data->LockCountByUse || !FindPool->Data->IsAutoRelease || !FindPool->Data->Buffer)
					continue;
				if(!SubPool || FindPool->Data->LastTimeByUse < LastTimeMax)
				{
					SubPool = FindPool;
					LastTimeMax = FindPool->Data->LastTimeByUse;
				}
			}
			BxAssert("BxAutoBuffer<�� Buffer�� ������ �Ҵ��� �� �����ϴ�>", SubPool);
			SubPool->Data->Free();
			return true;
		}
		return false;
	}
};
