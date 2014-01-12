#pragma once
#include <BxMemory.hpp>
#include <BxSingleton.hpp>

//! \brief BxUtil관련 글로벌함수
namespace BxUtilGlobal
{
	/*!
	\brief 파이값 구하기
	\return 파이값(3.141592...)
	*/
	static inline fint Pi()
	{
		return (fint)(3.14159265358979323846264338327950288419716939937510 * 0x10000 + 0.5);
	}

	/*!
	\brief 밀리초에 따른 프레임수 구하기
	\return 프레임수
	*/
	static inline int Fps(const int ms)
	{
		return ms / BxCore::Main::GetCurrentFrameTime();
	}

	/*!
	\brief 최소값 구하기
	\param A : 인수A
	\param B : 인수B
	\return 최소값
	\see Max
	*/
	static inline const int Min(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & Mask) | (B & ~Mask);
	}

	/*!
	\brief 최대값 구하기
	\param A : 인수A
	\param B : 인수B
	\return 최대값
	\see Min
	*/
	static inline const int Max(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & ~Mask) | (B & Mask);
	}

	/*!
	\brief 최소최대 한계설정
	\param MinValue : 최소값
	\param Value : 입력값
	\param MaxValue : 최대값
	\return 최소최대 한계설정을 적용한 입력값
	\see Min
	*/
	static inline const int MinMax(const int MinValue, const int Value, const int MaxValue)
	{
		return Min(Max(MinValue, Value), MaxValue);
	}

	/*!
	\brief 절대값 구하기
	\param A : 인수A
	\return 절대값
	*/
	static inline int Abs(const int A)
	{
		const int Mask = A >> 31;
		return (A & ~Mask) | (-A & Mask);
	}

	/*!
	\brief 제곱값 구하기
	\param A : 인수A
	\return 제곱값
	*/
	static inline const int Pow(const int A)
	{
		return A * A;
	}

	/*!
	\brief 64비트 제곱값 구하기
	\param A : 인수A
	\return 64비트 제곱값
	*/
	static inline const huge Pow64(const int A)
	{
		return ((huge) A) * A;
	}

	/*!
	\brief 제곱근값 구하기
	\param A : 인수A
	\return 제곱근값
	*/
	static const int Sqrt(const int A)
	{
		uint CheckFlag = 0x8000, BitResult = 0, Result = 0;
		for(int i = 0; i < 16; i++)
			if(Pow(BitResult = Result | (CheckFlag >> i)) <= A)
				Result = BitResult;
		return (int) Result;
	}

	/*!
	\brief 64비트 제곱근값 구하기
	\param A : 인수A
	\return 64비트 제곱근값
	*/
	static const int Sqrt64(const huge A)
	{
		uint CheckFlag = 0x80000000, BitResult = 0, Result = 0;
		for(int i = 0; i < 32; i++)
			if(Pow64(BitResult = Result | (CheckFlag >> i)) <= A)
				Result = BitResult;
		return (int) Result;
	}

	/*!
	\brief 스트링길이 구하기
	\param String : 스트링
	\return 스트링의 바이트길이
	*/
	static int StrLen(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0');
		return i;
	}

	/*!
	\brief 템플릿타입 스트링길이 구하기
	\param String : 스트링
	\return 스트링의 배열수량
	*/
	template<typename TYPE>
	static int StrLenT(const TYPE* String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i]);
		return i;
	}

	/*!
	\brief 확장된 스트링길이 구하기(\r과 \n도 인식)
	\param String : 스트링
	\return 스트링의 바이트길이
	*/
	static int StrLenEx(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != '\r' && String[i] != '\n');
		return i;
	}

	/*!
	\brief 확장된 스트링길이 구하기(사용자의 끝문자)
	\param String : 스트링
	\param ExitCode : 종료문자
	\return 스트링의 바이트길이
	*/
	static int StrLenEx(string String, char ExitCode)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != ExitCode);
		return i;
	}

	/*!
	\brief 스트링 할당
	\param SrcLength : 할당할 사이즈(NULL문자가 더해짐)
	\see StrFree
	*/
	static string StrAlloc(int SrcLength)
	{
		string_rw AllocedString = (string_rw) BxCore::Util::Malloc(SrcLength + 1);
		AllocedString[0] = '\0';
		AllocedString[SrcLength] = '\0';
		return AllocedString;
	}

	/*!
	\brief 스트링 해제
	\param AllocedString : 할당된 스트링
	\return 간편한 사용을 위한 nullptr값 리턴
	\see StrAlloc, StrCpyWithAlloc
	*/
	static string StrFree(string AllocedString)
	{
		BxCore::Util::Free((id_memory) AllocedString);
		return nullptr;
	}

	/*!
	\brief 스트링 복사하기
	\param DstString : 복사할 스트링(restrict속성)
	\param SrcString : 가져올 스트링(restrict속성)
	\param SrcLength : 복사될 사이즈(StringSrc의 길이로 자동셋팅 : -1)
	\return 복사된 사이즈(NULL문자 제외)
	*/
	static int StrCpy(string_rw_prm DstString, string_prm SrcString, int SrcLength = -1)
	{
		const int SrcLen = (SrcLength < 0)? StrLen(SrcString) : SrcLength;
		BxCore::Util::MemMove(DstString, SrcString, SrcLen);
		DstString[SrcLen] = '\0';
		return SrcLen;
	}

	/*!
	\brief 스트링 할당 및 복사하기
	\param SrcString : 가져올 스트링
	\param SrcLength : 복사될 사이즈(StringSrc의 길이로 자동셋팅 : -1)
	\return 할당 및 복사된 스트링
	\see StrFree
	*/
	static string StrCpyWithAlloc(string SrcString, int SrcLength = -1)
	{
		const int SrcLen = (SrcLength < 0)? StrLen(SrcString) : SrcLength;
		string_rw AllocedString = (string_rw) BxCore::Util::Malloc(SrcLen + 1);
		BxCore::Util::MemMove(AllocedString, SrcString, SrcLen);
		AllocedString[SrcLen] = '\0';
		return AllocedString;
	}

	/*!
	\brief 스트링 비교하기
	\param StringA : 스트링A(restrict속성)
	\param StringB : 스트링B(restrict속성)
	\param FixedLength : 고정식 비교구간(사용안함 : -1)
	\return same-같음, b_is_more-스트링B가 큼, b_is_less-스트링B가 작음
	*/
	static compare StrCmp(string_prm StringA, string_prm StringB, int FixedLength = -1)
	{
		if(FixedLength < 0)
		{
			do
			{
				if(*StringA != *StringB)
					return (*StringA < *StringB)? b_is_more : b_is_less;
			}
			while(*(++StringA) | *(++StringB));
		}
		else for(int i = 0; i < FixedLength; ++i)
			if(StringA[i] != StringB[i])
				return (StringA[i] < StringB[i])? b_is_more : b_is_less;
		return same;
	}

	/*!
	\brief 대소문자 구분없이 스트링 비교하기
	\param StringA : 스트링A(restrict속성)
	\param StringB : 스트링B(restrict속성)
	\param FixedLength : 고정식 비교구간(사용안함 : -1)
	\return same-같음, b_is_more-스트링B가 큼, b_is_less-스트링B가 작음
	*/
	static compare StrICmp(string_prm StringA, string_prm StringB, int FixedLength = -1)
	{
		if(FixedLength < 0)
		{
			do
			{
				char CharA = *StringA, CharB = *StringB;
				if('A' <= CharA && CharA <= 'Z') CharA += 'a' - 'A';
				if('A' <= CharB && CharB <= 'Z') CharB += 'a' - 'A';
				if(CharA != CharB)
					return (CharA < CharB)? b_is_more : b_is_less;
			}
			while(*(++StringA) | *(++StringB));
		}
		else for(int i = 0; i < FixedLength; ++i)
		{
			char CharA = StringA[i], CharB = StringB[i];
			if('A' <= CharA && CharA <= 'Z') CharA += 'a' - 'A';
			if('A' <= CharB && CharB <= 'Z') CharB += 'a' - 'A';
			if(CharA != CharB)
				return (CharA < CharB)? b_is_more : b_is_less;
		}
		return same;
	}

	/*!
	\brief 스트링간 동일구역의 길이 구하기
	\param StringA : 스트링A(restrict속성)
	\param StringB : 스트링B(restrict속성)
	\return 바이트길이
	*/
	static int StrSameCount(string_prm StringA, string_prm StringB)
	{
		int Focus = 0;
		while(StringA[Focus] != '\0' && StringA[Focus] == StringB[Focus])
			++Focus;
		return Focus;
	}

	/*!
	\brief 빠른 스트링비교를 위한 해시코드 만들기
	\param String : 스트링
	\param SrcLength : 스트링 사이즈(String 길이로 자동셋팅 : -1)
	\return 해시코드
	*/
	static uint StrHash(string String, int SrcLength = -1)
	{
		char Hash[4] = {'\xFF', '\xFF', '\xFF', '\xFF'};
		if(SrcLength == -1)
		{
			--String;
			for(int i = 0; *(++String) != '\0'; ++i)
				Hash[i & 3] ^= *String;
		}
		else for(int i = 0; i < SrcLength; ++i)
			Hash[i & 3] ^= String[i];
		return *((uint*) Hash);
	}

	/*!
	\brief 스트링검색
	\param DstString : 대상스트링
	\param SrcString : 찾을 스트링
	\param SrcSearchMap : 찾을 스트링의 검색맵
	\param DstLength : 대상스트링 사이즈(DstString 길이로 자동셋팅 : -1)
	\return 찾은 위치(바이트단위), 검색실패시 -1
	*/
	static int StrSearch(string DstString, string SrcString, const int (&SrcSearchMap)[256], int DstLength = -1)
	{
		if(DstLength == -1)
			DstLength = StrLen(DstString);
		const int SrcLength = SrcSearchMap[0];
		const char SrcTail = SrcString[SrcLength - 1];
		int DstFocus = SrcLength - 1;
		while(DstFocus < DstLength)
		{
			if(DstString[DstFocus] == SrcTail)
			{
				const int Result = DstFocus + 1 - SrcLength;
				if(!BxCore::Util::MemCmp(&DstString[Result], SrcString, SrcLength))
					return Result;
			}
			DstFocus += SrcSearchMap[(int) DstString[DstFocus]];
		}
		return -1;
	}

	/*!
	\brief 스트링검색을 위한 검색맵 만들기
	\param String : 스트링
	\param SearchMap : 만들어질 검색맵
	*/
	static void StrSearchMap(string String, int (_out_ SearchMap)[256])
	{
		BxCore::Util::MemSet(SearchMap, 0, sizeof(int) * 256);
		int Focus = 0;
		while(String[++Focus]) SearchMap[(int) String[Focus - 1]] = Focus;
		for(int i = 0; i < 256; ++i)
			SearchMap[i] = Focus - SearchMap[i];
	}

	/*!
	\brief 해당 스트링이 숫자인지의 여부
	\param String : 스트링
	\param SrcLength : 스트링 사이즈(String 길이로 자동셋팅 : -1)
	\return true-숫자, false-숫자아님
	*/
	static bool IsNumeric(string String, int StringLength = -1)
	{
		const int StringLen = (StringLength < 0)? StrLenEx(String) : StringLength;
		for(int i = 0; i < StringLen; ++i)
		{
			if('0' <= String[i] && String[i] <= '9') continue;
			else if(i == 0 && (String[i] == '-' || String[i] == '+')) continue;
			return false;
		}
		return true;
	}

	/*!
	\brief 스트링을 정수로 변환
	\param String : 스트링
	\param StringLength : 처리될 사이즈(String의 길이로 자동셋팅 : -1)
	\return 변환된 정수
	*/
	static int AtoI(string String, int StringLength = -1)
	{
		int IntValue = 0, IsMinus = -1;
		const int StringLen = (StringLength < 0)? StrLenEx(String) : StringLength;
		for(int i = 0; i < StringLen; ++i)
		{
			if('0' <= String[i] && String[i] <= '9')
			{
				IntValue *= 10;
				IntValue += String[i] - '0';
				if(IsMinus == -1) IsMinus = 0;
			}
			else if(IsMinus == -1 && String[i] == '-')
				IsMinus = 1;
			else if(IsMinus == -1 && String[i] == '+')
				IsMinus = 0;
			else if(String[i] != ' ' && String[i] != '\t')
				break;
		}
		return (IsMinus == 1)? -IntValue : IntValue;
	}

	/*!
	\brief 정수를 스트링으로 변환
	\param Value : 정수
	\return 변환된 스트링
	*/
	static string _tmp_ ItoA(int Value)
	{
		const int Length = 255;
		global_data char Result[Length + 1];
		Result[Length] = '\0';
		const bool IsMinus = (Value < 0);
		uint UValue = (uint)((IsMinus)? -Value : Value);
		for(int i = Length - 1; 0 <= i; --i)
		{
			Result[i] = '0' + (UValue % 10);
			UValue /= 10;
			if(UValue == 0)
			{
				if(IsMinus)
				{
					Result[i - 1] = '-';
					return &Result[i - 1];
				}
				else return &Result[i];
			}
		}
		return &Result[Length];
	}

	/*!
	\brief 헥사덤프를 양수로 변환
	\param Hex : 헥사덤프
	\param HexLength : 처리될 사이즈(String의 길이로 자동셋팅 : -1)
	\return 변환된 양수
	*/
	static uint HtoI(string Hex, int HexLength = -1)
	{
		uint UintValue = 0;
		const int StringLen = (HexLength < 0)? StrLenEx(Hex) : HexLength;
		for(int i = 0; i < StringLen; ++i)
		{
			if('0' <= Hex[i] && Hex[i] <= '9')
			{
				UintValue <<= 4;
				UintValue |= Hex[i] - '0';
			}
			else if('a' <= Hex[i] && Hex[i] <= 'f')
			{
				UintValue <<= 4;
				UintValue |= Hex[i] - 'a' + 10;
			}
			else if('A' <= Hex[i] && Hex[i] <= 'F')
			{
				UintValue <<= 4;
				UintValue |= Hex[i] - 'A' + 10;
			}
		}
		return UintValue;
	}

	/*!
	\brief 양수를 헥사덤프로 변환
	\param Value : 양수
	\param IsUpper : 대문자여부(true-대문자, false-소문자)
	\return 변환된 헥사덤프
	*/
	static string _tmp_ ItoH(uint Value, bool IsUpper)
	{
		const int Length = 255;
		global_data char Result[Length + 1];
		Result[Length] = '\0';
		for(int i = Length - 1; 0 <= i; --i)
		{
			if((Value & 0xF) < 10)
				Result[i] = '0' + (Value & 0xF);
			else Result[i] = ((IsUpper)? 'A' : 'a') + (Value & 0xF) - 10;
			Value >>= 4;
			if(Value == 0)
				return &Result[i];
		}
		return &Result[Length];
	}

	/*!
	\brief 데이터에서 로드(char)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline char LoadInt8(byte*& Resource)
	{
		char Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief 데이터에서 로드(short)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline short LoadInt16(byte*& Resource)
	{
		short Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief 데이터에서 로드(int)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline int LoadInt32(byte*& Resource)
	{
		int Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief 데이터에서 로드(byte)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline byte LoadUint8(byte*& Resource)
	{
		byte Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief 데이터에서 로드(ushort)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline ushort LoadUint16(byte*& Resource)
	{
		ushort Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief 데이터에서 로드(uint)
	\param Resource : 읽을 데이터
	\return 로드한 값
	*/
	static inline uint LoadUint32(byte*& Resource)
	{
		uint Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief 데이터에서 바이트배열 로드(uint)
	\param Resource : 읽을 데이터
	\param Dest : 쓸 데이터
	\param Length : 쓸 바이트길이
	*/
	static inline void LoadBytes(byte*& Resource, void* Dest, uint Length)
	{
		BxCore::Util::MemMove(Dest, Resource, Length);
		Resource += Length;
	}

	/*!
	\brief 데이터에서 비트배열 로드(uint)
	\param Resource : 읽을 데이터
	\param CurrentBit : 현재 비트
	\param BitCount : 읽을 비트길이
	\return 로드한 값
	*/
	static uint LoadBits(byte*& Resource, int& CurrentBit, const int BitCount)
	{
		uint Result = 0;
		for(int i = 0; i < BitCount; ++i)
		{
			Result |= ((*Resource >> CurrentBit) & 0x1) << i;
			if(!(++CurrentBit & 0x7))
			{
				CurrentBit = 0;
				++Resource;
			}
		}
		return Result;
	}

	/*!
	\brief 데이터에서 임의배열 로드
	\param Resource : 읽을 데이터
	\param TypeLen : 읽을 바이트길이
	\return 로드한 데이터
	*/
	static inline byte* LoadType(byte*& Resource, uint TypeLen)
	{
		byte* Result = Resource;
		Resource += TypeLen;
		return Result;
	}

	/*!
	\brief 데이터에서 스트링 로드
	\param Resource : uint16으로 스트링사이즈를 얻은 다음, 해당 스트링을 로드
	\return 로드한 스트링
	*/
	static inline string LoadString(byte*& Resource)
	{
		int Length = LoadInt16(Resource);
		string_rw Chars = (string_rw) StrAlloc(Length);
		LoadBytes(Resource, Chars, Length);
		Chars[Length] = '\0';
		return Chars;
	}

	/*!
	\brief 상수나열로 배열을 초기화
	\param Dst : 초기화할 배열
	\param Count : 상수나열길이
	\param Value : 첫번째 상수값
	\param ... : 상수값들
	*/
	template<typename TYPE>
	static inline void SetArray(TYPE* Dst, const BxThrow& Src)
	{
		for(int i = 0, iend = Src.Length(); i < iend; ++i)
			Dst[i] = *Src.Access<TYPE>(i);
	}
}

//! \brief 각종 수학/물리모듈 제공
class BxUtil
{
public:
	/*!
	\brief 랜덤함수 초기화
	\param Seed : 초기화값
	\see BxCore::System::GetTimerMs
	*/
	global_func void SetRandom(const uint Seed)
	{
		_SetRand(Seed);
		for(int i = 0; i < 256; ++i)
			RandomTable(i) = (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
	}

	/*!
	\brief 랜덤값 구하기
	\param Key : 랜덤테이블 인덱스값, -1은 실 랜덤값
	\return 결과값
	*/
	global_func inline uint GetRandom(const int Key = -1)
	{
		if(Key < 0) return (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
		return RandomTable(Key & 0xFF);
	}

	/*!
	\brief 구간내 랜덤값 구하기
	\param MinValue : 최소값
	\param MaxValue : 최대값
	\param Key : 랜덤테이블 인덱스값, -1은 실 랜덤값
	\return 최소값(포함)에서 최대값(포함)사이의 결과값
	*/
	global_func inline int GetValue(const int MinValue, const int MaxValue, int Key = -1)
	{
		return MinValue + (GetRandom(Key) % (MaxValue - MinValue + 1));
	}

	/*!
	\brief 각도내 랜덤값 구하기
	\param MinValue : 최소각도값
	\param MaxValue : 최대각도값
	\param Key : 랜덤테이블 인덱스값, -1은 실 랜덤값
	\return 최소값(포함)에서 최대값(포함)사이의 결과값
	*/
	global_func inline int GetValueAngle(const int MinValue, const int MaxValue, int Key = -1)
	{
		if(180 < MaxValue - MinValue)
			return (MaxValue + (GetRandom(Key) % (360 - MaxValue + MinValue + 1))) % 360;
		return GetValue(MinValue, MaxValue, Key) % 360;
	}

	/*!
	\brief Cos값 구하기
	\param Angle1024 : 각도값
	\return Cos값
	*/
	global_func inline fint Cos(int Angle1024)
	{
		Angle1024 = Angle1024 & 0x3FF;
		return BxCore::Util::Cos1024Table()[Angle90 - BxUtilGlobal::Abs((Angle1024 & 0x1FF) - Angle90)]
			* ((((Angle1024 + Angle270 - 1) & 0x3FF) < Angle180)? -1 : 1);
	}

	/*!
	\brief Sin값 구하기
	\param Angle1024 : 각도값
	\return Sin값
	*/
	global_func inline fint Sin(int Angle1024)
	{
		return Cos(Angle1024 + Angle270);
	}

	/*!
	\brief 방향으로 각도 구하기
	\param VectorX : 방향X
	\param VectorY : 방향Y
	\return 각도값
	*/
	global_func int GetAngle1024(const int VectorX, const int VectorY)
	{
		if(VectorX == 0) return (VectorY < 0)? Angle270 : Angle90;
		int Angle1024 = -1, Tan1024 = BxUtilGlobal::Abs((int)((((huge) VectorY) << 16) / ((huge) VectorX)));
		while(++Angle1024 < Angle90 && BxCore::Util::Tan1024Table()[Angle1024] <= Tan1024);
		return 0x3FF & (((VectorY < 0)? Angle180 : 0)
			+ (((VectorX < 0) == (VectorY < 0))? Angle1024 : Angle180 - Angle1024));
	}

	/*!
	\brief 점 3개의 방향성 값을 확인
	\param Start : 첫점
	\param End : 둘째점
	\param Src : 비교할 셋째점
	\return 음수 : 시계방향, 양수 : 반시계방향, 0 : 중립
	*/
	global_func inline const int GetClockwiseValue(const point& Start, const point& End, const point& Src)
	{
		return (Start.x - End.x) * (Src.y - End.y) - (Start.y - End.y) * (Src.x - End.x);
	}

	/*!
	\brief 단위행렬화
	\param Dst : 적용될 행렬
	*/
	global_func inline void MatrixInit(matrix _out_ Dst)
	{
		BxCore::Util::MemSet(&Dst, 0, sizeof(matrix));
		Dst.m[0][0] = Dst.m[1][1] = Dst.m[2][2] = Dst.m[3][3] = ItoF(1);
	}

	/*!
	\brief 행렬에 행렬곱하기
	\param Dst : 적용될 행렬
	\param Src : 곱할 행렬
	*/
	global_func inline void MatrixCombine(matrix _inout_ Dst, const matrix _in_ Src)
	{
		matrix Result;
		BxCore::Util::MemSet(&Result, 0, sizeof(matrix));
		Result.m[0][0] += (Dst.m[0][0] * (huge) Src.m[0][0]) >> 16;
		Result.m[0][0] += (Dst.m[0][1] * (huge) Src.m[1][0]) >> 16;
		Result.m[0][0] += (Dst.m[0][2] * (huge) Src.m[2][0]) >> 16;
		Result.m[0][0] += (Dst.m[0][3] * (huge) Src.m[3][0]) >> 16;
		Result.m[0][1] += (Dst.m[0][0] * (huge) Src.m[0][1]) >> 16;
		Result.m[0][1] += (Dst.m[0][1] * (huge) Src.m[1][1]) >> 16;
		Result.m[0][1] += (Dst.m[0][2] * (huge) Src.m[2][1]) >> 16;
		Result.m[0][1] += (Dst.m[0][3] * (huge) Src.m[3][1]) >> 16;
		Result.m[0][2] += (Dst.m[0][0] * (huge) Src.m[0][2]) >> 16;
		Result.m[0][2] += (Dst.m[0][1] * (huge) Src.m[1][2]) >> 16;
		Result.m[0][2] += (Dst.m[0][2] * (huge) Src.m[2][2]) >> 16;
		Result.m[0][2] += (Dst.m[0][3] * (huge) Src.m[3][2]) >> 16;
		Result.m[0][3] += (Dst.m[0][0] * (huge) Src.m[0][3]) >> 16;
		Result.m[0][3] += (Dst.m[0][1] * (huge) Src.m[1][3]) >> 16;
		Result.m[0][3] += (Dst.m[0][2] * (huge) Src.m[2][3]) >> 16;
		Result.m[0][3] += (Dst.m[0][3] * (huge) Src.m[3][3]) >> 16;
		Result.m[1][0] += (Dst.m[1][0] * (huge) Src.m[0][0]) >> 16;
		Result.m[1][0] += (Dst.m[1][1] * (huge) Src.m[1][0]) >> 16;
		Result.m[1][0] += (Dst.m[1][2] * (huge) Src.m[2][0]) >> 16;
		Result.m[1][0] += (Dst.m[1][3] * (huge) Src.m[3][0]) >> 16;
		Result.m[1][1] += (Dst.m[1][0] * (huge) Src.m[0][1]) >> 16;
		Result.m[1][1] += (Dst.m[1][1] * (huge) Src.m[1][1]) >> 16;
		Result.m[1][1] += (Dst.m[1][2] * (huge) Src.m[2][1]) >> 16;
		Result.m[1][1] += (Dst.m[1][3] * (huge) Src.m[3][1]) >> 16;
		Result.m[1][2] += (Dst.m[1][0] * (huge) Src.m[0][2]) >> 16;
		Result.m[1][2] += (Dst.m[1][1] * (huge) Src.m[1][2]) >> 16;
		Result.m[1][2] += (Dst.m[1][2] * (huge) Src.m[2][2]) >> 16;
		Result.m[1][2] += (Dst.m[1][3] * (huge) Src.m[3][2]) >> 16;
		Result.m[1][3] += (Dst.m[1][0] * (huge) Src.m[0][3]) >> 16;
		Result.m[1][3] += (Dst.m[1][1] * (huge) Src.m[1][3]) >> 16;
		Result.m[1][3] += (Dst.m[1][2] * (huge) Src.m[2][3]) >> 16;
		Result.m[1][3] += (Dst.m[1][3] * (huge) Src.m[3][3]) >> 16;
		Result.m[2][0] += (Dst.m[2][0] * (huge) Src.m[0][0]) >> 16;
		Result.m[2][0] += (Dst.m[2][1] * (huge) Src.m[1][0]) >> 16;
		Result.m[2][0] += (Dst.m[2][2] * (huge) Src.m[2][0]) >> 16;
		Result.m[2][0] += (Dst.m[2][3] * (huge) Src.m[3][0]) >> 16;
		Result.m[2][1] += (Dst.m[2][0] * (huge) Src.m[0][1]) >> 16;
		Result.m[2][1] += (Dst.m[2][1] * (huge) Src.m[1][1]) >> 16;
		Result.m[2][1] += (Dst.m[2][2] * (huge) Src.m[2][1]) >> 16;
		Result.m[2][1] += (Dst.m[2][3] * (huge) Src.m[3][1]) >> 16;
		Result.m[2][2] += (Dst.m[2][0] * (huge) Src.m[0][2]) >> 16;
		Result.m[2][2] += (Dst.m[2][1] * (huge) Src.m[1][2]) >> 16;
		Result.m[2][2] += (Dst.m[2][2] * (huge) Src.m[2][2]) >> 16;
		Result.m[2][2] += (Dst.m[2][3] * (huge) Src.m[3][2]) >> 16;
		Result.m[2][3] += (Dst.m[2][0] * (huge) Src.m[0][3]) >> 16;
		Result.m[2][3] += (Dst.m[2][1] * (huge) Src.m[1][3]) >> 16;
		Result.m[2][3] += (Dst.m[2][2] * (huge) Src.m[2][3]) >> 16;
		Result.m[2][3] += (Dst.m[2][3] * (huge) Src.m[3][3]) >> 16;
		Result.m[3][0] += (Dst.m[3][0] * (huge) Src.m[0][0]) >> 16;
		Result.m[3][0] += (Dst.m[3][1] * (huge) Src.m[1][0]) >> 16;
		Result.m[3][0] += (Dst.m[3][2] * (huge) Src.m[2][0]) >> 16;
		Result.m[3][0] += (Dst.m[3][3] * (huge) Src.m[3][0]) >> 16;
		Result.m[3][1] += (Dst.m[3][0] * (huge) Src.m[0][1]) >> 16;
		Result.m[3][1] += (Dst.m[3][1] * (huge) Src.m[1][1]) >> 16;
		Result.m[3][1] += (Dst.m[3][2] * (huge) Src.m[2][1]) >> 16;
		Result.m[3][1] += (Dst.m[3][3] * (huge) Src.m[3][1]) >> 16;
		Result.m[3][2] += (Dst.m[3][0] * (huge) Src.m[0][2]) >> 16;
		Result.m[3][2] += (Dst.m[3][1] * (huge) Src.m[1][2]) >> 16;
		Result.m[3][2] += (Dst.m[3][2] * (huge) Src.m[2][2]) >> 16;
		Result.m[3][2] += (Dst.m[3][3] * (huge) Src.m[3][2]) >> 16;
		Result.m[3][3] += (Dst.m[3][0] * (huge) Src.m[0][3]) >> 16;
		Result.m[3][3] += (Dst.m[3][1] * (huge) Src.m[1][3]) >> 16;
		Result.m[3][3] += (Dst.m[3][2] * (huge) Src.m[2][3]) >> 16;
		Result.m[3][3] += (Dst.m[3][3] * (huge) Src.m[3][3]) >> 16;
		BxCore::Util::MemMove(&Dst, &Result, sizeof(matrix));
	}

	/*!
	\brief 행렬에 행렬곱하기
	\param Dst : 적용될 행렬
	\param Src00 : 곱할 행렬
	*/
	global_func inline void MatrixCombine(matrix _inout_ Dst,
		const fint _in_ Src00, const fint _in_ Src10, const fint _in_ Src20, const fint _in_ Src30,
		const fint _in_ Src01, const fint _in_ Src11, const fint _in_ Src21, const fint _in_ Src31,
		const fint _in_ Src02, const fint _in_ Src12, const fint _in_ Src22, const fint _in_ Src32,
		const fint _in_ Src03, const fint _in_ Src13, const fint _in_ Src23, const fint _in_ Src33)
	{
		matrix Result;
		BxCore::Util::MemSet(&Result, 0, sizeof(matrix));
		Result.m[0][0] += (Dst.m[0][0] * (huge) Src00) >> 16;
		Result.m[0][0] += (Dst.m[0][1] * (huge) Src10) >> 16;
		Result.m[0][0] += (Dst.m[0][2] * (huge) Src20) >> 16;
		Result.m[0][0] += (Dst.m[0][3] * (huge) Src30) >> 16;
		Result.m[0][1] += (Dst.m[0][0] * (huge) Src01) >> 16;
		Result.m[0][1] += (Dst.m[0][1] * (huge) Src11) >> 16;
		Result.m[0][1] += (Dst.m[0][2] * (huge) Src21) >> 16;
		Result.m[0][1] += (Dst.m[0][3] * (huge) Src31) >> 16;
		Result.m[0][2] += (Dst.m[0][0] * (huge) Src02) >> 16;
		Result.m[0][2] += (Dst.m[0][1] * (huge) Src12) >> 16;
		Result.m[0][2] += (Dst.m[0][2] * (huge) Src22) >> 16;
		Result.m[0][2] += (Dst.m[0][3] * (huge) Src32) >> 16;
		Result.m[0][3] += (Dst.m[0][0] * (huge) Src03) >> 16;
		Result.m[0][3] += (Dst.m[0][1] * (huge) Src13) >> 16;
		Result.m[0][3] += (Dst.m[0][2] * (huge) Src23) >> 16;
		Result.m[0][3] += (Dst.m[0][3] * (huge) Src33) >> 16;
		Result.m[1][0] += (Dst.m[1][0] * (huge) Src00) >> 16;
		Result.m[1][0] += (Dst.m[1][1] * (huge) Src10) >> 16;
		Result.m[1][0] += (Dst.m[1][2] * (huge) Src20) >> 16;
		Result.m[1][0] += (Dst.m[1][3] * (huge) Src30) >> 16;
		Result.m[1][1] += (Dst.m[1][0] * (huge) Src01) >> 16;
		Result.m[1][1] += (Dst.m[1][1] * (huge) Src11) >> 16;
		Result.m[1][1] += (Dst.m[1][2] * (huge) Src21) >> 16;
		Result.m[1][1] += (Dst.m[1][3] * (huge) Src31) >> 16;
		Result.m[1][2] += (Dst.m[1][0] * (huge) Src02) >> 16;
		Result.m[1][2] += (Dst.m[1][1] * (huge) Src12) >> 16;
		Result.m[1][2] += (Dst.m[1][2] * (huge) Src22) >> 16;
		Result.m[1][2] += (Dst.m[1][3] * (huge) Src32) >> 16;
		Result.m[1][3] += (Dst.m[1][0] * (huge) Src03) >> 16;
		Result.m[1][3] += (Dst.m[1][1] * (huge) Src13) >> 16;
		Result.m[1][3] += (Dst.m[1][2] * (huge) Src23) >> 16;
		Result.m[1][3] += (Dst.m[1][3] * (huge) Src33) >> 16;
		Result.m[2][0] += (Dst.m[2][0] * (huge) Src00) >> 16;
		Result.m[2][0] += (Dst.m[2][1] * (huge) Src10) >> 16;
		Result.m[2][0] += (Dst.m[2][2] * (huge) Src20) >> 16;
		Result.m[2][0] += (Dst.m[2][3] * (huge) Src30) >> 16;
		Result.m[2][1] += (Dst.m[2][0] * (huge) Src01) >> 16;
		Result.m[2][1] += (Dst.m[2][1] * (huge) Src11) >> 16;
		Result.m[2][1] += (Dst.m[2][2] * (huge) Src21) >> 16;
		Result.m[2][1] += (Dst.m[2][3] * (huge) Src31) >> 16;
		Result.m[2][2] += (Dst.m[2][0] * (huge) Src02) >> 16;
		Result.m[2][2] += (Dst.m[2][1] * (huge) Src12) >> 16;
		Result.m[2][2] += (Dst.m[2][2] * (huge) Src22) >> 16;
		Result.m[2][2] += (Dst.m[2][3] * (huge) Src32) >> 16;
		Result.m[2][3] += (Dst.m[2][0] * (huge) Src03) >> 16;
		Result.m[2][3] += (Dst.m[2][1] * (huge) Src13) >> 16;
		Result.m[2][3] += (Dst.m[2][2] * (huge) Src23) >> 16;
		Result.m[2][3] += (Dst.m[2][3] * (huge) Src33) >> 16;
		Result.m[3][0] += (Dst.m[3][0] * (huge) Src00) >> 16;
		Result.m[3][0] += (Dst.m[3][1] * (huge) Src10) >> 16;
		Result.m[3][0] += (Dst.m[3][2] * (huge) Src20) >> 16;
		Result.m[3][0] += (Dst.m[3][3] * (huge) Src30) >> 16;
		Result.m[3][1] += (Dst.m[3][0] * (huge) Src01) >> 16;
		Result.m[3][1] += (Dst.m[3][1] * (huge) Src11) >> 16;
		Result.m[3][1] += (Dst.m[3][2] * (huge) Src21) >> 16;
		Result.m[3][1] += (Dst.m[3][3] * (huge) Src31) >> 16;
		Result.m[3][2] += (Dst.m[3][0] * (huge) Src02) >> 16;
		Result.m[3][2] += (Dst.m[3][1] * (huge) Src12) >> 16;
		Result.m[3][2] += (Dst.m[3][2] * (huge) Src22) >> 16;
		Result.m[3][2] += (Dst.m[3][3] * (huge) Src32) >> 16;
		Result.m[3][3] += (Dst.m[3][0] * (huge) Src03) >> 16;
		Result.m[3][3] += (Dst.m[3][1] * (huge) Src13) >> 16;
		Result.m[3][3] += (Dst.m[3][2] * (huge) Src23) >> 16;
		Result.m[3][3] += (Dst.m[3][3] * (huge) Src33) >> 16;
		BxCore::Util::MemMove(&Dst, &Result, sizeof(matrix));
	}

	/*!
	\brief 행렬의 회전변환(쿼터니언)
	\param Dst : 적용될 행렬
	\param Src : 회전할 쿼터니언
	*/
	global_func inline void MatrixRotateQ(matrix _inout_ Dst, const quater _in_ Src)
	{
		const huge Hx = ItoF(Src.x), Hy = ItoF(Src.y), Hz = ItoF(Src.z);
		const fint Scalar = (fint) BxUtilGlobal::Sqrt64(Hx * Hx + Hy * Hy + Hz * Hz);
		const huge x = Hx * BxUtil::Sin(Src.angle1024 / 2) / Scalar;
		const huge y = Hy * BxUtil::Sin(Src.angle1024 / 2) / Scalar;
		const huge z = Hz * BxUtil::Sin(Src.angle1024 / 2) / Scalar;
		const huge w = BxUtil::Cos(Src.angle1024 / 2);
		const fint xx = (fint) ((x * x) >> 16), yy = (fint) ((y * y) >> 16), zz = (fint) ((z * z) >> 16);
		const fint xy = (fint) ((x * y) >> 16), yz = (fint) ((y * z) >> 16), zx = (fint) ((z * x) >> 16);
		const fint xw = (fint) ((x * w) >> 16), yw = (fint) ((y * w) >> 16), zw = (fint) ((z * w) >> 16);
		return MatrixCombine(Dst,
			ItoF(1) - 2 * (yy + zz),           2 * (xy + zw),           2 * (zx - yw), 0,
			          2 * (xy - zw), ItoF(1) - 2 * (xx + zz),           2 * (yz + xw), 0,
			          2 * (zx + yw),           2 * (yz - xw), ItoF(1) - 2 * (xx + yy), 0,
			0,                       0,                       0,                       ItoF(1));
	}

	/*!
	\brief 버텍스에 행렬곱하기
	\param Dst : 적용될 버텍스
	\param Src : 곱할 행렬
	*/
	global_func inline void MatrixApply(vertex _inout_ Dst, const matrix _in_ Src)
	{
		vertex Result;
		Result.x = FtoI(Src.m[0][0] * Dst.x + Src.m[0][1] * Dst.y + Src.m[0][2] * Dst.z + Src.m[0][3]);
		Result.y = FtoI(Src.m[1][0] * Dst.x + Src.m[1][1] * Dst.y + Src.m[1][2] * Dst.z + Src.m[1][3]);
		Result.z = FtoI(Src.m[2][0] * Dst.x + Src.m[2][1] * Dst.y + Src.m[2][2] * Dst.z + Src.m[2][3]);
		BxCore::Util::MemMove(&Dst, &Result, sizeof(vertex));
	}

	/*!
	\brief CP949를 UTF16으로 전환
	\param cp949 : 변환할 CP949스트링
	\param cp949length : 변환할 CP949스트링 배열수량(-1은 길이 자동측정)
	\param utf16 : 받을 UTF16스트링
	\param utf16length : 받을 UTF16스트링 배열수량
	\return 변환된 배열수량
	*/
	global_func int CP949ToUTF16(string cp949, int cp949length, wstring_rw utf16, int utf16length)
	{
		if(cp949length < 0) cp949length = 0x7FFFFFFF;
		utf16length *= 2;
		byte* utf16bytes = (byte*) utf16;
		int utf16focus = 0;
		for(int i = 0; cp949[i] && i < cp949length && utf16focus < utf16length; ++i)
		{
			if(!(cp949[i] & 0x80))
			{
				const wchar onechar = CP949ToUTF16_Search(cp949[i]);
				utf16bytes[utf16focus++] = (byte)(onechar & 0xFF);
				utf16bytes[utf16focus++] = (byte)(onechar >> 8);
			}
			else if(cp949[++i] && i < cp949length)
			{
				const wchar onechar = CP949ToUTF16_Search(cp949[i - 1], cp949[i]);
				utf16bytes[utf16focus++] = (byte)(onechar & 0xFF);
				utf16bytes[utf16focus++] = (byte)(onechar >> 8);
			}
		}
		utf16bytes[utf16focus++] = 0;
		utf16bytes[utf16focus] = 0;
		return utf16focus / 2;
	}

	/*!
	\brief UTF16를 CP949로 전환
	\param utf16 : 변환할 UTF16스트링
	\param utf16length : 변환할 UTF16스트링 배열수량(-1은 길이 자동측정)
	\param cp949 : 받을 CP949스트링
	\param cp949length : 받을 CP949스트링 배열수량
	\return 변환된 배열수량
	*/
	global_func int UTF16ToCP949(wstring utf16, int utf16length, string_rw cp949, const int cp949length)
	{
		if(utf16length < 0) utf16length = 0x7FFFFFFF;
		else utf16length *= 2;
		const byte* utf16bytes = (const byte*) utf16;
		int cp949focus = 0;
		for(int i = 0; (utf16bytes[i] || utf16bytes[i + 1]) && i < utf16length && cp949focus < cp949length; i += 2)
		{
			string Result = UTF16ToCP949_Search((utf16bytes[i] & 0xFF) | (utf16bytes[i + 1] << 8));
			cp949[cp949focus++] = Result[0];
			if((Result[0] & 0x80) && cp949focus < cp949length)
				cp949[cp949focus++] = Result[1];
		}
		cp949[cp949focus] = 0;
		return cp949focus;
	}

	/*!
	\brief UTF16를 UTF8로 전환
	\param utf16 : 변환할 UTF16스트링
	\param utf16length : 변환할 UTF16스트링 배열수량(-1은 길이 자동측정)
	\param utf8 : 받을 UTF8스트링
	\see BxUtil::GetLengthForUTF8
	*/
	global_func void UTF16ToUTF8(wstring utf16, int utf16length, string_rw utf8)
	{
		if(utf16length < 0) utf16length = 0x7FFFFFFF;
		for(int i = 0; i < utf16length && utf16[i]; ++i)
		{
			if((utf16[i] & 0xFF80) == 0x0000) // Ascii
				*(utf8++) = utf16[i] & 0x00FF;
			else if((utf16[i] & 0xF800) == 0x0000) // 2Bytes-Less
			{
				*(utf8++) = 0xC0 | ((utf16[i] & 0x07C0) >> 6);
				*(utf8++) = 0x80 | (utf16[i] & 0x003F);
			}
			else if((utf16[i] & 0xFC00) == 0xD800 && (utf16[i + 1] & 0xFC00) == 0xDC00) // 4Bytes
			{
				const wchar U_Value = ((utf16[i] & 0x03C0) >> 6) + 1;
				*(utf8++) = 0xF0 | ((U_Value & 0x001C) >> 2);
				*(utf8++) = 0x80 | ((U_Value & 0x0003) << 4) | ((utf16[i] & 0x003C) >> 2);
				*(utf8++) = 0x80 | ((utf16[i] & 0x0003) << 4) | ((utf16[i + 1] & 0x03C0) >> 6);
				*(utf8++) = 0x80 | (utf16[i + 1] & 0x003F);
				i += 1;
			}
			else // 2Bytes-Full
			{
				*(utf8++) = 0xE0 | ((utf16[i] & 0xF000) >> 12);
				*(utf8++) = 0x80 | ((utf16[i] & 0x0FC0) >> 6);
				*(utf8++) = 0x80 | (utf16[i] & 0x003F);
			}
		}
	}

	/*!
	\brief UTF8을 UTF16으로 전환
	\param utf8 : 변환할 UTF8스트링
	\param utf8length : 변환할 UTF8스트링 배열수량(-1은 길이 자동측정)
	\param utf16 : 받을 UTF16스트링
	\see BxUtil::GetLengthForUTF16
	*/
	global_func void UTF8ToUTF16(string utf8, int utf8length, wstring_rw utf16)
	{
		if(utf8length < 0) utf8length = 0x7FFFFFFF;
		for(int i = 0; i < utf8length && utf8[i]; ++i)
		{
			if((utf8[i] & 0xF8) == 0xF0) // 4Bytes
			{
				const char W_Value = (((utf8[i] & 0x07) << 2) | ((utf8[i + 1] & 0x30) >> 4)) - 1;
				*(utf16++) = 0xD800 | (W_Value << 6) | ((utf8[i + 1] & 0x0F) << 2) | ((utf8[i + 2] & 0x30) >> 4);
				*(utf16++) = 0xDC00 | ((utf8[i + 2] & 0x0F) << 6) | (utf8[i + 3] & 0x3F);
				i += 3;
			}
			else if((utf8[i] & 0xF0) == 0xE0) // 3Bytes
			{
				*(utf16++) = ((utf8[i] & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
				i += 2;
			}
			else if((utf8[i] & 0xE0) == 0xC0) // 2Bytes
			{
				*(utf16++) = ((utf8[i] & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
				i += 1;
			}
			else // 1Bytes
			{
				*(utf16++) = utf8[i] & 0xFF;
			}
		}
	}

	/*!
	\brief UTF16을 UTF8로 전환시 필요한 UTF8의 바이트길이
	\param utf16 : 변환할 UTF16스트링
	\param utf16length : 변환할 UTF16스트링 배열수량(-1은 길이 자동측정)
	\return 필요한 바이트길이
	*/
	global_func int GetLengthForUTF8(wstring utf16, int utf16length)
	{
		if(utf16length < 0) utf16length = 0x7FFFFFFF;
		int Result = 0;
		for(int i = 0; i < utf16length && utf16[i]; ++i)
		{
			if((utf16[i] & 0xFF80) == 0x0000) // Ascii
				Result += 1;
			else if((utf16[i] & 0xF800) == 0x0000) // 2Bytes-Less
				Result += 2;
			else if((utf16[i] & 0xFC00) == 0xD800 && (utf16[i + 1] & 0xFC00) == 0xDC00) // 4Bytes
			{
				Result += 4;
				i += 1;
			}
			else // 2Bytes-Full
				Result += 3;
		}
		return Result;
	}

	/*!
	\brief UTF8을 UTF16으로 전환시 필요한 UTF16의 배열수량
	\param utf8 : 변환할 UTF8스트링
	\param utf8length : 변환할 UTF8스트링 배열수량(-1은 길이 자동측정)
	\return 필요한 UTF16의 배열수량
	*/
	global_func int GetLengthForUTF16(string utf8, int utf8length)
	{
		if(utf8length < 0) utf8length = 0x7FFFFFFF;
		int Result = 0;
		for(int i = 0; i < utf8length && utf8[i]; ++i)
		{
			if((utf8[i] & 0xF8) == 0xF0) // 4Bytes
			{
				Result += 2;
				i += 3;
			}
			else if((utf8[i] & 0xF0) == 0xE0) // 3Bytes
			{
				Result += 1;
				i += 2;
			}
			else if((utf8[i] & 0xE0) == 0xC0) // 2Bytes
			{
				Result += 1;
				i += 1;
			}
			else // 1Bytes
				Result += 1;
		}
		return Result;
	}

	/*!
	\brief 교차테스트후 교차점을 구함
	\param DstB : 선분1 첫점
	\param DstE : 선분1 끝점
	\param SrcB : 선분2 첫점
	\param SrcE : 선분2 끝점
	\return 교차시 교차정보를 리턴함
	*/
	global_func const point* GetDotByLineCross(const point& DstB, const point& DstE, const point& SrcB, const point& SrcE)
	{
		global_data point Result = {0, 0};
		if(BxUtilGlobal::Max(SrcB.x, SrcE.x) < BxUtilGlobal::Min(DstB.x, DstE.x)
			|| BxUtilGlobal::Max(DstB.x, DstE.x) < BxUtilGlobal::Min(SrcB.x, SrcE.x)
			|| BxUtilGlobal::Max(SrcB.y, SrcE.y) < BxUtilGlobal::Min(DstB.y, DstE.y)
			|| BxUtilGlobal::Max(DstB.y, DstE.y) < BxUtilGlobal::Min(SrcB.y, SrcE.y))
			return nullptr;
		const int ResultA1 = GetClockwiseValue(DstB, DstE, SrcB);
		const int ResultB1 = GetClockwiseValue(DstB, DstE, SrcE);
		const int ResultA2 = GetClockwiseValue(SrcB, SrcE, DstB);
		const int ResultB2 = GetClockwiseValue(SrcB, SrcE, DstE);
		if((ResultA1 == 0 || ResultB1 == 0 || (ResultA1 < 0 && 0 < ResultB1) || (0 < ResultA1 && ResultB1 < 0))
			&& (ResultA2 == 0 || ResultB2 == 0 || (ResultA2 < 0 && 0 < ResultB2) || (0 < ResultA2 && ResultB2 < 0)))
		{
			// 수직
			if(DstB.x == DstE.x && SrcB.x == SrcE.x)
			{
				Result.x = DstB.x;
				if(DstB.y < DstE.y)
				{
					const bool IsInSrcB = (DstB.y <= SrcB.y && SrcB.y <= DstE.y);
					const bool IsInSrcE = (DstB.y <= SrcE.y && SrcE.y <= DstE.y);
					if(IsInSrcB && IsInSrcE)
						Result.y = (SrcB.y < SrcE.y)? SrcB.y : SrcE.y;
					else if(IsInSrcB) Result.y = SrcB.y;
					else if(IsInSrcE) Result.y = SrcE.y;
					else Result.y = (SrcB.y < SrcE.y)? DstB.y : DstE.y;
				}
				else
				{
					const bool IsInSrcB = (DstE.y <= SrcB.y && SrcB.y <= DstB.y);
					const bool IsInSrcE = (DstE.y <= SrcE.y && SrcE.y <= DstB.y);
					if(IsInSrcB && IsInSrcE)
						Result.y = (SrcB.y < SrcE.y)? SrcE.y : SrcB.y;
					else if(IsInSrcB) Result.y = SrcB.y;
					else if(IsInSrcE) Result.y = SrcE.y;
					else Result.y = (SrcB.y < SrcE.y)? DstE.y : DstB.y;
				}
			}
			// 수평
			else if(DstB.y == DstE.y && SrcB.y == SrcE.y)
			{
				Result.y = DstB.y;
				if(DstB.x < DstE.x)
				{
					const bool IsInSrcB = (DstB.x <= SrcB.x && SrcB.x <= DstE.x);
					const bool IsInSrcE = (DstB.x <= SrcE.x && SrcE.x <= DstE.x);
					if(IsInSrcB && IsInSrcE)
						Result.x = (SrcB.x < SrcE.x)? SrcB.x : SrcE.x;
					else if(IsInSrcB) Result.x = SrcB.x;
					else if(IsInSrcE) Result.x = SrcE.x;
					else Result.x = (SrcB.x < SrcE.x)? DstB.x : DstE.x;
				}
				else
				{
					const bool IsInSrcB = (DstE.x <= SrcB.x && SrcB.x <= DstB.x);
					const bool IsInSrcE = (DstE.x <= SrcE.x && SrcE.x <= DstB.x);
					if(IsInSrcB && IsInSrcE)
						Result.x = (SrcB.x < SrcE.x)? SrcE.x : SrcB.x;
					else if(IsInSrcB) Result.x = SrcB.x;
					else if(IsInSrcE) Result.x = SrcE.x;
					else Result.x = (SrcB.x < SrcE.x)? DstE.x : DstB.x;
				}
			}
			// 직교
			else if(DstB.x == DstE.x && SrcB.y == SrcE.y)
			{
				Result.x = DstB.x;
				Result.y = SrcB.y;
			}
			else if(SrcB.x == SrcE.x && DstB.y == DstE.y)
			{
				Result.x = SrcB.x;
				Result.y = DstB.y;
			}
			// 꼭지점닿음
			else if((DstB.x == SrcB.x && DstB.y == SrcB.y) || (DstE.x == SrcE.x && DstE.y == SrcE.y)
				|| (DstB.x == SrcE.x && DstB.y == SrcE.y) || (DstE.x == SrcB.x && DstE.y == SrcB.y))
				return nullptr;
			else
			{
				// 직선A
				fint A_A = ItoF(DstB.y - DstE.y);
				fint A_B = ItoF(DstE.x - DstB.x);
				const fint A_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(A_A) + BxUtilGlobal::Pow64(A_B));
				A_A = (((huge) A_A) << 16) / A_Distance;
				A_B = (((huge) A_B) << 16) / A_Distance;
				const fint A_C = A_A * DstB.x + A_B * DstB.y;
				// 직선B
				fint B_A = ItoF(SrcB.y - SrcE.y);
				fint B_B = ItoF(SrcE.x - SrcB.x);
				const fint B_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(B_A) + BxUtilGlobal::Pow64(B_B));
				B_A = (((huge) B_A) << 16) / B_Distance;
				B_B = (((huge) B_B) << 16) / B_Distance;
				const fint B_C = B_A * SrcB.x + B_B * SrcB.y;
				// 교점구하기
				const fint Rate = ((A_B * (huge) B_A) >> 16) - ((A_A * (huge) B_B) >> 16);
				Result.x = FtoI((A_B * (huge) B_C - B_B * (huge) A_C) / Rate);
				Result.y = FtoI((A_A * (huge) B_C - B_A * (huge) A_C) / -Rate);
			}
			return &Result;
		}
		return nullptr;
	}

private:
	enum {Angle90 = 256, Angle180 = 512, Angle270 = 768};

	global_func inline uint& RandomTable(int Index)
	{
		global_data uint _RandomTable[256] = {0};
		return _RandomTable[Index];
	}

	global_func const wchar CP949ToUTF16_Search(const char cp949a, const char cp949b = ' ')
	{
		global_data const int Length = 98 + 51 + 2350;
		global_data wchar CP949[Length];
		global_data wchar UTF16[Length];
		global_data bool DoCopyAndSortByCP949 = true;
		if(DoCopyAndSortByCP949)
		{
			DoCopyAndSortByCP949 = false;
			BxCore::Util::MemMove(CP949, BxCore::Util::CP949Table(), sizeof(wchar) * Length);
			BxCore::Util::MemMove(UTF16, BxCore::Util::UTF16Table(), sizeof(wchar) * Length);
			for(int i = 0; i < Length - 1; ++i)
			{
				int iBest = i;
				for(int j = i + 1; j < Length; ++j)
					if(CP949[j] < CP949[iBest])
						iBest = j;
				if(iBest != i)
				{
					wchar Temp = CP949[i];
					CP949[i] = CP949[iBest];
					CP949[iBest] = Temp;
					Temp = UTF16[i];
					UTF16[i] = UTF16[iBest];
					UTF16[iBest] = Temp;
				}
			}
		}
		const wchar Key = ((cp949b & 0xFF) << 8) | (cp949a & 0xFF);
		int Lower = 0, Upper = Length - 1;
		while(Lower <= Upper)
		{
			const int Middle = (Lower + Upper) / 2;
			if(CP949[Middle] == Key)
				return (const wchar) UTF16[Middle];
			if(CP949[Middle] < Key)
				Lower = Middle + 1;
			else Upper = Middle - 1;
		}
		return (const wchar) L'\x25A1';
	}

	global_func string UTF16ToCP949_Search(const wchar utf16)
	{
		global_data const int Length = 98 + 51 + 2350;
		global_data wchar UTF16[Length];
		global_data wchar CP949[Length];
		global_data bool DoCopyAndSortByUTF16 = true;
		if(DoCopyAndSortByUTF16)
		{
			DoCopyAndSortByUTF16 = false;
			BxCore::Util::MemMove(UTF16, BxCore::Util::UTF16Table(), sizeof(wchar) * Length);
			BxCore::Util::MemMove(CP949, BxCore::Util::CP949Table(), sizeof(wchar) * Length);
			for(int i = 0; i < Length - 1; ++i)
			{
				int iBest = i;
				for(int j = i + 1; j < Length; ++j)
					if(UTF16[j] < UTF16[iBest])
						iBest = j;
				if(iBest != i)
				{
					wchar Temp = UTF16[i];
					UTF16[i] = UTF16[iBest];
					UTF16[iBest] = Temp;
					Temp = CP949[i];
					CP949[i] = CP949[iBest];
					CP949[iBest] = Temp;
				}
			}
		}
		const wchar Key = utf16;
		int Lower = 0, Upper = Length - 1;
		while(Lower <= Upper)
		{
			const int Middle = (Lower + Upper) / 2;
			if(UTF16[Middle] == Key)
				return (string) &CP949[Middle];
			if(UTF16[Middle] < Key)
				Lower = Middle + 1;
			else Upper = Middle - 1;
		}
		return (string) "□";
	}

	global_func inline uint& _RandValue()
	{
		global_data uint RandValue = 0;
		return RandValue;
	}

	global_func inline void _SetRand(const uint Key)
	{
		_RandValue() = Key;
	}

	global_func inline uint _MakeRand()
	{
		_RandValue() *= 0x000343fd;
		_RandValue() += 0x00269ec3;
		return (_RandValue() >> 16) & 0x7fff;
	}
};
