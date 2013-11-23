#pragma once
#include <BxType.hpp>

//! \brief BxPool관련 글로벌함수
namespace BxPoolGlobal
{
	class Unit
	{
	public:
		Unit* Next;
		Unit() : Next(nullptr) {}
		virtual ~Unit()
		{
			Unit* CurNode = nullptr;
			Unit* NextNode = Next;
			while(CurNode = NextNode)
			{
				NextNode = CurNode->Next;
				CurNode->Next = nullptr;
				delete CurNode;
			}
		}
		virtual bool Find(void* Ptr)
		{
			BxAssert("BxPoolGlobal<잘못된 호출입니다>", false);
			return false;
		}
		virtual void Free(void* Ptr)
		{
			BxAssert("BxPoolGlobal<잘못된 호출입니다>", false);
		}
	};

	class UnitLink
	{
	public:
		Unit* _ref_ Data;
		UnitLink* Next;
		UnitLink() : Data(nullptr), Next(nullptr) {}
		~UnitLink()
		{
			UnitLink* CurNode = nullptr;
			UnitLink* NextNode = Next;
			while(CurNode = NextNode)
			{
				NextNode = CurNode->Next;
				CurNode->Next = nullptr;
				delete CurNode;
			}
		}
		global_func UnitLink* Make(Unit* _ref_ data)
		{
			UnitLink* NewUnitLink = new UnitLink;
			NewUnitLink->Data = data;
			return NewUnitLink;
		}
	public:
		global_func inline UnitLink& PoolBegin()
		{
			global_data UnitLink Begin;
			return Begin;
		}
	};

	global_func void PoolBind(Unit* _ref_ Data)
	{
		UnitLink* NewUnitLink = UnitLink::Make(Data);
		NewUnitLink->Next = UnitLink::PoolBegin().Next;
		UnitLink::PoolBegin().Next = NewUnitLink;
	}

	global_func Unit* FindUnit(void* Ptr)
	{
		UnitLink* ListLink = &UnitLink::PoolBegin();
		while(ListLink = ListLink->Next)
		{
			Unit* List = ListLink->Data;
			while(List)
			{
				if(List->Find(Ptr))
					return List;
				List = List->Next;
			}
		}
		BxAssert("BxPoolGlobal<해당 Ptr을 찾을 수 없습니다>", false);
		return nullptr;
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
		TYPE* Ptr = PoolFocus()->Make();
		if(!Ptr)
		{
			Unit* List = &PoolBegin();
			while(!(Ptr = List->Make()))
			{
				if(!List->Next && !(List->Next = new Unit))
					return nullptr;
				List = (Unit*) List->Next;
			}
			PoolFocus() = List;
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
		BxPoolGlobal::Unit* List = FindUnit(Ptr);
		if(!List) return false;
		List->Free(Ptr);
		return true;
	}

	/*!
	\brief 템플릿단위 하나의 메모리공간 할당 및 생성자호출
	\return 할당된 메모리포인터
	\see FreeClass
	*/
	global_func TYPE* MakeClass()
	{
		return new(Make()) TYPE;
	}

	/*!
	\brief MakeClass()로 생성된 메모리공간 반환 및 소멸자호출
	\param Ptr : 반환시킬 메모리포인터
	\return 반환성공여부
	\see MakeClass
	*/
	global_func bool FreeClass(TYPE* Ptr)
	{
		BxPoolGlobal::Unit* List = FindUnit(Ptr);
		if(!List) return false;
		Ptr->~TYPE();
		List->Free(Ptr);
		return true;
	}

private:
	class Unit : public BxPoolGlobal::Unit
	{
	public:
		enum {MAX = 256};
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
		virtual ~Unit()
		{
		}
		virtual bool Find(void* Ptr)
		{
			BxAssert("BxPool<Ptr이 nullptr입니다>", Ptr != nullptr);
			if(Ptr < ((TYPE*) &Data[0][0]) || ((TYPE*) &Data[0][0]) + MAX <= Ptr)
				return false;
			return true;
		}
		TYPE* Make()
		{
			TYPE* Ptr = Focus;
			if(Count < MAX)
				Focus = (++Count == MAX)? nullptr : ((TYPE*) &Data[0][0]) + *((byte*) Focus);
			return Ptr;
		}
		virtual void Free(void* Ptr)
		{
			BxAssert("BxPool<연속된 중복삭제입니다>", Focus != Ptr);
			BxAssert("BxPool<할당된 메모리가 없습니다>", 0 < Count);
			BxAssert("BxPool<부정확한 메모리주소입니다>", ((((byte*) Ptr) - &Data[0][0]) % sizeof(TYPE)) == 0);
			if(Count-- < MAX)
			{
				BxAssert("BxPool<포커스정보가 유효하지 않습니다>", Focus != nullptr);
				BxAssert("BxPool<포커스정보가 유효하지 않습니다>", ((TYPE*) &Data[0][0]) <= Focus && Focus < ((TYPE*) &Data[0][0]) + MAX);
				BxAssert("BxPool<포커스정보가 유효하지 않습니다>", ((((byte*) Focus) - &Data[0][0]) % sizeof(TYPE)) == 0);
				*((byte*) Ptr) = (((byte*) Focus) - &Data[0][0]) / sizeof(TYPE);
			}
			Focus = (TYPE*) Ptr;
		}
	};

	global_func inline Unit& PoolBegin()
	{
		global_data Unit Begin;
		global_data bool IsBinded = false;
		if(!IsBinded)
		{
			IsBinded = true;
			BxPoolGlobal::PoolBind(&Begin);
		}
		return Begin;
	}

	global_func inline Unit*& PoolFocus()
	{
		global_data Unit* Focus = &PoolBegin();
		return Focus;
	}

	global_func Unit* FindUnit(TYPE* Ptr)
	{
		if(!Ptr) return nullptr;
		if(PoolFocus()->Find(Ptr))
			return PoolFocus();
		Unit* List = &PoolBegin();
		while(List)
		{
			if(List->Find(Ptr))
			{
				PoolFocus() = List;
				return List;
			}
			List = (Unit*) List->Next;
		}
		return (Unit*) BxPoolGlobal::FindUnit(Ptr);
	}
};
