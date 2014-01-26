#pragma once
#include <BxCore.hpp>
#include <BxMemory.hpp>

//! \brief 사용자모듈을 위한 실시간로딩도구
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
		BxASSERT("BxAutoBuffer<이미 Buffer가 존재합니다>", !Buffer);
		LastTimeByUse = BxCore::System::GetTimeMilliSecond();
		// 풀에 추가
		BufferPoolUsed() += size;
		List* AddPool = BxNew_Param(List, this, size);
		AddPool->Next = BufferPoolFirst()->Next;
		BufferPoolFirst()->Next = AddPool;
		// 버퍼확보
		Buffer = BxAlloc(size);
		return Buffer;
	}
	void Free()
	{
		BxASSERT("BxAutoBuffer<해제할 Buffer가 없습니다>", Buffer);
		BxASSERT("BxAutoBuffer<Buffer가 Lock상태입니다>", LockCountByUse == 0);
		LastTimeByUse = 0;
		// 풀검색
		List* FindPool = BufferPoolFirst();
		while(FindPool->Next->Data != this)
			FindPool = FindPool->Next;
		// 풀에서 제외
		BufferPoolUsed() -= FindPool->Next->Size;
		List* SubPool = FindPool->Next;
		FindPool->Next = SubPool->Next;
		SubPool->Next = nullptr;
		BxDelete(SubPool);
		// 버퍼해제
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
		BxASSERT("BxAutoBuffer<Lock시킬 Buffer가 없습니다>", Buffer);
		++LockCountByUse;
	}
	inline void Unlock()
	{
		BxASSERT("BxAutoBuffer<Unlock시킬 Buffer가 없습니다>", Buffer);
		BxASSERT("BxAutoBuffer<Lock의 수를 넘어선 Unlock입니다>", 0 < LockCountByUse);
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
			BxASSERT("BxAutoBuffer<새 Buffer의 공간을 할당할 수 없습니다>", SubPool);
			SubPool->Data->Free();
			return true;
		}
		return false;
	}
};
