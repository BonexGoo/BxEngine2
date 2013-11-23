#pragma once
#include <BxCore.hpp>

//! \brief map�� �����迭 ����
template<typename TYPE>
class BxVarMap
{
public:
	// ����Key�� �������Ҵ�
	TYPE& operator[](uint key)
	{
		BxVarMap* CurNode = this;
		while(key)
		{
			const uint index = (key & 0xF);
			CurNode->ValidChild();
			CurNode = (CurNode->Child[index])? CurNode->Child[index]
				: (CurNode->Child[index] = new BxVarMap);
			key >>= 4;
		}
		if(!CurNode->Data)
		{
			CurNode->Data = new TYPE;
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// ���ڿ�Key��(CP949) �������Ҵ�
	TYPE& operator[](string key)
	{
		BxVarMap* CurNode = this;
		while(*key)
		{
			const uint indexHi = ((*key & 0xF0) >> 4);
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexHi])? CurNode->Child[indexHi]
				: (CurNode->Child[indexHi] = new BxVarMap);
			const uint indexLo = (*(key++) & 0x0F);
			if(!*key && !indexLo) break;
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexLo])? CurNode->Child[indexLo]
				: (CurNode->Child[indexLo] = new BxVarMap);
		}
		if(!CurNode->Data)
		{
			CurNode->Data = new TYPE;
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// ���ڿ�Key��(UTF16) �������Ҵ�
	TYPE& operator[](wstring key)
	{
		BxVarMap* CurNode = this;
		while(*key)
		{
			const uint indexHiHi = ((*key & 0xF000) >> 12);
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexHiHi])? CurNode->Child[indexHiHi]
				: (CurNode->Child[indexHiHi] = new BxVarMap);
			const uint indexHiLo = ((*key & 0x0F00) >> 8);
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexHiLo])? CurNode->Child[indexHiLo]
				: (CurNode->Child[indexHiLo] = new BxVarMap);
			const uint indexLoHi = ((*key & 0x00F0) >> 4);
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexLoHi])? CurNode->Child[indexLoHi]
				: (CurNode->Child[indexLoHi] = new BxVarMap);
			const uint indexLoLo = (*(key++) & 0x000F);
			if(!*key && !indexLoLo) break;
			CurNode->ValidChild();
			CurNode = (CurNode->Child[indexLoLo])? CurNode->Child[indexLoLo]
				: (CurNode->Child[indexLoLo] = new BxVarMap);
		}
		if(!CurNode->Data)
		{
			CurNode->Data = new TYPE;
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// ����Key�� ����������
	TYPE* Access(uint key) const
	{
		const BxVarMap* CurNode = this;
		while(key)
		{
			const uint index = (key & 0xF);
			if(!CurNode->Child || !CurNode->Child[index]) return nullptr;
			CurNode = CurNode->Child[index];
			key >>= 4;
		}
		return CurNode->Data;
	}

	// ���ڿ�Key��(CP949) ����������
	TYPE* Access(string key) const
	{
		const BxVarMap* CurNode = this;
		while(*key)
		{
			const uint indexHi = ((*key & 0xF0) >> 4);
			if(!CurNode->Child || !CurNode->Child[indexHi]) return nullptr;
			CurNode = CurNode->Child[indexHi];
			const uint indexLo = (*(key++) & 0x0F);
			if(!*key && !indexLo) break;
			if(!CurNode->Child || !CurNode->Child[indexLo]) return nullptr;
			CurNode = CurNode->Child[indexLo];
		}
		return CurNode->Data;
	}

	// ���ڿ�Key��(UTF16) ����������
	TYPE* Access(wstring key) const
	{
		const BxVarMap* CurNode = this;
		while(*key)
		{
			const uint indexHiHi = ((*key & 0xF000) >> 12);
			if(!CurNode->Child || !CurNode->Child[indexHiHi]) return nullptr;
			CurNode = CurNode->Child[indexHiHi];
			const uint indexHiLo = ((*key & 0x0F00) >> 8);
			if(!CurNode->Child || !CurNode->Child[indexHiLo]) return nullptr;
			CurNode = CurNode->Child[indexHiLo];
			const uint indexLoHi = ((*key & 0x00F0) >> 4);
			if(!CurNode->Child || !CurNode->Child[indexLoHi]) return nullptr;
			CurNode = CurNode->Child[indexLoHi];
			const uint indexLoLo = (*(key++) & 0x000F);
			if(!*key && !indexLoLo) break;
			if(!CurNode->Child || !CurNode->Child[indexLoLo]) return nullptr;
			CurNode = CurNode->Child[indexLoLo];
		}
		return CurNode->Data;
	}

	// ��ü�ʱ�ȭ
	void Reset()
	{
		delete Data;
		if(Child)
		{
			for(uint i = 0; i < 16; ++i)
				delete Child[i];
			delete[] Child;
		}
		Data = nullptr;
		Child = nullptr;
	}

	// ������/�Ҹ���
	BxVarMap()
	{
		Data = nullptr;
		Child = nullptr;
	}
	~BxVarMap()
	{
		Reset();
	}

private:
	TYPE* Data;
	BxVarMap** Child;

	inline void ValidChild()
	{
		if(Child) return;
		Child = new BxVarMap*[16];
		BxCore::Util::MemSet(Child, 0x00, sizeof(BxVarMap*) * 16);
	}
};
