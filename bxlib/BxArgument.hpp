#pragma once
#include <BxPool.hpp>

// BxARG-모듈
#define BxARG(...) (BxArgument(), __VA_ARGS__)

// BxARG 전용타입
class ARG_STR_UPPER
{
public:
	string _tmp_ Value;
	ARG_STR_UPPER(const ARG_STR_UPPER& RHS) {operator=(RHS);}
	explicit ARG_STR_UPPER(string RHS) : Value(RHS) {}
	ARG_STR_UPPER& operator=(const ARG_STR_UPPER& RHS) {Value = RHS.Value; return *this;}
};
class ARG_STR_LOWER
{
public:
	string _tmp_ Value;
	ARG_STR_LOWER(const ARG_STR_LOWER& RHS) {operator=(RHS);}
	explicit ARG_STR_LOWER(string RHS) : Value(RHS) {}
	ARG_STR_LOWER& operator=(const ARG_STR_LOWER& RHS) {Value = RHS.Value; return *this;}
};
class ARG_HEX_UPPER
{
public:
	uint Value;
	ARG_HEX_UPPER(const ARG_HEX_UPPER& RHS) {operator=(RHS);}
	explicit ARG_HEX_UPPER(const int RHS) : Value((uint) RHS) {}
	explicit ARG_HEX_UPPER(const void* RHS) : Value((uint) RHS) {}
	ARG_HEX_UPPER& operator=(const ARG_HEX_UPPER& RHS) {Value = RHS.Value; return *this;}
};
class ARG_HEX_LOWER
{
public:
	uint Value;
	ARG_HEX_LOWER(const ARG_HEX_LOWER& RHS) {operator=(RHS);}
	explicit ARG_HEX_LOWER(const int RHS) : Value((uint) RHS) {}
	explicit ARG_HEX_LOWER(const void* RHS) : Value((uint) RHS) {}
	ARG_HEX_LOWER& operator=(const ARG_HEX_LOWER& RHS) {Value = RHS.Value; return *this;}
};

//! \brief 가변인자 제공
class BxArgument
{
public:
	/*!
	\brief 범용 Comma연산자 중복함수
	\param rhs : 우측 피연산자
	\return 자신을 리턴
	*/
	template<typename TYPE>
	inline BxArgument& operator,(const TYPE& rhs)
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
	inline BxArgument& operator,(string rhs)
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
		if(AccessedUnit->VTablePtr() != PtrUnit<TYPE>::VTablePtr())
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
	global_func inline const BxArgument& zero()
	{
		global_data const BxArgument Zero;
		return Zero;
	}

	/*!
	\brief 생성자
	*/
	BxArgument() {}

	/*!
	\brief 소멸자
	*/
	~BxArgument()
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
		const void* VTablePtr() const {return *((const void**) this);}
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
		global_func const void* VTablePtr()
		{
			global_data void* _ = nullptr;
			if(!_) {PtrUnit VTable; _ = *((void**) &VTable);}
			return _;
		}
	};
	HeadUnit List;
};
