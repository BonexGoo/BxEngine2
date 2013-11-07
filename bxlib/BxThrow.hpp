#pragma once
#include <BxPool.hpp>

#define throw(...) (BxThrow(), __VA_ARGS__)

//! \brief �������� ����
class BxThrow
{
public:
	/*!
	\brief ���� Comma������ �ߺ��Լ�
	\param rhs : ���� �ǿ�����
	\return �ڽ��� ����
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
	\brief string���� Comma������ �ߺ��Լ�
	\param rhs : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	inline BxThrow& operator,(string rhs)
	{
		PtrUnit<string>* NewUint = BxPool< PtrUnit<string> >::MakeClass();
		NewUint->Ptr = (string*) rhs; // Ư��ȭ
		List.AttachLast(NewUint);
		return *this;
	}

	/*!
	\brief ������ �����ϱ�
	\param i : ������ ����
	\return �����͸� ����(���н� null)
	*/
	template<typename TYPE>
	inline TYPE* Access(int i) const
	{
		if(i < 0 || List.Length() <= i) return null;
		Unit* AccessedUnit = List.Access(i);
		if(!AccessedUnit) return null;
		if(AccessedUnit->TablePtr() != PtrUnit<TYPE>::StaticTablePtr())
			return null;
		return ((PtrUnit<TYPE>*) AccessedUnit)->Ptr;
	}

	/*!
	\brief �����ͱ��� ��������
	\return �����ͱ��̸� ����
	*/
	inline const int Length() const
	{
		return List.Length();
	}

	/*!
	\brief ��������
	\return �������͸� ����
	*/
	global_func inline const BxThrow& zero()
	{
		global_data const BxThrow Zero;
		return Zero;
	}

	/*!
	\brief ������
	*/
	BxThrow() {}

	/*!
	\brief �Ҹ���
	*/
	~BxThrow()
	{
		Unit* OldUnit = null;
		while(OldUnit = List.DetachFirst())
			BxPool<Unit>::FreeClass(OldUnit);
	}

private:
	class Unit
	{
	public:
		Unit* _ref_ Next;
		Unit() : Next(null) {}
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
		HeadUnit() : Count(0), Last(null) {Last = this;}
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
			if(!Next) return null;
			if(Last == Next) Last = this;
			--Count;
			Unit* OldUnit = Next;
			Next = OldUnit->Next;
			OldUnit->Next = null;
			return OldUnit;
		}
	};
	template<typename TYPE>
	class PtrUnit : public Unit
	{
	public:
		TYPE* _ref_ Ptr;
		PtrUnit() : Ptr(null) {}
		virtual ~PtrUnit() {}
	public:
		global_func const void* StaticTablePtr()
		{
			global_data const PtrUnit Uint;
			return ((const void**) &Uint)[0];
		}
	};
	HeadUnit List;
};
