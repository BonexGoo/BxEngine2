#pragma once
#include <BxKeyword.hpp>

namespace BxCore
{
	namespace Main
	{
		/// @cond SECTION_NAME
		int& ForMain_LetUpdateCount() {global_data int UpdateCount = 0; return UpdateCount;}
		int& ForMain_LetRenderCount() {global_data int RenderCount = 0; return RenderCount;}
		int& ForMain_LetFrameTime() {global_data int FrameTime = 50; return FrameTime;}
		int& ForMain_LetFrameDelay() {global_data int FrameDelay = 0; return FrameDelay;}
		rect& ForMain_LetGUIMargin() {global_data rect GUIMargin = {0, 0, 0, 0}; return GUIMargin;}
		/// @endcond
	}
	
	namespace System
	{
		/// @cond SECTION_NAME
		void _CertifyConfigLoad()
		{
			global_data bool IsLoaded = false;
			if(IsLoaded) return;
			const bool LoadResult = BxKeyword::Load("config/default.key");
			BxASSERT("BxCore::System<config/default.key파일을 로드할 수 없습니다>", LoadResult);
			IsLoaded = true;
		}
		/// @endcond
		
		/// @cond SECTION_NAME
		class CallbackList
		{
		public:
			callback_frame Method;
			bool IsDataRef;
			int Value;
			void* Data;
			CallbackList* Next;
		public:
			CallbackList(callback_frame method, int value, void* data, int datasize)
			{
				Method = method;
				Value = value;
				IsDataRef = (datasize <= -1);
				if(!IsDataRef)
				{
					if(0 < datasize && data)
					{
						Data = BxAlloc(datasize);
						BxCore::Util::MemCpy(Data, data, datasize);
					}
					else Data = nullptr;
				}
				else Data = data;
				Next = nullptr;
			}
			virtual ~CallbackList()
			{
				if(!IsDataRef && Data) BxFree(Data);
				if(Next) BxDelete(Next);
			}
			global_func CallbackList* GetFirst()
			{
				global_data CallbackList First(nullptr, 0, nullptr, -1);
				return &First;
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		void ForSystem_CallbackRun()
		{
			CallbackList* List = CallbackList::GetFirst();
			while(List->Next)
			{
				if(!List->Next->Method(List->Next->Value, List->Next->Data))
				{
					CallbackList* DeleteList = List->Next;
					List->Next = DeleteList->Next;
					DeleteList->Next = nullptr;
					BxDelete(DeleteList);
				}
				else List = List->Next;
			}
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func inline callback_process& _ChildProcess() {global_data callback_process ChildProcess = nullptr; return ChildProcess;}
		void ForSystem_ClearChildProcess()
		{
			_ChildProcess() = nullptr;
		}
		bool ForSystem_HasChildProcess()
		{
			return (_ChildProcess() != nullptr);
		}
		void ForSystem_RunChildProcess(callback_procedure yield)
		{
			BxASSERT("BxCore::System::_RunChildProcess<등록된 ChildProcess가 없습니다>", _ChildProcess());
			_ChildProcess()(yield);
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func inline callback_edk& _EDKClient() {global_data callback_edk EDKClient = nullptr; return EDKClient;}
		bool _EDKCallback(int value, void* data)
		{
			if(_EDKClient())
				_EDKClient()((string) data, value);
			return false;
		}
		void _EDKReceiver(string param1_str, int param2)
		{
			AddCallback(_EDKCallback, param2, (void*) param1_str, BxUtilGlobal::StrLen(param1_str) + 1);
		}
		/// @endcond
	}

	namespace AddOn
	{
		/// @cond SECTION_NAME
		typedef const byte* (*CoreJPGToBMP)(const byte*);
		const byte* DefaultJPGToBMP(const byte*)
		{BxASSERT("BxCore::AddOn<해당 JPGToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_JPGToBMP() {static void* Function = (void*) DefaultJPGToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreGIFToBMP)(const byte*, const int, int _out_);
		const byte* DefaultGIFToBMP(const byte*, const int, int _out_)
		{BxASSERT("BxCore::AddOn<해당 GIFToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_GIFToBMP() {static void* Function = (void*) DefaultGIFToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreHQXToBMP)(const byte*, int);
		const byte* DefaultHQXToBMP(const byte*, int)
		{BxASSERT("BxCore::AddOn<해당 HQXToBMP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_HQXToBMP() {static void* Function = (void*) DefaultHQXToBMP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef string _tmp_ (*CoreBUFToMD5)(const byte*, const int);
		string _tmp_ DefaultBUFToMD5(const byte*, const int)
		{BxASSERT("BxCore::AddOn<해당 BUFToMD5확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_BUFToMD5() {static void* Function = (void*) DefaultBUFToMD5; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef id_zip (*CoreCreateZIP)(const byte*, const int, int _out_, string);
		id_zip DefaultCreateZIP(const byte*, const int, int _out_, string)
		{BxASSERT("BxCore::AddOn<해당 CreateZIP확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_CreateZIP() {static void* Function = (void*) DefaultCreateZIP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef void (*CoreReleaseZIP)(id_zip);
		void DefaultReleaseZIP(id_zip)
		{BxASSERT("BxCore::AddOn<해당 ReleaseZIP확장함수가 없습니다>", false);}
		void* _inout_ ForExtend_ReleaseZIP() {static void* Function = (void*) DefaultReleaseZIP; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef const byte* (*CoreZIPToFILE)(id_zip, const int, int _out_);
		const byte* DefaultZIPToFILE(id_zip, const int, int _out_)
		{BxASSERT("BxCore::AddOn<해당 ZIPToFILE확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_ZIPToFILE() {static void* Function = (void*) DefaultZIPToFILE; return Function;}
		/// @endcond

		/// @cond SECTION_NAME
		typedef string _tmp_ (*CoreZIPToINFO)(id_zip, const int, bool*, uhuge*, uhuge*, uhuge*, bool*, bool*, bool*, bool*);
		string _tmp_ DefaultZIPToINFO(id_zip, const int, bool*, uhuge*, uhuge*, uhuge*, bool*, bool*, bool*, bool*)
		{BxASSERT("BxCore::AddOn<해당 ZIPToINFO확장함수가 없습니다>", false); return nullptr;}
		void* _inout_ ForExtend_ZIPToINFO() {static void* Function = (void*) DefaultZIPToINFO; return Function;}
		/// @endcond
	}
}
