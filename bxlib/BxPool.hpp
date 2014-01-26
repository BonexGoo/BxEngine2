#pragma once
#include <BxType.hpp>

/// @cond SECTION_NAME
namespace BxCore
{
	namespace Thread
	{
		void* BindStorage(int* storagekey);
	}
}
/// @endcond

#include <stdio.h>/////////////////////////////////////////////////////////////////////////////////

//! \brief BxPool관련 글로벌함수
namespace BxPoolGlobal
{
	class Unit
	{
	public:
		Unit* Next;
		Unit() : Next(nullptr) {}
		virtual void Quit()
		{
			if(Next)
			{
				Unit* CurUnit = nullptr;
				Unit* NextUnit = Next;
				Next = nullptr;
				while(CurUnit = NextUnit)
				{
					NextUnit = CurUnit->Next;
					CurUnit->Next = nullptr;
					delete CurUnit;
				}
			}
		}
	public:
		virtual void Free(void* Ptr) {BxASSERT("BxPoolGlobal<잘못된 호출입니다>", false);}
		virtual bool Find(void* Ptr) {BxASSERT("BxPoolGlobal<잘못된 호출입니다>", false); return false;}
	};

	class UnitLink
	{
	public:
		Unit* _ref_ Head;
		UnitLink* Next;
		UnitLink(Unit* _ref_ head) : Head(head), Next(nullptr) {}
		~UnitLink()
		{
			if(Head)
			{
				Head->Quit();
				Head = nullptr;
			}
			if(Next)
			{
				UnitLink* CurLink = nullptr;
				UnitLink* NextLink = Next;
				Next = nullptr;
				while(CurLink = NextLink)
				{
					NextLink = CurLink->Next;
					CurLink->Next = nullptr;
					delete CurLink;
				}
			}
		}
	public:
		global_func inline UnitLink* LinkHead()
		{thread_storage _ = sizeof(UnitLink); return (UnitLink*) BxCore::Thread::BindStorage(&_);}
	};

	global_func void Bind(Unit* _ref_ Head)
	{
		UnitLink* NewLink = new UnitLink(Head);
		NewLink->Next = UnitLink::LinkHead()->Next;
		UnitLink::LinkHead()->Next = NewLink;
	}

	global_func Unit* FindUnit(void* Ptr)
	{
		UnitLink* CurLink = UnitLink::LinkHead();
		while(CurLink = CurLink->Next)
		{
			Unit* CurUnit = CurLink->Head;
			while(CurUnit)
			{
				if(CurUnit->Find(Ptr))
					return CurUnit;
				CurUnit = CurUnit->Next;
			}
		}
		BxASSERT("BxPoolGlobal<해당 Ptr을 찾을 수 없습니다>", false);
		return nullptr;
	}

	global_func void UnbindAll()
	{
		UnitLink::LinkHead()->~UnitLink();
	}
}

//! \brief 사용자모듈을 위한 풀링도구
template<typename TYPE>
class BxPool
{
public:
	/*!
	\brief 템플릿단위 하나의 메모리공간 할당
	\return 할당된 메모리포인터, 초기화 없음
	\see Free
	*/
	global_func TYPE* Make()
	{
		TYPE* Ptr = UnitFocus()->Make();
		if(!Ptr)
		{
			Unit* CurUnit = UnitHead();
			while(!(Ptr = CurUnit->Make()))
			{
				if(!CurUnit->Next && !(CurUnit->Next = new Unit))
				{
					BxASSERT("BxPool<메모리가 부족합니다>", false);
					return nullptr;
				}
				CurUnit = (Unit*) CurUnit->Next;
			}
			UnitFocus(CurUnit);
		}
		return Ptr;
	}

	/*!
	\brief Make()로 생성된 메모리공간 반환
	\param Ptr : 반환시킬 메모리포인터
	\return 반환성공여부
	\see Make
	*/
	global_func bool Free(TYPE* Ptr)
	{
		BxPoolGlobal::Unit* DeleteUnit = FindUnit(Ptr);
		if(!DeleteUnit) return false;
		DeleteUnit->Free(Ptr);
		return true;
	}

	/*!
	\brief 템플릿단위 하나의 메모리공간 할당 및 생성자호출
	\return 할당된 메모리포인터
	\see FreeClass
	*/
	global_func TYPE* MakeClass()
	{
		return new((mint) Make()) TYPE;
	}

	/*!
	\brief MakeClass()로 생성된 메모리공간 반환 및 소멸자호출
	\param Ptr : 반환시킬 메모리포인터
	\return 반환성공여부
	\see MakeClass
	*/
	global_func bool FreeClass(TYPE* Ptr)
	{
		if(Ptr) Ptr->~TYPE();
		return Free(Ptr);
	}

private:
	class Unit : public BxPoolGlobal::Unit
	{
	public:
		enum:int {MAX = 256}; // 최대 256까지 가능
		byte Data[MAX][sizeof(TYPE)];
		TYPE* Focus;
		int Count;
		Unit()
		{
			// 마지막은 링크기록 불필요
			for(int i = 0; i < MAX - 1; ++i)
				Data[i][0] = i + 1;
			Focus = (TYPE*) &Data[0][0];
			Count = 0;
		}
		virtual void Quit()
		{
			if(Focus)
			{
				UnitFocus(this);
				Focus = nullptr;
				Count = 0;
			}
			BxPoolGlobal::Unit::Quit();
		}
	public:
		TYPE* Make()
		{
			TYPE* Result = Focus;
			if(Count < MAX)
				Focus = (++Count == MAX)? nullptr : (TYPE*) &Data[*((byte*) Focus)][0];
			return Result;
		}
		virtual void Free(void* Ptr)
		{
			BxASSERT("BxPool<연속된 중복삭제입니다>", Focus != Ptr);
			BxASSERT("BxPool<할당된 메모리가 없습니다>", 0 < Count);
			BxASSERT("BxPool<부정확한 메모리주소입니다>", ((((byte*) Ptr) - &Data[0][0]) % sizeof(TYPE)) == 0);
			if(Count-- < MAX)
			{
				BxASSERT("BxPool<포커스정보가 유효하지 않습니다>", Focus != nullptr);
				BxASSERT("BxPool<포커스정보가 유효하지 않습니다>", &Data[0][0] <= ((byte*) Focus) && ((byte*) Focus) < &Data[MAX][0]);
				BxASSERT("BxPool<포커스정보가 유효하지 않습니다>", ((((byte*) Focus) - &Data[0][0]) % sizeof(TYPE)) == 0);
				*((byte*) Ptr) = (((byte*) Focus) - &Data[0][0]) / sizeof(TYPE);
			}
			Focus = (TYPE*) Ptr;
		}
		virtual bool Find(void* Ptr)
		{
			BxASSERT("BxPool<Ptr이 nullptr입니다>", Ptr != nullptr);
			return (&Data[0][0] <= Ptr && Ptr < &Data[MAX][0]);
		}
	};

	global_func inline Unit* UnitHead()
	{
		thread_storage _ = sizeof(Unit);
		Unit* Result = (Unit*) BxCore::Thread::BindStorage(&_);
		if(!*((void**) Result))
			BxPoolGlobal::Bind(new((mint) Result) Unit);
		return (Unit*) Result;
	}

	global_func inline Unit* UnitFocus(Unit* Set = nullptr)
	{
		thread_storage _ = sizeof(Unit*);
		Unit*& Result = *((Unit**) BxCore::Thread::BindStorage(&_));
		if(Set) Result = Set;
		else if(!Result) Result = UnitHead();
		return Result;
	}

	global_func Unit* FindUnit(TYPE* Ptr)
	{
		if(!Ptr) return nullptr;
		// 해당풀 포커스검사
		if(UnitFocus()->Find(Ptr))
			return UnitFocus();
		// 해당풀 검색
		Unit* CurUnit = UnitHead();
		while(CurUnit)
		{
			if(CurUnit->Find(Ptr))
				return UnitFocus(CurUnit);
			CurUnit = (Unit*) CurUnit->Next;
		}
		// 전체풀 검색
		return (Unit*) BxPoolGlobal::FindUnit(Ptr);
	}
};
