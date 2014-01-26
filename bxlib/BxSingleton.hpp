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

// BxSINGLETON-모듈
#define BxSINGLETON(RET, LEN) \
	do { \
		if(LEN == 0) {RET = nullptr; break;} \
		thread_storage Key = sizeof(decltype(RET)) + sizeof(int); \
		decltype(RET)* Value = (decltype(RET)*) BxCore::Thread::BindStorage(&Key); \
		int* Count = (int*) &Value[1]; \
		if(!*Count) *Count = BxSingleton::Link(Value, Count __DEBUG_MCR__); \
		RET = BxSingleton::Bind(Value, Count, LEN); \
	} while(false)

//! \brief 싱글톤인스턴스들의 일괄소멸을 위한 도구
class BxSingleton
{
	public: template<typename TYPE>
	global_func int Link(TYPE** value, int* count __DEBUG_PRM__)
	{
		return List::Link(value, count __DEBUG_ARG__);
	}

	public: template<typename TYPE>
	global_func inline TYPE* Bind(TYPE** value, int* count, const int newcount)
	{
		BxASSERT("BxSingleton<싱글톤의 요청수량은 0이 될 수 없습니다>", 0 < newcount);
		if(*count == newcount) return *value;
		return Transfer(value, count, new TYPE[newcount], newcount);
	}

	public: template<typename TYPE>
	global_func inline void Rebind(const TYPE* value, const int newcount)
	{
		List::Rebind(value, (0 < newcount)? new TYPE[newcount] : nullptr, newcount);
	}

	public: global_func inline void Unbind(const void* value)
	{
		List::Unbind(value);
	}

	public: global_func inline void UnbindAll(bool dokill)
	{
		List::UnbindAll(dokill);
	}

	private: template<typename TYPE>
	global_func TYPE* Transfer(TYPE** value, int* count, TYPE* newvalue, const int newcount)
	{
		for(int i = 0, iend = (*count < newcount)? *count : newcount; i < iend; ++i)
			newvalue[i] = (*value)[i];
		delete[] *value;
		*value = newvalue;
		*count = newcount;
		return *value;
	}

	private: class Element
	{
		protected: void** ValuePtr;
		protected: int* CountPtr;
		public: Element(void** value_ptr, int* count_ptr) : ValuePtr(value_ptr), CountPtr(count_ptr) {}
		public: virtual ~Element() {}
		public: inline bool IsSame(const void* RHS) {return (*ValuePtr == RHS);}
	};

	private: template<typename TYPE>
	class Value : public Element
	{
		__DEBUG_VAL__
		public: Value(TYPE** value, int* count __DEBUG_PRM__)
			: Element((void**) value, count) __DEBUG_INT__ {}
		public: ~Value() {delete[] (TYPE*) *ValuePtr; *ValuePtr = nullptr; *CountPtr = 0;}
		public: inline void Rebind(TYPE* newvalue, const int newcount)
		{Transfer((TYPE**) ValuePtr, CountPtr, newvalue, newcount);}
	};

	private: class List
	{
		private: Element* Unit;
		private: List* Next;
		private: global_func inline List* Head()
		{thread_storage _ = sizeof(List); return (List*) BxCore::Thread::BindStorage(&_);}
		private: List(Element* unit)
		{
			Unit = unit;
			Next = nullptr;
		}
		private: ~List()
		{
			delete Unit;
			BxASSERT("BxSingleton<객체가 해제되지 못한 채로 스레드가 종료됩니다>", Next == nullptr);
			Unit = nullptr;
			Next = nullptr;
		}
		public: template<typename TYPE>
		global_func int Link(TYPE** value, int* count __DEBUG_PRM__)
		{
			List* NewList = new List(new Value<TYPE>(value, count __DEBUG_ARG__));
			NewList->Next = Head()->Next;
			Head()->Next = NewList;
			return 0;
		}
		public: template<typename TYPE>
		global_func void Rebind(const TYPE* value, TYPE* newvalue, const int newcount)
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
		public: global_func void UnbindAll(bool dokill)
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
			if(dokill)
				Head()->~List();
		}
	};
};
