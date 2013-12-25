#pragma once
#include <BxVar.hpp>
#include <BxUtil.hpp>
#include <BxSingleton.hpp>

//! \brief 스트링 관리
class BxString
{
public:
	//! \brief 파싱 관리
	class Parse
	{
	public:
		/*!
		\brief 기본생성자
		*/
		Parse()
		{
			ValidCount = 0;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc("", ValidCount);
		}

		/*!
		\brief 복사생성자(Parse)
		*/
		Parse(const Parse& RHS)
		{
			ValidCount = RHS.ValidCount;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(RHS.String, ValidCount);
		}

		/*!
		\brief 복사생성자(string, int)
		*/
		explicit Parse(string RHS, int Length = -1)
		{
			ValidCount = (Length < 0)? BxUtilGlobal::StrLen(RHS) : Length;
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(RHS, ValidCount);
		}

		/*!
		\brief 복사생성자(Parse*, string)
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
		\brief 복사생성자(string, BxThrow)
		*/
		explicit Parse(string RHS, const BxThrow& args)
		{
			ValidCount = 0;
			string Result = BxCore::Util::Print(RHS, args, &ValidCount);
			StringLength = ValidCount + 1;
			String = BxUtilGlobal::StrCpyWithAlloc(Result, ValidCount);
		}

		/*!
		\brief 소멸자
		*/
		~Parse()
		{
			String = BxUtilGlobal::StrFree(String);
		}

		/*!
		\brief 길이 구하기
		\return 스트링의 글자길이
		*/
		inline int GetLength()
		{
			return ValidCount;
		}

		/*!
		\brief 마지막 글자 구하기
		\return 마지막 글자
		*/
		inline char GetLast()
		{
			return String[BxUtilGlobal::Max(0, ValidCount - 1)];
		}

		/*!
		\brief 마지막 글자 지우기
		*/
		inline void DeleteLast()
		{
			ValidCount = BxUtilGlobal::Max(0, ValidCount - 1);
			((char*) String)[ValidCount] = '\0';
		}

		/*!
		\brief 연산자 중복함수+(string)
		\param RHS : 우측 피연산자
		\return 새로운 인스턴스
		*/
		Parse operator+(string RHS) const
		{
			return Parse(this, RHS);
		}

		/*!
		\brief 연산자 중복함수=(string)
		\param RHS : 우측 피연산자
		\return 자신을 리턴
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
		\brief 연산자 중복함수+=(char)
		\param RHS : 우측 피연산자
		\return 자신을 리턴
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
		\brief 형변환 중복함수(string)
		\return string형 전체스트링
		*/
		inline operator string()
		{
			return String;
		}

		/*!
		\brief 배열접근
		\param index : 배열번호
		\return char형 해당 데이터
		*/
		inline const char operator[](int Index)
		{
			if(Index < 0 || ValidCount <= Index)
				return '\0';
			return String[Index];
		}

		/*!
		\brief 파티션스트링 나누기(|A|B|C|, /A/B/C/, *A*B*C*...)
		\param Result : 나눔기호(첫바이트 자동인식)로 분리된 결과(강제 Reset은 안함)
		\param Other : 작업할 스트링
		\param Length : 작업할 스트링의 사이즈(Other의 길이로 자동셋팅 : -1)
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
		\brief 유니트스트링 이름 가져오기(A<B:C:D>에서 A부분)
		\param Unit : 작업할 스트링
		\return 이름
		*/
		global_func Parse UnitName(string Unit)
		{
			int i = -1;
			while(Unit[++i] != '<');
			return Parse(Unit, i);
		}

		/*!
		\brief 유니트스트링 인수 가져오기(A<B:C:D>에서 B, C, D부분)
		\param Unit : 작업할 스트링
		\param Order : 인수중 가져올 순번
		\return 인수
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
		\brief 유니트스트링 인수수량 가져오기(A<B:C:D>의 경우, 3을 리턴)
		\param Unit : 작업할 스트링
		\return 인수수량
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
	\brief 기본생성자
	*/
	BxString() {Chars[0] = '\0';}

	/*!
	\brief 복사생성자(BxString)
	\param RHS : 복사할 인스턴스
	*/
	BxString(const BxString& RHS) {operator=(RHS);}

	/*!
	\brief 복사생성자(char)
	\param RHS : 복사할 인스턴스
	*/
	explicit BxString(char RHS) {operator=(RHS);}

	/*!
	\brief 복사생성자(string)
	\param RHS : 복사할 인스턴스
	*/
	explicit BxString(string RHS) {operator=(RHS);}

	/*!
	\brief 복사생성자(sprintf)
	\param src : printf양식의 맵(%d/%s/...)
	\param nouse : 생성자 식별용 값
	\param ... : 가변인자들
	*/
	explicit BxString(string src, void* nouse, ...)
	{
		va_list List;
		va_start(List, nouse);
		FormatV(src, List);
		va_end(List);
	}

	/*!
	\brief 소멸자
	*/
	virtual ~BxString() {Chars.Reset();}

public:
	/*!
	\brief 내용 비우기
	*/
	inline void Empty()
	{
		Chars.Reset();
		Chars[0] = '\0';
	}

	/*!
	\brief 길이 구하기
	\return 스트링의 글자길이
	*/
	inline int GetLength()
	{
		return BxUtilGlobal::Max(0, Chars.Length() - 1);
	}

	/*!
	\brief 배열접근
	\param index : 배열번호
	\return char형 해당 데이터
	*/
	inline char& operator[](int Index)
	{
		global_data char NullResult = '\0';
		if(0 <= Index && Index < Chars.Length())
			return Chars[Index];
		return NullResult;
	}

	/*!
	\brief 형변환 중복함수(string)
	\return string형 전체스트링
	*/
	inline operator string()
	{
		return (string) Chars.GetBytes();
	}

	/*!
	\brief 연산자 중복함수+(char)
	\param RHS : 우측 피연산자
	\return 새로운 인스턴스
	*/
	inline BxString operator+(char RHS) const
	{
		BxString NewString = *this;
		NewString.Chars.Insert(END) = RHS;
		return NewString;
	}

	/*!
	\brief 연산자 중복함수+(string)
	\param RHS : 우측 피연산자
	\return 새로운 인스턴스
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
	\brief 연산자 중복함수+(BxString)
	\param RHS : 우측 피연산자
	\return 새로운 인스턴스
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
	\brief 연산자 중복함수=(char)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
	*/
	inline BxString& operator=(char RHS)
	{
		Empty();
		Chars.Insert(END) = RHS;
		return *this;
	}

	/*!
	\brief 연산자 중복함수=(string)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
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
	\brief 연산자 중복함수=(BxString)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
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
	\brief 연산자 중복함수+=(char)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
	*/
	inline BxString& operator+=(char RHS)
	{
		Chars.Insert(END) = RHS;
		return *this;
	}

	/*!
	\brief 연산자 중복함수+=(string)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
	*/
	BxString& operator+=(string RHS)
	{
		const int RHSLen = BxUtilGlobal::StrLen(RHS);
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(END) = RHS[i];
		return *this;
	}

	/*!
	\brief 연산자 중복함수+=(BxString)
	\param RHS : 우측 피연산자
	\return 자신을 리턴
	*/
	BxString& operator+=(BxString& RHS)
	{
		const int RHSLen = RHS.GetLength();
		for(int i = 0; i < RHSLen; ++i)
			Chars.Insert(END) = RHS[i];
		return *this;
	}

	/*!
	\brief 스트링으로 초기화
	\param Src : 스트링
	\param Length : 스트링의 바이트길이(-1은 길이 자동측정)
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
	\brief 공백제거(TrimLeft + TrimRight)
	\return 자신을 리턴
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
	\brief 인용구제거("A"나 'A'를 A로 변경)
	\return 자신을 리턴
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
	\brief 좌측 복사하기
	\param Count : 복사할 바이트수
	\return 새로운 인스턴스
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
	\brief 우측 복사하기
	\param Count : 복사할 바이트수
	\return 새로운 인스턴스
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
	\brief 좌측 제거하기
	\param Count : 제거할 바이트수
	\return 자신을 리턴
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
	\brief 우측 제거하기
	\param Count : 제거할 바이트수
	\return 자신을 리턴
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
	\brief 해당 글자의 확장문자여부 반환
	\param Index :배열번호
	\return 확장문자여부, true이면 2바이트문자, false이면 1바이트문자
	\see IsLastCharExtend
	*/
	bool IsCharExtend(int Index)
	{
		BxAssert("BxString<스트링의 구간을 초과합니다>", 0 <= Index && Index < Chars.Length() - 1);
		return ((Chars[Index] & 0x80) == 0x80);
	}

	/*!
	\brief 마지막 글자의 확장문자여부 반환
	\return 확장문자여부, true이면 2바이트문자, false이면 1바이트문자
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
	\brief 글자 찾기
	\param Key : 찾을 글자
	\return 배열위치, -1은 실패
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
	\brief 꺼꾸로 글자 찾기
	\param Key : 찾을 글자
	\return 배열위치, -1은 실패
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
	\brief 할당된 전체스트링 반환
	\return string형 전체스트링
	*/
	inline string CloneWithAlloc()
	{
		return BxUtilGlobal::StrCpyWithAlloc((string) Chars.GetBytes());
	}

	/*!
	\brief 스트링간 비교(string)
	\param Other : 비교할 스트링
	\return same-같음, param_is_more-인수가 큼, param_is_less-인수가 작음
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
	\brief 스트링간 비교(BxString)
	\param Other : 비교할 스트링
	\return same-같음, param_is_more-인수가 큼, param_is_less-인수가 작음
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
	\brief 스트링간 대소문자 구분없는 비교(string)
	\param Other : 비교할 스트링
	\return same-같음, param_is_more-인수가 큼, param_is_less-인수가 작음
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
	\brief 스트링간 대소문자 구분없는 비교(BxString)
	\param Other : 비교할 스트링
	\return same-같음, param_is_more-인수가 큼, param_is_less-인수가 작음
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
	\brief sprintf형태로 스트링 생성
	\param format : 생성할 포맷스트링
	\param ... : 데이터 인수들
	*/
	void Format(string format, ...)
	{
		va_list List;
		va_start(List, format);
		FormatV(format, List);
		va_end(List);
	}

	/*!
	\brief vsprintf형태로 스트링 생성
	\param format : 생성할 포맷스트링
	\param list : 데이터 인수들
	*/
	void FormatV(string format, va_list list)
	{
		const int TempLen = BxCore::Util::VsnPrintf(nullptr, 0, format, list);
		BxAssert("BxString<Temp의 예상길이를 얻는데 실패하였습니다>", 0 <= TempLen);
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
	\brief 멀티라인 CSV스트링(CP949/UTF8) 나누기(A,"B,B",C를 A와 B,B와 C로 나눔)
	\param Other : 작업할 스트링
	\param Result : 분리된 결과(강제 Reset은 안함)
	\return 파싱된 길이
	*/
	global_func int ParseCSV(string Other, BxVar<BxString>& Result)
	{
		int iBegin = 0, iFocus = -1;
		do
		{
			++iFocus;
			// 인용구 스킵
			if(Other[iFocus] == '\'' || Other[iFocus] == '\"')
			{
				const char QuoteCode = Other[iFocus++];
				while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n'
					&& Other[iFocus] != QuoteCode) ++iFocus;
			}
			// 확장문자 스킵(UTF8고려)
			else if(Other[iFocus] & 0x80)
			{
				++iFocus;
				while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n'
					&& (Other[iFocus] & 0x80)) ++iFocus;
			}
			// 스트링 저장
			if(Other[iFocus] == ',' || Other[iFocus] == '\0' || Other[iFocus] == '\r' || Other[iFocus] == '\n')
			{
				Result[LAST].SetString(&Other[iBegin], iFocus - iBegin);
				Result[END].TrimBlank(); // 공백제거
				Result[END].TrimQuote(); // 인용구제거
				iBegin = iFocus + 1;
			}
		}
		while(Other[iFocus] != '\0' && Other[iFocus] != '\r' && Other[iFocus] != '\n');
		return iFocus;
	}

	/*!
	\brief 멀티라인 ISO-2022스트링 나누기
	\param Other : 작업할 스트링
	\param Result : 줄바꿈기호로 분리된 결과(강제 Reset은 안함)
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
	\brief 경로에서 파일명 추출
	\return 추출된 스트링
	*/
	inline BxString GetFileName()
	{
		return (FindPathSlash() < 0)? *this : Right(GetLength() - ReverseFindPathSlash() - 1);
	}

	/*!
	\brief 경로에서 파일명을 제외한 패스명(/문자 포함) 추출
	\return 추출된 스트링
	*/
	inline BxString GetFilePath()
	{
		if(FindPathSlash() < 0) return BxString("");
		return Left(ReverseFindPathSlash() + 1);
	}

	/*!
	\brief 경로에서 파일타이틀(확장자 제외) 추출
	\return 추출된 스트링
	*/
	inline BxString GetFileTitle()
	{
		BxString FileName = GetFileName();
		return (FileName.Find('.') < 0)? FileName : FileName.Left(FileName.ReverseFind('.'));
	}

	/*!
	\brief 경로에서 파일확장자 추출
	\return 추출된 스트링
	*/
	inline BxString GetFileExt()
	{
		BxString FileExt((Find('.') < 0)? "" : Right(GetLength() - ReverseFind('.') - 1));
		if(0 <= FileExt.FindPathSlash()) return BxString("");
		return FileExt;
	}

	/*!
	\brief 데이터에서 로드
	\param Resource : uint16으로 스트링사이즈를 얻은 다음, 해당 스트링을 로드
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
