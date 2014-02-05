#pragma once
#include <BxCore.hpp>

//! \brief map식 가변배열 제공
template<typename TYPE>
class BxVarMap
{
public:
	// 정수Key식 데이터할당
	TYPE& operator[](uint key)
	{
		BxVarMap* CurNode = this;
		while(key)
		{
			CurNode = CurNode->ValidChild(key & 0xF);
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

	// 문자열Key식(CP949) 데이터할당
	TYPE& operator[](string key)
	{
		BxVarMap* CurNode = this;
		while(*key)
		{
			CurNode = CurNode->ValidChild((*key & 0xF0) >> 4);
			CurNode = CurNode->ValidChild(*(key++) & 0x0F);
		}
		if(!CurNode->Data)
		{
			CurNode->Data = new TYPE;
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// 문자열Key식(UTF16) 데이터할당
	TYPE& operator[](wstring key)
	{
		BxVarMap* CurNode = this;
		while(*key)
		{
			CurNode = CurNode->ValidChild((*key & 0xF000) >> 12);
			CurNode = CurNode->ValidChild((*key & 0x0F00) >> 8);
			CurNode = CurNode->ValidChild((*key & 0x00F0) >> 4);
			CurNode = CurNode->ValidChild(*(key++) & 0x000F);
		}
		if(!CurNode->Data)
		{
			CurNode->Data = new TYPE;
			if(IsTypePointer<TYPE>())
				BxCore::Util::MemSet(CurNode->Data, 0, sizeof(TYPE));
		}
		return *CurNode->Data;
	}

	// 정수Key식 데이터접근
	TYPE* Access(uint key) const
	{
		const BxVarMap* CurNode = this;
		while(key)
		{
			if(!CurNode->Child || !(CurNode = CurNode->Child[key & 0xF])) return nullptr;
			key >>= 4;
		}
		return CurNode->Data;
	}

	// 문자열Key식(CP949) 데이터접근
	TYPE* Access(string key) const
	{
		const BxVarMap* CurNode = this;
		while(*key)
		{
			if(!CurNode->Child || !(CurNode = CurNode->Child[(*key & 0xF0) >> 4])) return nullptr;
			if(!CurNode->Child || !(CurNode = CurNode->Child[*(key++) & 0x0F])) return nullptr;
		}
		return CurNode->Data;
	}

	// 문자열Key식(UTF16) 데이터접근
	TYPE* Access(wstring key) const
	{
		const BxVarMap* CurNode = this;
		while(*key)
		{
			if(!CurNode->Child || !(CurNode = CurNode->Child[(*key & 0xF000) >> 12])) return nullptr;
			if(!CurNode->Child || !(CurNode = CurNode->Child[(*key & 0x0F00) >> 8])) return nullptr;
			if(!CurNode->Child || !(CurNode = CurNode->Child[(*key & 0x00F0) >> 4])) return nullptr;
			if(!CurNode->Child || !(CurNode = CurNode->Child[*(key++) & 0x000F])) return nullptr;
		}
		return CurNode->Data;
	}

	// 정수Key식 데이터제거
	bool Remove(uint key)
	{
		bool Result = !key;
		BxVarMap* CurChild = nullptr;
		if(Result) {delete Data; Data = nullptr;}
		else if(Child && (CurChild = Child[key & 0xF]))
		{
			Result = CurChild->Remove(key >> 4);
			if(Result && !CurChild->Data && !CurChild->Child)
				InvalideChild(key & 0xF);
		}
		return Result;
	}

	// 문자열Key식(CP949) 데이터제거
	bool Remove(string key)
	{
		bool Result = !*key;
		BxVarMap* CurChild1 = nullptr;
		BxVarMap* CurChild2 = nullptr;
		if(Result) {delete Data; Data = nullptr;}
		else if(Child && (CurChild1 = Child[(*key & 0xF0) >> 4]) && (CurChild2 = CurChild1->Child[*key & 0x0F]))
		{
			Result = CurChild2->Remove(key + 1);
			if(Result && !CurChild2->Data && !CurChild2->Child)
			{
				CurChild1->InvalideChild(*key & 0x0F);
				if(!CurChild1->Data && !CurChild1->Child)
					InvalideChild((*key & 0xF0) >> 4);
			}
		}
		return Result;
	}

	// 문자열Key식(UTF16) 데이터제거
	bool Remove(wstring key)
	{
		bool Result = !*key;
		BxVarMap* CurChild1 = nullptr;
		BxVarMap* CurChild2 = nullptr;
		BxVarMap* CurChild3 = nullptr;
		BxVarMap* CurChild4 = nullptr;
		if(Result) {delete Data; Data = nullptr;}
		else if(Child && (CurChild1 = Child[(*key & 0xF000) >> 12]) && (CurChild2 = CurChild1->Child[(*key & 0x0F00) >> 8])
			&& (CurChild3 = CurChild2->Child[(*key & 0x00F0) >> 4]) && (CurChild4 = CurChild3->Child[*key & 0x000F]))
		{
			Result = CurChild4->Remove(key + 1);
			if(Result && !CurChild4->Data && !CurChild4->Child)
			{
				CurChild3->InvalideChild(*key & 0x000F);
				if(!CurChild3->Data && !CurChild3->Child)
				{
					CurChild2->InvalideChild((*key & 0x00F0) >> 4);
					if(!CurChild2->Data && !CurChild2->Child)
					{
						CurChild1->InvalideChild((*key & 0x0F00) >> 8);
						if(!CurChild1->Data && !CurChild1->Child)
							InvalideChild((*key & 0xF000) >> 12);
					}
				}
			}
		}
		return Result;
	}

	// 전체초기화
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

	// 생성자/소멸자
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

	inline BxVarMap* ValidChild(const int i)
	{
		if(!Child)
		{
			Child = new BxVarMap*[16];
			BxCore::Util::MemSet(Child, 0x00, sizeof(BxVarMap*) * 16);
		}
		return (Child[i])? Child[i] : (Child[i] = new BxVarMap);
	}
	inline void InvalideChild(const int i)
	{
		delete Child[i];
		Child[i] = nullptr;
		if(!Child[0x0] && !Child[0x1] && !Child[0x2] && !Child[0x3]
		&& !Child[0x4] && !Child[0x5] && !Child[0x6] && !Child[0x7]
		&& !Child[0x8] && !Child[0x9] && !Child[0xA] && !Child[0xB]
		&& !Child[0xC] && !Child[0xD] && !Child[0xE] && !Child[0xF])
		{
			delete Child;
			Child = nullptr;
		}
	}
};
