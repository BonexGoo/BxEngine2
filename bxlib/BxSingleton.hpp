#pragma once
#include <BxCore.hpp>
#include <BxMemory.hpp>

#define BxSINGLETON(RET, TYPE, ...) \
	do { \
		static TYPE* Value = nullptr; \
		static uint Count = BxSingleton::Link(Value, Count); \
		RET = BxSingleton::Bind(Value, Count, __VA_ARGS__); \
	} while(false)

//! \brief 싱글톤인스턴스들의 일괄소멸을 위한 도구
class BxSingleton
{
	public: template<typename TYPE>
	global_func uint Link(TYPE*& value, uint& count)
	{
		return List::Link(value, count);
	}

	public: template<typename TYPE>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount)
	{
		if(count == newcount) return value;
		return Transfer(value, count, (0 < newcount)? BxNew_Array(TYPE, newcount) : nullptr, newcount);
	}

	public: template<typename TYPE, typename P1>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount, P1 v1)
	{if(count == newcount) return value;
	return Transfer(value, count, (0 < newcount)? BxNew_ArrayParam(TYPE, newcount, v1) : nullptr, newcount);}
	public: template<typename TYPE, typename P1, typename P2>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount, P1 v1, P2 v2)
	{if(count == newcount) return value;
	return Transfer(value, count, (0 < newcount)? BxNew_ArrayParam(TYPE, newcount, v1, v2) : nullptr, newcount);}
	public: template<typename TYPE, typename P1, typename P2, typename P3>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount, P1 v1, P2 v2, P3 v3)
	{if(count == newcount) return value;
	return Transfer(value, count, (0 < newcount)? BxNew_ArrayParam(TYPE, newcount, v1, v2, v3) : nullptr, newcount);}
	public: template<typename TYPE, typename P1, typename P2, typename P3, typename P4>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount, P1 v1, P2 v2, P3 v3, P4 v4)
	{if(count == newcount) return value;
	return Transfer(value, count, (0 < newcount)? BxNew_ArrayParam(TYPE, newcount, v1, v2, v3, v4) : nullptr, newcount);}
	public: template<typename TYPE, typename P1, typename P2, typename P3, typename P4, typename P5>
	global_func inline TYPE* Bind(TYPE*& value, uint& count, const uint newcount, P1 v1, P2 v2, P3 v3, P4 v4, P5 v5)
	{if(count == newcount) return value;
	return Transfer(value, count, (0 < newcount)? BxNew_ArrayParam(TYPE, newcount, v1, v2, v3, v4, v5) : nullptr, newcount);}

	public: template<typename TYPE>
	global_func inline void Rebind(const TYPE* value, const uint newcount)
	{
		List::Rebind(value, (0 < newcount)? BxNew_Array(TYPE, newcount) : nullptr, newcount);
	}

	public: global_func inline void Unbind(const void* value)
	{
		List::Unbind(value);
	}

	public: global_func inline void UnbindAll()
	{
		List::UnbindAll();
	}

	private: template<typename TYPE>
	global_func TYPE* Transfer(TYPE*& value, uint& count, TYPE* newvalue, const uint newcount)
	{
		for(uint i = 0, iend = (count < newcount)? count : newcount; i < iend; ++i)
			newvalue[i] = value[i];
		BxDelete_Array(value);
		value = newvalue;
		count = newcount;
		return value;
	}

	private: class Element
	{
		protected: void** ValuePtr;
		protected: uint* CountPtr;
		public: Element(void** value_ptr, uint* count_ptr) : ValuePtr(value_ptr), CountPtr(count_ptr) {}
		public: virtual ~Element() {}
		public: inline bool IsSame(const void* RHS) {return (*ValuePtr == RHS);}
	};

	private: template<typename TYPE>
	class Value : public Element
	{
		public: Value(TYPE** value, uint* count) : Element((void**) value, count) {}
		public: ~Value() {BxDelete_ArrayByType(TYPE, *ValuePtr); *CountPtr = 0;}
		public: inline void Rebind(TYPE* newvalue, const uint newcount)
		{Transfer(*((TYPE**) ValuePtr), *CountPtr, newvalue, newcount);}
	};

	private: class List
	{
		private: Element* Unit;
		private: List* Next;
		private: global_func List* Head() {global_data List _; return &_;}
		private: List(Element* unit = nullptr)
		{
			Unit = unit;
			Next = nullptr;
		}
		private: ~List()
		{
			delete Unit;
			BxAssert("BxSingleton<객체가 해제되지 못한 채로 프로그램이 종료됩니다>", !Next);
		}
		public: template<typename TYPE>
		global_func uint Link(TYPE*& value, uint& count)
		{
			List* NewList = new List(new Value<TYPE>(&value, &count));
			NewList->Next = Head()->Next;
			Head()->Next = NewList;
			return 0;
		}
		public: template<typename TYPE>
		global_func void Rebind(const TYPE* value, TYPE* newvalue, const uint newcount)
		{
			List* CurList = Head();
			while(CurList = CurList->Next)
			{
				if(CurList->Unit->IsSame(value))
				{
					((Value<TYPE>*) CurList->Unit)->Rebind(newvalue, newcount);
					return;
				}
			}
		}
		public: global_func void Unbind(const void* value)
		{
			List* CurList = Head();
			while(CurList->Next)
			{
				List* CurListNext = CurList->Next;
				if(CurListNext->Unit->IsSame(value))
				{
					CurList->Next = CurListNext->Next;
					CurListNext->Next = nullptr;
					delete CurListNext;
					return;
				}
				CurList = CurListNext;
			}
		}
		public: global_func void UnbindAll()
		{
			List* CurList = Head()->Next;
			Head()->Next = nullptr;
			while(CurList)
			{
				List* CurListNext = CurList->Next;
				CurList->Next = nullptr;
				delete CurList;
				CurList = CurListNext;
			}
		}
	};
};
