#pragma once
#include <BxMemory.hpp>
#include <BxPool.hpp>

#ifndef END
#define END   ((int) 0xD0000000)
#endif
#ifndef LAST
#define LAST  ((int) 0xD0000000 + 1)
#endif
#ifndef FINAL
#define FINAL ((int) 0xDFFFFFFF)
#endif

//! \brief �����迭 ����
template<typename TYPE>
class BxVar
{
	class VarList;
	class VarTree;
	typedef BxPool<VarList> VarListPool;
	typedef BxPool<VarTree> VarTreePool;
	typedef BxPool<TYPE> TypePool;

public:
	enum Parameter {AutoCreate = 0, NoNeed = 0};
	enum Type {LengthOfRange, LengthOfCount};

	// �������Ҵ�
	inline TYPE& operator[](int index)
	{
		CalcIndex(index);
		// ����ó��
		const int index16 = index & 0xF;
		if(CacheIdx[index16] == index)
			return *CachePtr[index16];
		IsDirtyCache = true;
		CacheIdx[index16] = index;
		// �Ϲ�ó��
		VarTree& Leaf = TreeHead.GetLeafMutabled(index);
		const VarList* List = Leaf.GetList(index);
		if(List) return *(CachePtr[index16] = List->Data);
		return *(CachePtr[index16] = Leaf.AddList(index, MakeNewData(nullptr), VarTree::NoRanging));
	}

	// ����������
	inline TYPE* Access(int index) const
	{
		CalcIndex(index);
		const VarTree& Leaf = TreeHead.GetLeaf(index);
		const VarList* List = Leaf.GetList(index);
		if(List) return List->Data;
		return nullptr;
	}

	// �����ͱ���
	inline const int Length(Type type = LengthOfRange) const
	{
		if(type == LengthOfRange)
			return TreeHead.Range;
		return TreeHead.Count;
	}

	// ��ü�ʱ�ȭ
	inline void Reset(const bool doRemove = true)
	{
		CleaningCache();
		VarList::EnableRemove() = doRemove;
		TreeHead.Reset();
		VarList::EnableRemove() = true;
	}

	// ����Ʈ����
	inline TYPE& Insert(int index, TYPE* newData = (TYPE*) AutoCreate)
	{
		CalcIndex(index);
		CleaningCache();
		VarTree& Leaf = TreeHead.GetLeafMutabled(index);
		return *Leaf.AddList(index, MakeNewData(newData), VarTree::DoRanging);
	}

	// ����Ʈ����
	void Delete(int index, TYPE** oldData = (TYPE**) NoNeed)
	{
		if(0 < CalcIndex(index)) return;
		CleaningCache();
		VarTree& Leaf = TreeHead.GetLeafMutabled(index);
		VarList* List = Leaf.GetListMutabled(index);
		if(List)
		{
			if(oldData) *oldData = List->Data, List->Data = nullptr;
			if(Leaf.SubList(List, VarTree::DoRanging) == VarTree::DeleteMe)
				VarTreePool::FreeClass(&Leaf);
		}
		else Leaf.DeleteBlank(index);
	}

	// �����ͼ���
	bool ModifyData(int index, TYPE* newData = (TYPE*) AutoCreate, TYPE** oldData = (TYPE**) NoNeed)
	{
		if(0 < CalcIndex(index)) return false;
		CleaningCache();
		VarTree& Leaf = TreeHead.GetLeaf(index);
		VarList* List = Leaf.GetList(index);
		if(List)
		{
			if(oldData) *oldData = List->Data;
			else List->RemoveData();
			List->Data = MakeNewData(newData);
		}
		else Leaf.AddList(index, MakeNewData(newData), VarTree::NoRanging);
		return true;
	}

	// ����������
	bool RemoveData(int index, TYPE** oldData = (TYPE**) NoNeed)
	{
		if(0 < CalcIndex(index)) return false;
		VarTree& Leaf = TreeHead.GetLeafMutabled(index);
		VarList* List = Leaf.GetListMutabled(index);
		if(List)
		{
			CleaningCache();
			if(oldData) *oldData = List->Data, List->Data = nullptr;
			if(Leaf.SubList(List, VarTree::NoRanging) == VarTree::DeleteMe)
				VarTreePool::FreeClass(&Leaf);
			return true;
		}
		return false;
	}

	// �����ͱ�ü
	inline void SwapData(int index1, int index2)
	{
		TYPE* Data1 = Access(index1);
		TYPE* Data2 = Access(index2);
		if(!Data1 && !Data2) return;
		TYPE* NoDelete = nullptr;
		if(Data1) ModifyData(index2, Data1, &NoDelete);
		else RemoveData(index2, &NoDelete);
		if(Data2) ModifyData(index1, Data2, &NoDelete);
		else RemoveData(index1, &NoDelete);
	}

	// ����������ȭ
	const byte* GetBytes()
	{
		if(!BytesUpdated)
		{
			ReleaseBytes();
			BytesUpdated = true;
			BytesData = (TYPE*) BxAlloc(sizeof(TYPE) * TreeHead.Range);
			for(int i = 0; i < TreeHead.Range; ++i)
			{
				TYPE* Data = Access(i);
				if(Data) BxCore::Util::MemMove(&BytesData[i], Data, sizeof(TYPE));
				else BxCore::Util::MemSet(&BytesData[i], 0, sizeof(TYPE));
			}
		}
		return (const byte*) BytesData;
	}

	// ����������ȭ ����
	inline void ReleaseBytes()
	{
		BytesUpdated = false;
		BxFree(BytesData);
	}

	// �����ͺ���
	BxVar& operator=(BxVar& RHS)
	{
		Reset();
		for(int i = 0; i < RHS.Length(); ++i)
		{
			TYPE* Data = RHS.Access(i);
			if(Data) operator[](i) = *Data;
		}
		TreeHead.Range = RHS.Length();
		return *this;
	}

	// ������
	BxVar() : TreeHead(nullptr), BytesUpdated(false), BytesData(nullptr), IsDirtyCache(true) {CleaningCache();}
	// ������
	BxVar(const BxVar& RHS) : TreeHead(nullptr), BytesUpdated(false), BytesData(nullptr), IsDirtyCache(true) {operator=(RHS);}
	// �Ҹ���
	virtual ~BxVar() {ReleaseBytes();}

private:
	bool BytesUpdated;
	TYPE* BytesData;
	bool IsDirtyCache;
	int CacheIdx[16];
	TYPE* CachePtr[16];

	macro int CalcIndex(int _inout_ index) const
	{
		const int LastIndex = TreeHead.Range - 1;
		if((index & 0xE0000000) == 0xC0000000)
		{
			if(index == FINAL) index = (LastIndex < 0)? 0 : LastIndex;
			else index = index - END + LastIndex;
		}
		BxAssert("BxVar<index���� ������ �� �� �����ϴ�>", 0 <= index);
		return index - LastIndex;
	}

	macro TYPE* MakeNewData(TYPE* newData)
	{
		BytesUpdated = false;
		TYPE* NewData = (newData)? newData : TypePool::MakeClass();
		if(!newData && IsTypePointer<TYPE>())
			BxCore::Util::MemSet(NewData, 0, sizeof(TYPE));
		return NewData;
	}

	void CleaningCache()
	{
		BytesUpdated = false;
		if(IsDirtyCache)
		{
			IsDirtyCache = false;
			for(int i = 0; i < 16; ++i)
				CacheIdx[i] = -1;
		}
	}

	class VarList
	{
	public:
		int Index; // ����
		TYPE* Data; // �ڱⵥ����
		VarList* Next; // ����������
	public:
		VarList() {BxAssert("BxVar<�߸��� ȣ���Դϴ�>", false);}
		~VarList()
		{
			RemoveData();
			if(Next) VarListPool::FreeClass(Next);
		}
	public:
		VarList* Init(int index, TYPE* data) {Index = index; Data = data; Next = nullptr; return this;}
		void RemoveData()
		{
			if(Data && EnableRemove())
			{
				const bool Result = TypePool::FreeClass(Data);
				BxAssert("BxVar<�ܺε����Ͱ� �־� �޸𸮴����� �߻��մϴ�>", Result);
			}
			Data = nullptr;
		}
		global_func bool& EnableRemove() {global_data bool _ = true; return _;}
	};

	class VarTree
	{
	public:
		enum Parameter {DoRanging, NoRanging};
		enum Action {NoAction, DeleteMe};
	public:
		VarTree* Parent; // �θ�
		int Range; // ����Ʈ����
		int Count; // ����Ʈ����
		union
		{
			VarTree* Child[2]; // [�¼�][�켱]
			VarList* List[2]; // [����Ʈ][nullptr]
		};
	public:
		VarTree() {BxAssert("BxVar<�߸��� ȣ���Դϴ�>", false);}
		VarTree(void*) // TreeHead����
		{
			Parent = nullptr;
			Range = 0;
			Count = 0;
			List[0] = nullptr;
			List[1] = nullptr;
		}
		~VarTree() {Quit();}
	public:
		VarTree* InitPath(VarTree* parent, int range, int count, VarTree* leftchild, VarTree* rightchild)
		{
			Parent = parent;
			Range = range;
			Count = count;
			Child[0] = leftchild;
			Child[1] = rightchild;
			return this;
		}
		VarTree* InitLeaf(VarTree* parent, int range, int count, VarList* list)
		{
			Parent = parent;
			Range = range;
			Count = count;
			List[0] = list;
			List[1] = nullptr;
			return this;
		}
		void Reset()
		{
			BxAssert("BxVar<�߸��� ȣ���Դϴ�>", !Parent);
			Quit();
			Range = 0;
			Count = 0;
			List[0] = nullptr;
			List[1] = nullptr;
		}
	private:
		void Quit()
		{
			if(HasChild())
			{
				VarTreePool::FreeClass(Child[0]);
				VarTreePool::FreeClass(Child[1]);
			}
			else VarListPool::FreeClass(List[0]);
		}
		macro bool HasChild() const {return Child[1] != nullptr;}
	public:
		const VarTree& GetLeaf(int _inout_ index) const
		{
			const VarTree* CurTree = this;
			while(CurTree->HasChild())
			{
				if(index < CurTree->Child[0]->Range)
					CurTree = CurTree->Child[0];
				else
				{
					index -= CurTree->Child[0]->Range;
					CurTree = CurTree->Child[1];
				}
			}
			return *CurTree;
		}
		VarTree& GetLeafMutabled(int _inout_ index)
		{
			return *((VarTree*) &GetLeaf(index));
		}
		const VarList* GetList(int index) const
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			const VarList* CurList = List[0];
			while(CurList && (index -= CurList->Index))
				CurList = CurList->Next;
			return CurList;
		}
		VarList* GetListMutabled(int index)
		{
			return (VarList*) GetList(index);
		}
		TYPE* AddList(const int index, TYPE* data, const Parameter option)
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			VarList* NewList = VarListPool::Make()->Init(index, data);
			if(List[0])
			{
				VarList* PrevList = nullptr;
				VarList* CurList = List[0];
				do
				{
					if(CurList->Index < NewList->Index)
						NewList->Index -= CurList->Index;
					else
					{
						CurList->Index -= NewList->Index - ((option == DoRanging)? 1 : 0); // �ε�������
						((PrevList)? PrevList->Next : List[0]) = NewList; // ����Ʈ����
						NewList->Next = CurList;
						AdjustLoop((option == DoRanging)? 1 : 0, 1); // Ʈ������
						if(Count == 4) LeafToPath(); // �н��и�
						return data;
					}
				}
				while(CurList = (PrevList = CurList)->Next);
				// �� ��ġ�� Var�� ������ ��� �߰��� �ǹ�
				PrevList->Next = NewList; // ����Ʈ����
				AdjustLoop(NewList->Index, 1); // Ʈ������
				if(Count == 4) LeafToPath(); // �н��и�
			}
			else
			{
				List[0] = NewList; // ù������
				AdjustLoop(index + 1, 1); // Ʈ������
			}
			return data;
		}
		Action SubList(VarList* list, const Parameter option)
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			VarList* PrevList = nullptr;
			VarList* CurList = List[0];
			while(true)
			{
				BxAssert("BxVar<�߸��� �ó������Դϴ�>", CurList);
				if(CurList == list)
				{
					if(CurList->Next) // �ε�������
						CurList->Next->Index += CurList->Index - ((option == DoRanging)? 1 : 0);
					((PrevList)? PrevList->Next : List[0]) = CurList->Next; // ����Ʈ�и�
					CurList->Next = nullptr;
					VarListPool::FreeClass(CurList);
					AdjustLoop((option == DoRanging)? -1 : 0, -1); // Ʈ������
					if(Count == 0) return BrotherToParent(); // �н�����
					return NoAction;
				}
				CurList = (PrevList = CurList)->Next;
			}
			return NoAction;
		}
		void LeafToPath()
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			// ��������
			const int NewRange1 = 1 + List[0]->Index + List[0]->Next->Index;
			const int NewRange2 = Range - NewRange1;
			VarList* NewList1 = List[0];
			VarList* NewList2 = NewList1->Next->Next;
			NewList1->Next->Next = nullptr; // ����и�
			--NewList2->Index;
			// ��������
			Child[0] = VarTreePool::Make()->InitLeaf(this, NewRange1, 2, NewList1);
			Child[1] = VarTreePool::Make()->InitLeaf(this, NewRange2, 2, NewList2);
			// ƿ��
			TiltingLoop(true);
		}
		Action BrotherToParent()
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			if(Parent)
			{
				VarTree* Brother = Parent->Child[Parent->Child[1] != this];
				if(Brother->HasChild())
				{
					if(Parent->Child[0] = Brother->Child[0])
						Brother->Child[0]->Parent = Parent;
					if(Parent->Child[1] = Brother->Child[1])
						Brother->Child[1]->Parent = Parent;
					Brother->Child[0] = nullptr;
					Brother->Child[1] = nullptr;
				}
				else
				{
					Parent->List[0] = Brother->List[0];
					Parent->List[1] = nullptr;
					Brother->List[0] = nullptr;
				}
				VarTreePool::FreeClass(Brother); // ��������
				// ƿ��
				Parent->TiltingLoop(false);
				return DeleteMe;
			}
			// �ֻ���
			VarListPool::FreeClass(List[0]);
			List[0] = nullptr;
			return NoAction;
		}
		void DeleteBlank(int index)
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			VarList* CurList = List[0];
			do
			{
				if(index < CurList->Index)
				{
					--CurList->Index;
					break;
				}
				index -= CurList->Index;
			}
			while(CurList = CurList->Next);
			AdjustLoop(-1, 0);
		}
		void AdjustLoop(const int range, const int count)
		{
			BxAssert("BxVar<Leaf�� �ƴմϴ�>", !HasChild());
			VarTree* CurTree = this;
			do
			{
				CurTree->Range += range;
				CurTree->Count += count;
			}
			while(CurTree = CurTree->Parent);
		}
		void TiltingLoop(const bool isadd)
		{
			VarTree* CurTree = nullptr;
			VarTree* ParentTree = this;
			while(ParentTree = (CurTree = ParentTree)->Parent)
			{
				const bool testside = (ParentTree->Child[isadd] == CurTree);
				if(ParentTree->Child[testside]->HasChild())
					ParentTree->Tilting(testside);
			}
		}
		bool Tilting(const bool testside)
		{
			BxAssert("BxVar<Path�� �ƴմϴ�>", HasChild());
			// �����
			VarTree* Test = Child[testside];
			VarTree* Loss = Child[!testside];
			VarTree* Benefit = Test->Child[testside];
			VarTree* Same = Test->Child[!testside];
			if(Benefit->Count <= Loss->Count)
				return false;
			// 1�ܰ�
			Benefit->Parent = this;
			Child[testside] = Benefit;
			Child[!testside] = Test;
			// 2�ܰ�
			Loss->Parent = Test;
			Test->Child[testside] = Same;
			Test->Child[!testside] = Loss;
			Test->Range = Loss->Range + Same->Range;
			Test->Count = Loss->Count + Same->Count;
			return true;
		}
	} TreeHead;
};
