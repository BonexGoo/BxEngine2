#pragma once
#include <BxPool.hpp>

#define Throw(...) (BxThrow(), __VA_ARGS__)

//! \brief 가변인자 제공
class BxThrow
{
public:
	/*!
	\brief 범용 Comma연산자 중복함수
	\param rhs : 우측 피연산자
	\return 자신을 리턴
	*/
	template<typename TYPE>
	inline BxThrow& operator,(const TYPE& rhs)
	{
		PtrUnit<TYPE>* NewUint = BxPool< PtrUnit<TYPE> >::MakeClass();
		NewUint->Ptr = (TYPE*) &rhs;
		List.AttachLast(NewUint);
		return *this;
	}

	/*!
	\brief string전용 Comma연산자 중복함수
	\param rhs : 우측 피연산자
	\return 자신을 리턴
	*/
	inline BxThrow& operator,(string rhs)
	{
		PtrUnit<string>* NewUint = BxPool< PtrUnit<string> >::MakeClass();
		NewUint->Ptr = (string*) rhs; // 특수화
		List.AttachLast(NewUint);
		return *this;
	}

	/*!
	\brief 데이터 접근하기
	\param i : 데이터 순번
	\return 데이터를 리턴(실패시 nullptr)
	*/
	template<typename TYPE>
	inline TYPE* Access(int i) const
	{
		if(i < 0 || List.Length() <= i) return nullptr;
		Unit* AccessedUnit = List.Access(i);
		if(!AccessedUnit) return nullptr;
		if(AccessedUnit->TablePtr() != PtrUnit<TYPE>::StaticTablePtr())
			return nullptr;
		return ((PtrUnit<TYPE>*) AccessedUnit)->Ptr;
	}

	/*!
	\brief 데이터길이 가져오기
	\return 데이터길이를 리턴
	*/
	inline const int Length() const
	{
		return List.Length();
	}

	/*!
	\brief 공데이터
	\return 공데이터를 리턴
	*/
	global_func inline const BxThrow& zero()
	{
		global_data const BxThrow Zero;
		return Zero;
	}

	/*!
	\brief 생성자
	*/
	BxThrow() {}

	/*!
	\brief 소멸자
	*/
	~BxThrow()
	{
		Unit* OldUnit = nullptr;
		while(OldUnit = List.DetachFirst())
			BxPool<Unit>::FreeClass(OldUnit);
	}

private:
	class Unit
	{
	public:
		Unit* _ref_ Next;
		Unit() : Next(nullptr) {}
		virtual ~Unit() {}
	public:
		const void* TablePtr() const
		{
			return ((const void**) this)[0];
		}
	};
	class HeadUnit : public Unit
	{
	public:
		int Count;
		Unit* _ref_ Last;
		HeadUnit() : Count(0), Last(nullptr) {Last = this;}
		virtual ~HeadUnit() {}
	public:
		Unit* Access(int index) const
		{
			Unit* CurUnit = Next;
			while(CurUnit && 0 < index--)
				CurUnit = CurUnit->Next;
			return CurUnit;
		}
		inline int Length() const
		{
			return Count;
		}
		inline void AttachLast(Unit* NewUnit)
		{
			++Count;
			Last->Next = NewUnit;
			Last = NewUnit;
		}
		inline Unit* DetachFirst()
		{
			if(!Next) return nullptr;
			if(Last == Next) Last = this;
			--Count;
			Unit* OldUnit = Next;
			Next = OldUnit->Next;
			OldUnit->Next = nullptr;
			return OldUnit;
		}
	};
	template<typename TYPE>
	class PtrUnit : public Unit
	{
	public:
		TYPE* _ref_ Ptr;
		PtrUnit() : Ptr(nullptr) {}
		virtual ~PtrUnit() {}
	public:
		global_func const void* StaticTablePtr()
		{
			global_data const PtrUnit Unit;
			return ((const void**) &Unit)[0];
		}
	};
	HeadUnit List;
};
