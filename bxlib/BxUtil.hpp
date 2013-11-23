#pragma once
#include <BxMemory.hpp>
#include <BxSingleton.hpp>

//! \brief BxUtil°ü·Ã ±Û·Î¹úÇÔ¼ö
namespace BxUtilGlobal
{
	/*!
	\brief ÆÄÀÌ°ª ±¸ÇÏ±â
	\return ÆÄÀÌ°ª(3.141592...)
	*/
	static inline fint Pi()
	{
		return (fint)(3.14159265358979323846264338327950288419716939937510 * 0x10000 + 0.5);
	}

	/*!
	\brief ¹Ð¸®ÃÊ¿¡ µû¸¥ ÇÁ·¹ÀÓ¼ö ±¸ÇÏ±â
	\return ÇÁ·¹ÀÓ¼ö
	*/
	static inline int Fps(const int ms)
	{
		return ms / BxCore::Main::GetCurrentFrameTime();
	}

	/*!
	\brief ÃÖ¼Ò°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\param B : ÀÎ¼öB
	\return ÃÖ¼Ò°ª
	\see Max
	*/
	static inline const int Min(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & Mask) | (B & ~Mask);
	}

	/*!
	\brief ÃÖ´ë°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\param B : ÀÎ¼öB
	\return ÃÖ´ë°ª
	\see Min
	*/
	static inline const int Max(const int A, const int B)
	{
		const int Mask = (A - B) >> 31;
		return (A & ~Mask) | (B & Mask);
	}

	/*!
	\brief ÃÖ¼ÒÃÖ´ë ÇÑ°è¼³Á¤
	\param MinValue : ÃÖ¼Ò°ª
	\param Value : ÀÔ·Â°ª
	\param MaxValue : ÃÖ´ë°ª
	\return ÃÖ¼ÒÃÖ´ë ÇÑ°è¼³Á¤À» Àû¿ëÇÑ ÀÔ·Â°ª
	\see Min
	*/
	static inline const int MinMax(const int MinValue, const int Value, const int MaxValue)
	{
		return Min(Max(MinValue, Value), MaxValue);
	}

	/*!
	\brief Àý´ë°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\return Àý´ë°ª
	*/
	static inline int Abs(const int A)
	{
		const int Mask = A >> 31;
		return (A & ~Mask) | (-A & Mask);
	}

	/*!
	\brief Á¦°ö°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\return Á¦°ö°ª
	*/
	static inline const int Pow(const int A)
	{
		return A * A;
	}

	/*!
	\brief 64ºñÆ® Á¦°ö°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\return 64ºñÆ® Á¦°ö°ª
	*/
	static inline const huge Pow64(const int A)
	{
		return ((huge) A) * A;
	}

	/*!
	\brief Á¦°ö±Ù°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\return Á¦°ö±Ù°ª
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
	\brief 64ºñÆ® Á¦°ö±Ù°ª ±¸ÇÏ±â
	\param A : ÀÎ¼öA
	\return 64ºñÆ® Á¦°ö±Ù°ª
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
	\brief ½ºÆ®¸µ±æÀÌ ±¸ÇÏ±â
	\param String : ½ºÆ®¸µ
	\return ½ºÆ®¸µÀÇ ¹ÙÀÌÆ®±æÀÌ
	*/
	static int StrLen(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0');
		return i;
	}

	/*!
	\brief ÅÛÇÃ¸´Å¸ÀÔ ½ºÆ®¸µ±æÀÌ ±¸ÇÏ±â
	\param String : ½ºÆ®¸µ
	\return ½ºÆ®¸µÀÇ ¹ÙÀÌÆ®±æÀÌ
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
	\brief È®ÀåµÈ ½ºÆ®¸µ±æÀÌ ±¸ÇÏ±â(\r°ú \nµµ ÀÎ½Ä)
	\param String : ½ºÆ®¸µ
	\return ½ºÆ®¸µÀÇ ¹ÙÀÌÆ®±æÀÌ
	*/
	static int StrLenEx(string String)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != '\r' && String[i] != '\n');
		return i;
	}

	/*!
	\brief È®ÀåµÈ ½ºÆ®¸µ±æÀÌ ±¸ÇÏ±â(»ç¿ëÀÚÀÇ ³¡¹®ÀÚ)
	\param String : ½ºÆ®¸µ
	\param ExitCode : Á¾·á¹®ÀÚ
	\return ½ºÆ®¸µÀÇ ¹ÙÀÌÆ®±æÀÌ
	*/
	static int StrLenEx(string String, char ExitCode)
	{
		BxAssert("BxUtil", String);
		int i = -1;
		while(String[++i] != '\0' && String[i] != ExitCode);
		return i;
	}

	/*!
	\brief ½ºÆ®¸µ ÇÒ´ç
	\param SrcLength : ÇÒ´çÇÒ »çÀÌÁî(NULL¹®ÀÚ°¡ ´õÇØÁü)
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
	\brief ½ºÆ®¸µ ÇØÁ¦
	\param AllocedString : ÇÒ´çµÈ ½ºÆ®¸µ
	\return °£ÆíÇÑ »ç¿ëÀ» À§ÇÑ nullptr°ª ¸®ÅÏ
	\see StrAlloc, StrCpyWithAlloc
	*/
	static string StrFree(string AllocedString)
	{
		BxCore::Util::Free((id_memory) AllocedString);
		return nullptr;
	}

	/*!
	\brief ½ºÆ®¸µ º¹»çÇÏ±â
	\param DstString : º¹»çÇÒ ½ºÆ®¸µ(restrict¼Ó¼º)
	\param SrcString : °¡Á®¿Ã ½ºÆ®¸µ(restrict¼Ó¼º)
	\param SrcLength : º¹»çµÉ »çÀÌÁî(StringSrcÀÇ ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return º¹»çµÈ »çÀÌÁî(NULL¹®ÀÚ Á¦¿Ü)
	*/
	static int StrCpy(string_rw_prm DstString, string_prm SrcString, int SrcLength = -1)
	{
		const int SrcLen = (SrcLength < 0)? StrLen(SrcString) : SrcLength;
		BxCore::Util::MemMove(DstString, SrcString, SrcLen);
		DstString[SrcLen] = '\0';
		return SrcLen;
	}

	/*!
	\brief ½ºÆ®¸µ ÇÒ´ç ¹× º¹»çÇÏ±â
	\param SrcString : °¡Á®¿Ã ½ºÆ®¸µ
	\param SrcLength : º¹»çµÉ »çÀÌÁî(StringSrcÀÇ ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return ÇÒ´ç ¹× º¹»çµÈ ½ºÆ®¸µ
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
	\brief ½ºÆ®¸µ ºñ±³ÇÏ±â
	\param StringA : ½ºÆ®¸µA(restrict¼Ó¼º)
	\param StringB : ½ºÆ®¸µB(restrict¼Ó¼º)
	\param FixedLength : °íÁ¤½Ä ºñ±³±¸°£(»ç¿ë¾ÈÇÔ : -1)
	\return same-°°À½, b_is_more-½ºÆ®¸µB°¡ Å­, b_is_less-½ºÆ®¸µB°¡ ÀÛÀ½
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
	\brief ´ë¼Ò¹®ÀÚ ±¸ºÐ¾øÀÌ ½ºÆ®¸µ ºñ±³ÇÏ±â
	\param StringA : ½ºÆ®¸µA(restrict¼Ó¼º)
	\param StringB : ½ºÆ®¸µB(restrict¼Ó¼º)
	\param FixedLength : °íÁ¤½Ä ºñ±³±¸°£(»ç¿ë¾ÈÇÔ : -1)
	\return same-°°À½, b_is_more-½ºÆ®¸µB°¡ Å­, b_is_less-½ºÆ®¸µB°¡ ÀÛÀ½
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
	\brief ½ºÆ®¸µ°£ µ¿ÀÏ±¸¿ªÀÇ ±æÀÌ ±¸ÇÏ±â
	\param StringA : ½ºÆ®¸µA(restrict¼Ó¼º)
	\param StringB : ½ºÆ®¸µB(restrict¼Ó¼º)
	\return ¹ÙÀÌÆ®±æÀÌ
	*/
	static int StrSameCount(string_prm StringA, string_prm StringB)
	{
		int Focus = 0;
		while(StringA[Focus] != '\0' && StringA[Focus] == StringB[Focus])
			++Focus;
		return Focus;
	}

	/*!
	\brief ºü¸¥ ½ºÆ®¸µºñ±³¸¦ À§ÇÑ ÇØ½ÃÄÚµå ¸¸µé±â
	\param String : ½ºÆ®¸µ
	\param SrcLength : ½ºÆ®¸µ »çÀÌÁî(String ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return ÇØ½ÃÄÚµå
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
	\brief ½ºÆ®¸µ°Ë»ö
	\param DstString : ´ë»ó½ºÆ®¸µ
	\param SrcString : Ã£À» ½ºÆ®¸µ
	\param SrcSearchMap : Ã£À» ½ºÆ®¸µÀÇ °Ë»ö¸Ê
	\param DstLength : ´ë»ó½ºÆ®¸µ »çÀÌÁî(DstString ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return Ã£Àº À§Ä¡(¹ÙÀÌÆ®´ÜÀ§), °Ë»ö½ÇÆÐ½Ã -1
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
	\brief ½ºÆ®¸µ°Ë»öÀ» À§ÇÑ °Ë»ö¸Ê ¸¸µé±â
	\param String : ½ºÆ®¸µ
	\param SearchMap : ¸¸µé¾îÁú °Ë»ö¸Ê
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
	\brief ÇØ´ç ½ºÆ®¸µÀÌ ¼ýÀÚÀÎÁöÀÇ ¿©ºÎ
	\param String : ½ºÆ®¸µ
	\param SrcLength : ½ºÆ®¸µ »çÀÌÁî(String ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return true-¼ýÀÚ, false-¼ýÀÚ¾Æ´Ô
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
	\brief ½ºÆ®¸µÀ» Á¤¼ö·Î º¯È¯
	\param String : ½ºÆ®¸µ
	\param StringLength : Ã³¸®µÉ »çÀÌÁî(StringÀÇ ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return º¯È¯µÈ Á¤¼ö
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
	\brief Á¤¼ö¸¦ ½ºÆ®¸µÀ¸·Î º¯È¯
	\param Value : Á¤¼ö
	\return º¯È¯µÈ ½ºÆ®¸µ
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
	\brief Çí»ç´ýÇÁ¸¦ ¾ç¼ö·Î º¯È¯
	\param Hex : Çí»ç´ýÇÁ
	\param HexLength : Ã³¸®µÉ »çÀÌÁî(StringÀÇ ±æÀÌ·Î ÀÚµ¿¼ÂÆÃ : -1)
	\return º¯È¯µÈ ¾ç¼ö
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
	\brief ¾ç¼ö¸¦ Çí»ç´ýÇÁ·Î º¯È¯
	\param Value : ¾ç¼ö
	\return º¯È¯µÈ Çí»ç´ýÇÁ
	*/
	static string _tmp_ ItoH(uint Value)
	{
		const int Length = 255;
		global_data char Result[Length + 1];
		Result[Length] = '\0';
		for(int i = Length - 1; 0 <= i; --i)
		{
			if((Value & 0xF) < 10)
				Result[i] = '0' + (Value & 0xF);
			else Result[i] = 'A' + (Value & 0xF) - 10;
			Value >>= 4;
			if(Value == 0)
				return &Result[i];
		}
		return &Result[Length];
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(char)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline char LoadInt8(byte*& Resource)
	{
		char Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(short)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline short LoadInt16(byte*& Resource)
	{
		short Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(int)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline int LoadInt32(byte*& Resource)
	{
		int Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(byte)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline byte LoadUint8(byte*& Resource)
	{
		byte Result = (Resource[0] & 0xFF);
		Resource += 1;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(ushort)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline ushort LoadUint16(byte*& Resource)
	{
		ushort Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8);
		Resource += 2;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ·Îµå(uint)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\return ·ÎµåÇÑ °ª
	*/
	static inline uint LoadUint32(byte*& Resource)
	{
		uint Result = (Resource[0] & 0xFF) | ((Resource[1] & 0xFF) << 8) | ((Resource[2] & 0xFF) << 16) | ((Resource[3] & 0xFF) << 24);
		Resource += 4;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ¹ÙÀÌÆ®¹è¿­ ·Îµå(uint)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\param Dest : ¾µ µ¥ÀÌÅÍ
	\param Length : ¾µ ¹ÙÀÌÆ®±æÀÌ
	*/
	static inline void LoadBytes(byte*& Resource, void* Dest, uint Length)
	{
		BxCore::Util::MemMove(Dest, Resource, Length);
		Resource += Length;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ºñÆ®¹è¿­ ·Îµå(uint)
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\param CurrentBit : ÇöÀç ºñÆ®
	\param BitCount : ÀÐÀ» ºñÆ®±æÀÌ
	\return ·ÎµåÇÑ °ª
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
	\brief µ¥ÀÌÅÍ¿¡¼­ ÀÓÀÇ¹è¿­ ·Îµå
	\param Resource : ÀÐÀ» µ¥ÀÌÅÍ
	\param TypeLen : ÀÐÀ» ¹ÙÀÌÆ®±æÀÌ
	\return ·ÎµåÇÑ µ¥ÀÌÅÍ
	*/
	static inline byte* LoadType(byte*& Resource, uint TypeLen)
	{
		byte* Result = Resource;
		Resource += TypeLen;
		return Result;
	}

	/*!
	\brief µ¥ÀÌÅÍ¿¡¼­ ½ºÆ®¸µ ·Îµå
	\param Resource : uint16À¸·Î ½ºÆ®¸µ»çÀÌÁî¸¦ ¾òÀº ´ÙÀ½, ÇØ´ç ½ºÆ®¸µÀ» ·Îµå
	\return ·ÎµåÇÑ ½ºÆ®¸µ
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
	\brief »ó¼ö³ª¿­·Î ¹è¿­À» ÃÊ±âÈ­
	\param Dst : ÃÊ±âÈ­ÇÒ ¹è¿­
	\param Count : »ó¼ö³ª¿­±æÀÌ
	\param Value : Ã¹¹øÂ° »ó¼ö°ª
	\param ... : »ó¼ö°ªµé
	*/
	template<typename TYPE>
	static inline void SetArray(TYPE* Dst, const BxThrow& Src)
	{
		for(int i = 0, iend = Src.Length(); i < iend; ++i)
			Dst[i] = *Src.Access<TYPE>(i);
	}
}

//! \brief °¢Á¾ ¼öÇÐ/¹°¸®¸ðµâ Á¦°ø
class BxUtil
{
public:
	/*!
	\brief ·£´ýÇÔ¼ö ÃÊ±âÈ­
	\param Seed : ÃÊ±âÈ­°ª
	\see BxCore::System::GetTimerMs
	*/
	global_func void SetRandom(const uint Seed)
	{
		_SetRand(Seed);
		for(int i = 0; i < 256; ++i)
			RandomTable(i) = (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
	}

	/*!
	\brief ·£´ý°ª ±¸ÇÏ±â
	\param Key : ·£´ýÅ×ÀÌºí ÀÎµ¦½º°ª, -1Àº ½Ç ·£´ý°ª
	\return °á°ú°ª
	*/
	global_func inline uint GetRandom(const int Key = -1)
	{
		if(Key < 0) return (_MakeRand() << 30) | (_MakeRand() << 15) | _MakeRand();
		return RandomTable(Key & 0xFF);
	}

	/*!
	\brief ±¸°£³» ·£´ý°ª ±¸ÇÏ±â
	\param MinValue : ÃÖ¼Ò°ª
	\param MaxValue : ÃÖ´ë°ª
	\param Key : ·£´ýÅ×ÀÌºí ÀÎµ¦½º°ª, -1Àº ½Ç ·£´ý°ª
	\return ÃÖ¼Ò°ª(Æ÷ÇÔ)¿¡¼­ ÃÖ´ë°ª(Æ÷ÇÔ)»çÀÌÀÇ °á°ú°ª
	*/
	global_func inline int GetValue(const int MinValue, const int MaxValue, int Key = -1)
	{
		return MinValue + (GetRandom(Key) % (MaxValue - MinValue + 1));
	}

	/*!
	\brief °¢µµ³» ·£´ý°ª ±¸ÇÏ±â
	\param MinValue : ÃÖ¼Ò°¢µµ°ª
	\param MaxValue : ÃÖ´ë°¢µµ°ª
	\param Key : ·£´ýÅ×ÀÌºí ÀÎµ¦½º°ª, -1Àº ½Ç ·£´ý°ª
	\return ÃÖ¼Ò°ª(Æ÷ÇÔ)¿¡¼­ ÃÖ´ë°ª(Æ÷ÇÔ)»çÀÌÀÇ °á°ú°ª
	*/
	global_func inline int GetValueAngle(const int MinValue, const int MaxValue, int Key = -1)
	{
		if(180 < MaxValue - MinValue)
			return (MaxValue + (GetRandom(Key) % (360 - MaxValue + MinValue + 1))) % 360;
		return GetValue(MinValue, MaxValue, Key) % 360;
	}

	/*!
	\brief Cos°ª ±¸ÇÏ±â
	\param Angle1024 : °¢µµ°ª
	\return Cos°ª
	*/
	global_func inline fint Cos(int Angle1024)
	{
		Angle1024 = Angle1024 & 0x3FF;
		return Cos1024Table(Angle90 - BxUtilGlobal::Abs((Angle1024 & 0x1FF) - Angle90))
			* ((((Angle1024 + Angle270 - 1) & 0x3FF) < Angle180)? -1 : 1);
	}

	/*!
	\brief Sin°ª ±¸ÇÏ±â
	\param Angle1024 : °¢µµ°ª
	\return Sin°ª
	*/
	global_func inline fint Sin(int Angle1024)
	{
		return Cos(Angle1024 + Angle270);
	}

	/*!
	\brief ¹æÇâÀ¸·Î °¢µµ ±¸ÇÏ±â
	\param VectorX : ¹æÇâX
	\param VectorY : ¹æÇâY
	\return °¢µµ°ª
	*/
	global_func int GetAngle1024(const int VectorX, const int VectorY)
	{
		if(VectorX == 0) return (VectorY < 0)? Angle270 : Angle90;
		int Angle1024 = -1, Tan1024 = BxUtilGlobal::Abs((int)((((huge) VectorY) << 16) / ((huge) VectorX)));
		while(++Angle1024 < Angle90 && Tan1024Table(Angle1024) <= Tan1024);
		return 0x3FF & (((VectorY < 0)? Angle180 : 0)
			+ (((VectorX < 0) == (VectorY < 0))? Angle1024 : Angle180 - Angle1024));
	}

	/*!
	\brief Á¡ 3°³ÀÇ ¹æÇâ¼º °ªÀ» È®ÀÎ
	\param Start : Ã¹Á¡
	\param End : µÑÂ°Á¡
	\param Src : ºñ±³ÇÒ ¼ÂÂ°Á¡
	\return À½¼ö : ½Ã°è¹æÇâ, ¾ç¼ö : ¹Ý½Ã°è¹æÇâ, 0 : Áß¸³
	*/
	global_func inline const int GetClockwiseValue(const point& Start, const point& End, const point& Src)
	{
		return (Start.x - End.x) * (Src.y - End.y) - (Start.y - End.y) * (Src.x - End.x);
	}

	/*!
	\brief ´ÜÀ§Çà·ÄÈ­
	\param Dst : Àû¿ëµÉ Çà·Ä
	*/
	global_func inline void MatrixInit(matrix _out_ Dst)
	{
		BxCore::Util::MemSet(&Dst, 0, sizeof(matrix));
		Dst.m[0][0] = Dst.m[1][1] = Dst.m[2][2] = Dst.m[3][3] = ItoF(1);
	}

	/*!
	\brief Çà·Ä¿¡ Çà·Ä°öÇÏ±â
	\param Dst : Àû¿ëµÉ Çà·Ä
	\param Src : °öÇÒ Çà·Ä
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
	\brief Çà·Ä¿¡ Çà·Ä°öÇÏ±â
	\param Dst : Àû¿ëµÉ Çà·Ä
	\param Src00 : °öÇÒ Çà·Ä
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
	\brief Çà·ÄÀÇ È¸Àüº¯È¯(ÄõÅÍ´Ï¾ð)
	\param Dst : Àû¿ëµÉ Çà·Ä
	\param Src : È¸ÀüÇÒ ÄõÅÍ´Ï¾ð
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
	\brief ¹öÅØ½º¿¡ Çà·Ä°öÇÏ±â
	\param Dst : Àû¿ëµÉ ¹öÅØ½º
	\param Src : °öÇÒ Çà·Ä
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
	\brief CP949¸¦ UTF16À¸·Î ÀüÈ¯
	\param cp949 : º¯È¯ÇÒ CP949½ºÆ®¸µ
	\param cp949length : º¯È¯ÇÒ CP949½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\param utf16 : ¹ÞÀ» UTF16½ºÆ®¸µ
	\param utf16length : ¹ÞÀ» UTF16½ºÆ®¸µ ¹è¿­¼ö·®
	\return º¯È¯µÈ ¹è¿­¼ö·®
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
	\brief UTF16¸¦ CP949·Î ÀüÈ¯
	\param utf16 : º¯È¯ÇÒ UTF16½ºÆ®¸µ
	\param utf16length : º¯È¯ÇÒ UTF16½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\param cp949 : ¹ÞÀ» CP949½ºÆ®¸µ
	\param cp949length : ¹ÞÀ» CP949½ºÆ®¸µ ¹è¿­¼ö·®
	\return º¯È¯µÈ ¹è¿­¼ö·®
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
			if(0x80 <= Result[0] && cp949focus < cp949length)
				cp949[cp949focus++] = Result[1];
		}
		cp949[cp949focus] = 0;
		return cp949focus;
	}

	/*!
	\brief UTF16¸¦ UTF8·Î ÀüÈ¯
	\param utf16 : º¯È¯ÇÒ UTF16½ºÆ®¸µ
	\param utf16length : º¯È¯ÇÒ UTF16½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\param utf8 : ¹ÞÀ» UTF8½ºÆ®¸µ
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
	\brief UTF16À» UTF8·Î ÀüÈ¯½Ã ÇÊ¿äÇÑ UTF8ÀÇ ¹è¿­¼ö·®
	\param utf16 : º¯È¯ÇÒ UTF16½ºÆ®¸µ
	\param utf16length : º¯È¯ÇÒ UTF16½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\return ÇÊ¿äÇÑ ¹è¿­¼ö·®
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
	\brief UTF8À» UTF16À¸·Î ÀüÈ¯
	\param utf8 : º¯È¯ÇÒ UTF8½ºÆ®¸µ
	\param utf8length : º¯È¯ÇÒ UTF8½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\param utf16 : ¹ÞÀ» UTF16½ºÆ®¸µ
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
	\brief UTF8À» UTF16À¸·Î ÀüÈ¯½Ã ÇÊ¿äÇÑ UTF16ÀÇ ¹è¿­¼ö·®
	\param utf8 : º¯È¯ÇÒ UTF8½ºÆ®¸µ
	\param utf8length : º¯È¯ÇÒ UTF8½ºÆ®¸µ ¹è¿­¼ö·®(-1Àº ±æÀÌ ÀÚµ¿ÃøÁ¤)
	\return ÇÊ¿äÇÑ ¹è¿­¼ö·®
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
	\brief ±³Â÷Å×½ºÆ®ÈÄ ±³Â÷Á¡À» ±¸ÇÔ
	\param DstB : ¼±ºÐ1 Ã¹Á¡
	\param DstE : ¼±ºÐ1 ³¡Á¡
	\param SrcB : ¼±ºÐ2 Ã¹Á¡
	\param SrcE : ¼±ºÐ2 ³¡Á¡
	\return ±³Â÷½Ã ±³Â÷Á¤º¸¸¦ ¸®ÅÏÇÔ
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
			// ¼öÁ÷
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
			// ¼öÆò
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
			// Á÷±³
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
			// ²ÀÁöÁ¡´êÀ½
			else if((DstB.x == SrcB.x && DstB.y == SrcB.y) || (DstE.x == SrcE.x && DstE.y == SrcE.y)
				|| (DstB.x == SrcE.x && DstB.y == SrcE.y) || (DstE.x == SrcB.x && DstE.y == SrcB.y))
				return nullptr;
			else
			{
				// Á÷¼±A
				fint A_A = ItoF(DstB.y - DstE.y);
				fint A_B = ItoF(DstE.x - DstB.x);
				const fint A_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(A_A) + BxUtilGlobal::Pow64(A_B));
				A_A = (((huge) A_A) << 16) / A_Distance;
				A_B = (((huge) A_B) << 16) / A_Distance;
				const fint A_C = A_A * DstB.x + A_B * DstB.y;
				// Á÷¼±B
				fint B_A = ItoF(SrcB.y - SrcE.y);
				fint B_B = ItoF(SrcE.x - SrcB.x);
				const fint B_Distance = BxUtilGlobal::Sqrt64(BxUtilGlobal::Pow64(B_A) + BxUtilGlobal::Pow64(B_B));
				B_A = (((huge) B_A) << 16) / B_Distance;
				B_B = (((huge) B_B) << 16) / B_Distance;
				const fint B_C = B_A * SrcB.x + B_B * SrcB.y;
				// ±³Á¡±¸ÇÏ±â
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

	global_func inline const int& Cos1024Table(int Index)
	{
		global_data const int _Cos1024Table[] = {
			0x10000,
			0xFFFF, 0xFFFB, 0xFFF5, 0xFFEC, 0xFFE1, 0xFFD4, 0xFFC4, 0xFFB1, 0xFF9C, 0xFF85, 0xFF6B, 0xFF4E, 0xFF30, 0xFF0E, 0xFEEB, 0xFEC4,
			0xFE9C, 0xFE71, 0xFE43, 0xFE13, 0xFDE1, 0xFDAC, 0xFD74, 0xFD3B, 0xFCFE, 0xFCC0, 0xFC7F, 0xFC3B, 0xFBF5, 0xFBAD, 0xFB62, 0xFB15,
			0xFAC5, 0xFA73, 0xFA1F, 0xF9C8, 0xF96E, 0xF913, 0xF8B4, 0xF854, 0xF7F1, 0xF78C, 0xF724, 0xF6BA, 0xF64E, 0xF5DF, 0xF56E, 0xF4FA,
			0xF484, 0xF40C, 0xF391, 0xF314, 0xF295, 0xF213, 0xF18F, 0xF109, 0xF080, 0xEFF5, 0xEF68, 0xEED9, 0xEE47, 0xEDB3, 0xED1C, 0xEC83,
			0xEBE8, 0xEB4B, 0xEAAB, 0xEA0A, 0xE966, 0xE8BF, 0xE817, 0xE76C, 0xE6BF, 0xE610, 0xE55E, 0xE4AA, 0xE3F4, 0xE33C, 0xE282, 0xE1C6,
			0xE107, 0xE046, 0xDF83, 0xDEBE, 0xDDF7, 0xDD2D, 0xDC62, 0xDB94, 0xDAC4, 0xD9F2, 0xD91E, 0xD848, 0xD770, 0xD696, 0xD5BA, 0xD4DB,
			0xD3FB, 0xD318, 0xD234, 0xD14D, 0xD065, 0xCF7A, 0xCE8E, 0xCD9F, 0xCCAE, 0xCBBC, 0xCAC7, 0xC9D1, 0xC8D9, 0xC7DE, 0xC6E2, 0xC5E4,
			0xC4E4, 0xC3E2, 0xC2DE, 0xC1D8, 0xC0D1, 0xBFC7, 0xBEBC, 0xBDAF, 0xBCA0, 0xBB8F, 0xBA7D, 0xB968, 0xB852, 0xB73A, 0xB620, 0xB505,
			0xB3E8, 0xB2C9, 0xB1A8, 0xB086, 0xAF62, 0xAE3C, 0xAD14, 0xABEB, 0xAAC1, 0xA994, 0xA866, 0xA736, 0xA605, 0xA4D2, 0xA39E, 0xA268,
			0xA130, 0x9FF7, 0x9EBC, 0x9D80, 0x9C42, 0x9B03, 0x99C2, 0x9880, 0x973C, 0x95F7, 0x94B0, 0x9368, 0x921F, 0x90D4, 0x8F88, 0x8E3A,
			0x8CEB, 0x8B9A, 0x8A49, 0x88F6, 0x87A1, 0x864C, 0x84F5, 0x839C, 0x8243, 0x80E8, 0x7F8C, 0x7E2F, 0x7CD0, 0x7B70, 0x7A10, 0x78AD,
			0x774A, 0x75E6, 0x7480, 0x731A, 0x71B2, 0x7049, 0x6EDF, 0x6D74, 0x6C08, 0x6A9B, 0x692D, 0x67BE, 0x664E, 0x64DD, 0x636B, 0x61F8,
			0x6084, 0x5F0F, 0x5D99, 0x5C22, 0x5AAA, 0x5932, 0x57B9, 0x563E, 0x54C3, 0x5348, 0x51CB, 0x504D, 0x4ECF, 0x4D50, 0x4BD1, 0x4A50,
			0x48CF, 0x474D, 0x45CB, 0x4447, 0x42C3, 0x413F, 0x3FBA, 0x3E34, 0x3CAE, 0x3B27, 0x399F, 0x3817, 0x368E, 0x3505, 0x337C, 0x31F1,
			0x3067, 0x2EDC, 0x2D50, 0x2BC4, 0x2A38, 0x28AB, 0x271E, 0x2590, 0x2402, 0x2274, 0x20E5, 0x1F56, 0x1DC7, 0x1C38, 0x1AA8, 0x1918,
			0x1787, 0x15F7, 0x1466, 0x12D5, 0x1144, 0x0FB3, 0x0E21, 0x0C90, 0x0AFE, 0x096C, 0x07DA, 0x0648, 0x04B6, 0x0324, 0x0192, 0x0000
		};
		return _Cos1024Table[Index];
	}

	global_func inline const int& Tan1024Table(int Index)
	{
		global_data const int _Tan1024Table[] = {
			0x000000C9, 0x0000025B, 0x000003ED, 0x0000057F, 0x00000712, 0x000008A4, 0x00000A37, 0x00000BCA, 0x00000D5D, 0x00000EF0, 0x00001084, 0x00001218, 0x000013AC, 0x00001541, 0x000016D6, 0x0000186B,
			0x00001A01, 0x00001B98, 0x00001D2F, 0x00001EC7, 0x0000205F, 0x000021F8, 0x00002391, 0x0000252B, 0x000026C6, 0x00002862, 0x000029FF, 0x00002B9C, 0x00002D3A, 0x00002ED9, 0x00003079, 0x0000321B,
			0x000033BD, 0x00003560, 0x00003704, 0x000038A9, 0x00003A4F, 0x00003BF7, 0x00003DA0, 0x00003F4A, 0x000040F5, 0x000042A2, 0x00004450, 0x000045FF, 0x000047B0, 0x00004963, 0x00004B17, 0x00004CCC,
			0x00004E83, 0x0000503C, 0x000051F7, 0x000053B3, 0x00005571, 0x00005731, 0x000058F2, 0x00005AB6, 0x00005C7C, 0x00005E43, 0x0000600D, 0x000061D9, 0x000063A7, 0x00006577, 0x00006749, 0x0000691E,
			0x00006AF5, 0x00006CCF, 0x00006EAB, 0x00007089, 0x0000726B, 0x0000744E, 0x00007635, 0x0000781E, 0x00007A0A, 0x00007BF9, 0x00007DEB, 0x00007FE0, 0x000081D8, 0x000083D3, 0x000085D1, 0x000087D3,
			0x000089D8, 0x00008BE1, 0x00008DED, 0x00008FFC, 0x0000920F, 0x00009426, 0x00009641, 0x0000985F, 0x00009A82, 0x00009CA9, 0x00009ED4, 0x0000A103, 0x0000A336, 0x0000A56E, 0x0000A7AA, 0x0000A9EB,
			0x0000AC31, 0x0000AE7B, 0x0000B0CB, 0x0000B31F, 0x0000B579, 0x0000B7D7, 0x0000BA3C, 0x0000BCA5, 0x0000BF15, 0x0000C18A, 0x0000C405, 0x0000C686, 0x0000C90D, 0x0000CB9A, 0x0000CE2E, 0x0000D0C8,
			0x0000D369, 0x0000D611, 0x0000D8BF, 0x0000DB75, 0x0000DE33, 0x0000E0F8, 0x0000E3C4, 0x0000E699, 0x0000E975, 0x0000EC5A, 0x0000EF47, 0x0000F23D, 0x0000F53B, 0x0000F843, 0x0000FB54, 0x0000FE6F,
			0x00010193, 0x000104C1, 0x000107FA, 0x00010B3D, 0x00010E8A, 0x000111E3, 0x00011547, 0x000118B7, 0x00011C33, 0x00011FBB, 0x0001234F, 0x000126F0, 0x00012A9F, 0x00012E5B, 0x00013225, 0x000135FE,
			0x000139E5, 0x00013DDB, 0x000141E1, 0x000145F7, 0x00014A1D, 0x00014E55, 0x0001529E, 0x000156F8, 0x00015B66, 0x00015FE6, 0x0001647A, 0x00016922, 0x00016DDF, 0x000172B1, 0x00017799, 0x00017C99,
			0x000181AF, 0x000186DF, 0x00018C27, 0x00019189, 0x00019706, 0x00019C9F, 0x0001A254, 0x0001A827, 0x0001AE18, 0x0001B42A, 0x0001BA5C, 0x0001C0B0, 0x0001C727, 0x0001CDC3, 0x0001D484, 0x0001DB6D,
			0x0001E27F, 0x0001E9BB, 0x0001F123, 0x0001F8B9, 0x0002007E, 0x00020875, 0x0002109F, 0x000218FF, 0x00022197, 0x00022A69, 0x00023378, 0x00023CC6, 0x00024657, 0x0002502D, 0x00025A4C, 0x000264B7,
			0x00026F71, 0x00027A7E, 0x000285E3, 0x000291A3, 0x00029DC4, 0x0002AA4A, 0x0002B73B, 0x0002C49C, 0x0002D273, 0x0002E0C7, 0x0002EFA0, 0x0002FF03, 0x00030EFA, 0x00031F8E, 0x000330C7, 0x000342B1,
			0x00035555, 0x000368C1, 0x00037D02, 0x00039225, 0x0003A83A, 0x0003BF54, 0x0003D783, 0x0003F0DE, 0x00040B7A, 0x00042771, 0x000444DE, 0x000463E1, 0x0004849B, 0x0004A733, 0x0004CBD4, 0x0004F2AD,
			0x00051BF5, 0x000547E9, 0x000576CE, 0x0005A8F5, 0x0005DEB8, 0x0006187F, 0x000656C3, 0x00069A12, 0x0006E30F, 0x0007327E, 0x00078943, 0x0007E873, 0x00085158, 0x0008C585, 0x000946EA, 0x0009D7EE,
			0x000A7B97, 0x000B35BF, 0x000C0B69, 0x000D032C, 0x000E25EF, 0x000F7FFA, 0x001122C3, 0x001327F6, 0x0015B6F0, 0x00190F4A, 0x001D9ED9, 0x00243517, 0x002E8E99, 0x00412F4B, 0x006CA58E, 0x0145F2C3
		};
		return _Tan1024Table[Index];
	}

	global_func wstring CP949Table()
	{
		global_data wstring CP949 = (wstring)
			"\t \n \r   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L "
			"M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ "
			"¤¡¤¢¤£¤¤¤¥¤¦¤§¤¨¤©¤ª¤«¤¬¤­¤®¤¯¤°¤±¤²¤³¤´¤µ¤¶¤·¤¸¤¹¤º¤»¤¼¤½¤¾"
			"¤¿¤À¤Á¤Â¤Ã¤Ä¤Å¤Æ¤Ç¤È¤É¤Ê¤Ë¤Ì¤Í¤Î¤Ï¤Ð¤Ñ¤Ò¤Ó"
			"°¡°¢°£°¤°¥°¦°§°¨°©°ª°«°¬°­°®°¯°°°±°²°³°´°µ°¶°·°¸°¹°º°»°¼°½°¾°¿°À°Á°Â°Ã°Ä°Å°Æ°Ç°È°É°Ê°Ë°Ì°Í°Î°Ï°Ð°Ñ°Ò°Ó°Ô°Õ°Ö"
			"°×°Ø°Ù°Ú°Û°Ü°Ý°Þ°ß°à°á°â°ã°ä°å°æ°ç°è°é°ê°ë°ì°í°î°ï°ð°ñ°ò°ó°ô°õ°ö°÷°ø°ù°ú°û°ü°ý°þ±¡±¢±£±¤±¥±¦±§‚R±©±ª±«±¬±­±®"
			"±¯±°±±±²±³±´±µ±¶±·±¸±¹±º±»±¼±½±¾±¿±À±Á±Â±Ã±Ä±Å±Æ±Ç±È±É±Ê±Ë±Ì±Í±Î±Ï±Ð±Ñ±Ò±Ó±Ô±Õ±Ö±×±Ø±Ù±Ú±Û±Ü±Ý±Þ±ß±à±á±â±ã±ä"
			"±å±æ±ç±è±é±ê±ë±ì±í±î±ï±ð±ñ±ò±ó±ô±õ±ö±÷±ø±ù±ú±û±ü±ý±þ²¡²¢²£²¤²¥²¦²§²¨²©²ª²«²¬²­²®²¯²°²±²²²³²´²µ²¶²·²¸²¹²º²»²¼"
			"²½²¾²¿²À²Á²Â²Ã²Ä²Å²Æ²Ç²È²É²Ê²Ë²Ì²Í²Î²Ï²Ð²Ñ²Ò²Ó²Ô²Õ²Ö²×²Ø²Ù²Ú²Û²Ü²Ý²Þ²ß²à²á²â²ã²ä²å²æ²ç²è²é²ê²ë²ì²í²î²ï²ð²ñ²ò"
			"²ó²ô²õ²ö²÷²ø²ù²ú²û²ü²ý²þ³¡³¢³£³¤³¥³¦³§³¨³©³ª³«³¬³­³®³¯³°³±³²³³³´³µ³¶³·³¸³¹³º³»³¼³½³¾³¿³À³Á³Â³Ã³Ä³Å³Æ³Ç³È³É³Ê"
			"³Ë³Ì³Í³Î³Ï³Ð³Ñ³Ò³Ó³Ô³Õ³Ö³×³Ø³Ù³Ú³Û³Ü³Ý³Þ³ß³à³á³â³ã³ä³å³æ³ç³è³é³ê³ë³ì³í³î³ï³ð³ñ³ò³ó³ô³õ³ö³÷³ø³ù³ú³û³ü³ý³þ´¡´¢"
			"´£´¤´¥´¦´§´¨´©´ª´«´¬´­´®´¯´°´±´²´³´´´µ´¶´·´¸´¹´º´»´¼´½´¾´¿´À´Á´Â´Ã´Ä´Å´Æ´Ç´È´É´Ê´Ë´Ì´Í´Î´Ï´Ð´Ñ´Òˆš´Ô´Õ´Ö´×´Ø"
			"´Ù´Ú´Û´Ü´Ý´Þ´ß´à´á´â´ã´ä´å´æ´ç´è´é´ê´ë´ì´í´î´ï´ð´ñ´ò´ó´ô´õ´ö´÷´ø´ù´ú´û´ü´ý´þµ¡µ¢µ£µ¤µ¥µ¦µ§µ¨µ©µªµ«µ¬µ­µ®µ¯µ°"
			"µ±µ²µ³µ´µµµ¶µ·µ¸µ¹µºµ»µ¼µ½µ¾µ¿µÀµÁµÂµÃµÄµÅµÆµÇµÈµÉµÊµËµÌµÍµÎµÏµÐµÑµÒµÓµÔµÕµÖµ×µØµÙµÚµÛµÜµÝµÞµßµàµáµâµãµäµåµæ"
			"µçµèµéµêµëµìµíµîµïµðµñµòµóµôµõµöµ÷µøµùµúµûµüµýµþ¶¡¶¢¶£¶¤¶¥¶¦¶§¶¨¶©¶ª¶«¶¬¶­¶®¶¯¶°¶±¶²¶³¶´¶µ¶¶¶·¶¸¶¹¶º¶»¶¼¶½¶¾"
			"¶¿¶À¶Á¶Â¶Ã¶Ä¶Å¶Æ¶Ç¶È¶É¶Ê¶Ë¶Ì¶Í¶Î¶Ï¶Ð¶Ñ¶Ò¶Ó¶Ô¶Õ¶Ö¶×¶Ø¶Ù¶Ú¶Û¶Ü¶Ý¶Þ¶ß¶à¶á¶â¶ã¶ä¶å¶æ¶ç¶è¶é¶ê¶ë¶ì¶í¶î¶ï¶ð¶ñ¶ò¶ó¶ô"
			"¶õ¶ö¶÷¶ø¶ù¶ú¶û¶ü¶ý¶þ·¡·¢·£·¤·¥·¦·§·¨·©·ª·«·¬·­·®·¯·°·±·²·³·´·µ·¶···¸·¹·º·»·¼·½·¾·¿·À·Á·Â·Ã·Ä·Å·Æ·Ç·È·É·Ê·Ë·Ì"
			"·Í·Î·Ï·Ð·Ñ·Ò·Ó·Ô·Õ·Ö·×·Ø·Ù·Ú·Û·Ü·Ý·Þ·ß·à·á·â·ã·ä·å·æ·ç·è·é·ê·ë·ì·í·î·ï·ð·ñ·ò·ó·ô·õ·ö·÷·ø·ù·ú·û·ü·ý·þ¸¡¸¢¸£¸¤"
			"¸¥¸¦¸§¸¨¸©¸ª¸«¸¬¸­¸®¸¯¸°¸±¸²¸³¸´¸µ¸¶¸·¸¸¸¹¸º¸»¸¼¸½¸¾¸¿¸À¸Á¸Â¸Ã¸Ä¸Å¸Æ¸Ç¸È¸É¸Ê¸Ë¸Ì¸Í¸Î¸Ï¸Ð¸Ñ¸Ò¸Ó¸Ô¸Õ¸Ö¸×¸Ø¸Ù¸Ú"
			"¸Û¸Ü¸Ý¸Þ¸ß¸à¸á¸â¸ã¸ä¸å¸æ¸ç¸è¸é¸ê¸ë¸ì¸í¸î¸ï¸ð¸ñ¸ò¸ó¸ô¸õ¸ö¸÷¸ø¸ù¸ú¸û¸ü¸ý¸þ¹¡¹¢¹£¹¤¹¥¹¦¹§¹¨¹©¹ª¹«¹¬¹­¹®¹¯¹°¹±¹²"
			"¹³¹´¹µ¹¶¹·¹¸¹¹¹º¹»¹¼¹½¹¾¹¿¹À¹Á¹Â¹Ã¹Ä¹Å¹Æ¹Ç¹È¹É¹Ê¹Ë¹Ì¹Í¹Î¹Ï¹Ð¹Ñ¹Ò¹Ó¹Ô¹Õ¹Ö¹×¹Ø¹Ù¹Ú¹Û¹Ü¹Ý¹Þ¹ß¹à¹á¹â¹ã¹ä¹å¹æ¹ç¹è"
			"¹é¹ê¹ë¹ì¹í¹î¹ï¹ð¹ñ¹ò¹ó¹ô¹õ¹ö¹÷¹ø¹ù¹ú¹û¹ü¹ý¹þº¡º¢º£º¤º¥º¦º§º¨º©ºªº«º¬º­º®º¯º°º±º²º³º´ºµº¶º·º¸º¹ººº»º¼º½º¾º¿ºÀ"
			"ºÁºÂºÃºÄºÅºÆºÇºÈºÉºÊºËºÌºÍºÎºÏºÐºÑºÒºÓºÔºÕºÖº×ºØºÙºÚºÛºÜºÝºÞºßºàºáºâºãºäºåºæºçºèºéºêºëºìºíºîºïºðºñºòºóºôºõºö"
			"º÷ºøºùºúºûºüºýºþ»¡»¢»£»¤»¥»¦»§»¨»©»ª»«»¬»­»®»¯»°»±»²»³»´»µ»¶»·»¸»¹»º»»»¼»½»¾»¿»À»Á»Â»Ã»Ä»Å»Æ»Ç»È»É»Ê»Ë»Ì»Í»Î"
			"»Ï»Ð»Ñ»Ò»Ó»Ô»Õ»Ö»×»Ø»Ù»Ú»Û»Ü»Ý»Þ»ß»à»á»â»ã»ä»å»æ»ç»è»é»ê»ë»ì»í»î»ï»ð»ñ»ò»ó»ô»õ»ö»÷»ø»ù»ú»û»ü»ý»þ¼¡¼¢¼£¼¤¼¥¼¦"
			"¼§¼¨¼©¼ª¼«¼¬¼­¼®¼¯¼°¼±¼²¼³¼´¼µ¼¶¼·¼¸¼¹¼º¼»¼¼¼½¼¾¼¿¼À¼Á¼Â¼Ã¼Ä¼Å¼Æ¼Ç¼È¼É¼Ê¼Ë¼Ì¼Í¼Î¼Ï¼Ð¼Ñ¼Ò¼Ó¼Ô¼Õ¼Ö¼×¼Ø¼Ù¼Ú¼Û¼Ü"
			"¼Ý¼Þ¼ß¼à¼á¼â¼ã¼ä¼å¼æ¼ç¼è¼é¼ê¼ë¼ì¼í¼î¼ï¼ð¼ñ¼ò¼ó¼ô¼õ¼ö¼÷¼ø¼ù¼ú¼û¼ü¼ý¼þ½¡½¢½£½¤½¥½¦½§½¨½©½ª½«½¬½­½®½¯½°½±½²½³½´"
			"½µ½¶½·½¸½¹½º½»½¼½½½¾½¿½À½Á½Â½Ã½Ä½Å½Æ½Ç½È½É½Ê½Ë½Ì½Í½Î½Ï½Ð½Ñ½Ò½Ó½Ô½Õ½Ö½×½Ø½Ù½Ú½Û½Ü½Ý½Þ½ß½à½á½â½ã½ä½å½æ½ç½è½é½ê"
			"½ë½ì›À½î½ï½ð½ñ½ò½ó½ô½õ½ö½÷½ø½ù½ú½û½ü½ý½þ¾¡¾¢¾£¾¤¾¥¾¦¾§¾¨¾©¾ª¾«¾¬¾­¾®¾¯¾°¾±¾²¾³¾´¾µ¾¶¾·¾¸¾¹¾º¾»¾¼¾½¾¾¾¿¾À¾Á¾Â"
			"¾Ã¾Ä¾Å¾Æ¾Ç¾È¾É¾Ê¾Ë¾Ì¾Í¾Î¾Ï¾Ð¾Ñ¾Ò¾Ó¾Ô¾Õ¾Ö¾×¾Ø¾Ù¾Ú¾Û¾Ü¾Ý¾Þ¾ß¾à¾á¾â¾ã¾ä¾å¾æ¾ç¾è¾é¾ê¾ë¾ì¾í¾î¾ï¾ð¾ñ¾ò¾ó¾ô¾õ¾ö¾÷¾ø"
			"¾ù¾ú¾û¾ü¾ý¾þ¿¡¿¢¿£¿¤¿¥¿¦¿§¿¨¿©¿ª¿«¿¬¿­¿®¿¯¿°¿±¿²¿³¿´¿µ¿¶¿·¿¸¿¹¿º¿»¿¼¿½¿¾¿¿¿À¿Á¿Â¿Ã¿Ä¿Å¿Æ¿Ç¿È¿É¿Ê¿Ë¿Ì¿Í¿Î¿Ï¿Ð"
			"¿Ñ¿Ò¿Ó¿Ô¿Õ¿Ö¿×¿Ø¿Ù¿Ú¿Û¿Ü¿Ý¿Þ¿ß¿à¿á¿â¿ã¿ä¿å¿æ¿ç¿è¿é¿ê¿ë¿ì¿í¿î¿ï¿ð¿ñ¿ò¿ó¿ô¿õ¿ö¿÷¿ø¿ù¿ú¿û¿ü¿ý¿þÀ¡À¢À£À¤À¥À¦À§À¨"
			"À©ÀªÀ«À¬À­À®À¯À°À±À²À³À´ÀµÀ¶À·À¸À¹ÀºÀ»À¼À½À¾À¿ÀÀÀÁÀÂÀÃÀÄÀÅÀÆÀÇÀÈÀÉÀÊÀËÀÌÀÍÀÎÀÏÀÐÀÑÀÒÀÓÀÔÀÕÀÖÀ×ÀØÀÙÀÚÀÛÀÜÀÝÀÞ"
			"ÀßÀàÀáÀâÀãÀäÀåÀæÀçÀèÀéÀêÀëÀìÀíÀîÀïÀðÀñÀòÀóÀôÀõÀöÀ÷ÀøÀùÀúÀûÀüÀýÀþÁ¡Á¢Á£Á¤Á¥Á¦Á§Á¨Á©ÁªÁ«Á¬Á­Á®Á¯Á°Á±Á²Á³Á´ÁµÁ¶"
			"Á·Á¸Á¹ÁºÁ»Á¼Á½Á¾Á¿ÁÀÁÁÁÂÁÃÁÄÁÅÁÆÁÇÁÈÁÉÁÊÁËÁÌÁÍÁÎÁÏÁÐÁÑÁÒÁÓÁÔÁÕÁÖÁ×ÁØÁÙÁÚÁÛÁÜÁÝÁÞÁßÁàÁáÁâÁãÁäÁåÁæÁçÁèÁéÁêÁëÁì"
			"ÁíÁîÁïÁðÁñÁòÁóÁôÁõÁöÁ÷ÁøÁùÁúÁûÁüÁýÁþÂ¡Â¢Â£Â¤Â¥Â¦Â§Â¨Â©ÂªÂ«Â¬Â­Â®Â¯Â°Â±Â²Â³Â´ÂµÂ¶Â·Â¸Â¹ÂºÂ»Â¼Â½Â¾Â¿ÂÀÂÁÂÂÂÃÂÄ"
			"ÂÅÂÆÂÇÂÈÂÉÂÊÂËÂÌÂÍÂÎÂÏÂÐÂÑÂÒÂÓÂÔÂÕÂÖÂ×ÂØÂÙÂÚÂÛÂÜÂÝÂÞÂßÂàÂáÂâÂãÂäÂåÂæÂçÂèÂéÂêÂëÂìÂíÂîÂïÂðÂñÂòÂóÂôÂõÂöÂ÷ÂøÂùÂú"
			"ÂûÂüÂýÂþÃ¡Ã¢Ã£Ã¤Ã¥Ã¦Ã§Ã¨Ã©ÃªÃ«Ã¬Ã­Ã®Ã¯Ã°Ã±Ã²Ã³Ã´ÃµÃ¶Ã·Ã¸Ã¹ÃºÃ»Ã¼Ã½Ã¾Ã¿ÃÀÃÁÃÂÃÃÃÄÃÅÃÆÃÇÃÈÃÉÃÊÃËÃÌÃÍÃÎÃÏÃÐÃÑÃÒ"
			"ÃÓÃÔÃÕÃÖÃ×ÃØÃÙÃÚÃÛÃÜÃÝÃÞÃßÃàÃáÃâÃãÃäÃåÃæÃçÃèÃéÃêÃëÃìÃíÃîÃïÃðÃñÃòÃóÃôÃõÃöÃ÷ÃøÃùÃúÃûÃüÃýÃþÄ¡Ä¢Ä£Ä¤Ä¥Ä¦Ä§Ä¨Ä©Äª"
			"Ä«Ä¬Ä­Ä®Ä¯Ä°Ä±Ä²Ä³Ä´ÄµÄ¶Ä·Ä¸Ä¹ÄºÄ»Ä¼Ä½Ä¾Ä¿ÄÀÄÁÄÂÄÃÄÄÄÅÄÆÄÇÄÈÄÉÄÊÄËÄÌÄÍÄÎÄÏÄÐÄÑÄÒÄÓÄÔÄÕÄÖÄ×ÄØÄÙÄÚÄÛÄÜÄÝÄÞÄßÄà"
			"ÄáÄâÄãÄäÄåÄæÄçÄèÄéÄêÄëÄìÄíÄîÄïÄðÄñÄòÄóÄôÄõÄöÄ÷ÄøÄùÄúÄûÄüÄýÄþÅ¡Å¢Å£Å¤Å¥Å¦Å§Å¨Å©ÅªÅ«Å¬Å­Å®Å¯Å°Å±Å²Å³Å´ÅµÅ¶Å·Å¸"
			"Å¹ÅºÅ»Å¼Å½Å¾Å¿ÅÀÅÁÅÂÅÃÅÄÅÅÅÆÅÇÅÈÅÉÅÊÅËÅÌÅÍÅÎÅÏÅÐÅÑÅÒÅÓÅÔÅÕÅÖÅ×ÅØÅÙÅÚÅÛÅÜÅÝÅÞÅßÅàÅáÅâÅãÅäÅåÅæÅçÅèÅéÅêÅëÅìÅíÅî"
			"ÅïÅðÅñÅòÅóÅôÅõÅöÅ÷ÅøÅùÅúÅûÅüÅýÅþÆ¡Æ¢Æ£Æ¤Æ¥Æ¦Æ§Æ¨Æ©ÆªÆ«Æ¬Æ­Æ®Æ¯Æ°Æ±Æ²Æ³Æ´ÆµÆ¶Æ·Æ¸Æ¹ÆºÆ»Æ¼Æ½Æ¾Æ¿ÆÀÆÁÆÂÆÃÆÄÆÅÆÆ"
			"ÆÇÆÈÆÉÆÊÆËÆÌÆÍÆÎÆÏÆÐÆÑÆÒÆÓÆÔÆÕÆÖÆ×ÆØÆÙÆÚÆÛÆÜÆÝÆÞÆßÆàÆáÆâÆãÆäÆåÆæÆçÆèÆéÆêÆëÆìÆíÆîÆïÆðÆñÆòÆóÆôÆõÆöÆ÷ÆøÆùÆúÆûÆü"
			"ÆýÆþÇ¡Ç¢Ç£Ç¤Ç¥Ç¦Ç§Ç¨Ç©ÇªÇ«Ç¬Ç­Ç®Ç¯Ç°Ç±Ç²Ç³Ç´ÇµÇ¶Ç·Ç¸Ç¹ÇºÇ»Ç¼Ç½Ç¾Ç¿ÇÀÇÁÇÂÇÃÇÄÇÅÇÆÇÇÇÈÇÉÇÊÇËÇÌÇÍÇÎÇÏÇÐÇÑÇÒÇÓÇÔ"
			"ÇÕÇÖÇ×ÇØÇÙÇÚÇÛÇÜÇÝÇÞÇßÇàÇáÇâÇãÇäÇåÇæÇçÇèÇéÇêÇëÇìÇíÇîÇïÇðÇñÇòÇóÇôÇõÇöÇ÷ÇøÇùÇúÇûÇüÇýÇþÈ¡È¢È£È¤È¥È¦È§È¨È©ÈªÈ«È¬"
			"È­È®È¯È°È±È²È³È´ÈµÈ¶È·È¸È¹ÈºÈ»È¼È½È¾È¿ÈÀÈÁÈÂÈÃÈÄÈÅÈÆÈÇÈÈÈÉÈÊÈËÈÌÈÍÈÎÈÏÈÐÈÑÈÒÈÓÈÔÈÕÈÖÈ×ÈØÈÙÈÚÈÛÈÜÈÝÈÞÈßÈàÈáÈâ"
			"ÈãÈäÈåÈæÈçÈèÈéÈêÈëÈìÈíÈîÈïÈðÈñÈòÈóÈôÈõÈöÈ÷ÈøÈùÈúÈûÈüÈýÈþ";
		return CP949;
	}

	global_func wstring UTF16Table()
	{
		global_data wstring UTF16 = (wstring)
			L"\t\n\r !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKL"
			L"MNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
			L"\x3131\x3132\x3133\x3134\x3135\x3136\x3137\x3138\x3139\x313A\x313B\x313C\x313D\x313E\x313F\x3140\x3141\x3142\x3143\x3144\x3145\x3146\x3147\x3148\x3149\x314A\x314B\x314C\x314D\x314E"
			L"\x314F\x3150\x3151\x3152\x3153\x3154\x3155\x3156\x3157\x3158\x3159\x315A\x315B\x315C\x315D\x315E\x315F\x3160\x3161\x3162\x3163"
			L"\xAC00\xAC01\xAC04\xAC07\xAC08\xAC09\xAC0A\xAC10\xAC11\xAC12\xAC13\xAC14\xAC15\xAC16\xAC17\xAC19\xAC1A\xAC1B\xAC1C\xAC1D\xAC20\xAC24\xAC2C\xAC2D\xAC2F"
			L"\xAC30\xAC31\xAC38\xAC39\xAC3C\xAC40\xAC4B\xAC4D\xAC54\xAC58\xAC5C\xAC70\xAC71\xAC74\xAC77\xAC78\xAC7A\xAC80\xAC81\xAC83\xAC84\xAC85\xAC86\xAC89\xAC8A"
			L"\xAC8B\xAC8C\xAC90\xAC94\xAC9C\xAC9D\xAC9F\xACA0\xACA1\xACA8\xACA9\xACAA\xACAC\xACAF\xACB0\xACB8\xACB9\xACBB\xACBC\xACBD\xACC1\xACC4\xACC8\xACCC\xACD5"
			L"\xACD7\xACE0\xACE1\xACE4\xACE7\xACE8\xACEA\xACEC\xACEF\xACF0\xACF1\xACF3\xACF5\xACF6\xACFC\xACFD\xAD00\xAD04\xAD06\xAD0C\xAD0D\xAD0F\xAD11\xAD18\xAD1C"
			L"\xAD20\xAD28\xAD2C\xAD2D\xAD34\xAD35\xAD38\xAD3C\xAD44\xAD45\xAD47\xAD49\xAD50\xAD54\xAD58\xAD61\xAD63\xAD6C\xAD6D\xAD70\xAD73\xAD74\xAD75\xAD76\xAD7B"
			L"\xAD7C\xAD7D\xAD7F\xAD81\xAD82\xAD88\xAD89\xAD8C\xAD90\xAD9C\xAD9D\xADA4\xADB7\xADC0\xADC1\xADC4\xADC8\xADD0\xADD1\xADD3\xADDC\xADE0\xADE4\xADF8\xADF9"
			L"\xADFC\xADFF\xAE00\xAE01\xAE08\xAE09\xAE0B\xAE0D\xAE14\xAE30\xAE31\xAE34\xAE37\xAE38\xAE3A\xAE40\xAE41\xAE43\xAE45\xAE46\xAE4A\xAE4C\xAE4D\xAE4E\xAE50"
			L"\xAE54\xAE56\xAE5C\xAE5D\xAE5F\xAE60\xAE61\xAE65\xAE68\xAE69\xAE6C\xAE70\xAE78\xAE79\xAE7B\xAE7C\xAE7D\xAE84\xAE85\xAE8C\xAEBC\xAEBD\xAEBE\xAEC0\xAEC4"
			L"\xAECC\xAECD\xAECF\xAED0\xAED1\xAED8\xAED9\xAEDC\xAEE8\xAEEB\xAEED\xAEF4\xAEF8\xAEFC\xAF07\xAF08\xAF0D\xAF10\xAF2C\xAF2D\xAF30\xAF32\xAF34\xAF3C\xAF3D"
			L"\xAF3F\xAF41\xAF42\xAF43\xAF48\xAF49\xAF50\xAF5C\xAF5D\xAF64\xAF65\xAF79\xAF80\xAF84\xAF88\xAF90\xAF91\xAF95\xAF9C\xAFB8\xAFB9\xAFBC\xAFC0\xAFC7\xAFC8"
			L"\xAFC9\xAFCB\xAFCD\xAFCE\xAFD4\xAFDC\xAFE8\xAFE9\xAFF0\xAFF1\xAFF4\xAFF8\xB000\xB001\xB004\xB00C\xB010\xB014\xB01C\xB01D\xB028\xB044\xB045\xB048\xB04A"
			L"\xB04C\xB04E\xB053\xB054\xB055\xB057\xB059\xB05D\xB07C\xB07D\xB080\xB084\xB08C\xB08D\xB08F\xB091\xB098\xB099\xB09A\xB09C\xB09F\xB0A0\xB0A1\xB0A2\xB0A8"
			L"\xB0A9\xB0AB\xB0AC\xB0AD\xB0AE\xB0AF\xB0B1\xB0B3\xB0B4\xB0B5\xB0B8\xB0BC\xB0C4\xB0C5\xB0C7\xB0C8\xB0C9\xB0D0\xB0D1\xB0D4\xB0D8\xB0E0\xB0E5\xB108\xB109"
			L"\xB10B\xB10C\xB110\xB112\xB113\xB118\xB119\xB11B\xB11C\xB11D\xB123\xB124\xB125\xB128\xB12C\xB134\xB135\xB137\xB138\xB139\xB140\xB141\xB144\xB148\xB150"
			L"\xB151\xB154\xB155\xB158\xB15C\xB160\xB178\xB179\xB17C\xB180\xB182\xB188\xB189\xB18B\xB18D\xB192\xB193\xB194\xB198\xB19C\xB1A8\xB1CC\xB1D0\xB1D4\xB1DC"
			L"\xB1DD\xB1DF\xB1E8\xB1E9\xB1EC\xB1F0\xB1F9\xB1FB\xB1FD\xB204\xB205\xB208\xB20B\xB20C\xB214\xB215\xB217\xB219\xB220\xB234\xB23C\xB258\xB25C\xB260\xB268"
			L"\xB269\xB274\xB275\xB27C\xB284\xB285\xB289\xB290\xB291\xB294\xB298\xB299\xB29A\xB2A0\xB2A1\xB2A3\xB2A5\xB2A6\xB2AA\xB2AC\xB2B0\xB2B4\xB2C8\xB2C9\xB2CC"
			L"\xB2D0\xB2D6\xB2D8\xB2D9\xB2DB\xB2DD\xB2E2\xB2E4\xB2E5\xB2E6\xB2E8\xB2EB\xB2EC\xB2ED\xB2EE\xB2EF\xB2F3\xB2F4\xB2F5\xB2F7\xB2F8\xB2F9\xB2FA\xB2FB\xB2FF"
			L"\xB300\xB301\xB304\xB308\xB310\xB311\xB313\xB314\xB315\xB31C\xB354\xB355\xB356\xB358\xB35B\xB35C\xB35E\xB35F\xB364\xB365\xB367\xB369\xB36B\xB36E\xB370"
			L"\xB371\xB374\xB378\xB380\xB381\xB383\xB384\xB385\xB38C\xB390\xB394\xB3A0\xB3A1\xB3A8\xB3AC\xB3C4\xB3C5\xB3C8\xB3CB\xB3CC\xB3CE\xB3D0\xB3D4\xB3D5\xB3D7"
			L"\xB3D9\xB3DB\xB3DD\xB3E0\xB3E4\xB3E8\xB3FC\xB410\xB418\xB41C\xB420\xB428\xB429\xB42B\xB434\xB450\xB451\xB454\xB458\xB460\xB461\xB463\xB465\xB46C\xB480"
			L"\xB488\xB49D\xB4A4\xB4A8\xB4AC\xB4B5\xB4B7\xB4B9\xB4C0\xB4C4\xB4C8\xB4D0\xB4D5\xB4DC\xB4DD\xB4E0\xB4E3\xB4E4\xB4E6\xB4EC\xB4ED\xB4EF\xB4F1\xB4F8\xB514"
			L"\xB515\xB518\xB51B\xB51C\xB524\xB525\xB527\xB528\xB529\xB52A\xB530\xB531\xB534\xB538\xB540\xB541\xB543\xB544\xB545\xB54B\xB54C\xB54D\xB550\xB554\xB55C"
			L"\xB55D\xB55F\xB560\xB561\xB5A0\xB5A1\xB5A4\xB5A8\xB5AA\xB5AB\xB5B0\xB5B1\xB5B3\xB5B4\xB5B5\xB5BB\xB5BC\xB5BD\xB5C0\xB5C4\xB5CC\xB5CD\xB5CF\xB5D0\xB5D1"
			L"\xB5D8\xB5EC\xB610\xB611\xB614\xB618\xB625\xB62C\xB634\xB648\xB664\xB668\xB69C\xB69D\xB6A0\xB6A4\xB6AB\xB6AC\xB6B1\xB6D4\xB6F0\xB6F4\xB6F8\xB700\xB701"
			L"\xB705\xB728\xB729\xB72C\xB72F\xB730\xB738\xB739\xB73B\xB744\xB748\xB74C\xB754\xB755\xB760\xB764\xB768\xB770\xB771\xB773\xB775\xB77C\xB77D\xB780\xB784"
			L"\xB78C\xB78D\xB78F\xB790\xB791\xB792\xB796\xB797\xB798\xB799\xB79C\xB7A0\xB7A8\xB7A9\xB7AB\xB7AC\xB7AD\xB7B4\xB7B5\xB7B8\xB7C7\xB7C9\xB7EC\xB7ED\xB7F0"
			L"\xB7F4\xB7FC\xB7FD\xB7FF\xB800\xB801\xB807\xB808\xB809\xB80C\xB810\xB818\xB819\xB81B\xB81D\xB824\xB825\xB828\xB82C\xB834\xB835\xB837\xB838\xB839\xB840"
			L"\xB844\xB851\xB853\xB85C\xB85D\xB860\xB864\xB86C\xB86D\xB86F\xB871\xB878\xB87C\xB88D\xB8A8\xB8B0\xB8B4\xB8B8\xB8C0\xB8C1\xB8C3\xB8C5\xB8CC\xB8D0\xB8D4"
			L"\xB8DD\xB8DF\xB8E1\xB8E8\xB8E9\xB8EC\xB8F0\xB8F8\xB8F9\xB8FB\xB8FD\xB904\xB918\xB920\xB93C\xB93D\xB940\xB944\xB94C\xB94F\xB951\xB958\xB959\xB95C\xB960"
			L"\xB968\xB969\xB96B\xB96D\xB974\xB975\xB978\xB97C\xB984\xB985\xB987\xB989\xB98A\xB98D\xB98E\xB9AC\xB9AD\xB9B0\xB9B4\xB9BC\xB9BD\xB9BF\xB9C1\xB9C8\xB9C9"
			L"\xB9CC\xB9CE\xB9CF\xB9D0\xB9D1\xB9D2\xB9D8\xB9D9\xB9DB\xB9DD\xB9DE\xB9E1\xB9E3\xB9E4\xB9E5\xB9E8\xB9EC\xB9F4\xB9F5\xB9F7\xB9F8\xB9F9\xB9FA\xBA00\xBA01"
			L"\xBA08\xBA15\xBA38\xBA39\xBA3C\xBA40\xBA42\xBA48\xBA49\xBA4B\xBA4D\xBA4E\xBA53\xBA54\xBA55\xBA58\xBA5C\xBA64\xBA65\xBA67\xBA68\xBA69\xBA70\xBA71\xBA74"
			L"\xBA78\xBA83\xBA84\xBA85\xBA87\xBA8C\xBAA8\xBAA9\xBAAB\xBAAC\xBAB0\xBAB2\xBAB8\xBAB9\xBABB\xBABD\xBAC4\xBAC8\xBAD8\xBAD9\xBAFC\xBB00\xBB04\xBB0D\xBB0F"
			L"\xBB11\xBB18\xBB1C\xBB20\xBB29\xBB2B\xBB34\xBB35\xBB36\xBB38\xBB3B\xBB3C\xBB3D\xBB3E\xBB44\xBB45\xBB47\xBB49\xBB4D\xBB4F\xBB50\xBB54\xBB58\xBB61\xBB63"
			L"\xBB6C\xBB88\xBB8C\xBB90\xBBA4\xBBA8\xBBAC\xBBB4\xBBB7\xBBC0\xBBC4\xBBC8\xBBD0\xBBD3\xBBF8\xBBF9\xBBFC\xBBFF\xBC00\xBC02\xBC08\xBC09\xBC0B\xBC0C\xBC0D"
			L"\xBC0F\xBC11\xBC14\xBC15\xBC16\xBC17\xBC18\xBC1B\xBC1C\xBC1D\xBC1E\xBC1F\xBC24\xBC25\xBC27\xBC29\xBC2D\xBC30\xBC31\xBC34\xBC38\xBC40\xBC41\xBC43\xBC44"
			L"\xBC45\xBC49\xBC4C\xBC4D\xBC50\xBC5D\xBC84\xBC85\xBC88\xBC8B\xBC8C\xBC8E\xBC94\xBC95\xBC97\xBC99\xBC9A\xBCA0\xBCA1\xBCA4\xBCA7\xBCA8\xBCB0\xBCB1\xBCB3"
			L"\xBCB4\xBCB5\xBCBC\xBCBD\xBCC0\xBCC4\xBCCD\xBCCF\xBCD0\xBCD1\xBCD5\xBCD8\xBCDC\xBCF4\xBCF5\xBCF6\xBCF8\xBCFC\xBD04\xBD05\xBD07\xBD09\xBD10\xBD14\xBD24"
			L"\xBD2C\xBD40\xBD48\xBD49\xBD4C\xBD50\xBD58\xBD59\xBD64\xBD68\xBD80\xBD81\xBD84\xBD87\xBD88\xBD89\xBD8A\xBD90\xBD91\xBD93\xBD95\xBD99\xBD9A\xBD9C\xBDA4"
			L"\xBDB0\xBDB8\xBDD4\xBDD5\xBDD8\xBDDC\xBDE9\xBDF0\xBDF4\xBDF8\xBE00\xBE03\xBE05\xBE0C\xBE0D\xBE10\xBE14\xBE1C\xBE1D\xBE1F\xBE44\xBE45\xBE48\xBE4C\xBE4E"
			L"\xBE54\xBE55\xBE57\xBE59\xBE5A\xBE5B\xBE60\xBE61\xBE64\xBE68\xBE6A\xBE70\xBE71\xBE73\xBE74\xBE75\xBE7B\xBE7C\xBE7D\xBE80\xBE84\xBE8C\xBE8D\xBE8F\xBE90"
			L"\xBE91\xBE98\xBE99\xBEA8\xBED0\xBED1\xBED4\xBED7\xBED8\xBEE0\xBEE3\xBEE4\xBEE5\xBEEC\xBF01\xBF08\xBF09\xBF18\xBF19\xBF1B\xBF1C\xBF1D\xBF40\xBF41\xBF44"
			L"\xBF48\xBF50\xBF51\xBF55\xBF94\xBFB0\xBFC5\xBFCC\xBFCD\xBFD0\xBFD4\xBFDC\xBFDF\xBFE1\xC03C\xC051\xC058\xC05C\xC060\xC068\xC069\xC090\xC091\xC094\xC098"
			L"\xC0A0\xC0A1\xC0A3\xC0A5\xC0AC\xC0AD\xC0AF\xC0B0\xC0B3\xC0B4\xC0B5\xC0B6\xC0BC\xC0BD\xC0BF\xC0C0\xC0C1\xC0C5\xC0C8\xC0C9\xC0CC\xC0D0\xC0D8\xC0D9\xC0DB"
			L"\xC0DC\xC0DD\xC0E4\xC0E5\xC0E8\xC0EC\xC0F4\xC0F5\xC0F7\xC0F9\xC100\xC104\xC108\xC110\xC115\xC11C\xC11D\xC11E\xC11F\xC120\xC123\xC124\xC126\xC127\xC12C"
			L"\xC12D\xC12F\xC130\xC131\xC136\xC138\xC139\xC13C\xC140\xC148\xC149\xC14B\xC14C\xC14D\xC154\xC155\xC158\xC15C\xC164\xC165\xC167\xC168\xC169\xC170\xC174"
			L"\xC178\xC185\xC18C\xC18D\xC18E\xC190\xC194\xC196\xC19C\xC19D\xC19F\xC1A1\xC1A5\xC1A8\xC1A9\xC1AC\xC1B0\xC1BD\xC1C4\xC1C8\xC1CC\xC1D4\xC1D7\xC1D8\xC1E0"
			L"\xC1E4\xC1E8\xC1F0\xC1F1\xC1F3\xC1FC\xC1FD\xC200\xC204\xC20C\xC20D\xC20F\xC211\xC218\xC219\xC21C\xC21F\xC220\xC228\xC229\xC22B\xC22D\xC22F\xC231\xC232"
			L"\xC234\xC248\xC250\xC251\xC254\xC258\xC260\xC265\xC26C\xC26D\xC270\xC274\xC27C\xC27D\xC27F\xC281\xC288\xC289\xC290\xC298\xC29B\xC29D\xC2A4\xC2A5\xC2A8"
			L"\xC2AC\xC2AD\xC2B4\xC2B5\xC2B7\xC2B9\xC2DC\xC2DD\xC2E0\xC2E3\xC2E4\xC2EB\xC2EC\xC2ED\xC2EF\xC2F1\xC2F6\xC2F8\xC2F9\xC2FB\xC2FC\xC300\xC308\xC309\xC30C"
			L"\xC30D\xC313\xC314\xC315\xC318\xC31C\xC324\xC325\xC328\xC329\xC345\xC368\xC369\xC36C\xC370\xC372\xC378\xC379\xC37C\xC37D\xC384\xC388\xC38C\xC399\xC3D8"
			L"\xC3D9\xC3DC\xC3DF\xC3E0\xC3E2\xC3E8\xC3E9\xC3ED\xC3F4\xC3F5\xC3F8\xC408\xC410\xC424\xC42C\xC430\xC434\xC43C\xC43D\xC448\xC464\xC465\xC468\xC46C\xC474"
			L"\xC475\xC479\xC480\xC494\xC49C\xC4B8\xC4BC\xC4E9\xC4F0\xC4F1\xC4F4\xC4F8\xC4FA\xC4FF\xC500\xC501\xC50C\xC510\xC514\xC51C\xC528\xC529\xC52C\xC530\xC538"
			L"\xC539\xC53B\xC53D\xC544\xC545\xC548\xC549\xC54A\xC54C\xC54D\xC54E\xC553\xC554\xC555\xC557\xC558\xC559\xC55D\xC55E\xC560\xC561\xC564\xC568\xC570\xC571"
			L"\xC573\xC574\xC575\xC57C\xC57D\xC580\xC584\xC587\xC58C\xC58D\xC58F\xC591\xC595\xC597\xC598\xC59C\xC5A0\xC5A9\xC5B4\xC5B5\xC5B8\xC5B9\xC5BB\xC5BC\xC5BD"
			L"\xC5BE\xC5C4\xC5C5\xC5C6\xC5C7\xC5C8\xC5C9\xC5CA\xC5CC\xC5CE\xC5D0\xC5D1\xC5D4\xC5D8\xC5E0\xC5E1\xC5E3\xC5E5\xC5EC\xC5ED\xC5EE\xC5F0\xC5F4\xC5F6\xC5F7"
			L"\xC5FC\xC5FD\xC5FE\xC5FF\xC600\xC601\xC605\xC606\xC607\xC608\xC60C\xC610\xC618\xC619\xC61B\xC61C\xC624\xC625\xC628\xC62C\xC62D\xC62E\xC630\xC633\xC634"
			L"\xC635\xC637\xC639\xC63B\xC640\xC641\xC644\xC648\xC650\xC651\xC653\xC654\xC655\xC65C\xC65D\xC660\xC66C\xC66F\xC671\xC678\xC679\xC67C\xC680\xC688\xC689"
			L"\xC68B\xC68D\xC694\xC695\xC698\xC69C\xC6A4\xC6A5\xC6A7\xC6A9\xC6B0\xC6B1\xC6B4\xC6B8\xC6B9\xC6BA\xC6C0\xC6C1\xC6C3\xC6C5\xC6CC\xC6CD\xC6D0\xC6D4\xC6DC"
			L"\xC6DD\xC6E0\xC6E1\xC6E8\xC6E9\xC6EC\xC6F0\xC6F8\xC6F9\xC6FD\xC704\xC705\xC708\xC70C\xC714\xC715\xC717\xC719\xC720\xC721\xC724\xC728\xC730\xC731\xC733"
			L"\xC735\xC737\xC73C\xC73D\xC740\xC744\xC74A\xC74C\xC74D\xC74F\xC751\xC752\xC753\xC754\xC755\xC756\xC757\xC758\xC75C\xC760\xC768\xC76B\xC774\xC775\xC778"
			L"\xC77C\xC77D\xC77E\xC783\xC784\xC785\xC787\xC788\xC789\xC78A\xC78E\xC790\xC791\xC794\xC796\xC797\xC798\xC79A\xC7A0\xC7A1\xC7A3\xC7A4\xC7A5\xC7A6\xC7AC"
			L"\xC7AD\xC7B0\xC7B4\xC7BC\xC7BD\xC7BF\xC7C0\xC7C1\xC7C8\xC7C9\xC7CC\xC7CE\xC7D0\xC7D8\xC7DD\xC7E4\xC7E8\xC7EC\xC800\xC801\xC804\xC808\xC80A\xC810\xC811"
			L"\xC813\xC815\xC816\xC81C\xC81D\xC820\xC824\xC82C\xC82D\xC82F\xC831\xC838\xC83C\xC840\xC848\xC849\xC84C\xC84D\xC854\xC870\xC871\xC874\xC878\xC87A\xC880"
			L"\xC881\xC883\xC885\xC886\xC887\xC88B\xC88C\xC88D\xC894\xC89D\xC89F\xC8A1\xC8A8\xC8BC\xC8BD\xC8C4\xC8C8\xC8CC\xC8D4\xC8D5\xC8D7\xC8D9\xC8E0\xC8E1\xC8E4"
			L"\xC8F5\xC8FC\xC8FD\xC900\xC904\xC905\xC906\xC90C\xC90D\xC90F\xC911\xC918\xC92C\xC934\xC950\xC951\xC954\xC958\xC960\xC961\xC963\xC96C\xC970\xC974\xC97C"
			L"\xC988\xC989\xC98C\xC990\xC998\xC999\xC99B\xC99D\xC9C0\xC9C1\xC9C4\xC9C7\xC9C8\xC9CA\xC9D0\xC9D1\xC9D3\xC9D5\xC9D6\xC9D9\xC9DA\xC9DC\xC9DD\xC9E0\xC9E2"
			L"\xC9E4\xC9E7\xC9EC\xC9ED\xC9EF\xC9F0\xC9F1\xC9F8\xC9F9\xC9FC\xCA00\xCA08\xCA09\xCA0B\xCA0C\xCA0D\xCA14\xCA18\xCA29\xCA4C\xCA4D\xCA50\xCA54\xCA5C\xCA5D"
			L"\xCA5F\xCA60\xCA61\xCA68\xCA7D\xCA84\xCA98\xCABC\xCABD\xCAC0\xCAC4\xCACC\xCACD\xCACF\xCAD1\xCAD3\xCAD8\xCAD9\xCAE0\xCAEC\xCAF4\xCB08\xCB10\xCB14\xCB18"
			L"\xCB20\xCB21\xCB41\xCB48\xCB49\xCB4C\xCB50\xCB58\xCB59\xCB5D\xCB64\xCB78\xCB79\xCB9C\xCBB8\xCBD4\xCBE4\xCBE7\xCBE9\xCC0C\xCC0D\xCC10\xCC14\xCC1C\xCC1D"
			L"\xCC21\xCC22\xCC27\xCC28\xCC29\xCC2C\xCC2E\xCC30\xCC38\xCC39\xCC3B\xCC3C\xCC3D\xCC3E\xCC44\xCC45\xCC48\xCC4C\xCC54\xCC55\xCC57\xCC58\xCC59\xCC60\xCC64"
			L"\xCC66\xCC68\xCC70\xCC75\xCC98\xCC99\xCC9C\xCCA0\xCCA8\xCCA9\xCCAB\xCCAC\xCCAD\xCCB4\xCCB5\xCCB8\xCCBC\xCCC4\xCCC5\xCCC7\xCCC9\xCCD0\xCCD4\xCCE4\xCCEC"
			L"\xCCF0\xCD01\xCD08\xCD09\xCD0C\xCD10\xCD18\xCD19\xCD1B\xCD1D\xCD24\xCD28\xCD2C\xCD39\xCD5C\xCD60\xCD64\xCD6C\xCD6D\xCD6F\xCD71\xCD78\xCD88\xCD94\xCD95"
			L"\xCD98\xCD9C\xCDA4\xCDA5\xCDA7\xCDA9\xCDB0\xCDC4\xCDCC\xCDD0\xCDE8\xCDEC\xCDF0\xCDF8\xCDF9\xCDFB\xCDFD\xCE04\xCE08\xCE0C\xCE14\xCE19\xCE20\xCE21\xCE24"
			L"\xCE28\xCE30\xCE31\xCE33\xCE35\xCE58\xCE59\xCE5C\xCE5F\xCE60\xCE61\xCE68\xCE69\xCE6B\xCE6D\xCE74\xCE75\xCE78\xCE7C\xCE84\xCE85\xCE87\xCE89\xCE90\xCE91"
			L"\xCE94\xCE98\xCEA0\xCEA1\xCEA3\xCEA4\xCEA5\xCEAC\xCEAD\xCEC1\xCEE4\xCEE5\xCEE8\xCEEB\xCEEC\xCEF4\xCEF5\xCEF7\xCEF8\xCEF9\xCF00\xCF01\xCF04\xCF08\xCF10"
			L"\xCF11\xCF13\xCF15\xCF1C\xCF20\xCF24\xCF2C\xCF2D\xCF2F\xCF30\xCF31\xCF38\xCF54\xCF55\xCF58\xCF5C\xCF64\xCF65\xCF67\xCF69\xCF70\xCF71\xCF74\xCF78\xCF80"
			L"\xCF85\xCF8C\xCFA1\xCFA8\xCFB0\xCFC4\xCFE0\xCFE1\xCFE4\xCFE8\xCFF0\xCFF1\xCFF3\xCFF5\xCFFC\xD000\xD004\xD011\xD018\xD02D\xD034\xD035\xD038\xD03C\xD044"
			L"\xD045\xD047\xD049\xD050\xD054\xD058\xD060\xD06C\xD06D\xD070\xD074\xD07C\xD07D\xD081\xD0A4\xD0A5\xD0A8\xD0AC\xD0B4\xD0B5\xD0B7\xD0B9\xD0C0\xD0C1\xD0C4"
			L"\xD0C8\xD0C9\xD0D0\xD0D1\xD0D3\xD0D4\xD0D5\xD0DC\xD0DD\xD0E0\xD0E4\xD0EC\xD0ED\xD0EF\xD0F0\xD0F1\xD0F8\xD10D\xD130\xD131\xD134\xD138\xD13A\xD140\xD141"
			L"\xD143\xD144\xD145\xD14C\xD14D\xD150\xD154\xD15C\xD15D\xD15F\xD161\xD168\xD16C\xD17C\xD184\xD188\xD1A0\xD1A1\xD1A4\xD1A8\xD1B0\xD1B1\xD1B3\xD1B5\xD1BA"
			L"\xD1BC\xD1C0\xD1D8\xD1F4\xD1F8\xD207\xD209\xD210\xD22C\xD22D\xD230\xD234\xD23C\xD23D\xD23F\xD241\xD248\xD25C\xD264\xD280\xD281\xD284\xD288\xD290\xD291"
			L"\xD295\xD29C\xD2A0\xD2A4\xD2AC\xD2B1\xD2B8\xD2B9\xD2BC\xD2BF\xD2C0\xD2C2\xD2C8\xD2C9\xD2CB\xD2D4\xD2D8\xD2DC\xD2E4\xD2E5\xD2F0\xD2F1\xD2F4\xD2F8\xD300"
			L"\xD301\xD303\xD305\xD30C\xD30D\xD30E\xD310\xD314\xD316\xD31C\xD31D\xD31F\xD320\xD321\xD325\xD328\xD329\xD32C\xD330\xD338\xD339\xD33B\xD33C\xD33D\xD344"
			L"\xD345\xD37C\xD37D\xD380\xD384\xD38C\xD38D\xD38F\xD390\xD391\xD398\xD399\xD39C\xD3A0\xD3A8\xD3A9\xD3AB\xD3AD\xD3B4\xD3B8\xD3BC\xD3C4\xD3C5\xD3C8\xD3C9"
			L"\xD3D0\xD3D8\xD3E1\xD3E3\xD3EC\xD3ED\xD3F0\xD3F4\xD3FC\xD3FD\xD3FF\xD401\xD408\xD41D\xD440\xD444\xD45C\xD460\xD464\xD46D\xD46F\xD478\xD479\xD47C\xD47F"
			L"\xD480\xD482\xD488\xD489\xD48B\xD48D\xD494\xD4A9\xD4CC\xD4D0\xD4D4\xD4DC\xD4DF\xD4E8\xD4EC\xD4F0\xD4F8\xD4FB\xD4FD\xD504\xD508\xD50C\xD514\xD515\xD517"
			L"\xD53C\xD53D\xD540\xD544\xD54C\xD54D\xD54F\xD551\xD558\xD559\xD55C\xD560\xD565\xD568\xD569\xD56B\xD56D\xD574\xD575\xD578\xD57C\xD584\xD585\xD587\xD588"
			L"\xD589\xD590\xD5A5\xD5C8\xD5C9\xD5CC\xD5D0\xD5D2\xD5D8\xD5D9\xD5DB\xD5DD\xD5E4\xD5E5\xD5E8\xD5EC\xD5F4\xD5F5\xD5F7\xD5F9\xD600\xD601\xD604\xD608\xD610"
			L"\xD611\xD613\xD614\xD615\xD61C\xD620\xD624\xD62D\xD638\xD639\xD63C\xD640\xD645\xD648\xD649\xD64B\xD64D\xD651\xD654\xD655\xD658\xD65C\xD667\xD669\xD670"
			L"\xD671\xD674\xD683\xD685\xD68C\xD68D\xD690\xD694\xD69D\xD69F\xD6A1\xD6A8\xD6AC\xD6B0\xD6B9\xD6BB\xD6C4\xD6C5\xD6C8\xD6CC\xD6D1\xD6D4\xD6D7\xD6D9\xD6E0"
			L"\xD6E4\xD6E8\xD6F0\xD6F5\xD6FC\xD6FD\xD700\xD704\xD711\xD718\xD719\xD71C\xD720\xD728\xD729\xD72B\xD72D\xD734\xD735\xD738\xD73C\xD744\xD747\xD749\xD750"
			L"\xD751\xD754\xD756\xD757\xD758\xD759\xD760\xD761\xD763\xD765\xD769\xD76C\xD770\xD774\xD77C\xD77D\xD781\xD788\xD789\xD78C\xD790\xD798\xD799\xD79B\xD79D";
		return UTF16;
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
			BxCore::Util::MemMove(CP949, CP949Table(), sizeof(wchar) * Length);
			BxCore::Util::MemMove(UTF16, UTF16Table(), sizeof(wchar) * Length);
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
			BxCore::Util::MemMove(UTF16, UTF16Table(), sizeof(wchar) * Length);
			BxCore::Util::MemMove(CP949, CP949Table(), sizeof(wchar) * Length);
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
		return (string) "¡à";
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
