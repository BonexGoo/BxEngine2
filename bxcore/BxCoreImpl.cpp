// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxCoreImpl.hpp>
#include <BxCore.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxAutoBuffer.hpp>
#include <BxExpress.hpp>
#include <BxKeyword.hpp>
#include <BxMemory.hpp>
#include <BxScene.hpp>
#include <BxUtil.hpp>
#include <BxString.hpp>
#include <BxVarMap.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
namespace BxCore
{
	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Main
	{
		int GetCurrentUpdateCount()
		{
			return ForMain_LetUpdateCount();
		}

		int GetCurrentRenderCount()
		{
			return ForMain_LetRenderCount();
		}

		void SetCurrentFrameTime(int ms)
		{
			ForMain_LetFrameTime() = ms;
		}

		int GetCurrentFrameTime(bool frameOnly)
		{
			if(frameOnly) return ForMain_LetFrameTime();
			return ForMain_LetFrameTime() + ForMain_LetFrameDelay();
		}

		rect GetCurrentGUIMargin()
		{
			return ForMain_LetGUIMargin();
		}

		void PushFrameDelay()
		{
			ForMain_LetFrameDelay() = BxUtilGlobal::Min(ForMain_LetFrameDelay() + 5, 500);
		}

		void PullFrameDelay()
		{
			ForMain_LetFrameDelay() = 0;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace System
	{
		bool IsExistConfig(keyword type, string name)
		{
			_CertifyConfigLoad();
			return BxKeyword::IsExist(type, name);
		}

		bool GetConfigCheck(string name, bool defaultCheck)
		{
			_CertifyConfigLoad();
			if(BxKeyword::IsExist(keyword_check, name))
				return BxKeyCheckBySearch(name);
			return defaultCheck;
		}

		int GetConfigNumber(string name, int defaultNumber)
		{
			_CertifyConfigLoad();
			if(BxKeyword::IsExist(keyword_number, name))
				return BxKeyNumberBySearch(name);
			return defaultNumber;
		}

		string _tmp_ GetConfigString(string name, string defaultString)
		{
			_CertifyConfigLoad();
			if(BxKeyword::IsExist(keyword_string, name))
				return BxKeyStringBySearch(name);
			return defaultString;
		}

		string const GetUTF8LiteralByCP949(string const cp949)
		{
			// 캐시
			BxVarMap<string_rw>* UTF8Literals = nullptr;
			BxSINGLETON(UTF8Literals, 1);
			if(UTF8Literals->Access((uint) cp949))
				return (*UTF8Literals)[(uint) cp949];
			// 임시공간
			const int LetterLen = BxUtilGlobal::StrLen(cp949);
			thread_storage _ = sizeof(int);
			int& TempSizeMax = *((int*) BxCore::Thread::BindStorage(&_));
			if(TempSizeMax < LetterLen + 1) TempSizeMax = LetterLen + 1;
			wstring_rw Temp = nullptr;
			BxSINGLETON(Temp, TempSizeMax);
			// 변환
			const int UTF16Len = BxUtil::CP949ToUTF16(cp949, LetterLen, Temp, LetterLen);
			string_rw UTF8 = new char[BxUtil::GetLengthForUTF8(Temp, UTF16Len) + 1];
			BxUtil::UTF16ToUTF8(Temp, UTF16Len, UTF8);
			return ((*UTF8Literals)[(uint) cp949] = UTF8);
		}

		string const GetCP949LiteralByUTF8(string const utf8)
		{
			// 캐시
			BxVarMap<string_rw>* CP949Literals = nullptr;
			BxSINGLETON(CP949Literals, 1);
			if(CP949Literals->Access((uint) utf8))
				return (*CP949Literals)[(uint) utf8];
			// 임시공간
			const int LetterLen = BxUtil::GetLengthForUTF16(utf8, -1);
			thread_storage _ = sizeof(int);
			int& TempSizeMax = *((int*) BxCore::Thread::BindStorage(&_));
			if(TempSizeMax < LetterLen + 1) TempSizeMax = LetterLen + 1;
			wstring_rw Temp = nullptr;
			BxSINGLETON(Temp, TempSizeMax);
			// 변환
			BxUtil::UTF8ToUTF16(utf8, -1, Temp);
			string_rw CP949 = new char[LetterLen * 2 + 1];
			BxUtil::UTF16ToCP949(Temp, LetterLen, CP949, LetterLen * 2);
			return ((*CP949Literals)[(uint) utf8] = CP949);
		}

		void AddCallback(callback_frame method, int value, void* data, int datasize)
		{
			CallbackList* List = (CallbackList*) CallbackList::GetFirst();
			while(List->Next) List = List->Next;
			List->Next = BxNew_Param(CallbackList, method, value, data, datasize);
		}

		bool BeginChildProcess(callback_process process)
		{
			if(_ChildProcess()) return false;
			_ChildProcess() = process;
			return true;
		}

		callback_edk SetCallbackEDK(callback_edk methodCB)
		{
			_EDKClient() = methodCB;
			return _EDKReceiver;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Surface
	{
		int GetWidth(bool* isfixedvalue, bool usemargin)
		{
			global_data int SurfaceWidth = -1;
			if(SurfaceWidth == -1)
			{
				if(BxUtilGlobal::StrCmp(BxCore::System::GetOSName(), "WINDOWS") != same
					&& BxUtilGlobal::StrCmp(BxCore::System::GetArchName(), "X86") != same
					&& BxCore::System::GetConfigCheck("Bx.VirtualScreen.Enable", false))
					SurfaceWidth = BxCore::System::GetConfigNumber("Bx.VirtualScreen.Width", 480);
				else SurfaceWidth = 0;
			}
			if(SurfaceWidth != 0)
			{
				if(isfixedvalue) *isfixedvalue = true;
				return SurfaceWidth;
			}
			if(isfixedvalue) *isfixedvalue = false;
			return GetWidthHW(usemargin);
		}

		int GetHeight(bool* isfixedvalue, bool usemargin)
		{
			global_data int SurfaceHeight = -1;
			if(SurfaceHeight == -1)
			{
				if(BxUtilGlobal::StrCmp(BxCore::System::GetOSName(), "WINDOWS") != same
					&& BxUtilGlobal::StrCmp(BxCore::System::GetArchName(), "X86") != same
					&& BxCore::System::GetConfigCheck("Bx.VirtualScreen.Enable", false))
					SurfaceHeight = BxCore::System::GetConfigNumber("Bx.VirtualScreen.Height", 320);
				else SurfaceHeight = 0;
			}
			if(SurfaceHeight != 0)
			{
				if(isfixedvalue) *isfixedvalue = true;
				return SurfaceHeight;
			}
			if(isfixedvalue) *isfixedvalue = false;
			return GetHeightHW(usemargin);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Util
	{
		string _tmp_ Print(string map, const BxArgument& args, int* length)
		{
			BxASSERT("BxCore::Util::Print<스트링맵의 형식을 지키지 않았습니다>", map[0] == '<' && map[1] == '>' && map[2] == ':');
			// 결과버퍼사이즈
			thread_storage _ = sizeof(int);
			int& ResultSize = *((int*) BxCore::Thread::BindStorage(&_));
			if(!ResultSize) ResultSize = 1024;
			// 결과버퍼
			string_rw Result = nullptr;
			BxSINGLETON(Result, ResultSize);
			// 루프
			string_rw Focus = Result;
			int LastArgID = 0;
			map += 3 - 1; // +3은 맵형식, -1은 아래 루프를 시작시키기 위함
			while(*(++map) != '\0')
			{
				if(*map == '<' && *(++map) != '<')
				{
					int Count = 0;
					while(map[Count] != '\0' && map[Count] != '>') ++Count;
					BxASSERT("BxCore::Util::Print<미완성 태그가 존재합니다(\"<\"표현시 \"<<\"를 사용)>", map[Count] == '>');
					if(map[Count] == '\0') break;
					--Count;
					switch(*(map++))
					{
					case 'A':
						BxASSERT("BxCore::Util::Print<참조할 인수가 없습니다>", 0 < args.Length());
						if(0 < args.Length())
						{
							// 인수매칭
							string SubMap = map;
							while(*SubMap != ':' && *SubMap != '>') ++SubMap;
							const int ArgID = ((SubMap - map)? BxUtilGlobal::AtoI(map, SubMap - map) : LastArgID) % args.Length();
							// 공간제약
							char SpaceCode[2] = {' ', ' '};
							int SpaceSize[2] = {-1, -1};
							if(*SubMap == ':')
							{
								bool IsRight = false;
								while(*(++SubMap) != '>')
								{
									if('0' <= *SubMap && *SubMap <= '9')
									{
										if(SpaceSize[IsRight] == -1)
										{
											SpaceSize[IsRight] = *SubMap - '0';
											if(*SubMap == '0') SpaceCode[IsRight] = '0';
										}
										else SpaceSize[IsRight] = SpaceSize[IsRight] * 10 + (*SubMap - '0');
									}
									else if(*SubMap == '.') IsRight = true;
									else SpaceCode[IsRight] = *SubMap;
								}
							}
							// 인수수집
							void* ArgData = nullptr;
							string ArgString[2] = {nullptr, nullptr};
							int ArgStringLength[2] = {0, 0};
							if(ArgData = args.Access<char>(ArgID))
							{
								ArgString[0] = (char*) ArgData;
								ArgStringLength[0] = 1;
							}
							else if(ArgData = args.Access<int>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoA(*((int*) ArgData));
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<unsigned int>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoA(*((unsigned int*) ArgData));
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<float>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoA((int) *((float*) ArgData));
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
								if(SpaceSize[0] == -1 && SpaceSize[1] != -1)
									SpaceSize[0] = ArgStringLength[0];
								// 소수처리
								global_data char CipherString[8] = {'.',};
								string_rw CipherFocus = CipherString + 1;
								int CipherCode = 1000000;
								int Value = BxUtilGlobal::Abs((int) (*((float*) ArgData) * CipherCode)) % CipherCode;
								do {*(CipherFocus++) = '0' + ((Value / (CipherCode /= 10)) % 10);}
								while(Value %= CipherCode);
								ArgString[1] = CipherString;
								ArgStringLength[1] = CipherFocus - CipherString;
							}
							else if(ArgData = args.Access<double>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoA((int) *((double*) ArgData));
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
								if(SpaceSize[0] == -1 && SpaceSize[1] != -1)
									SpaceSize[0] = ArgStringLength[0];
								// 소수처리
								global_data char CipherString[8] = {'.',};
								string_rw CipherFocus = CipherString + 1;
								int CipherCode = 1000000;
								int Value = BxUtilGlobal::Abs((int) (*((double*) ArgData) * CipherCode)) % CipherCode;
								do {*(CipherFocus++) = '0' + ((Value / (CipherCode /= 10)) % 10);}
								while(Value %= CipherCode);
								ArgString[1] = CipherString;
								ArgStringLength[1] = CipherFocus - CipherString;
							}
							else if(ArgData = args.Access<string>(ArgID))
							{
								ArgString[0] = (string) ArgData; // 특수처리
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<char*>(ArgID))
							{
								ArgString[0] = *((char**) ArgData);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<BxString>(ArgID))
							{
								ArgString[0] = (string) *((BxString*) ArgData);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<ARG_STR_UPPER>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::AtoA(true, ((ARG_STR_UPPER*) ArgData)->Value);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<ARG_STR_LOWER>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::AtoA(false, ((ARG_STR_LOWER*) ArgData)->Value);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<ARG_HEX_UPPER>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoH(true, ((ARG_HEX_UPPER*) ArgData)->Value);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<ARG_HEX_LOWER>(ArgID))
							{
								ArgString[0] = BxUtilGlobal::ItoH(false, ((ARG_HEX_LOWER*) ArgData)->Value);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else BxASSERT("BxCore::Util::Print<지원하지 않는 타입의 인수입니다>", false);
							// 인수출력
							if(SpaceSize[0] == -1 && SpaceSize[1] == -1)
							{
								if(ArgString[0]) Focus += BxUtilGlobal::StrCpy(Focus, ArgString[0], ArgStringLength[0]);
								if(ArgString[1]) Focus += BxUtilGlobal::StrCpy(Focus, ArgString[1], ArgStringLength[1]);
							}
							else
							{
								if(0 < SpaceSize[0])
								{
									int SpaceCount = SpaceSize[0] - ArgStringLength[0];
									if(0 < SpaceCount)
									{
										while(SpaceCount--) *(Focus++) = SpaceCode[0];
										Focus += BxUtilGlobal::StrCpy(Focus, ArgString[0], ArgStringLength[0]);
									}
									else Focus += BxUtilGlobal::StrCpy(Focus, ArgString[0] - SpaceCount, ArgStringLength[0] + SpaceCount);
								}
								if(0 < SpaceSize[1])
								{
									bool IsRight = (ArgString[1] != nullptr);
									int SpaceCount = SpaceSize[1] - ArgStringLength[IsRight];
									if(0 < SpaceCount)
									{
										Focus += BxUtilGlobal::StrCpy(Focus, ArgString[IsRight], ArgStringLength[IsRight]);
										while(SpaceCount--) *(Focus++) = SpaceCode[1];
									}
									else Focus += BxUtilGlobal::StrCpy(Focus, ArgString[IsRight], ArgStringLength[IsRight] + SpaceCount);
								}
							}
							++LastArgID;
						}
						break;
					case 'B': if(Count == 0) *(Focus++) = '\b'; break;
					case 'D': if(Count == 1 && *map == 'Q') *(Focus++) = '\"'; break;
					case 'F': if(Count == 1 && *map == 'S') *(Focus++) = '/'; break;
					case 'N': if(Count == 0) *(Focus++) = '\n'; break;
					case 'R': if(Count == 0) *(Focus++) = '\r'; else if(Count == 1 && *map == 'S') *(Focus++) = '\\'; break;
					case 'S': if(Count == 1 && *map == 'Q') *(Focus++) = '\''; break;
					case 'T': if(Count == 0) *(Focus++) = '\t'; break;
					case 'X':
						for(int i = 0; i + 1 < Count; i += 2)
						{
							if('0' <= map[i] && map[i] <= '9')
								*Focus = ((map[i] - '0') & 0x0F) << 4;
							else if('A' <= map[i] && map[i] <= 'F')
								*Focus = ((map[i] - 'A' + 10) & 0x0F) << 4;
							else *Focus = 0;
							if('0' <= map[i + 1] && map[i + 1] <= '9')
								*(Focus++) |= ((map[i + 1] - '0') & 0x0F);
							else if('A' <= map[i + 1] && map[i + 1] <= 'F')
								*(Focus++) |= ((map[i + 1] - 'A' + 10) & 0x0F);
							else ++Focus;
						}
						break;
					case 'Z': if(Count == 0) *(Focus++) = '\0'; break;
					default: BxASSERT("BxCore::Util::Print<알 수 없는 태그입니다(\"<\"표현시 \"<<\"를 사용)>", false); break;
					}
					map += Count;
				}
				else if(*map == '>' && *(++map) != '>')
					BxASSERT("BxCore::Util::Print<불완전한 태그가 존재합니다(\">\"표현시 \">>\"를 사용)>", false);
				else *(Focus++) = *map;
				if(Result + ResultSize < Focus + 512)
				{
					const int Count = Focus - Result;
					BxSingleton::Rebind(Result, ResultSize * 2);
					Focus = Result + Count;
				}
			}
			*Focus = '\0';
			if(length) *length = Focus - Result;
			return Result;
		}

		const int* Cos1024Table()
		{
			global_data const int Cos1024[] = {
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
				0x1787, 0x15F7, 0x1466, 0x12D5, 0x1144, 0x0FB3, 0x0E21, 0x0C90, 0x0AFE, 0x096C, 0x07DA, 0x0648, 0x04B6, 0x0324, 0x0192, 0x0000};
			return Cos1024;
		}

		const int* Tan1024Table()
		{
			global_data const int Tan1024[] = {
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
				0x000A7B97, 0x000B35BF, 0x000C0B69, 0x000D032C, 0x000E25EF, 0x000F7FFA, 0x001122C3, 0x001327F6, 0x0015B6F0, 0x00190F4A, 0x001D9ED9, 0x00243517, 0x002E8E99, 0x00412F4B, 0x006CA58E, 0x0145F2C3};
			return Tan1024;
		}

		wstring UTF16Table()
		{
			global_data wstring UTF16 = (wstring)
				_W2("\t\n\r !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKL")
				_W2("MNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
				_W2("\x3131\x3132\x3133\x3134\x3135\x3136\x3137\x3138\x3139\x313A\x313B\x313C\x313D\x313E\x313F\x3140\x3141\x3142\x3143\x3144\x3145\x3146\x3147\x3148\x3149\x314A\x314B\x314C\x314D\x314E")
				_W2("\x314F\x3150\x3151\x3152\x3153\x3154\x3155\x3156\x3157\x3158\x3159\x315A\x315B\x315C\x315D\x315E\x315F\x3160\x3161\x3162\x3163")
				_W2("\xAC00\xAC01\xAC04\xAC07\xAC08\xAC09\xAC0A\xAC10\xAC11\xAC12\xAC13\xAC14\xAC15\xAC16\xAC17\xAC19\xAC1A\xAC1B\xAC1C\xAC1D\xAC20\xAC24\xAC2C\xAC2D\xAC2F")
				_W2("\xAC30\xAC31\xAC38\xAC39\xAC3C\xAC40\xAC4B\xAC4D\xAC54\xAC58\xAC5C\xAC70\xAC71\xAC74\xAC77\xAC78\xAC7A\xAC80\xAC81\xAC83\xAC84\xAC85\xAC86\xAC89\xAC8A")
				_W2("\xAC8B\xAC8C\xAC90\xAC94\xAC9C\xAC9D\xAC9F\xACA0\xACA1\xACA8\xACA9\xACAA\xACAC\xACAF\xACB0\xACB8\xACB9\xACBB\xACBC\xACBD\xACC1\xACC4\xACC8\xACCC\xACD5")
				_W2("\xACD7\xACE0\xACE1\xACE4\xACE7\xACE8\xACEA\xACEC\xACEF\xACF0\xACF1\xACF3\xACF5\xACF6\xACFC\xACFD\xAD00\xAD04\xAD06\xAD0C\xAD0D\xAD0F\xAD11\xAD18\xAD1C")
				_W2("\xAD20\xAD28\xAD2C\xAD2D\xAD34\xAD35\xAD38\xAD3C\xAD44\xAD45\xAD47\xAD49\xAD50\xAD54\xAD58\xAD61\xAD63\xAD6C\xAD6D\xAD70\xAD73\xAD74\xAD75\xAD76\xAD7B")
				_W2("\xAD7C\xAD7D\xAD7F\xAD81\xAD82\xAD88\xAD89\xAD8C\xAD90\xAD9C\xAD9D\xADA4\xADB7\xADC0\xADC1\xADC4\xADC8\xADD0\xADD1\xADD3\xADDC\xADE0\xADE4\xADF8\xADF9")
				_W2("\xADFC\xADFF\xAE00\xAE01\xAE08\xAE09\xAE0B\xAE0D\xAE14\xAE30\xAE31\xAE34\xAE37\xAE38\xAE3A\xAE40\xAE41\xAE43\xAE45\xAE46\xAE4A\xAE4C\xAE4D\xAE4E\xAE50")
				_W2("\xAE54\xAE56\xAE5C\xAE5D\xAE5F\xAE60\xAE61\xAE65\xAE68\xAE69\xAE6C\xAE70\xAE78\xAE79\xAE7B\xAE7C\xAE7D\xAE84\xAE85\xAE8C\xAEBC\xAEBD\xAEBE\xAEC0\xAEC4")
				_W2("\xAECC\xAECD\xAECF\xAED0\xAED1\xAED8\xAED9\xAEDC\xAEE8\xAEEB\xAEED\xAEF4\xAEF8\xAEFC\xAF07\xAF08\xAF0D\xAF10\xAF2C\xAF2D\xAF30\xAF32\xAF34\xAF3C\xAF3D")
				_W2("\xAF3F\xAF41\xAF42\xAF43\xAF48\xAF49\xAF50\xAF5C\xAF5D\xAF64\xAF65\xAF79\xAF80\xAF84\xAF88\xAF90\xAF91\xAF95\xAF9C\xAFB8\xAFB9\xAFBC\xAFC0\xAFC7\xAFC8")
				_W2("\xAFC9\xAFCB\xAFCD\xAFCE\xAFD4\xAFDC\xAFE8\xAFE9\xAFF0\xAFF1\xAFF4\xAFF8\xB000\xB001\xB004\xB00C\xB010\xB014\xB01C\xB01D\xB028\xB044\xB045\xB048\xB04A")
				_W2("\xB04C\xB04E\xB053\xB054\xB055\xB057\xB059\xB05D\xB07C\xB07D\xB080\xB084\xB08C\xB08D\xB08F\xB091\xB098\xB099\xB09A\xB09C\xB09F\xB0A0\xB0A1\xB0A2\xB0A8")
				_W2("\xB0A9\xB0AB\xB0AC\xB0AD\xB0AE\xB0AF\xB0B1\xB0B3\xB0B4\xB0B5\xB0B8\xB0BC\xB0C4\xB0C5\xB0C7\xB0C8\xB0C9\xB0D0\xB0D1\xB0D4\xB0D8\xB0E0\xB0E5\xB108\xB109")
				_W2("\xB10B\xB10C\xB110\xB112\xB113\xB118\xB119\xB11B\xB11C\xB11D\xB123\xB124\xB125\xB128\xB12C\xB134\xB135\xB137\xB138\xB139\xB140\xB141\xB144\xB148\xB150")
				_W2("\xB151\xB154\xB155\xB158\xB15C\xB160\xB178\xB179\xB17C\xB180\xB182\xB188\xB189\xB18B\xB18D\xB192\xB193\xB194\xB198\xB19C\xB1A8\xB1CC\xB1D0\xB1D4\xB1DC")
				_W2("\xB1DD\xB1DF\xB1E8\xB1E9\xB1EC\xB1F0\xB1F9\xB1FB\xB1FD\xB204\xB205\xB208\xB20B\xB20C\xB214\xB215\xB217\xB219\xB220\xB234\xB23C\xB258\xB25C\xB260\xB268")
				_W2("\xB269\xB274\xB275\xB27C\xB284\xB285\xB289\xB290\xB291\xB294\xB298\xB299\xB29A\xB2A0\xB2A1\xB2A3\xB2A5\xB2A6\xB2AA\xB2AC\xB2B0\xB2B4\xB2C8\xB2C9\xB2CC")
				_W2("\xB2D0\xB2D6\xB2D8\xB2D9\xB2DB\xB2DD\xB2E2\xB2E4\xB2E5\xB2E6\xB2E8\xB2EB\xB2EC\xB2ED\xB2EE\xB2EF\xB2F3\xB2F4\xB2F5\xB2F7\xB2F8\xB2F9\xB2FA\xB2FB\xB2FF")
				_W2("\xB300\xB301\xB304\xB308\xB310\xB311\xB313\xB314\xB315\xB31C\xB354\xB355\xB356\xB358\xB35B\xB35C\xB35E\xB35F\xB364\xB365\xB367\xB369\xB36B\xB36E\xB370")
				_W2("\xB371\xB374\xB378\xB380\xB381\xB383\xB384\xB385\xB38C\xB390\xB394\xB3A0\xB3A1\xB3A8\xB3AC\xB3C4\xB3C5\xB3C8\xB3CB\xB3CC\xB3CE\xB3D0\xB3D4\xB3D5\xB3D7")
				_W2("\xB3D9\xB3DB\xB3DD\xB3E0\xB3E4\xB3E8\xB3FC\xB410\xB418\xB41C\xB420\xB428\xB429\xB42B\xB434\xB450\xB451\xB454\xB458\xB460\xB461\xB463\xB465\xB46C\xB480")
				_W2("\xB488\xB49D\xB4A4\xB4A8\xB4AC\xB4B5\xB4B7\xB4B9\xB4C0\xB4C4\xB4C8\xB4D0\xB4D5\xB4DC\xB4DD\xB4E0\xB4E3\xB4E4\xB4E6\xB4EC\xB4ED\xB4EF\xB4F1\xB4F8\xB514")
				_W2("\xB515\xB518\xB51B\xB51C\xB524\xB525\xB527\xB528\xB529\xB52A\xB530\xB531\xB534\xB538\xB540\xB541\xB543\xB544\xB545\xB54B\xB54C\xB54D\xB550\xB554\xB55C")
				_W2("\xB55D\xB55F\xB560\xB561\xB5A0\xB5A1\xB5A4\xB5A8\xB5AA\xB5AB\xB5B0\xB5B1\xB5B3\xB5B4\xB5B5\xB5BB\xB5BC\xB5BD\xB5C0\xB5C4\xB5CC\xB5CD\xB5CF\xB5D0\xB5D1")
				_W2("\xB5D8\xB5EC\xB610\xB611\xB614\xB618\xB625\xB62C\xB634\xB648\xB664\xB668\xB69C\xB69D\xB6A0\xB6A4\xB6AB\xB6AC\xB6B1\xB6D4\xB6F0\xB6F4\xB6F8\xB700\xB701")
				_W2("\xB705\xB728\xB729\xB72C\xB72F\xB730\xB738\xB739\xB73B\xB744\xB748\xB74C\xB754\xB755\xB760\xB764\xB768\xB770\xB771\xB773\xB775\xB77C\xB77D\xB780\xB784")
				_W2("\xB78C\xB78D\xB78F\xB790\xB791\xB792\xB796\xB797\xB798\xB799\xB79C\xB7A0\xB7A8\xB7A9\xB7AB\xB7AC\xB7AD\xB7B4\xB7B5\xB7B8\xB7C7\xB7C9\xB7EC\xB7ED\xB7F0")
				_W2("\xB7F4\xB7FC\xB7FD\xB7FF\xB800\xB801\xB807\xB808\xB809\xB80C\xB810\xB818\xB819\xB81B\xB81D\xB824\xB825\xB828\xB82C\xB834\xB835\xB837\xB838\xB839\xB840")
				_W2("\xB844\xB851\xB853\xB85C\xB85D\xB860\xB864\xB86C\xB86D\xB86F\xB871\xB878\xB87C\xB88D\xB8A8\xB8B0\xB8B4\xB8B8\xB8C0\xB8C1\xB8C3\xB8C5\xB8CC\xB8D0\xB8D4")
				_W2("\xB8DD\xB8DF\xB8E1\xB8E8\xB8E9\xB8EC\xB8F0\xB8F8\xB8F9\xB8FB\xB8FD\xB904\xB918\xB920\xB93C\xB93D\xB940\xB944\xB94C\xB94F\xB951\xB958\xB959\xB95C\xB960")
				_W2("\xB968\xB969\xB96B\xB96D\xB974\xB975\xB978\xB97C\xB984\xB985\xB987\xB989\xB98A\xB98D\xB98E\xB9AC\xB9AD\xB9B0\xB9B4\xB9BC\xB9BD\xB9BF\xB9C1\xB9C8\xB9C9")
				_W2("\xB9CC\xB9CE\xB9CF\xB9D0\xB9D1\xB9D2\xB9D8\xB9D9\xB9DB\xB9DD\xB9DE\xB9E1\xB9E3\xB9E4\xB9E5\xB9E8\xB9EC\xB9F4\xB9F5\xB9F7\xB9F8\xB9F9\xB9FA\xBA00\xBA01")
				_W2("\xBA08\xBA15\xBA38\xBA39\xBA3C\xBA40\xBA42\xBA48\xBA49\xBA4B\xBA4D\xBA4E\xBA53\xBA54\xBA55\xBA58\xBA5C\xBA64\xBA65\xBA67\xBA68\xBA69\xBA70\xBA71\xBA74")
				_W2("\xBA78\xBA83\xBA84\xBA85\xBA87\xBA8C\xBAA8\xBAA9\xBAAB\xBAAC\xBAB0\xBAB2\xBAB8\xBAB9\xBABB\xBABD\xBAC4\xBAC8\xBAD8\xBAD9\xBAFC\xBB00\xBB04\xBB0D\xBB0F")
				_W2("\xBB11\xBB18\xBB1C\xBB20\xBB29\xBB2B\xBB34\xBB35\xBB36\xBB38\xBB3B\xBB3C\xBB3D\xBB3E\xBB44\xBB45\xBB47\xBB49\xBB4D\xBB4F\xBB50\xBB54\xBB58\xBB61\xBB63")
				_W2("\xBB6C\xBB88\xBB8C\xBB90\xBBA4\xBBA8\xBBAC\xBBB4\xBBB7\xBBC0\xBBC4\xBBC8\xBBD0\xBBD3\xBBF8\xBBF9\xBBFC\xBBFF\xBC00\xBC02\xBC08\xBC09\xBC0B\xBC0C\xBC0D")
				_W2("\xBC0F\xBC11\xBC14\xBC15\xBC16\xBC17\xBC18\xBC1B\xBC1C\xBC1D\xBC1E\xBC1F\xBC24\xBC25\xBC27\xBC29\xBC2D\xBC30\xBC31\xBC34\xBC38\xBC40\xBC41\xBC43\xBC44")
				_W2("\xBC45\xBC49\xBC4C\xBC4D\xBC50\xBC5D\xBC84\xBC85\xBC88\xBC8B\xBC8C\xBC8E\xBC94\xBC95\xBC97\xBC99\xBC9A\xBCA0\xBCA1\xBCA4\xBCA7\xBCA8\xBCB0\xBCB1\xBCB3")
				_W2("\xBCB4\xBCB5\xBCBC\xBCBD\xBCC0\xBCC4\xBCCD\xBCCF\xBCD0\xBCD1\xBCD5\xBCD8\xBCDC\xBCF4\xBCF5\xBCF6\xBCF8\xBCFC\xBD04\xBD05\xBD07\xBD09\xBD10\xBD14\xBD24")
				_W2("\xBD2C\xBD40\xBD48\xBD49\xBD4C\xBD50\xBD58\xBD59\xBD64\xBD68\xBD80\xBD81\xBD84\xBD87\xBD88\xBD89\xBD8A\xBD90\xBD91\xBD93\xBD95\xBD99\xBD9A\xBD9C\xBDA4")
				_W2("\xBDB0\xBDB8\xBDD4\xBDD5\xBDD8\xBDDC\xBDE9\xBDF0\xBDF4\xBDF8\xBE00\xBE03\xBE05\xBE0C\xBE0D\xBE10\xBE14\xBE1C\xBE1D\xBE1F\xBE44\xBE45\xBE48\xBE4C\xBE4E")
				_W2("\xBE54\xBE55\xBE57\xBE59\xBE5A\xBE5B\xBE60\xBE61\xBE64\xBE68\xBE6A\xBE70\xBE71\xBE73\xBE74\xBE75\xBE7B\xBE7C\xBE7D\xBE80\xBE84\xBE8C\xBE8D\xBE8F\xBE90")
				_W2("\xBE91\xBE98\xBE99\xBEA8\xBED0\xBED1\xBED4\xBED7\xBED8\xBEE0\xBEE3\xBEE4\xBEE5\xBEEC\xBF01\xBF08\xBF09\xBF18\xBF19\xBF1B\xBF1C\xBF1D\xBF40\xBF41\xBF44")
				_W2("\xBF48\xBF50\xBF51\xBF55\xBF94\xBFB0\xBFC5\xBFCC\xBFCD\xBFD0\xBFD4\xBFDC\xBFDF\xBFE1\xC03C\xC051\xC058\xC05C\xC060\xC068\xC069\xC090\xC091\xC094\xC098")
				_W2("\xC0A0\xC0A1\xC0A3\xC0A5\xC0AC\xC0AD\xC0AF\xC0B0\xC0B3\xC0B4\xC0B5\xC0B6\xC0BC\xC0BD\xC0BF\xC0C0\xC0C1\xC0C5\xC0C8\xC0C9\xC0CC\xC0D0\xC0D8\xC0D9\xC0DB")
				_W2("\xC0DC\xC0DD\xC0E4\xC0E5\xC0E8\xC0EC\xC0F4\xC0F5\xC0F7\xC0F9\xC100\xC104\xC108\xC110\xC115\xC11C\xC11D\xC11E\xC11F\xC120\xC123\xC124\xC126\xC127\xC12C")
				_W2("\xC12D\xC12F\xC130\xC131\xC136\xC138\xC139\xC13C\xC140\xC148\xC149\xC14B\xC14C\xC14D\xC154\xC155\xC158\xC15C\xC164\xC165\xC167\xC168\xC169\xC170\xC174")
				_W2("\xC178\xC185\xC18C\xC18D\xC18E\xC190\xC194\xC196\xC19C\xC19D\xC19F\xC1A1\xC1A5\xC1A8\xC1A9\xC1AC\xC1B0\xC1BD\xC1C4\xC1C8\xC1CC\xC1D4\xC1D7\xC1D8\xC1E0")
				_W2("\xC1E4\xC1E8\xC1F0\xC1F1\xC1F3\xC1FC\xC1FD\xC200\xC204\xC20C\xC20D\xC20F\xC211\xC218\xC219\xC21C\xC21F\xC220\xC228\xC229\xC22B\xC22D\xC22F\xC231\xC232")
				_W2("\xC234\xC248\xC250\xC251\xC254\xC258\xC260\xC265\xC26C\xC26D\xC270\xC274\xC27C\xC27D\xC27F\xC281\xC288\xC289\xC290\xC298\xC29B\xC29D\xC2A4\xC2A5\xC2A8")
				_W2("\xC2AC\xC2AD\xC2B4\xC2B5\xC2B7\xC2B9\xC2DC\xC2DD\xC2E0\xC2E3\xC2E4\xC2EB\xC2EC\xC2ED\xC2EF\xC2F1\xC2F6\xC2F8\xC2F9\xC2FB\xC2FC\xC300\xC308\xC309\xC30C")
				_W2("\xC30D\xC313\xC314\xC315\xC318\xC31C\xC324\xC325\xC328\xC329\xC345\xC368\xC369\xC36C\xC370\xC372\xC378\xC379\xC37C\xC37D\xC384\xC388\xC38C\xC399\xC3D8")
				_W2("\xC3D9\xC3DC\xC3DF\xC3E0\xC3E2\xC3E8\xC3E9\xC3ED\xC3F4\xC3F5\xC3F8\xC408\xC410\xC424\xC42C\xC430\xC434\xC43C\xC43D\xC448\xC464\xC465\xC468\xC46C\xC474")
				_W2("\xC475\xC479\xC480\xC494\xC49C\xC4B8\xC4BC\xC4E9\xC4F0\xC4F1\xC4F4\xC4F8\xC4FA\xC4FF\xC500\xC501\xC50C\xC510\xC514\xC51C\xC528\xC529\xC52C\xC530\xC538")
				_W2("\xC539\xC53B\xC53D\xC544\xC545\xC548\xC549\xC54A\xC54C\xC54D\xC54E\xC553\xC554\xC555\xC557\xC558\xC559\xC55D\xC55E\xC560\xC561\xC564\xC568\xC570\xC571")
				_W2("\xC573\xC574\xC575\xC57C\xC57D\xC580\xC584\xC587\xC58C\xC58D\xC58F\xC591\xC595\xC597\xC598\xC59C\xC5A0\xC5A9\xC5B4\xC5B5\xC5B8\xC5B9\xC5BB\xC5BC\xC5BD")
				_W2("\xC5BE\xC5C4\xC5C5\xC5C6\xC5C7\xC5C8\xC5C9\xC5CA\xC5CC\xC5CE\xC5D0\xC5D1\xC5D4\xC5D8\xC5E0\xC5E1\xC5E3\xC5E5\xC5EC\xC5ED\xC5EE\xC5F0\xC5F4\xC5F6\xC5F7")
				_W2("\xC5FC\xC5FD\xC5FE\xC5FF\xC600\xC601\xC605\xC606\xC607\xC608\xC60C\xC610\xC618\xC619\xC61B\xC61C\xC624\xC625\xC628\xC62C\xC62D\xC62E\xC630\xC633\xC634")
				_W2("\xC635\xC637\xC639\xC63B\xC640\xC641\xC644\xC648\xC650\xC651\xC653\xC654\xC655\xC65C\xC65D\xC660\xC66C\xC66F\xC671\xC678\xC679\xC67C\xC680\xC688\xC689")
				_W2("\xC68B\xC68D\xC694\xC695\xC698\xC69C\xC6A4\xC6A5\xC6A7\xC6A9\xC6B0\xC6B1\xC6B4\xC6B8\xC6B9\xC6BA\xC6C0\xC6C1\xC6C3\xC6C5\xC6CC\xC6CD\xC6D0\xC6D4\xC6DC")
				_W2("\xC6DD\xC6E0\xC6E1\xC6E8\xC6E9\xC6EC\xC6F0\xC6F8\xC6F9\xC6FD\xC704\xC705\xC708\xC70C\xC714\xC715\xC717\xC719\xC720\xC721\xC724\xC728\xC730\xC731\xC733")
				_W2("\xC735\xC737\xC73C\xC73D\xC740\xC744\xC74A\xC74C\xC74D\xC74F\xC751\xC752\xC753\xC754\xC755\xC756\xC757\xC758\xC75C\xC760\xC768\xC76B\xC774\xC775\xC778")
				_W2("\xC77C\xC77D\xC77E\xC783\xC784\xC785\xC787\xC788\xC789\xC78A\xC78E\xC790\xC791\xC794\xC796\xC797\xC798\xC79A\xC7A0\xC7A1\xC7A3\xC7A4\xC7A5\xC7A6\xC7AC")
				_W2("\xC7AD\xC7B0\xC7B4\xC7BC\xC7BD\xC7BF\xC7C0\xC7C1\xC7C8\xC7C9\xC7CC\xC7CE\xC7D0\xC7D8\xC7DD\xC7E4\xC7E8\xC7EC\xC800\xC801\xC804\xC808\xC80A\xC810\xC811")
				_W2("\xC813\xC815\xC816\xC81C\xC81D\xC820\xC824\xC82C\xC82D\xC82F\xC831\xC838\xC83C\xC840\xC848\xC849\xC84C\xC84D\xC854\xC870\xC871\xC874\xC878\xC87A\xC880")
				_W2("\xC881\xC883\xC885\xC886\xC887\xC88B\xC88C\xC88D\xC894\xC89D\xC89F\xC8A1\xC8A8\xC8BC\xC8BD\xC8C4\xC8C8\xC8CC\xC8D4\xC8D5\xC8D7\xC8D9\xC8E0\xC8E1\xC8E4")
				_W2("\xC8F5\xC8FC\xC8FD\xC900\xC904\xC905\xC906\xC90C\xC90D\xC90F\xC911\xC918\xC92C\xC934\xC950\xC951\xC954\xC958\xC960\xC961\xC963\xC96C\xC970\xC974\xC97C")
				_W2("\xC988\xC989\xC98C\xC990\xC998\xC999\xC99B\xC99D\xC9C0\xC9C1\xC9C4\xC9C7\xC9C8\xC9CA\xC9D0\xC9D1\xC9D3\xC9D5\xC9D6\xC9D9\xC9DA\xC9DC\xC9DD\xC9E0\xC9E2")
				_W2("\xC9E4\xC9E7\xC9EC\xC9ED\xC9EF\xC9F0\xC9F1\xC9F8\xC9F9\xC9FC\xCA00\xCA08\xCA09\xCA0B\xCA0C\xCA0D\xCA14\xCA18\xCA29\xCA4C\xCA4D\xCA50\xCA54\xCA5C\xCA5D")
				_W2("\xCA5F\xCA60\xCA61\xCA68\xCA7D\xCA84\xCA98\xCABC\xCABD\xCAC0\xCAC4\xCACC\xCACD\xCACF\xCAD1\xCAD3\xCAD8\xCAD9\xCAE0\xCAEC\xCAF4\xCB08\xCB10\xCB14\xCB18")
				_W2("\xCB20\xCB21\xCB41\xCB48\xCB49\xCB4C\xCB50\xCB58\xCB59\xCB5D\xCB64\xCB78\xCB79\xCB9C\xCBB8\xCBD4\xCBE4\xCBE7\xCBE9\xCC0C\xCC0D\xCC10\xCC14\xCC1C\xCC1D")
				_W2("\xCC21\xCC22\xCC27\xCC28\xCC29\xCC2C\xCC2E\xCC30\xCC38\xCC39\xCC3B\xCC3C\xCC3D\xCC3E\xCC44\xCC45\xCC48\xCC4C\xCC54\xCC55\xCC57\xCC58\xCC59\xCC60\xCC64")
				_W2("\xCC66\xCC68\xCC70\xCC75\xCC98\xCC99\xCC9C\xCCA0\xCCA8\xCCA9\xCCAB\xCCAC\xCCAD\xCCB4\xCCB5\xCCB8\xCCBC\xCCC4\xCCC5\xCCC7\xCCC9\xCCD0\xCCD4\xCCE4\xCCEC")
				_W2("\xCCF0\xCD01\xCD08\xCD09\xCD0C\xCD10\xCD18\xCD19\xCD1B\xCD1D\xCD24\xCD28\xCD2C\xCD39\xCD5C\xCD60\xCD64\xCD6C\xCD6D\xCD6F\xCD71\xCD78\xCD88\xCD94\xCD95")
				_W2("\xCD98\xCD9C\xCDA4\xCDA5\xCDA7\xCDA9\xCDB0\xCDC4\xCDCC\xCDD0\xCDE8\xCDEC\xCDF0\xCDF8\xCDF9\xCDFB\xCDFD\xCE04\xCE08\xCE0C\xCE14\xCE19\xCE20\xCE21\xCE24")
				_W2("\xCE28\xCE30\xCE31\xCE33\xCE35\xCE58\xCE59\xCE5C\xCE5F\xCE60\xCE61\xCE68\xCE69\xCE6B\xCE6D\xCE74\xCE75\xCE78\xCE7C\xCE84\xCE85\xCE87\xCE89\xCE90\xCE91")
				_W2("\xCE94\xCE98\xCEA0\xCEA1\xCEA3\xCEA4\xCEA5\xCEAC\xCEAD\xCEC1\xCEE4\xCEE5\xCEE8\xCEEB\xCEEC\xCEF4\xCEF5\xCEF7\xCEF8\xCEF9\xCF00\xCF01\xCF04\xCF08\xCF10")
				_W2("\xCF11\xCF13\xCF15\xCF1C\xCF20\xCF24\xCF2C\xCF2D\xCF2F\xCF30\xCF31\xCF38\xCF54\xCF55\xCF58\xCF5C\xCF64\xCF65\xCF67\xCF69\xCF70\xCF71\xCF74\xCF78\xCF80")
				_W2("\xCF85\xCF8C\xCFA1\xCFA8\xCFB0\xCFC4\xCFE0\xCFE1\xCFE4\xCFE8\xCFF0\xCFF1\xCFF3\xCFF5\xCFFC\xD000\xD004\xD011\xD018\xD02D\xD034\xD035\xD038\xD03C\xD044")
				_W2("\xD045\xD047\xD049\xD050\xD054\xD058\xD060\xD06C\xD06D\xD070\xD074\xD07C\xD07D\xD081\xD0A4\xD0A5\xD0A8\xD0AC\xD0B4\xD0B5\xD0B7\xD0B9\xD0C0\xD0C1\xD0C4")
				_W2("\xD0C8\xD0C9\xD0D0\xD0D1\xD0D3\xD0D4\xD0D5\xD0DC\xD0DD\xD0E0\xD0E4\xD0EC\xD0ED\xD0EF\xD0F0\xD0F1\xD0F8\xD10D\xD130\xD131\xD134\xD138\xD13A\xD140\xD141")
				_W2("\xD143\xD144\xD145\xD14C\xD14D\xD150\xD154\xD15C\xD15D\xD15F\xD161\xD168\xD16C\xD17C\xD184\xD188\xD1A0\xD1A1\xD1A4\xD1A8\xD1B0\xD1B1\xD1B3\xD1B5\xD1BA")
				_W2("\xD1BC\xD1C0\xD1D8\xD1F4\xD1F8\xD207\xD209\xD210\xD22C\xD22D\xD230\xD234\xD23C\xD23D\xD23F\xD241\xD248\xD25C\xD264\xD280\xD281\xD284\xD288\xD290\xD291")
				_W2("\xD295\xD29C\xD2A0\xD2A4\xD2AC\xD2B1\xD2B8\xD2B9\xD2BC\xD2BF\xD2C0\xD2C2\xD2C8\xD2C9\xD2CB\xD2D4\xD2D8\xD2DC\xD2E4\xD2E5\xD2F0\xD2F1\xD2F4\xD2F8\xD300")
				_W2("\xD301\xD303\xD305\xD30C\xD30D\xD30E\xD310\xD314\xD316\xD31C\xD31D\xD31F\xD320\xD321\xD325\xD328\xD329\xD32C\xD330\xD338\xD339\xD33B\xD33C\xD33D\xD344")
				_W2("\xD345\xD37C\xD37D\xD380\xD384\xD38C\xD38D\xD38F\xD390\xD391\xD398\xD399\xD39C\xD3A0\xD3A8\xD3A9\xD3AB\xD3AD\xD3B4\xD3B8\xD3BC\xD3C4\xD3C5\xD3C8\xD3C9")
				_W2("\xD3D0\xD3D8\xD3E1\xD3E3\xD3EC\xD3ED\xD3F0\xD3F4\xD3FC\xD3FD\xD3FF\xD401\xD408\xD41D\xD440\xD444\xD45C\xD460\xD464\xD46D\xD46F\xD478\xD479\xD47C\xD47F")
				_W2("\xD480\xD482\xD488\xD489\xD48B\xD48D\xD494\xD4A9\xD4CC\xD4D0\xD4D4\xD4DC\xD4DF\xD4E8\xD4EC\xD4F0\xD4F8\xD4FB\xD4FD\xD504\xD508\xD50C\xD514\xD515\xD517")
				_W2("\xD53C\xD53D\xD540\xD544\xD54C\xD54D\xD54F\xD551\xD558\xD559\xD55C\xD560\xD565\xD568\xD569\xD56B\xD56D\xD574\xD575\xD578\xD57C\xD584\xD585\xD587\xD588")
				_W2("\xD589\xD590\xD5A5\xD5C8\xD5C9\xD5CC\xD5D0\xD5D2\xD5D8\xD5D9\xD5DB\xD5DD\xD5E4\xD5E5\xD5E8\xD5EC\xD5F4\xD5F5\xD5F7\xD5F9\xD600\xD601\xD604\xD608\xD610")
				_W2("\xD611\xD613\xD614\xD615\xD61C\xD620\xD624\xD62D\xD638\xD639\xD63C\xD640\xD645\xD648\xD649\xD64B\xD64D\xD651\xD654\xD655\xD658\xD65C\xD667\xD669\xD670")
				_W2("\xD671\xD674\xD683\xD685\xD68C\xD68D\xD690\xD694\xD69D\xD69F\xD6A1\xD6A8\xD6AC\xD6B0\xD6B9\xD6BB\xD6C4\xD6C5\xD6C8\xD6CC\xD6D1\xD6D4\xD6D7\xD6D9\xD6E0")
				_W2("\xD6E4\xD6E8\xD6F0\xD6F5\xD6FC\xD6FD\xD700\xD704\xD711\xD718\xD719\xD71C\xD720\xD728\xD729\xD72B\xD72D\xD734\xD735\xD738\xD73C\xD744\xD747\xD749\xD750")
				_W2("\xD751\xD754\xD756\xD757\xD758\xD759\xD760\xD761\xD763\xD765\xD769\xD76C\xD770\xD774\xD77C\xD77D\xD781\xD788\xD789\xD78C\xD790\xD798\xD799\xD79B\xD79D");
			return UTF16;
		}

		wstring CP949Table()
		{
			global_data wstring CP949 = (wstring)
				_W2("\t\n\r !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKL")
				_W2("MNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
				_W2("\xA1A4\xA2A4\xA3A4\xA4A4\xA5A4\xA6A4\xA7A4\xA8A4\xA9A4\xAAA4\xABA4\xACA4\xADA4\xAEA4\xAFA4\xB0A4\xB1A4\xB2A4\xB3A4\xB4A4\xB5A4\xB6A4\xB7A4\xB8A4\xB9A4\xBAA4\xBBA4\xBCA4\xBDA4\xBEA4")
				_W2("\xBFA4\xC0A4\xC1A4\xC2A4\xC3A4\xC4A4\xC5A4\xC6A4\xC7A4\xC8A4\xC9A4\xCAA4\xCBA4\xCCA4\xCDA4\xCEA4\xCFA4\xD0A4\xD1A4\xD2A4\xD3A4")
				_W2("\xA1B0\xA2B0\xA3B0\xA4B0\xA5B0\xA6B0\xA7B0\xA8B0\xA9B0\xAAB0\xABB0\xACB0\xADB0\xAEB0\xAFB0\xB0B0\xB1B0\xB2B0\xB3B0\xB4B0\xB5B0\xB6B0\xB7B0\xB8B0\xB9B0")
				_W2("\xBAB0\xBBB0\xBCB0\xBDB0\xBEB0\xBFB0\xC0B0\xC1B0\xC2B0\xC3B0\xC4B0\xC5B0\xC6B0\xC7B0\xC8B0\xC9B0\xCAB0\xCBB0\xCCB0\xCDB0\xCEB0\xCFB0\xD0B0\xD1B0\xD2B0")
				_W2("\xD3B0\xD4B0\xD5B0\xD6B0\xD7B0\xD8B0\xD9B0\xDAB0\xDBB0\xDCB0\xDDB0\xDEB0\xDFB0\xE0B0\xE1B0\xE2B0\xE3B0\xE4B0\xE5B0\xE6B0\xE7B0\xE8B0\xE9B0\xEAB0\xEBB0")
				_W2("\xECB0\xEDB0\xEEB0\xEFB0\xF0B0\xF1B0\xF2B0\xF3B0\xF4B0\xF5B0\xF6B0\xF7B0\xF8B0\xF9B0\xFAB0\xFBB0\xFCB0\xFDB0\xFEB0\xA1B1\xA2B1\xA3B1\xA4B1\xA5B1\xA6B1")
				_W2("\xA7B1\x5282\xA9B1\xAAB1\xABB1\xACB1\xADB1\xAEB1\xAFB1\xB0B1\xB1B1\xB2B1\xB3B1\xB4B1\xB5B1\xB6B1\xB7B1\xB8B1\xB9B1\xBAB1\xBBB1\xBCB1\xBDB1\xBEB1\xBFB1")
				_W2("\xC0B1\xC1B1\xC2B1\xC3B1\xC4B1\xC5B1\xC6B1\xC7B1\xC8B1\xC9B1\xCAB1\xCBB1\xCCB1\xCDB1\xCEB1\xCFB1\xD0B1\xD1B1\xD2B1\xD3B1\xD4B1\xD5B1\xD6B1\xD7B1\xD8B1")
				_W2("\xD9B1\xDAB1\xDBB1\xDCB1\xDDB1\xDEB1\xDFB1\xE0B1\xE1B1\xE2B1\xE3B1\xE4B1\xE5B1\xE6B1\xE7B1\xE8B1\xE9B1\xEAB1\xEBB1\xECB1\xEDB1\xEEB1\xEFB1\xF0B1\xF1B1")
				_W2("\xF2B1\xF3B1\xF4B1\xF5B1\xF6B1\xF7B1\xF8B1\xF9B1\xFAB1\xFBB1\xFCB1\xFDB1\xFEB1\xA1B2\xA2B2\xA3B2\xA4B2\xA5B2\xA6B2\xA7B2\xA8B2\xA9B2\xAAB2\xABB2\xACB2")
				_W2("\xADB2\xAEB2\xAFB2\xB0B2\xB1B2\xB2B2\xB3B2\xB4B2\xB5B2\xB6B2\xB7B2\xB8B2\xB9B2\xBAB2\xBBB2\xBCB2\xBDB2\xBEB2\xBFB2\xC0B2\xC1B2\xC2B2\xC3B2\xC4B2\xC5B2")
				_W2("\xC6B2\xC7B2\xC8B2\xC9B2\xCAB2\xCBB2\xCCB2\xCDB2\xCEB2\xCFB2\xD0B2\xD1B2\xD2B2\xD3B2\xD4B2\xD5B2\xD6B2\xD7B2\xD8B2\xD9B2\xDAB2\xDBB2\xDCB2\xDDB2\xDEB2")
				_W2("\xDFB2\xE0B2\xE1B2\xE2B2\xE3B2\xE4B2\xE5B2\xE6B2\xE7B2\xE8B2\xE9B2\xEAB2\xEBB2\xECB2\xEDB2\xEEB2\xEFB2\xF0B2\xF1B2\xF2B2\xF3B2\xF4B2\xF5B2\xF6B2\xF7B2")
				_W2("\xF8B2\xF9B2\xFAB2\xFBB2\xFCB2\xFDB2\xFEB2\xA1B3\xA2B3\xA3B3\xA4B3\xA5B3\xA6B3\xA7B3\xA8B3\xA9B3\xAAB3\xABB3\xACB3\xADB3\xAEB3\xAFB3\xB0B3\xB1B3\xB2B3")
				_W2("\xB3B3\xB4B3\xB5B3\xB6B3\xB7B3\xB8B3\xB9B3\xBAB3\xBBB3\xBCB3\xBDB3\xBEB3\xBFB3\xC0B3\xC1B3\xC2B3\xC3B3\xC4B3\xC5B3\xC6B3\xC7B3\xC8B3\xC9B3\xCAB3\xCBB3")
				_W2("\xCCB3\xCDB3\xCEB3\xCFB3\xD0B3\xD1B3\xD2B3\xD3B3\xD4B3\xD5B3\xD6B3\xD7B3\xD8B3\xD9B3\xDAB3\xDBB3\xDCB3\xDDB3\xDEB3\xDFB3\xE0B3\xE1B3\xE2B3\xE3B3\xE4B3")
				_W2("\xE5B3\xE6B3\xE7B3\xE8B3\xE9B3\xEAB3\xEBB3\xECB3\xEDB3\xEEB3\xEFB3\xF0B3\xF1B3\xF2B3\xF3B3\xF4B3\xF5B3\xF6B3\xF7B3\xF8B3\xF9B3\xFAB3\xFBB3\xFCB3\xFDB3")
				_W2("\xFEB3\xA1B4\xA2B4\xA3B4\xA4B4\xA5B4\xA6B4\xA7B4\xA8B4\xA9B4\xAAB4\xABB4\xACB4\xADB4\xAEB4\xAFB4\xB0B4\xB1B4\xB2B4\xB3B4\xB4B4\xB5B4\xB6B4\xB7B4\xB8B4")
				_W2("\xB9B4\xBAB4\xBBB4\xBCB4\xBDB4\xBEB4\xBFB4\xC0B4\xC1B4\xC2B4\xC3B4\xC4B4\xC5B4\xC6B4\xC7B4\xC8B4\xC9B4\xCAB4\xCBB4\xCCB4\xCDB4\xCEB4\xCFB4\xD0B4\xD1B4")
				_W2("\xD2B4\x9A88\xD4B4\xD5B4\xD6B4\xD7B4\xD8B4\xD9B4\xDAB4\xDBB4\xDCB4\xDDB4\xDEB4\xDFB4\xE0B4\xE1B4\xE2B4\xE3B4\xE4B4\xE5B4\xE6B4\xE7B4\xE8B4\xE9B4\xEAB4")
				_W2("\xEBB4\xECB4\xEDB4\xEEB4\xEFB4\xF0B4\xF1B4\xF2B4\xF3B4\xF4B4\xF5B4\xF6B4\xF7B4\xF8B4\xF9B4\xFAB4\xFBB4\xFCB4\xFDB4\xFEB4\xA1B5\xA2B5\xA3B5\xA4B5\xA5B5")
				_W2("\xA6B5\xA7B5\xA8B5\xA9B5\xAAB5\xABB5\xACB5\xADB5\xAEB5\xAFB5\xB0B5\xB1B5\xB2B5\xB3B5\xB4B5\xB5B5\xB6B5\xB7B5\xB8B5\xB9B5\xBAB5\xBBB5\xBCB5\xBDB5\xBEB5")
				_W2("\xBFB5\xC0B5\xC1B5\xC2B5\xC3B5\xC4B5\xC5B5\xC6B5\xC7B5\xC8B5\xC9B5\xCAB5\xCBB5\xCCB5\xCDB5\xCEB5\xCFB5\xD0B5\xD1B5\xD2B5\xD3B5\xD4B5\xD5B5\xD6B5\xD7B5")
				_W2("\xD8B5\xD9B5\xDAB5\xDBB5\xDCB5\xDDB5\xDEB5\xDFB5\xE0B5\xE1B5\xE2B5\xE3B5\xE4B5\xE5B5\xE6B5\xE7B5\xE8B5\xE9B5\xEAB5\xEBB5\xECB5\xEDB5\xEEB5\xEFB5\xF0B5")
				_W2("\xF1B5\xF2B5\xF3B5\xF4B5\xF5B5\xF6B5\xF7B5\xF8B5\xF9B5\xFAB5\xFBB5\xFCB5\xFDB5\xFEB5\xA1B6\xA2B6\xA3B6\xA4B6\xA5B6\xA6B6\xA7B6\xA8B6\xA9B6\xAAB6\xABB6")
				_W2("\xACB6\xADB6\xAEB6\xAFB6\xB0B6\xB1B6\xB2B6\xB3B6\xB4B6\xB5B6\xB6B6\xB7B6\xB8B6\xB9B6\xBAB6\xBBB6\xBCB6\xBDB6\xBEB6\xBFB6\xC0B6\xC1B6\xC2B6\xC3B6\xC4B6")
				_W2("\xC5B6\xC6B6\xC7B6\xC8B6\xC9B6\xCAB6\xCBB6\xCCB6\xCDB6\xCEB6\xCFB6\xD0B6\xD1B6\xD2B6\xD3B6\xD4B6\xD5B6\xD6B6\xD7B6\xD8B6\xD9B6\xDAB6\xDBB6\xDCB6\xDDB6")
				_W2("\xDEB6\xDFB6\xE0B6\xE1B6\xE2B6\xE3B6\xE4B6\xE5B6\xE6B6\xE7B6\xE8B6\xE9B6\xEAB6\xEBB6\xECB6\xEDB6\xEEB6\xEFB6\xF0B6\xF1B6\xF2B6\xF3B6\xF4B6\xF5B6\xF6B6")
				_W2("\xF7B6\xF8B6\xF9B6\xFAB6\xFBB6\xFCB6\xFDB6\xFEB6\xA1B7\xA2B7\xA3B7\xA4B7\xA5B7\xA6B7\xA7B7\xA8B7\xA9B7\xAAB7\xABB7\xACB7\xADB7\xAEB7\xAFB7\xB0B7\xB1B7")
				_W2("\xB2B7\xB3B7\xB4B7\xB5B7\xB6B7\xB7B7\xB8B7\xB9B7\xBAB7\xBBB7\xBCB7\xBDB7\xBEB7\xBFB7\xC0B7\xC1B7\xC2B7\xC3B7\xC4B7\xC5B7\xC6B7\xC7B7\xC8B7\xC9B7\xCAB7")
				_W2("\xCBB7\xCCB7\xCDB7\xCEB7\xCFB7\xD0B7\xD1B7\xD2B7\xD3B7\xD4B7\xD5B7\xD6B7\xD7B7\xD8B7\xD9B7\xDAB7\xDBB7\xDCB7\xDDB7\xDEB7\xDFB7\xE0B7\xE1B7\xE2B7\xE3B7")
				_W2("\xE4B7\xE5B7\xE6B7\xE7B7\xE8B7\xE9B7\xEAB7\xEBB7\xECB7\xEDB7\xEEB7\xEFB7\xF0B7\xF1B7\xF2B7\xF3B7\xF4B7\xF5B7\xF6B7\xF7B7\xF8B7\xF9B7\xFAB7\xFBB7\xFCB7")
				_W2("\xFDB7\xFEB7\xA1B8\xA2B8\xA3B8\xA4B8\xA5B8\xA6B8\xA7B8\xA8B8\xA9B8\xAAB8\xABB8\xACB8\xADB8\xAEB8\xAFB8\xB0B8\xB1B8\xB2B8\xB3B8\xB4B8\xB5B8\xB6B8\xB7B8")
				_W2("\xB8B8\xB9B8\xBAB8\xBBB8\xBCB8\xBDB8\xBEB8\xBFB8\xC0B8\xC1B8\xC2B8\xC3B8\xC4B8\xC5B8\xC6B8\xC7B8\xC8B8\xC9B8\xCAB8\xCBB8\xCCB8\xCDB8\xCEB8\xCFB8\xD0B8")
				_W2("\xD1B8\xD2B8\xD3B8\xD4B8\xD5B8\xD6B8\xD7B8\xD8B8\xD9B8\xDAB8\xDBB8\xDCB8\xDDB8\xDEB8\xDFB8\xE0B8\xE1B8\xE2B8\xE3B8\xE4B8\xE5B8\xE6B8\xE7B8\xE8B8\xE9B8")
				_W2("\xEAB8\xEBB8\xECB8\xEDB8\xEEB8\xEFB8\xF0B8\xF1B8\xF2B8\xF3B8\xF4B8\xF5B8\xF6B8\xF7B8\xF8B8\xF9B8\xFAB8\xFBB8\xFCB8\xFDB8\xFEB8\xA1B9\xA2B9\xA3B9\xA4B9")
				_W2("\xA5B9\xA6B9\xA7B9\xA8B9\xA9B9\xAAB9\xABB9\xACB9\xADB9\xAEB9\xAFB9\xB0B9\xB1B9\xB2B9\xB3B9\xB4B9\xB5B9\xB6B9\xB7B9\xB8B9\xB9B9\xBAB9\xBBB9\xBCB9\xBDB9")
				_W2("\xBEB9\xBFB9\xC0B9\xC1B9\xC2B9\xC3B9\xC4B9\xC5B9\xC6B9\xC7B9\xC8B9\xC9B9\xCAB9\xCBB9\xCCB9\xCDB9\xCEB9\xCFB9\xD0B9\xD1B9\xD2B9\xD3B9\xD4B9\xD5B9\xD6B9")
				_W2("\xD7B9\xD8B9\xD9B9\xDAB9\xDBB9\xDCB9\xDDB9\xDEB9\xDFB9\xE0B9\xE1B9\xE2B9\xE3B9\xE4B9\xE5B9\xE6B9\xE7B9\xE8B9\xE9B9\xEAB9\xEBB9\xECB9\xEDB9\xEEB9\xEFB9")
				_W2("\xF0B9\xF1B9\xF2B9\xF3B9\xF4B9\xF5B9\xF6B9\xF7B9\xF8B9\xF9B9\xFAB9\xFBB9\xFCB9\xFDB9\xFEB9\xA1BA\xA2BA\xA3BA\xA4BA\xA5BA\xA6BA\xA7BA\xA8BA\xA9BA\xAABA")
				_W2("\xABBA\xACBA\xADBA\xAEBA\xAFBA\xB0BA\xB1BA\xB2BA\xB3BA\xB4BA\xB5BA\xB6BA\xB7BA\xB8BA\xB9BA\xBABA\xBBBA\xBCBA\xBDBA\xBEBA\xBFBA\xC0BA\xC1BA\xC2BA\xC3BA")
				_W2("\xC4BA\xC5BA\xC6BA\xC7BA\xC8BA\xC9BA\xCABA\xCBBA\xCCBA\xCDBA\xCEBA\xCFBA\xD0BA\xD1BA\xD2BA\xD3BA\xD4BA\xD5BA\xD6BA\xD7BA\xD8BA\xD9BA\xDABA\xDBBA\xDCBA")
				_W2("\xDDBA\xDEBA\xDFBA\xE0BA\xE1BA\xE2BA\xE3BA\xE4BA\xE5BA\xE6BA\xE7BA\xE8BA\xE9BA\xEABA\xEBBA\xECBA\xEDBA\xEEBA\xEFBA\xF0BA\xF1BA\xF2BA\xF3BA\xF4BA\xF5BA")
				_W2("\xF6BA\xF7BA\xF8BA\xF9BA\xFABA\xFBBA\xFCBA\xFDBA\xFEBA\xA1BB\xA2BB\xA3BB\xA4BB\xA5BB\xA6BB\xA7BB\xA8BB\xA9BB\xAABB\xABBB\xACBB\xADBB\xAEBB\xAFBB\xB0BB")
				_W2("\xB1BB\xB2BB\xB3BB\xB4BB\xB5BB\xB6BB\xB7BB\xB8BB\xB9BB\xBABB\xBBBB\xBCBB\xBDBB\xBEBB\xBFBB\xC0BB\xC1BB\xC2BB\xC3BB\xC4BB\xC5BB\xC6BB\xC7BB\xC8BB\xC9BB")
				_W2("\xCABB\xCBBB\xCCBB\xCDBB\xCEBB\xCFBB\xD0BB\xD1BB\xD2BB\xD3BB\xD4BB\xD5BB\xD6BB\xD7BB\xD8BB\xD9BB\xDABB\xDBBB\xDCBB\xDDBB\xDEBB\xDFBB\xE0BB\xE1BB\xE2BB")
				_W2("\xE3BB\xE4BB\xE5BB\xE6BB\xE7BB\xE8BB\xE9BB\xEABB\xEBBB\xECBB\xEDBB\xEEBB\xEFBB\xF0BB\xF1BB\xF2BB\xF3BB\xF4BB\xF5BB\xF6BB\xF7BB\xF8BB\xF9BB\xFABB\xFBBB")
				_W2("\xFCBB\xFDBB\xFEBB\xA1BC\xA2BC\xA3BC\xA4BC\xA5BC\xA6BC\xA7BC\xA8BC\xA9BC\xAABC\xABBC\xACBC\xADBC\xAEBC\xAFBC\xB0BC\xB1BC\xB2BC\xB3BC\xB4BC\xB5BC\xB6BC")
				_W2("\xB7BC\xB8BC\xB9BC\xBABC\xBBBC\xBCBC\xBDBC\xBEBC\xBFBC\xC0BC\xC1BC\xC2BC\xC3BC\xC4BC\xC5BC\xC6BC\xC7BC\xC8BC\xC9BC\xCABC\xCBBC\xCCBC\xCDBC\xCEBC\xCFBC")
				_W2("\xD0BC\xD1BC\xD2BC\xD3BC\xD4BC\xD5BC\xD6BC\xD7BC\xD8BC\xD9BC\xDABC\xDBBC\xDCBC\xDDBC\xDEBC\xDFBC\xE0BC\xE1BC\xE2BC\xE3BC\xE4BC\xE5BC\xE6BC\xE7BC\xE8BC")
				_W2("\xE9BC\xEABC\xEBBC\xECBC\xEDBC\xEEBC\xEFBC\xF0BC\xF1BC\xF2BC\xF3BC\xF4BC\xF5BC\xF6BC\xF7BC\xF8BC\xF9BC\xFABC\xFBBC\xFCBC\xFDBC\xFEBC\xA1BD\xA2BD\xA3BD")
				_W2("\xA4BD\xA5BD\xA6BD\xA7BD\xA8BD\xA9BD\xAABD\xABBD\xACBD\xADBD\xAEBD\xAFBD\xB0BD\xB1BD\xB2BD\xB3BD\xB4BD\xB5BD\xB6BD\xB7BD\xB8BD\xB9BD\xBABD\xBBBD\xBCBD")
				_W2("\xBDBD\xBEBD\xBFBD\xC0BD\xC1BD\xC2BD\xC3BD\xC4BD\xC5BD\xC6BD\xC7BD\xC8BD\xC9BD\xCABD\xCBBD\xCCBD\xCDBD\xCEBD\xCFBD\xD0BD\xD1BD\xD2BD\xD3BD\xD4BD\xD5BD")
				_W2("\xD6BD\xD7BD\xD8BD\xD9BD\xDABD\xDBBD\xDCBD\xDDBD\xDEBD\xDFBD\xE0BD\xE1BD\xE2BD\xE3BD\xE4BD\xE5BD\xE6BD\xE7BD\xE8BD\xE9BD\xEABD\xEBBD\xECBD\xC09B\xEEBD")
				_W2("\xEFBD\xF0BD\xF1BD\xF2BD\xF3BD\xF4BD\xF5BD\xF6BD\xF7BD\xF8BD\xF9BD\xFABD\xFBBD\xFCBD\xFDBD\xFEBD\xA1BE\xA2BE\xA3BE\xA4BE\xA5BE\xA6BE\xA7BE\xA8BE\xA9BE")
				_W2("\xAABE\xABBE\xACBE\xADBE\xAEBE\xAFBE\xB0BE\xB1BE\xB2BE\xB3BE\xB4BE\xB5BE\xB6BE\xB7BE\xB8BE\xB9BE\xBABE\xBBBE\xBCBE\xBDBE\xBEBE\xBFBE\xC0BE\xC1BE\xC2BE")
				_W2("\xC3BE\xC4BE\xC5BE\xC6BE\xC7BE\xC8BE\xC9BE\xCABE\xCBBE\xCCBE\xCDBE\xCEBE\xCFBE\xD0BE\xD1BE\xD2BE\xD3BE\xD4BE\xD5BE\xD6BE\xD7BE\xD8BE\xD9BE\xDABE\xDBBE")
				_W2("\xDCBE\xDDBE\xDEBE\xDFBE\xE0BE\xE1BE\xE2BE\xE3BE\xE4BE\xE5BE\xE6BE\xE7BE\xE8BE\xE9BE\xEABE\xEBBE\xECBE\xEDBE\xEEBE\xEFBE\xF0BE\xF1BE\xF2BE\xF3BE\xF4BE")
				_W2("\xF5BE\xF6BE\xF7BE\xF8BE\xF9BE\xFABE\xFBBE\xFCBE\xFDBE\xFEBE\xA1BF\xA2BF\xA3BF\xA4BF\xA5BF\xA6BF\xA7BF\xA8BF\xA9BF\xAABF\xABBF\xACBF\xADBF\xAEBF\xAFBF")
				_W2("\xB0BF\xB1BF\xB2BF\xB3BF\xB4BF\xB5BF\xB6BF\xB7BF\xB8BF\xB9BF\xBABF\xBBBF\xBCBF\xBDBF\xBEBF\xBFBF\xC0BF\xC1BF\xC2BF\xC3BF\xC4BF\xC5BF\xC6BF\xC7BF\xC8BF")
				_W2("\xC9BF\xCABF\xCBBF\xCCBF\xCDBF\xCEBF\xCFBF\xD0BF\xD1BF\xD2BF\xD3BF\xD4BF\xD5BF\xD6BF\xD7BF\xD8BF\xD9BF\xDABF\xDBBF\xDCBF\xDDBF\xDEBF\xDFBF\xE0BF\xE1BF")
				_W2("\xE2BF\xE3BF\xE4BF\xE5BF\xE6BF\xE7BF\xE8BF\xE9BF\xEABF\xEBBF\xECBF\xEDBF\xEEBF\xEFBF\xF0BF\xF1BF\xF2BF\xF3BF\xF4BF\xF5BF\xF6BF\xF7BF\xF8BF\xF9BF\xFABF")
				_W2("\xFBBF\xFCBF\xFDBF\xFEBF\xA1C0\xA2C0\xA3C0\xA4C0\xA5C0\xA6C0\xA7C0\xA8C0\xA9C0\xAAC0\xABC0\xACC0\xADC0\xAEC0\xAFC0\xB0C0\xB1C0\xB2C0\xB3C0\xB4C0\xB5C0")
				_W2("\xB6C0\xB7C0\xB8C0\xB9C0\xBAC0\xBBC0\xBCC0\xBDC0\xBEC0\xBFC0\xC0C0\xC1C0\xC2C0\xC3C0\xC4C0\xC5C0\xC6C0\xC7C0\xC8C0\xC9C0\xCAC0\xCBC0\xCCC0\xCDC0\xCEC0")
				_W2("\xCFC0\xD0C0\xD1C0\xD2C0\xD3C0\xD4C0\xD5C0\xD6C0\xD7C0\xD8C0\xD9C0\xDAC0\xDBC0\xDCC0\xDDC0\xDEC0\xDFC0\xE0C0\xE1C0\xE2C0\xE3C0\xE4C0\xE5C0\xE6C0\xE7C0")
				_W2("\xE8C0\xE9C0\xEAC0\xEBC0\xECC0\xEDC0\xEEC0\xEFC0\xF0C0\xF1C0\xF2C0\xF3C0\xF4C0\xF5C0\xF6C0\xF7C0\xF8C0\xF9C0\xFAC0\xFBC0\xFCC0\xFDC0\xFEC0\xA1C1\xA2C1")
				_W2("\xA3C1\xA4C1\xA5C1\xA6C1\xA7C1\xA8C1\xA9C1\xAAC1\xABC1\xACC1\xADC1\xAEC1\xAFC1\xB0C1\xB1C1\xB2C1\xB3C1\xB4C1\xB5C1\xB6C1\xB7C1\xB8C1\xB9C1\xBAC1\xBBC1")
				_W2("\xBCC1\xBDC1\xBEC1\xBFC1\xC0C1\xC1C1\xC2C1\xC3C1\xC4C1\xC5C1\xC6C1\xC7C1\xC8C1\xC9C1\xCAC1\xCBC1\xCCC1\xCDC1\xCEC1\xCFC1\xD0C1\xD1C1\xD2C1\xD3C1\xD4C1")
				_W2("\xD5C1\xD6C1\xD7C1\xD8C1\xD9C1\xDAC1\xDBC1\xDCC1\xDDC1\xDEC1\xDFC1\xE0C1\xE1C1\xE2C1\xE3C1\xE4C1\xE5C1\xE6C1\xE7C1\xE8C1\xE9C1\xEAC1\xEBC1\xECC1\xEDC1")
				_W2("\xEEC1\xEFC1\xF0C1\xF1C1\xF2C1\xF3C1\xF4C1\xF5C1\xF6C1\xF7C1\xF8C1\xF9C1\xFAC1\xFBC1\xFCC1\xFDC1\xFEC1\xA1C2\xA2C2\xA3C2\xA4C2\xA5C2\xA6C2\xA7C2\xA8C2")
				_W2("\xA9C2\xAAC2\xABC2\xACC2\xADC2\xAEC2\xAFC2\xB0C2\xB1C2\xB2C2\xB3C2\xB4C2\xB5C2\xB6C2\xB7C2\xB8C2\xB9C2\xBAC2\xBBC2\xBCC2\xBDC2\xBEC2\xBFC2\xC0C2\xC1C2")
				_W2("\xC2C2\xC3C2\xC4C2\xC5C2\xC6C2\xC7C2\xC8C2\xC9C2\xCAC2\xCBC2\xCCC2\xCDC2\xCEC2\xCFC2\xD0C2\xD1C2\xD2C2\xD3C2\xD4C2\xD5C2\xD6C2\xD7C2\xD8C2\xD9C2\xDAC2")
				_W2("\xDBC2\xDCC2\xDDC2\xDEC2\xDFC2\xE0C2\xE1C2\xE2C2\xE3C2\xE4C2\xE5C2\xE6C2\xE7C2\xE8C2\xE9C2\xEAC2\xEBC2\xECC2\xEDC2\xEEC2\xEFC2\xF0C2\xF1C2\xF2C2\xF3C2")
				_W2("\xF4C2\xF5C2\xF6C2\xF7C2\xF8C2\xF9C2\xFAC2\xFBC2\xFCC2\xFDC2\xFEC2\xA1C3\xA2C3\xA3C3\xA4C3\xA5C3\xA6C3\xA7C3\xA8C3\xA9C3\xAAC3\xABC3\xACC3\xADC3\xAEC3")
				_W2("\xAFC3\xB0C3\xB1C3\xB2C3\xB3C3\xB4C3\xB5C3\xB6C3\xB7C3\xB8C3\xB9C3\xBAC3\xBBC3\xBCC3\xBDC3\xBEC3\xBFC3\xC0C3\xC1C3\xC2C3\xC3C3\xC4C3\xC5C3\xC6C3\xC7C3")
				_W2("\xC8C3\xC9C3\xCAC3\xCBC3\xCCC3\xCDC3\xCEC3\xCFC3\xD0C3\xD1C3\xD2C3\xD3C3\xD4C3\xD5C3\xD6C3\xD7C3\xD8C3\xD9C3\xDAC3\xDBC3\xDCC3\xDDC3\xDEC3\xDFC3\xE0C3")
				_W2("\xE1C3\xE2C3\xE3C3\xE4C3\xE5C3\xE6C3\xE7C3\xE8C3\xE9C3\xEAC3\xEBC3\xECC3\xEDC3\xEEC3\xEFC3\xF0C3\xF1C3\xF2C3\xF3C3\xF4C3\xF5C3\xF6C3\xF7C3\xF8C3\xF9C3")
				_W2("\xFAC3\xFBC3\xFCC3\xFDC3\xFEC3\xA1C4\xA2C4\xA3C4\xA4C4\xA5C4\xA6C4\xA7C4\xA8C4\xA9C4\xAAC4\xABC4\xACC4\xADC4\xAEC4\xAFC4\xB0C4\xB1C4\xB2C4\xB3C4\xB4C4")
				_W2("\xB5C4\xB6C4\xB7C4\xB8C4\xB9C4\xBAC4\xBBC4\xBCC4\xBDC4\xBEC4\xBFC4\xC0C4\xC1C4\xC2C4\xC3C4\xC4C4\xC5C4\xC6C4\xC7C4\xC8C4\xC9C4\xCAC4\xCBC4\xCCC4\xCDC4")
				_W2("\xCEC4\xCFC4\xD0C4\xD1C4\xD2C4\xD3C4\xD4C4\xD5C4\xD6C4\xD7C4\xD8C4\xD9C4\xDAC4\xDBC4\xDCC4\xDDC4\xDEC4\xDFC4\xE0C4\xE1C4\xE2C4\xE3C4\xE4C4\xE5C4\xE6C4")
				_W2("\xE7C4\xE8C4\xE9C4\xEAC4\xEBC4\xECC4\xEDC4\xEEC4\xEFC4\xF0C4\xF1C4\xF2C4\xF3C4\xF4C4\xF5C4\xF6C4\xF7C4\xF8C4\xF9C4\xFAC4\xFBC4\xFCC4\xFDC4\xFEC4\xA1C5")
				_W2("\xA2C5\xA3C5\xA4C5\xA5C5\xA6C5\xA7C5\xA8C5\xA9C5\xAAC5\xABC5\xACC5\xADC5\xAEC5\xAFC5\xB0C5\xB1C5\xB2C5\xB3C5\xB4C5\xB5C5\xB6C5\xB7C5\xB8C5\xB9C5\xBAC5")
				_W2("\xBBC5\xBCC5\xBDC5\xBEC5\xBFC5\xC0C5\xC1C5\xC2C5\xC3C5\xC4C5\xC5C5\xC6C5\xC7C5\xC8C5\xC9C5\xCAC5\xCBC5\xCCC5\xCDC5\xCEC5\xCFC5\xD0C5\xD1C5\xD2C5\xD3C5")
				_W2("\xD4C5\xD5C5\xD6C5\xD7C5\xD8C5\xD9C5\xDAC5\xDBC5\xDCC5\xDDC5\xDEC5\xDFC5\xE0C5\xE1C5\xE2C5\xE3C5\xE4C5\xE5C5\xE6C5\xE7C5\xE8C5\xE9C5\xEAC5\xEBC5\xECC5")
				_W2("\xEDC5\xEEC5\xEFC5\xF0C5\xF1C5\xF2C5\xF3C5\xF4C5\xF5C5\xF6C5\xF7C5\xF8C5\xF9C5\xFAC5\xFBC5\xFCC5\xFDC5\xFEC5\xA1C6\xA2C6\xA3C6\xA4C6\xA5C6\xA6C6\xA7C6")
				_W2("\xA8C6\xA9C6\xAAC6\xABC6\xACC6\xADC6\xAEC6\xAFC6\xB0C6\xB1C6\xB2C6\xB3C6\xB4C6\xB5C6\xB6C6\xB7C6\xB8C6\xB9C6\xBAC6\xBBC6\xBCC6\xBDC6\xBEC6\xBFC6\xC0C6")
				_W2("\xC1C6\xC2C6\xC3C6\xC4C6\xC5C6\xC6C6\xC7C6\xC8C6\xC9C6\xCAC6\xCBC6\xCCC6\xCDC6\xCEC6\xCFC6\xD0C6\xD1C6\xD2C6\xD3C6\xD4C6\xD5C6\xD6C6\xD7C6\xD8C6\xD9C6")
				_W2("\xDAC6\xDBC6\xDCC6\xDDC6\xDEC6\xDFC6\xE0C6\xE1C6\xE2C6\xE3C6\xE4C6\xE5C6\xE6C6\xE7C6\xE8C6\xE9C6\xEAC6\xEBC6\xECC6\xEDC6\xEEC6\xEFC6\xF0C6\xF1C6\xF2C6")
				_W2("\xF3C6\xF4C6\xF5C6\xF6C6\xF7C6\xF8C6\xF9C6\xFAC6\xFBC6\xFCC6\xFDC6\xFEC6\xA1C7\xA2C7\xA3C7\xA4C7\xA5C7\xA6C7\xA7C7\xA8C7\xA9C7\xAAC7\xABC7\xACC7\xADC7")
				_W2("\xAEC7\xAFC7\xB0C7\xB1C7\xB2C7\xB3C7\xB4C7\xB5C7\xB6C7\xB7C7\xB8C7\xB9C7\xBAC7\xBBC7\xBCC7\xBDC7\xBEC7\xBFC7\xC0C7\xC1C7\xC2C7\xC3C7\xC4C7\xC5C7\xC6C7")
				_W2("\xC7C7\xC8C7\xC9C7\xCAC7\xCBC7\xCCC7\xCDC7\xCEC7\xCFC7\xD0C7\xD1C7\xD2C7\xD3C7\xD4C7\xD5C7\xD6C7\xD7C7\xD8C7\xD9C7\xDAC7\xDBC7\xDCC7\xDDC7\xDEC7\xDFC7")
				_W2("\xE0C7\xE1C7\xE2C7\xE3C7\xE4C7\xE5C7\xE6C7\xE7C7\xE8C7\xE9C7\xEAC7\xEBC7\xECC7\xEDC7\xEEC7\xEFC7\xF0C7\xF1C7\xF2C7\xF3C7\xF4C7\xF5C7\xF6C7\xF7C7\xF8C7")
				_W2("\xF9C7\xFAC7\xFBC7\xFCC7\xFDC7\xFEC7\xA1C8\xA2C8\xA3C8\xA4C8\xA5C8\xA6C8\xA7C8\xA8C8\xA9C8\xAAC8\xABC8\xACC8\xADC8\xAEC8\xAFC8\xB0C8\xB1C8\xB2C8\xB3C8")
				_W2("\xB4C8\xB5C8\xB6C8\xB7C8\xB8C8\xB9C8\xBAC8\xBBC8\xBCC8\xBDC8\xBEC8\xBFC8\xC0C8\xC1C8\xC2C8\xC3C8\xC4C8\xC5C8\xC6C8\xC7C8\xC8C8\xC9C8\xCAC8\xCBC8\xCCC8")
				_W2("\xCDC8\xCEC8\xCFC8\xD0C8\xD1C8\xD2C8\xD3C8\xD4C8\xD5C8\xD6C8\xD7C8\xD8C8\xD9C8\xDAC8\xDBC8\xDCC8\xDDC8\xDEC8\xDFC8\xE0C8\xE1C8\xE2C8\xE3C8\xE4C8\xE5C8")
				_W2("\xE6C8\xE7C8\xE8C8\xE9C8\xEAC8\xEBC8\xECC8\xEDC8\xEEC8\xEFC8\xF0C8\xF1C8\xF2C8\xF3C8\xF4C8\xF5C8\xF6C8\xF7C8\xF8C8\xF9C8\xFAC8\xFBC8\xFCC8\xFDC8\xFEC8");
			return CP949;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace AddOn
	{
		const byte* JPGToBMP(const byte* jpg)
		{
			return ((CoreJPGToBMP) ForExtend_JPGToBMP())(jpg);
		}

		const byte* GIFToBMP(const byte* gif, const int length, int _out_ numpage)
		{
			return ((CoreGIFToBMP) ForExtend_GIFToBMP())(gif, length, numpage);
		}

		const byte* HQXToBMP(const byte* bmp, int scale)
		{
			return ((CoreHQXToBMP) ForExtend_HQXToBMP())(bmp, scale);
		}

		string _tmp_ BUFToMD5(const byte* buffer, const int length)
		{
			return ((CoreBUFToMD5) ForExtend_BUFToMD5())(buffer, length);
		}

		id_zip CreateZIP(const byte* zip, const int length, int _out_ numfile, string password)
		{
			return ((CoreCreateZIP) ForExtend_CreateZIP())(zip, length, numfile, password);
		}

		void ReleaseZIP(id_zip zip)
		{
			return ((CoreReleaseZIP) ForExtend_ReleaseZIP())(zip);
		}

		const byte* ZIPToFILE(id_zip zip, const int fileindex, int _out_ filesize)
		{
			return ((CoreZIPToFILE) ForExtend_ZIPToFILE())(zip, fileindex, filesize);
		}

		string _tmp_ ZIPToINFO(id_zip zip, const int fileindex,
			bool* isdir, uhuge* ctime, uhuge* mtime, uhuge* atime,
			bool* archive, bool* hidden, bool* readonly, bool* system)
		{
			return ((CoreZIPToINFO) ForExtend_ZIPToINFO())(zip, fileindex, isdir, ctime, mtime, atime, archive, hidden, readonly, system);
		}

		void Release(const byte* buf)
		{
			BxFree(buf);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Socket
	{
		int RecvFully(id_socket sock, byte* buffer, int len, int timeout, bool cancelmode)
		{
			BxASSERT("BxCore::Socket<파라미터 len은 1 이상이어야 합니다>", 1 <= len);
			int RecvResult = 0, RecvedLength = 0, ErrorTime = 0, SleepTime = 0;
			while(0 <= (RecvResult = Recv(sock, buffer + RecvedLength, len - RecvedLength)))
			{
				RecvedLength += RecvResult;
				if(0 < RecvResult)
				{
					ErrorTime = 0;
					SleepTime >>= 1;
				}
				if(RecvedLength == len || (RecvedLength == 0 && cancelmode))
					return RecvedLength;
				else if(timeout < (ErrorTime += SleepTime))
				{
					Disconnect(sock);
					return RecvedLength;
				}
				else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, timeout >> 6));
			}
			return RecvResult;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace OpenGL2D
	{
		const int GetTextureMargin()
		{
			global_data int TextureMargin = -1;
			if(TextureMargin == -1)
				TextureMargin = BxCore::System::GetConfigNumber("Bx.Texture.Margin", 2);
			return TextureMargin;
		}

		const bool DoTextureInterpolation()
		{
			global_data int IsTextureInterpolation = -1;
			if(IsTextureInterpolation == -1)
				IsTextureInterpolation = BxCore::System::GetConfigNumber("Bx.Texture.Interpolation", 1);
			return (IsTextureInterpolation != 0);
		}
	}
}
