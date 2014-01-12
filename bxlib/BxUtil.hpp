#pragma once
#include <BxMemory.hpp>
#include <BxSingleton.hpp>

//! \brief BxUtil���� �۷ι��Լ�
namespace BxUtilGlobal
{
	/*!
	\brief ���̰� ���ϱ�
	\return ���̰�(3.141592...)
	*/
	static inline fint Pi()
	{
		return (fint)(3.14159265358979323846264338327950288419716939937510 * 0x10000 + 0.5);
	}

	/*!
	\brief �и��ʿ� ���� �����Ӽ� ���ϱ�
	\return �����Ӽ�
	*/
	static inline int Fps(const int ms)
	{
		return ms / BxCore::Main::GetCurrentFrameTime();
	}

	/*!
	\brief �ּҰ� ���ϱ�
	\param A : �μ�A
	\param B : �μ�B
	\return �ּҰ�
	\see Max
	*/
	static inline const int Min(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & Mask) | (B & ~Mask);
	}

	/*!
	\brief �ִ밪 ���ϱ�
	\param A : �μ�A
	\param B : �μ�B
	\return �ִ밪
	\see Min
	*/
	static inline const int Max(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & ~Mask) | (B & Mask);
	}

	/*!
	\brief �ּ��ִ� �Ѱ輳��
	\param MinValue : �ּҰ�
	\param Value : �Է°�
	\param MaxValue : �ִ밪
	\return �ּ��ִ� �Ѱ輳���� ������ �Է°�
	\see Min
	*/
	static inline const int MinMax(const int MinValue, const int Value, const int MaxValue)
	{
		return Min(Max(MinValue, Value), MaxValue);
	}

	/*!
	\brief ���밪 ���ϱ�
	\param A : �μ�A
	\return ���밪
	*/
	static inline int Abs(const int A)
	{
		const int Mask = A >> 31;
		return (A & ~Mask) | (-A & Mask);
	}

	/*!
	\brief ������ ���ϱ�
	\param A : �μ�A
	\return ������
	*/
	static inline const int Pow(const int A)
	{
		return A * A;
	}

	/*!
	\brief 64��Ʈ ������ ���ϱ�
	\param A : �μ�A
	\return 64��Ʈ ������
	*/
	static inline const huge Pow64(const int A)
	{
		return ((huge) A) * A;
	}

	/*!
	\brief �����ٰ� ���ϱ�
	\param A : �μ�A
	\return �����ٰ�
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
	\brief 64��Ʈ �����ٰ� ���ϱ�
	\param A : �μ�A
	\return 64��Ʈ �����ٰ�
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
	\brief ��Ʈ������ ���ϱ�
	\param String : ��Ʈ��
	\return ��Ʈ���� ����Ʈ����
	*/
	static int StrLen(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0');
		return i;
	}

	/*!
	\brief ���ø�Ÿ�� ��Ʈ������ ���ϱ�
	\param String : ��Ʈ��
	\return ��Ʈ���� �迭����
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
	\brief Ȯ��� ��Ʈ������ ���ϱ�(\r�� \n�� �ν�)
	\param String : ��Ʈ��
	\return ��Ʈ���� ����Ʈ����
	*/
	static int StrLenEx(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != '\r' && String[i] != '\n');
		return i;
	}

	/*!
	\brief Ȯ��� ��Ʈ������ ���ϱ�(������� ������)
	\param String : ��Ʈ��
	\param ExitCode : ���Ṯ��
	\return ��Ʈ���� ����Ʈ����
	*/
	static int StrLenEx(string String, char ExitCode)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != ExitCode);
		return i;
	}

	/*!
	\brief ��Ʈ�� �Ҵ�
	\param SrcLength : �Ҵ��� ������(NULL���ڰ� ������)
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
	\brief ��Ʈ�� ����
	\param AllocedString : �Ҵ�� ��Ʈ��
	\return ������ ����� ���� nullptr�� ����
	\see StrAlloc, StrCpyWithAlloc
	*/
	static string StrFree(string AllocedString)
	{
		BxCore::Util::Free((id_memory) AllocedString);
		return nullptr;
	}

	/*!
	\brief ��Ʈ�� �����ϱ�
	\param DstString : ������ ��Ʈ��(restrict�Ӽ�)
	\param SrcString : ������ ��Ʈ��(restrict�Ӽ�)
	\param SrcLength : ����� ������(StringSrc�� ���̷� �ڵ����� : -1)
	\return ����� ������(NULL���� ����)
	*/
	static int StrCpy(string_rw_prm DstString, string_prm SrcString, int SrcLength = -1)
	{
		const int SrcLen = (SrcLength < 0)? StrLen(SrcString) : SrcLength;
		BxCore::Util::MemMove(DstString, SrcString, SrcLen);
		DstString[SrcLen] = '\0';
		return SrcLen;
	}

	/*!
	\brief ��Ʈ�� �Ҵ� �� �����ϱ�
	\param SrcString : ������ ��Ʈ��
	\param SrcLength : ����� ������(StringSrc�� ���̷� �ڵ����� : -1)
	\return �Ҵ� �� ����� ��Ʈ��
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
	\brief ��Ʈ�� ���ϱ�
	\param StringA : ��Ʈ��A(restrict�Ӽ�)
	\param StringB : ��Ʈ��B(restrict�Ӽ�)
	\param FixedLength : ������ �񱳱���(������ : -1)
	\return same-����, b_is_more-��Ʈ��B�� ŭ, b_is_less-��Ʈ��B�� ����
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
	\brief ��ҹ��� ���о��� ��Ʈ�� ���ϱ�
	\param StringA : ��Ʈ��A(restrict�Ӽ�)
	\param StringB : ��Ʈ��B(restrict�Ӽ�)
	\param FixedLength : ������ �񱳱���(������ : -1)
	\return same-����, b_is_more-��Ʈ��B�� ŭ, b_is_less-��Ʈ��B�� ����
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
	\brief ��Ʈ���� ���ϱ����� ���� ���ϱ�
	\param StringA : ��Ʈ��A(restrict�Ӽ�)
	\param StringB : ��Ʈ��B(restrict�Ӽ�)
	\return ����Ʈ����
	*/
	static int StrSameCount(string_prm StringA, string_prm StringB)
	{
		int Focus = 0;
		while(StringA[Focus] != '\0' && StringA[Focus] == StringB[Focus])
			++Focus;
		return Focus;
	}

	/*!
	\brief ���� ��Ʈ���񱳸� ���� �ؽ��ڵ� �����
	\param String : ��Ʈ��
	\param SrcLength : ��Ʈ�� ������(String ���̷� �ڵ����� : -1)
	\return �ؽ��ڵ�
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
	\brief ��Ʈ���˻�
	\param DstString : ���Ʈ��
	\param SrcString : ã�� ��Ʈ��
	\param SrcSearchMap : ã�� ��Ʈ���� �˻���
	\param DstLength : ���Ʈ�� ������(DstString ���̷� �ڵ����� : -1)
	\return ã�� ��ġ(����Ʈ����), �˻����н� -1
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
	\brief ��Ʈ���˻��� ���� �˻��� �����
	\param String : ��Ʈ��
	\param SearchMap : ������� �˻���
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
	\brief �ش� ��Ʈ���� ���������� ����
	\param String : ��Ʈ��
	\param SrcLength : ��Ʈ�� ������(String ���̷� �ڵ����� : -1)
	\return true-����, false-���ھƴ�
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
	\brief ��Ʈ���� ������ ��ȯ
	\param String : ��Ʈ��
	\param StringLength : ó���� ������(String�� ���̷� �ڵ����� : -1)
	\return ��ȯ�� ����
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
	\brief ������ ��Ʈ������ ��ȯ
	\param Value : ����
	\return ��ȯ�� ��Ʈ��
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
	\brief �������� ����� ��ȯ
	\param Hex : ������
	\param HexLength : ó���� ������(String�� ���̷� �ڵ����� : -1)
	\return ��ȯ�� ���
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
	\brief ����� �������� ��ȯ
	\param Value : ���
	\param IsUpper : �빮�ڿ���(true-�빮��, false-�ҹ���)
	\return ��ȯ�� ������
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
	\brief �����Ϳ��� �ε�(char)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline char LoadInt8(byte*& Resource)
	{
		char Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief �����Ϳ��� �ε�(short)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline short LoadInt16(byte*& Resource)
	{
		short Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief �����Ϳ��� �ε�(int)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline int LoadInt32(byte*& Resource)
	{
		int Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief �����Ϳ��� �ε�(byte)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline byte LoadUint8(byte*& Resource)
	{
		byte Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief �����Ϳ��� �ε�(ushort)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline ushort LoadUint16(byte*& Resource)
	{
		ushort Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief �����Ϳ��� �ε�(uint)
	\param Resource : ���� ������
	\return �ε��� ��
	*/
	static inline uint LoadUint32(byte*& Resource)
	{
		uint Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief �����Ϳ��� ����Ʈ�迭 �ε�(uint)
	\param Resource : ���� ������
	\param Dest : �� ������
	\param Length : �� ����Ʈ����
	*/
	static inline void LoadBytes(byte*& Resource, void* Dest, uint Length)
	{
		BxCore::Util::MemMove(Dest, Resource, Length);
		Resource += Length;
	}

	/*!
	\brief �����Ϳ��� ��Ʈ�迭 �ε�(uint)
	\param Resource : ���� ������
	\param CurrentBit : ���� ��Ʈ
	\param BitCount : ���� ��Ʈ����
	\return �ε��� ��
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
	\brief �����Ϳ��� ���ǹ迭 �ε�
	\param Resource : ���� ������
	\param TypeLen : ���� ����Ʈ����
	\return �ε��� ������
	*/
	static inline byte* LoadType(byte*& Resource, uint TypeLen)
	{
		byte* Result = Resource;
		Resource += TypeLen;
		return Result;
	}

	/*!
	\brief �����Ϳ��� ��Ʈ�� �ε�
	\param Resource : uint16���� ��Ʈ������� ���� ����, �ش� ��Ʈ���� �ε�
	\return �ε��� ��Ʈ��
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
	\brief ��������� �迭�� �ʱ�ȭ
	\param Dst : �ʱ�ȭ�� �迭
	\param Count : �����������
	\param Value : ù��° �����
	\param ... : �������
	*/
	template<typename TYPE>
	static inline void SetArray(TYPE* Dst, const BxThrow& Src)
	{
		for(int i = 0, iend = Src.Length(); i < iend; ++i)
			Dst[i] = *Src.Access<TYPE>(i);
	}
}

//! \brief ���� ����/������� ����
class BxUtil
{
public:
	/*!
	\brief �����Լ� �ʱ�ȭ
	\param Seed : �ʱ�ȭ��
	\see BxCore::System::GetTimerMs
	*/
	global_func void SetRandom(const uint Seed)
	{
		_SetRand(Seed);
		for(int i = 0; i < 256; ++i)
			RandomTable(i) = (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
	}

	/*!
	\brief ������ ���ϱ�
	\param Key : �������̺� �ε�����, -1�� �� ������
	\return �����
	*/
	global_func inline uint GetRandom(const int Key = -1)
	{
		if(Key < 0) return (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
		return RandomTable(Key & 0xFF);
	}

	/*!
	\brief ������ ������ ���ϱ�
	\param MinValue : �ּҰ�
	\param MaxValue : �ִ밪
	\param Key : �������̺� �ε�����, -1�� �� ������
	\return �ּҰ�(����)���� �ִ밪(����)������ �����
	*/
	global_func inline int GetValue(const int MinValue, const int MaxValue, int Key = -1)
	{
		return MinValue + (GetRandom(Key) % (MaxValue - MinValue + 1));
	}

	/*!
	\brief ������ ������ ���ϱ�
	\param MinValue : �ּҰ�����
	\param MaxValue : �ִ밢����
	\param Key : �������̺� �ε�����, -1�� �� ������
	\return �ּҰ�(����)���� �ִ밪(����)������ �����
	*/
	global_func inline int GetValueAngle(const int MinValue, const int MaxValue, int Key = -1)
	{
		if(180 < MaxValue - MinValue)
			return (MaxValue + (GetRandom(Key) % (360 - MaxValue + MinValue + 1))) % 360;
		return GetValue(MinValue, MaxValue, Key) % 360;
	}

	/*!
	\brief Cos�� ���ϱ�
	\param Angle1024 : ������
	\return Cos��
	*/
	global_func inline fint Cos(int Angle1024)
	{
		Angle1024 = Angle1024 & 0x3FF;
		return BxCore::Util::Cos1024Table()[Angle90 - BxUtilGlobal::Abs((Angle1024 & 0x1FF) - Angle90)]
			* ((((Angle1024 + Angle270 - 1) & 0x3FF) < Angle180)? -1 : 1);
	}

	/*!
	\brief Sin�� ���ϱ�
	\param Angle1024 : ������
	\return Sin��
	*/
	global_func inline fint Sin(int Angle1024)
	{
		return Cos(Angle1024 + Angle270);
	}

	/*!
	\brief �������� ���� ���ϱ�
	\param VectorX : ����X
	\param VectorY : ����Y
	\return ������
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
	\brief �� 3���� ���⼺ ���� Ȯ��
	\param Start : ù��
	\param End : ��°��
	\param Src : ���� ��°��
	\return ���� : �ð����, ��� : �ݽð����, 0 : �߸�
	*/
	global_func inline const int GetClockwiseValue(const point& Start, const point& End, const point& Src)
	{
		return (Start.x - End.x) * (Src.y - End.y) - (Start.y - End.y) * (Src.x - End.x);
	}

	/*!
	\brief �������ȭ
	\param Dst : ����� ���
	*/
	global_func inline void MatrixInit(matrix _out_ Dst)
	{
		BxCore::Util::MemSet(&Dst, 0, sizeof(matrix));
		Dst.m[0][0] = Dst.m[1][1] = Dst.m[2][2] = Dst.m[3][3] = ItoF(1);
	}

	/*!
	\brief ��Ŀ� ��İ��ϱ�
	\param Dst : ����� ���
	\param Src : ���� ���
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
	\brief ��Ŀ� ��İ��ϱ�
	\param Dst : ����� ���
	\param Src00 : ���� ���
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
	\brief ����� ȸ����ȯ(���ʹϾ�)
	\param Dst : ����� ���
	\param Src : ȸ���� ���ʹϾ�
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
	\brief ���ؽ��� ��İ��ϱ�
	\param Dst : ����� ���ؽ�
	\param Src : ���� ���
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
	\brief CP949�� UTF16���� ��ȯ
	\param cp949 : ��ȯ�� CP949��Ʈ��
	\param cp949length : ��ȯ�� CP949��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\param utf16 : ���� UTF16��Ʈ��
	\param utf16length : ���� UTF16��Ʈ�� �迭����
	\return ��ȯ�� �迭����
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
	\brief UTF16�� CP949�� ��ȯ
	\param utf16 : ��ȯ�� UTF16��Ʈ��
	\param utf16length : ��ȯ�� UTF16��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\param cp949 : ���� CP949��Ʈ��
	\param cp949length : ���� CP949��Ʈ�� �迭����
	\return ��ȯ�� �迭����
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
	\brief UTF16�� UTF8�� ��ȯ
	\param utf16 : ��ȯ�� UTF16��Ʈ��
	\param utf16length : ��ȯ�� UTF16��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\param utf8 : ���� UTF8��Ʈ��
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
	\brief UTF8�� UTF16���� ��ȯ
	\param utf8 : ��ȯ�� UTF8��Ʈ��
	\param utf8length : ��ȯ�� UTF8��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\param utf16 : ���� UTF16��Ʈ��
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
	\brief UTF16�� UTF8�� ��ȯ�� �ʿ��� UTF8�� ����Ʈ����
	\param utf16 : ��ȯ�� UTF16��Ʈ��
	\param utf16length : ��ȯ�� UTF16��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\return �ʿ��� ����Ʈ����
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
	\brief UTF8�� UTF16���� ��ȯ�� �ʿ��� UTF16�� �迭����
	\param utf8 : ��ȯ�� UTF8��Ʈ��
	\param utf8length : ��ȯ�� UTF8��Ʈ�� �迭����(-1�� ���� �ڵ�����)
	\return �ʿ��� UTF16�� �迭����
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
	\brief �����׽�Ʈ�� �������� ����
	\param DstB : ����1 ù��
	\param DstE : ����1 ����
	\param SrcB : ����2 ù��
	\param SrcE : ����2 ����
	\return ������ ���������� ������
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
			// ����
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
			// ����
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
			// ����
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
			// ����������
			else if((DstB.x == SrcB.x && DstB.y == SrcB.y) || (DstE.x == SrcE.x && DstE.y == SrcE.y)
				|| (DstB.x == SrcE.x && DstB.y == SrcE.y) || (DstE.x == SrcB.x && DstE.y == SrcB.y))
				return nullptr;
			else
			{
				// ����A
				fint A_A = ItoF(DstB.y - DstE.y);
				fint A_B = ItoF(DstE.x - DstB.x);
				const fint A_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(A_A) + BxUtilGlobal::Pow64(A_B));
				A_A = (((huge) A_A) << 16) / A_Distance;
				A_B = (((huge) A_B) << 16) / A_Distance;
				const fint A_C = A_A * DstB.x + A_B * DstB.y;
				// ����B
				fint B_A = ItoF(SrcB.y - SrcE.y);
				fint B_B = ItoF(SrcE.x - SrcB.x);
				const fint B_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(B_A) + BxUtilGlobal::Pow64(B_B));
				B_A = (((huge) B_A) << 16) / B_Distance;
				B_B = (((huge) B_B) << 16) / B_Distance;
				const fint B_C = B_A * SrcB.x + B_B * SrcB.y;
				// �������ϱ�
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
		return (string) "��";
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
