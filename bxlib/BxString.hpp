#pragma once
#include <BxVar.hpp>
#include <BxUtil.hpp>
#include <BxSingleton.hpp>

//! \brief ��Ʈ�� ����
class BxString
{
public:
	//! \brief �Ľ� ����
	class Parse
	{
	public:
		/*!
		\brief �⺻������
		*/
		Parse()
		{
			ValidCount = 0;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc("", ValidCount);
		}

		/*!
		\brief ���������(Parse)
		*/
		Parse(const Parse& RHS)
		{
			ValidCount = RHS.ValidCount;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(RHS.String, ValidCount);
		}

		/*!
		\brief ���������(string, int)
		*/
		explicit Parse(string RHS, int Length = -1)
		{
			ValidCount = (Length < 0)? BxUtilGlobal::StrLen(RHS) : Length;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(RHS, ValidCount);
		}

		/*!
		\brief ���������(Parse*, string)
		*/
		explicit Parse(const Parse* RHS1, string RHS2)
		{
			const int RHS2Count = BxUtilGlobal::StrLen(RHS2);
			ValidCount = RHS1->ValidCount + RHS2Count;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrAlloc(ValidCount);
			BxCore::Util::MemMove(String, RHS1->String, RHS1->ValidCount);
			BxCore::Util::MemMove(String + RHS1->ValidCount, RHS2, RHS2Count);
			((char*) String)[ValidCount] = '\0';
		}

		/*!
		\brief ���������(string, BxThrow)
		*/
		explicit Parse(string RHS, const BxThrow& args)
		{
			ValidCount = 0;
			string Result = BxCore::Util::Print(RHS, args, &ValidCount);
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(Result, ValidCount);
		}

		/*!
		\brief �Ҹ���
		*/
		~Parse()
		{
			String = BxUtilGlobal::StrFree(String);
		}

		/*!
		\brief ���� ���ϱ�
		\return ��Ʈ���� ���ڱ���
		*/
		inline int GetLength()
		{
			return ValidCount;
		}

		/*!
		\brief ������ ���� ���ϱ�
		\return ������ ����
		*/
		inline char GetLast()
		{
			return String[BxUtilGlobal::Max(0, ValidCount - 1)];
		}

		/*!
		\brief ������ ���� �����
		*/
		inline void DeleteLast()
		{
			ValidCount = BxUtilGlobal::Max(0, ValidCount - 1);
			((char*) String)[ValidCount] = '\0';
		}

		/*!
		\brief ������ �ߺ��Լ�+(string)
		\param RHS : ���� �ǿ�����
		\return ���ο� �ν��Ͻ�
		*/
		Parse operator+(string RHS) const
		{
			return Parse(this, RHS);
		}

		/*!
		\brief ������ �ߺ��Լ�=(string)
		\param RHS : ���� �ǿ�����
		\return �ڽ��� ����
		*/
		Parse& operator=(string RHS)
		{
			String = BxUtilGlobal::StrFree(String);
			ValidCount = BxUtilGlobal::StrLen(RHS);
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(RHS, ValidCount);
			return *this;
		}

		/*!
		\brief ������ �ߺ��Լ�+=(char)
		\param RHS : ���� �ǿ�����
		\return �ڽ��� ����
		*/
		inline Parse& operator+=(char RHS)
		{
			if(StringLength <= ValidCount + 1)
			{
				const int OldStringLength = StringLength;
				string NewString = BxUtilGlobal::StrAlloc((StringLength *= 2) - 1);
				BxCore::Util::MemMove(NewString, String, OldStringLength);
				String = BxUtilGlobal::StrFree(String);
				String = NewString;
			}
			((char*) String)[ValidCount++] = RHS;
			((char*) String)[ValidCount] = '\0';
			return *this;
		}

		/*!
		\brief ����ȯ �ߺ��Լ�(string)
		\return string�� ��ü��Ʈ��
		*/
		inline operator string()
		{
			return String;
		}

		/*!
		\brief �迭����
		\param index : �迭��ȣ
		\return char�� �ش� ������
		*/
		inline const char operator[](int Index)
		{
			if(Index < 0 || ValidCount <= Index)
				return '\0';
			return String[Index];
		}

		/*!
		\brief ��Ƽ�ǽ�Ʈ�� ������(|A|B|C|, /A/B/C/, *A*B*C*...)
		\param Result : ������ȣ(ù����Ʈ �ڵ��ν�)�� �и��� ���(���� Reset�� ����)
		\param Other : �۾��� ��Ʈ��
		\param Length : �۾��� ��Ʈ���� ������(Other�� ���̷� �ڵ����� : -1)
		*/
		global_func void Division(BxVar<Parse>& Result, string Other, int Length = -1)
		{
			const int OtherLength = (Length < 0)? BxUtilGlobal::StrLen(Other) : Length;
			if(OtherLength && Other[0] == Other[OtherLength - 1])
			for(int i = 1, iprev = 1; i < OtherLength; ++i)
				if(Other[i] == Other[0])
				{
					Result[LAST].ValidCount = i - iprev;
					Result[END].StringLength = i - iprev + 1;
					Result[END].String = BxUtilGlobal::StrCpyWithAlloc(&Other[iprev], i - iprev);
					iprev = i + 1;
				}
		}

		/*!
		\brief ����Ʈ��Ʈ�� �̸� ��������(A<B:C:D>���� A�κ�)
		\param Unit : �۾��� ��Ʈ��
		\return �̸�
		*/
		global_func Parse UnitName(string Unit)
		{
			int i = -1;
			while(Unit[++i] != '<');
			return Parse(Unit, i);
		}

		/*!
		\brief ����Ʈ��Ʈ�� �μ� ��������(A<B:C:D>���� B, C, D�κ�)
		\param Unit : �۾��� ��Ʈ��
		\param Order : �μ��� ������ ����
		\return �μ�
		*/
		global_func Parse UnitParam(string Unit, int Order)
		{
			int i = 0;
			while(Unit[i++] != '<');
			for(int j = 0; j < Order; ++j)
				while(Unit[i++] != ':');
			const int ibegin = i--;
			while(Unit[++i] != ':' && Unit[i] != '>');
			return Parse(&Unit[ibegin], i - ibegin);
		}

		/*!
		\brief ����Ʈ��Ʈ�� �μ����� ��������(A<B:C:D>�� ���, 3�� ����)
		\param Unit : �۾��� ��Ʈ��
		\return �μ�����
		*/
		global_func int UnitParamLength(string Unit)
		{
			int Result = 1, i = 0;
			while(Unit[i++] != '<');
			do {if(Unit[i] == ':') ++Result;}
			while(Unit[i++] != '>');
			return Result;
		}

	protected:
		int ValidCount;
		int StringLength;
		string String;
	};

public:
	/*!
	\brief �⺻������
	*/
	BxString() {Chars[0] = '\0';}

	/*!
	\brief ���������(BxString)
	\param RHS : ������ �ν��Ͻ�
	*/
	BxString(const BxString& RHS) {operator=(RHS);}

	/*!
	\brief ���������(char)
	\param RHS : ������ �ν��Ͻ�
	*/
	explicit BxString(char RHS) {operator=(RHS);}

	/*!
	\brief ���������(string)
	\param RHS : ������ �ν��Ͻ�
	*/
	explicit BxString(string RHS) {operator=(RHS);}

	/*!
	\brief ���������(sprintf)
	\param src : printf����� ��(%d/%s/...)
	\param nouse : ������ �ĺ��� ��
	\param ... : �������ڵ�
	*/
	explicit BxString(string src, void* nouse, ...)
	{
		va_list List;
		va_start(List, nouse);
		FormatV(src, List);
		va_end(List);
	}

	/*!
	\brief �Ҹ���
	*/
	virtual ~BxString() {Chars.Reset();}

public:
	/*!
	\brief ���� ����
	*/
	inline void Empty()
	{
		Chars.Reset();
		Chars[0] = '\0';
	}

	/*!
	\brief ���� ���ϱ�
	\return ��Ʈ���� ���ڱ���
	*/
	inline int GetLength()
	{
		return BxUtilGlobal::Max(0, Chars.Length() - 1);
	}

	/*!
	\brief �迭����
	\param index : �迭��ȣ
	\return char�� �ش� ������
	*/
	inline char& operator[](int Index)
	{
		global_data char NullResult = '\0';
		if(0 <= Index && Index < Chars.Length())
			return Chars[Index];
		return NullResult;
	}

	/*!
	\brief ����ȯ �ߺ��Լ�(string)
	\return string�� ��ü��Ʈ��
	*/
	inline operator string()
	{
		return (string) Chars.GetBytes();
	}

	/*!
	\brief ������ �ߺ��Լ�+(char)
	\param RHS : ���� �ǿ�����
	\return ���ο� �ν��Ͻ�
	*/
	inline BxString operator+(char RHS) const
	{
		BxString NewString = *this;
		NewString.Chars.Insert(END) = RHS;
		return NewString;
	}

	/*!
	\brief ������ �ߺ��Լ�+(string)
	\param RHS : ���� �ǿ�����
	\return ���ο� �ν��Ͻ�
	*/
	BxString operator+(string RHS) const
	{
		BxString NewString = *this;
		const int RHSLen = BxUtilGlobal::StrLen(RHS);
		for(int i = 0; i < RHSLen; ++i)
			NewString.Chars.Insert(END) = RHS[i];
		return NewString;
	}

	/*!
	\brief ������ �ߺ��Լ�+(BxString)
	\param RHS : ���� �ǿ�����
	\return ���ο� �ν��Ͻ�
	*/
	BxString operator+(BxString& RHS) const
	{
		BxString NewString = *this;
		const int RHSLen = RHS.GetLength();
		for(int i = 0; i < RHSLen; ++i)
			NewString.Chars.Insert(END) = RHS[i];
		return NewString;
	}

	/*!
	\brief ������ �ߺ��Լ�=(char)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	inline BxString& operator=(char RHS)
	{
		Empty();
		Chars.Insert(END) = RHS;
		return *this;
	}

	/*!
	\brief ������ �ߺ��Լ�=(string)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	BxString& operator=(string RHS)
	{
		Empty();
		const int RHSLen = BxUtilGlobal::StrLen(RHS);
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(i) = RHS[i];
		return *this;
	}

	/*!
	\brief ������ �ߺ��Լ�=(BxString)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	BxString& operator=(const BxString& RHS)
	{
		Empty();
		BxString* RHSPtr = (BxString*) &RHS;
		const int RHSLen = RHSPtr->GetLength();
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(i) = (*RHSPtr)[i];
		return *this;
	}

	/*!
	\brief ������ �ߺ��Լ�+=(char)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	inline BxString& operator+=(char RHS)
	{
		Chars.Insert(END) = RHS;
		return *this;
	}

	/*!
	\brief ������ �ߺ��Լ�+=(string)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	BxString& operator+=(string RHS)
	{
		const int RHSLen = BxUtilGlobal::StrLen(RHS);
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(END) = RHS[i];
		return *this;
	}

	/*!
	\brief ������ �ߺ��Լ�+=(BxString)
	\param RHS : ���� �ǿ�����
	\return �ڽ��� ����
	*/
	BxString& operator+=(BxString& RHS)
	{
		const int RHSLen = RHS.GetLength();
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(END) = RHS[i];
		return *this;
	}

	/*!
	\brief ��Ʈ������ �ʱ�ȭ
	\param Src : ��Ʈ��
	\param Length : ��Ʈ���� ����Ʈ����(-1�� ���� �ڵ�����)
	*/
	void SetString(string Src, int Length = -1)
	{
		Empty();
		if(Length == -1)
			while(*Src != '\0')
				Chars.Insert(END) = *(Src++);
		else for(int i = 0; i < Length; ++i)
			Chars.Insert(i) = Src[i];
	}

	/*!
	\brief ��������(TrimLeft + TrimRight)
	\return �ڽ��� ����
	\see TrimQuote
	*/
	BxString& TrimBlank()
	{
		while(1 < Chars.Length() && Chars[0] == ' ')
			Chars.Delete(0);
		while(1 < Chars.Length() && Chars[END - 1] == ' ')
			Chars.Delete(END - 1);
		return *this;
	}

	/*!
	\brief �ο뱸����("A"�� 'A'�� A�� ����)
	\return �ڽ��� ����
	\see TrimBlank
	*/
	BxString& TrimQuote()
	{
		if(2 < Chars.Length())
		if((Chars[0] == '\'' && Chars[END - 1] == '\'') || (Chars[0] == '\"' && Chars[END - 1] == '\"'))
		{
			Chars.Delete(0);
			Chars.Delete(END - 1);
		}
		return *this;
	}

	/*!
	\brief ���� �����ϱ�
	\param Count : ������ ����Ʈ��
	\return ���ο� �ν��Ͻ�
	\see Right
	*/
	BxString Left(int Length) const
	{
		BxString NewString;
		Length = BxUtilGlobal::Min(Length, Chars.Length() - 1);
		for(int i = 0; i < Length; ++i)
			NewString.Chars.Insert(END) = *Chars.Access(i);
		return NewString;
	}

	/*!
	\brief ���� �����ϱ�
	\param Count : ������ ����Ʈ��
	\return ���ο� �ν��Ͻ�
	\see Left
	*/
	BxString Right(int Length) const
	{
		BxString NewString;
		Length = BxUtilGlobal::Min(Length, Chars.Length() - 1);
		const int Begin = Chars.Length() - 1 - Length;
		for(int i = 0; i < Length; ++i)
			NewString.Chars.Insert(END) = *Chars.Access(Begin + i);
		return NewString;
	}

	/*!
	\brief ���� �����ϱ�
	\param Count : ������ ����Ʈ��
	\return �ڽ��� ����
	\see DeleteRight
	*/
	BxString& DeleteLeft(int Length)
	{
		Length = BxUtilGlobal::Min(Length, Chars.Length() - 1);
		for(int i = 0; i < Length; ++i)
			Chars.Delete(0);
		return *this;
	}

	/*!
	\brief ���� �����ϱ�
	\param Count : ������ ����Ʈ��
	\return �ڽ��� ����
	\see DeleteLeft
	*/
	BxString& DeleteRight(int Length)
	{
		Length = BxUtilGlobal::Min(Length, Chars.Length() - 1);
		for(int i = 0; i < Length; ++i)
			Chars.Delete(END - 1);
		return *this;
	}

	/*!
	\brief �ش� ������ Ȯ�幮�ڿ��� ��ȯ
	\param Index :�迭��ȣ
	\return Ȯ�幮�ڿ���, true�̸� 2����Ʈ����, false�̸� 1����Ʈ����
	\see IsLastCharExtend
	*/
	bool IsCharExtend(int Index)
	{
		BxAssert("BxString<��Ʈ���� ������ �ʰ��մϴ�>", 0 <= Index && Index < Chars.Length() - 1);
		return ((Chars[Index] & 0x80) == 0x80);
	}

	/*!
	\brief ������ ������ Ȯ�幮�ڿ��� ��ȯ
	\return Ȯ�幮�ڿ���, true�̸� 2����Ʈ����, false�̸� 1����Ʈ����
	\see IsCharExtend
	*/
	bool IsLastCharExtend()
	{
		bool IsExtend = false;
		const int Length = Chars.Length() - 1;
		for(int i = 0; i < Length; i += 1 + IsExtend)
			IsExtend = ((Chars[i] & 0x80) == 0x80);
		return IsExtend;
	}

	/*!
	\brief ���� ã��
	\param Key : ã�� ����
	\return �迭��ġ, -1�� ����
	*/
	int Find(char Key)
	{
		const int Count = Chars.Length() - 1;
		for(int i = 0; i < Count; ++i)
			if(Chars[i] == Key)
				return i;
		return -1;
	}

	/*!
	\brief ���ٷ� ���� ã��
	\param Key : ã�� ����
	\return �迭��ġ, -1�� ����
	*/
	int ReverseFind(char Key)
	{
		const int Count = Chars.Length() - 1;
		for(int i = Count - 1; 0 <= i; --i)
			if(Chars[i] == Key)
				return i;
		return -1;
	}

	/*!
	\brief �Ҵ�� ��ü��Ʈ�� ��ȯ
	\return string�� ��ü��Ʈ��
	*/
	inline string CloneWithAlloc()
	{
		return BxUtilGlobal::StrCpyWithAlloc((string) Chars.GetBytes());
	}

	/*!
	\brief ��Ʈ���� ��(string)
	\param Other : ���� ��Ʈ��
	\return same-����, param_is_more-�μ��� ŭ, param_is_less-�μ��� ����
	*/
	compare Compare(string Other)
	{
		const int MinLen = BxUtilGlobal::Min(Chars.Length(), BxUtilGlobal::StrLen(Other) + 1);
		for(int i = 0; i < MinLen; ++i)
		{
			if(Chars[i] == Other[i]) continue;
			return (Chars[i] < Other[i])? param_is_more : param_is_less;
		}
		return same;
	}

	/*!
	\brief ��Ʈ���� ��(BxString)
	\param Other : ���� ��Ʈ��
	\return same-����, param_is_more-�μ��� ŭ, param_is_less-�μ��� ����
	*/
	compare Compare(BxString& Other)
	{
		const int MinLen = BxUtilGlobal::Min(Chars.Length(), Other.Chars.Length());
		for(int i = 0; i < MinLen; ++i)
		{
			if(Chars[i] == Other.Chars[i]) continue;
			return (Chars[i] < Other.Chars[i])? param_is_more : param_is_less;
		}
		return same;
	}

	/*!
	\brief ��Ʈ���� ��ҹ��� ���о��� ��(string)
	\param Other : ���� ��Ʈ��
	\return same-����, param_is_more-�μ��� ŭ, param_is_less-�μ��� ����
	*/
	compare CompareNoCase(string Other)
	{
		const int MinLen = BxUtilGlobal::Min(Chars.Length(), BxUtilGlobal::StrLen(Other) + 1);
		for(int i = 0; i < MinLen; ++i)
		{
			char CharC = Chars[i];
			char CharO = Other[i];
			if('A' <= CharC && CharC <= 'Z') CharC += 'a' - 'A';
			if('A' <= CharO && CharO <= 'Z') CharO += 'a' - 'A';
			if(CharC == CharO) continue;
			return (CharC < CharO)? param_is_more : param_is_less;
		}
		return same;
	}

	/*!
	\brief ��Ʈ���� ��ҹ��� ���о��� ��(BxString)
	\param Other : ���� ��Ʈ��
	\return same-����, param_is_more-�μ��� ŭ, param_is_less-�μ��� ����
	*/
	compare CompareNoCase(BxString& Other)
	{
		const int MinLen = BxUtilGlobal::Min(Chars.Length(), Other.Chars.Length());
		for(int i = 0; i < MinLen; ++i)
		{
			char CharC = Chars[i];
			char CharO = Other.Chars[i];
			if('A' <= CharC && CharC <= 'Z') CharC += 'a' - 'A';
			if('A' <= CharO && CharO <= 'Z') CharO += 'a' - 'A';
			if(CharC == CharO) continue;
			return (CharC < CharO)? param_is_more : param_is_less;
		}
		return same;
	}

	/*!
	\brief sprintf���·� ��Ʈ�� ����
	\param format : ������ ���˽�Ʈ��
	\param ... : ������ �μ���
	*/
	void Format(string format, ...)
	{
		va_list List;
		va_start(List, format);
		FormatV(format, List);
		va_end(List);
	}

	/*!
	\brief vsprintf���·� ��Ʈ�� ����
	\param format : ������ ���˽�Ʈ��
	\param list : ������ �μ���
	*/
	void FormatV(string format, va_list list)
	{
		const int TempLen = BxCore::Util::VsnPrintf(nullptr, 0, format, list);
		BxAssert("BxString<Temp�� ������̸� ��µ� �����Ͽ����ϴ�>", 0 <= TempLen);
		global_data int TempSizeMax = 0;
		if(TempSizeMax < TempLen + 1) TempSizeMax = TempLen + 1;
		string_rw Temp = nullptr;
		BxSINGLETON(Temp, TempSizeMax);
		BxCore::Util::VsnPrintf(Temp, TempLen + 1, format, list);
		Empty();
		for(int i = 0; i < TempLen; ++i)
			Chars.Insert(i) = Temp[i];
	}

	/*!
	\brief ��Ƽ���� CSV��Ʈ��(CP949/UTF8) ������(A,"B,B",C�� A�� B,B�� C�� ����)
	\param Other : �۾��� ��Ʈ��
	\param Result : �и��� ���(���� Reset�� ����)
	\return �Ľ̵� ����
	*/
	global_func int ParseCSV(string Other, BxVar<BxString>& Result)
	{
		int iBegin = 0, iFocus = -1;
		do
		{
			++iFocus;
			// �ο뱸 ��ŵ
			if(Other[iFocus] == '\'' || Other[iFocus] == '\"')
			{
				const char QuoteCode = Other[iFocus++];
				while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n'
					&& Other[iFocus] != QuoteCode) ++iFocus;
			}
			// Ȯ�幮�� ��ŵ(UTF8���)
			else if(Other[iFocus] & 0x80)
			{
				++iFocus;
				while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n'
					&& (Other[iFocus] & 0x80)) ++iFocus;
			}
			// ��Ʈ�� ����
			if(Other[iFocus] == ',' || Other[iFocus] == '\0' || Other[iFocus] == '\r' || Other[iFocus] == '\n')
			{
				Result[LAST].SetString(&Other[iBegin], iFocus - iBegin);
				Result[END].TrimBlank(); // ��������
				Result[END].TrimQuote(); // �ο뱸����
				iBegin = iFocus + 1;
			}
		}
		while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n');
		return iFocus;
	}

	/*!
	\brief ��Ƽ���� ISO-2022��Ʈ�� ������
	\param Other : �۾��� ��Ʈ��
	\param Result : �ٹٲޱ�ȣ�� �и��� ���(���� Reset�� ����)
	*/
	global_func void ParseISO2022(string Other, BxVar<BxString>& Result)
	{
		const int OtherLength = BxUtilGlobal::StrLen(Other);
		bool IsExtendChar = false;
		Result[LAST] = "";
		for(int i = 0; i < OtherLength; ++i)
		{
			if(Other[i] == '\x1B' && i + 2 < OtherLength)
			{
				if(Other[i + 1] == '$' && Other[i + 2] == 'B') IsExtendChar = true;
				else if(Other[i + 1] == '(' && Other[i + 2] == 'B') IsExtendChar = false;
				i += 2;
			}
			else if(IsExtendChar)
			{
				Result[END] += "\x1B$B";
				Result[END] += Other[i];
				Result[END] += Other[i + 1];
				i += 1;
			}
			else if(Other[i] == '\n')
				Result[LAST] = "";
			else if(Other[i] != '\r')
				Result[END] += Other[i];
		}
	}

	/*!
	\brief ��ο��� ���ϸ� ����
	\return ����� ��Ʈ��
	*/
	inline BxString GetFileName()
	{
		return (FindPathSlash() < 0)? *this : Right(GetLength() - ReverseFindPathSlash() - 1);
	}

	/*!
	\brief ��ο��� ���ϸ��� ������ �н���(/���� ����) ����
	\return ����� ��Ʈ��
	*/
	inline BxString GetFilePath()
	{
		if(FindPathSlash() < 0) return BxString("");
		return Left(ReverseFindPathSlash() + 1);
	}

	/*!
	\brief ��ο��� ����Ÿ��Ʋ(Ȯ���� ����) ����
	\return ����� ��Ʈ��
	*/
	inline BxString GetFileTitle()
	{
		BxString FileName = GetFileName();
		return (FileName.Find('.') < 0)? FileName : FileName.Left(FileName.ReverseFind('.'));
	}

	/*!
	\brief ��ο��� ����Ȯ���� ����
	\return ����� ��Ʈ��
	*/
	inline BxString GetFileExt()
	{
		BxString FileExt((Find('.') < 0)? "" : Right(GetLength() - ReverseFind('.') - 1));
		if(0 <= FileExt.FindPathSlash()) return BxString("");
		return FileExt;
	}

	/*!
	\brief �����Ϳ��� �ε�
	\param Resource : uint16���� ��Ʈ������� ���� ����, �ش� ��Ʈ���� �ε�
	*/
	void Load(byte*& Resource)
	{
		Empty();
		const int Length = BxUtilGlobal::LoadUint16(Resource);
		for(int i = 0; i < Length; ++i)
			Chars.Insert(END) = BxUtilGlobal::LoadInt8(Resource);
	}

protected:
	/// @cond SECTION_NAME
	BxVar<char> Chars;
	/// @endcond

	/// @cond SECTION_NAME
	int FindPathSlash()
	{
		const int Count = Chars.Length() - 1;
		for(int i = 0; i < Count; ++i)
			if(Chars[i] == '/' || Chars[i] == '\\')
				return i;
		return -1;
	}
	/// @endcond

	/// @cond SECTION_NAME
	int ReverseFindPathSlash()
	{
		const int Count = Chars.Length() - 1;
		for(int i = Count - 1; 0 <= i; --i)
			if(Chars[i] == '/' || Chars[i] == '\\')
				return i;
		return -1;
	}
	/// @endcond
};
