#pragma once
#include <BxType.hpp>

//! \brief BxPool���� �۷ι��Լ�
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
			BxAssert("BxPoolGlobal<�߸��� ȣ���Դϴ�>", false);
			return false;
		}
		virtual void Free(void* Ptr)
		{
			BxAssert("BxPoolGlobal<�߸��� ȣ���Դϴ�>", false);
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
		BxAssert("BxPoolGlobal<�ش� Ptr�� ã�� �� �����ϴ�>", false);
		return nullptr;
	}
}

//! \brief ����ڸ���� ���� Ǯ������
template<typename TYPE>
class BxPool
{
public:
	/*!
	\brief ���ø����� �ϳ��� �޸𸮰��� �Ҵ�
	\return �Ҵ�� �޸�������, �ʱ�ȭ ����
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
	\brief Make()�� ������ �޸𸮰��� ��ȯ
	\param Ptr : ��ȯ��ų �޸�������
	\return ��ȯ��������
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
	\brief ���ø����� �ϳ��� �޸𸮰��� �Ҵ� �� ������ȣ��
	\return �Ҵ�� �޸�������
	\see FreeClass
	*/
	global_func TYPE* MakeClass()
	{
		return new(Make()) TYPE;
	}

	/*!
	\brief MakeClass()�� ������ �޸𸮰��� ��ȯ �� �Ҹ���ȣ��
	\param Ptr : ��ȯ��ų �޸�������
	\return ��ȯ��������
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
			// �������� ��ũ��� ���ʿ�
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
			BxAssert("BxPool<Ptr�� nullptr�Դϴ�>", Ptr != nullptr);
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
			BxAssert("BxPool<���ӵ� �ߺ������Դϴ�>", Focus != Ptr);
			BxAssert("BxPool<�Ҵ�� �޸𸮰� �����ϴ�>", 0 < Count);
			BxAssert("BxPool<����Ȯ�� �޸��ּ��Դϴ�>", ((((byte*) Ptr) - &Data[0][0]) % sizeof(TYPE)) == 0);
			if(Count-- < MAX)
			{
				BxAssert("BxPool<��Ŀ�������� ��ȿ���� �ʽ��ϴ�>", Focus != nullptr);
				BxAssert("BxPool<��Ŀ�������� ��ȿ���� �ʽ��ϴ�>", ((TYPE*) &Data[0][0]) <= Focus && Focus < ((TYPE*) &Data[0][0]) + MAX);
				BxAssert("BxPool<��Ŀ�������� ��ȿ���� �ʽ��ϴ�>", ((((byte*) Focus) - &Data[0][0]) % sizeof(TYPE)) == 0);
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
