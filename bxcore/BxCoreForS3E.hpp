#pragma once
#include "s3e.h"
#include "netdb.h"
#include "s3eLibrary.h"
#include "s3eOSExec.h"
#include "s3eOSReadString.h"
#include "s3eAndroidJNI.h"
#include "../edk/h/jni.h"
#include "string.h"
#include "derbh.h"
#include "arpa/inet.h"
#include "IwGL.h" // for marmalade logo disabled
#include "IwGx.h"
#include "IwGxFont.h"
#include "IwGxFontContainer.h"
#include "IwUTF8.h"
#include "IwDebug.h"
#ifdef _WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif

#include <BxSingleton.hpp>
#include <BxPool.hpp>
#include <BxAutoBuffer.hpp>
#include <BxScene.hpp>
#include <BxVarMap.hpp>

namespace BxCore
{
	namespace Main
	{
		/// @cond SECTION_NAME
		local_func keykind GetKeyKind(s3eKey keycode)
		{
			switch(keycode)
			{
			case s3eKeyEsc: return keykind_esc;
			case s3eKeyTab: return keykind_tab;
			case s3eKeyBackspace: return keykind_bspace;
			case s3eKeyEnter: return keykind_enter;
			case s3eKeyLeftShift: return keykind_shift;
			case s3eKeyLeftControl: return keykind_ctrl;
			case s3eKeyReserved: return keykind_null;
			case s3eKeySpace: return keykind_space;
			case s3eKeyLeft: return keykind_left;
			case s3eKeyUp: return keykind_up;
			case s3eKeyRight: return keykind_right;
			case s3eKeyDown: return keykind_down;
			case s3eKey0: return keykind_0;
			case s3eKey1: return keykind_1;
			case s3eKey2: return keykind_2;
			case s3eKey3: return keykind_3;
			case s3eKey4: return keykind_4;
			case s3eKey5: return keykind_5;
			case s3eKey6: return keykind_6;
			case s3eKey7: return keykind_7;
			case s3eKey8: return keykind_8;
			case s3eKey9: return keykind_9;
			case s3eKeyA: return keykind_a;
			case s3eKeyB: return keykind_b;
			case s3eKeyC: return keykind_c;
			case s3eKeyD: return keykind_d;
			case s3eKeyE: return keykind_e;
			case s3eKeyF: return keykind_f;
			case s3eKeyG: return keykind_g;
			case s3eKeyH: return keykind_h;
			case s3eKeyI: return keykind_i;
			case s3eKeyJ: return keykind_j;
			case s3eKeyK: return keykind_k;
			case s3eKeyL: return keykind_l;
			case s3eKeyM: return keykind_m;
			case s3eKeyN: return keykind_n;
			case s3eKeyO: return keykind_o;
			case s3eKeyP: return keykind_p;
			case s3eKeyQ: return keykind_q;
			case s3eKeyR: return keykind_r;
			case s3eKeyS: return keykind_s;
			case s3eKeyT: return keykind_t;
			case s3eKeyU: return keykind_u;
			case s3eKeyV: return keykind_v;
			case s3eKeyW: return keykind_w;
			case s3eKeyX: return keykind_x;
			case s3eKeyY: return keykind_y;
			case s3eKeyZ: return keykind_z;
			case s3eKeyF1: return keykind_f1;
			case s3eKeyF2: return keykind_f2;
			case s3eKeyF3: return keykind_f3;
			case s3eKeyF4: return keykind_f4;
			case s3eKeyF5: return keykind_f5;
			case s3eKeyF6: return keykind_f6;
			case s3eKeyF7: return keykind_f7;
			case s3eKeyF8: return keykind_f8;
			case s3eKeyF9: return keykind_f9;
			case s3eKeyF10: return keykind_f10;
			case s3eKeyNumPad0: return keykind_pad_0;
			case s3eKeyNumPad1: return keykind_pad_1;
			case s3eKeyNumPad2: return keykind_pad_2;
			case s3eKeyNumPad3: return keykind_pad_3;
			case s3eKeyNumPad4: return keykind_pad_4;
			case s3eKeyNumPad5: return keykind_pad_5;
			case s3eKeyNumPad6: return keykind_pad_6;
			case s3eKeyNumPad7: return keykind_pad_7;
			case s3eKeyNumPad8: return keykind_pad_8;
			case s3eKeyNumPad9: return keykind_pad_9;
			case s3eKeyNumPadPlus: return keykind_pad_plus;
			case s3eKeyNumPadMinus: return keykind_pad_minus;
			case s3eKeyNumPadEnter: return keykind_pad_enter;
			case s3eKeyRSK: return keykind_soft_right;
			case s3eKeyLSK: return keykind_soft_left;
			case s3eKeyLS: return keykind_shoulder_left;
			case s3eKeyRS: return keykind_shoulder_right;
			case s3eKeyHash: return keykind_sharp;
			case s3eKeyStar: return keykind_star;
			case s3eKeyOk: return keykind_ok;
			case s3eKeyCLR: return keykind_clr;
			case s3eKeyVolUp: return keykind_vol_up;
			case s3eKeyVolDown: return keykind_vol_down;
			}
			return keykind_null;
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func int OnKeyboard(void* systemData, void* userData)
		{
			s3eKeyboardEvent* EventData = (s3eKeyboardEvent*) systemData;
			sysevent Event;
			Event.type = syseventtype_key;
			Event.key.type = (EventData->m_Pressed)? syskeytype_down : syskeytype_up;
			Event.key.code = GetKeyKind(EventData->m_Key);
			BxScene::__AddEvent__(Event, (EventData->m_Pressed)? syseventset_do_enable : syseventset_need_enable);
			return 0;
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func int OnPointerMotion(void* systemData, void* userData)
		{
			s3ePointerMotionEvent* EventData = (s3ePointerMotionEvent*) systemData;
			sysevent Event;
			Event.type = syseventtype_touch;
			Event.touch.type = systouchtype_move;
			Event.touch.id = 0;
			Event.touch.x = EventData->m_x - GetCurrentGUIMargin().l;
			Event.touch.y = EventData->m_y - GetCurrentGUIMargin().t;
			Event.touch.x = Event.touch.x * BxCore::Surface::GetWidth() / BxCore::Surface::GetWidthHW();
			Event.touch.y = Event.touch.y * BxCore::Surface::GetHeight() / BxCore::Surface::GetHeightHW();
			BxScene::__AddEvent__(Event, syseventset_need_enable);
			return 0;
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func int OnPointer(void* systemData, void* userData)
		{
			s3ePointerEvent* EventData = (s3ePointerEvent*) systemData;
			sysevent Event;
			Event.type = syseventtype_touch;
			Event.touch.type = (EventData->m_Pressed)? systouchtype_down : systouchtype_up;
			Event.touch.id = 0;
			Event.touch.x = EventData->m_x - GetCurrentGUIMargin().l;
			Event.touch.y = EventData->m_y - GetCurrentGUIMargin().t;
			Event.touch.x = Event.touch.x * BxCore::Surface::GetWidth() / BxCore::Surface::GetWidthHW();
			Event.touch.y = Event.touch.y * BxCore::Surface::GetHeight() / BxCore::Surface::GetHeightHW();
			BxScene::__AddEvent__(Event, (EventData->m_Pressed)? syseventset_do_enable : syseventset_need_enable);
			// 모션이벤트 설정/해제
			if(EventData->m_Pressed)
				s3ePointerRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion, nullptr);
			else s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion);
			return 0;
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func int OnPointerMotion_Multi(void* systemData, void* userData)
		{
			s3ePointerTouchMotionEvent* EventData = (s3ePointerTouchMotionEvent*) systemData;
			sysevent Event;
			Event.type = syseventtype_touch;
			Event.touch.type = systouchtype_move;
			Event.touch.id = EventData->m_TouchID;
			Event.touch.x = EventData->m_x - GetCurrentGUIMargin().l;
			Event.touch.y = EventData->m_y - GetCurrentGUIMargin().t;
			Event.touch.x = Event.touch.x * BxCore::Surface::GetWidth() / BxCore::Surface::GetWidthHW();
			Event.touch.y = Event.touch.y * BxCore::Surface::GetHeight() / BxCore::Surface::GetHeightHW();
			BxScene::__AddEvent__(Event, syseventset_need_enable);
			return 0;
		}
		/// @endcond

		/// @cond SECTION_NAME
		local_func int OnPointer_Multi(void* systemData, void* userData)
		{
			global_data bool IsPressed[S3E_POINTER_TOUCH_MAX] = {false,};
			s3ePointerTouchEvent* EventData = (s3ePointerTouchEvent*) systemData;
			sysevent Event;
			Event.type = syseventtype_touch;
			Event.touch.type = (EventData->m_Pressed)? systouchtype_down : systouchtype_up;
			Event.touch.id = EventData->m_TouchID;
			Event.touch.x = EventData->m_x - GetCurrentGUIMargin().l;
			Event.touch.y = EventData->m_y - GetCurrentGUIMargin().t;
			Event.touch.x = Event.touch.x * BxCore::Surface::GetWidth() / BxCore::Surface::GetWidthHW();
			Event.touch.y = Event.touch.y * BxCore::Surface::GetHeight() / BxCore::Surface::GetHeightHW();
			BxScene::__AddEvent__(Event, (EventData->m_Pressed)? syseventset_do_enable : syseventset_need_enable);
			// 모션이벤트 설정/해제
			IsPressed[EventData->m_TouchID] = (EventData->m_Pressed != 0);
			uint PressCount = 0;
			for(int i = 0; i < S3E_POINTER_TOUCH_MAX; ++i)
				if(IsPressed[i]) ++PressCount;
			if(EventData->m_Pressed && PressCount == 1)
				s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMotion_Multi, nullptr);
			if(!EventData->m_Pressed && PressCount == 0)
				s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMotion_Multi);
			return 0;
		}
		/// @endcond
	}

	namespace System
	{
		// 윈도우 제어
		/// @cond SECTION_NAME
		#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
			typedef struct {uint dwLowDateTime; uint dwHighDateTime;} FILETIME, *PFILETIME, *LPFILETIME;
			typedef struct {int x; int y;} POINT, *LPPOINT;
			typedef struct {int left; int top; int right; int bottom;} RECT, *LPRECT;
			typedef void (__stdcall *WindowsLib_GetSystemTimeAsFileTime)(LPFILETIME);
			typedef int (__stdcall *WindowsLib_FileTimeToLocalFileTime)(const FILETIME*, LPFILETIME);
			typedef uint (__stdcall *WindowsLib_GetFocus)(void);
			typedef uint (__stdcall *WindowsLib_FindWindowA)(uint, string);
			typedef int (__stdcall *WindowsLib_GetWindowLongA)(uint, int);
			typedef int (__stdcall *WindowsLib_SetWindowLongA)(uint, int, int);
			typedef uint (__stdcall *WindowsLib_GetClassLongA)(uint, int);
			typedef uint (__stdcall *WindowsLib_SetClassLongA)(uint, int, int);
			typedef uint (__stdcall *WindowsLib_SetMenu)(uint, uint);
			typedef uint (__stdcall *WindowsLib_MoveWindow)(uint, int, int, int, int, uint);
			typedef uint (__stdcall *WindowsLib_GetWindowRect)(uint, LPRECT);
			typedef uint (__stdcall *WindowsLib_GetCursorPos)(LPPOINT);
			typedef int (__stdcall *WindowsLib_GetSystemMetrics)(int);
			typedef int (__stdcall *WindowsLib_SendMessageA)(uint, uint, short, int);
			typedef int (__stdcall *WindowsLib_ShowWindow)(uint, int);
			local_data WindowsLib_GetSystemTimeAsFileTime BxDLL_GetSystemTimeAsFileTime = nullptr;
			local_data WindowsLib_FileTimeToLocalFileTime BxDLL_FileTimeToLocalFileTime = nullptr;
			local_data WindowsLib_GetFocus BxDLL_GetFocus = nullptr;
			local_data WindowsLib_FindWindowA BxDLL_FindWindowA = nullptr;
			local_data WindowsLib_GetWindowLongA BxDLL_GetWindowLongA = nullptr;
			local_data WindowsLib_SetWindowLongA BxDLL_SetWindowLongA = nullptr;
			local_data WindowsLib_GetClassLongA BxDLL_GetClassLongA = nullptr;
			local_data WindowsLib_SetClassLongA BxDLL_SetClassLongA = nullptr;
			local_data WindowsLib_SetMenu BxDLL_SetMenu = nullptr;
			local_data WindowsLib_MoveWindow BxDLL_MoveWindow = nullptr;
			local_data WindowsLib_GetWindowRect BxDLL_GetWindowRect = nullptr;
			local_data WindowsLib_GetCursorPos BxDLL_GetCursorPos = nullptr;
			local_data WindowsLib_GetSystemMetrics BxDLL_GetSystemMetrics = nullptr;
			local_data WindowsLib_SendMessageA BxDLL_SendMessageA = nullptr;
			local_data WindowsLib_ShowWindow BxDLL_ShowWindow = nullptr;
			local_data uint BxDLL_WindowHandle = 0;
			local_func bool LoadWindowsLibMain()
			{
				id_library WindowsLib_Kernel32 = BxCore::Library::Open("kernel32.dll");
				BxDLL_GetSystemTimeAsFileTime = (WindowsLib_GetSystemTimeAsFileTime) BxCore::Library::Link(WindowsLib_Kernel32, "GetSystemTimeAsFileTime");
				BxDLL_FileTimeToLocalFileTime = (WindowsLib_FileTimeToLocalFileTime) BxCore::Library::Link(WindowsLib_Kernel32, "FileTimeToLocalFileTime");
				id_library WindowsLib_User32 = BxCore::Library::Open("user32.dll");
				BxDLL_GetFocus = (WindowsLib_GetFocus) BxCore::Library::Link(WindowsLib_User32, "GetFocus");
				BxDLL_FindWindowA = (WindowsLib_FindWindowA) BxCore::Library::Link(WindowsLib_User32, "FindWindowA");
				BxDLL_GetWindowLongA = (WindowsLib_GetWindowLongA) BxCore::Library::Link(WindowsLib_User32, "GetWindowLongA");
				BxDLL_SetWindowLongA = (WindowsLib_SetWindowLongA) BxCore::Library::Link(WindowsLib_User32, "SetWindowLongA");
				BxDLL_GetClassLongA = (WindowsLib_GetClassLongA) BxCore::Library::Link(WindowsLib_User32, "GetClassLongA");
				BxDLL_SetClassLongA = (WindowsLib_SetClassLongA) BxCore::Library::Link(WindowsLib_User32, "SetClassLongA");
				BxDLL_SetMenu = (WindowsLib_SetMenu) BxCore::Library::Link(WindowsLib_User32, "SetMenu");
				BxDLL_MoveWindow = (WindowsLib_MoveWindow) BxCore::Library::Link(WindowsLib_User32, "MoveWindow");
				BxDLL_GetWindowRect = (WindowsLib_GetWindowRect) BxCore::Library::Link(WindowsLib_User32, "GetWindowRect");
				BxDLL_GetCursorPos = (WindowsLib_GetCursorPos) BxCore::Library::Link(WindowsLib_User32, "GetCursorPos");
				BxDLL_GetSystemMetrics = (WindowsLib_GetSystemMetrics) BxCore::Library::Link(WindowsLib_User32, "GetSystemMetrics");
				BxDLL_SendMessageA = (WindowsLib_SendMessageA) BxCore::Library::Link(WindowsLib_User32, "SendMessageA");
				BxDLL_ShowWindow = (WindowsLib_ShowWindow) BxCore::Library::Link(WindowsLib_User32, "ShowWindow");
				// 시뮬레이터 재조정
				BxDLL_WindowHandle = BxDLL_FindWindowA(0, "Marmalade- x86 Simulator");
				if(!BxDLL_WindowHandle) BxDLL_WindowHandle = BxDLL_FindWindowA(0, "BxSimulator");
				if(BxDLL_WindowHandle)
				{
					// 스타일조정
					const int GWL_STYLE = -16;
					const int GWL_EXSTYLE = -20;
					const int GCL_STYLE = -26;
					const int WS_CAPTION = 0x00C00000L;
					const int WS_THICKFRAME = 0x00040000L;
					const int WS_EX_TOPMOST = 0x00000008L;
					const int CS_DROPSHADOW = 0x00020000;
					BxDLL_SetWindowLongA(BxDLL_WindowHandle, GWL_STYLE, BxDLL_GetWindowLongA(BxDLL_WindowHandle, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
					BxDLL_SetWindowLongA(BxDLL_WindowHandle, GWL_EXSTYLE, WS_EX_TOPMOST);
					BxDLL_SetClassLongA(BxDLL_WindowHandle, GCL_STYLE, BxDLL_GetClassLongA(BxDLL_WindowHandle, GCL_STYLE) | CS_DROPSHADOW);
					BxDLL_SetMenu(BxDLL_WindowHandle, 0);
					// 위치조정
					const int SM_CXMAXIMIZED = 61;
					const int SM_CYMAXIMIZED = 62;
					const int SM_CXFRAME = 32;
					const int SM_CYFRAME = 33;
					const int ScreenWidth = BxDLL_GetSystemMetrics(SM_CXMAXIMIZED) - 2 * BxDLL_GetSystemMetrics(SM_CXFRAME);
					const int ScreenHeight = BxDLL_GetSystemMetrics(SM_CYMAXIMIZED) - 2 * BxDLL_GetSystemMetrics(SM_CYFRAME);
					BxCore::System::SetSimulatorWindowPos(
						(ScreenWidth - BxCore::Surface::GetWidthHW(false)) / 2,
						(ScreenHeight - BxCore::Surface::GetHeightHW(false)) / 2);
				}
				return true;
			}
			local_data bool UnknownA = LoadWindowsLibMain();
		#endif
		/// @endcond

		// JNI 제어
		/// @cond SECTION_NAME
		template<int ID>
		class CallbackListJNI
		{
			local_func string_rw ClassPath() {global_data char Name[256] = {'\0',}; return Name;}
			local_func string_rw MethodName() {global_data char Name[256] = {'\0',}; return Name;}

			local_func callback_jni& MethodJNI() {global_data callback_jni MethodJNI = nullptr; return MethodJNI;}

			local_func jint CallbackMethod(JNIEnv* ENV, jobject OBJ, jstring param1_str1024, jint param2)
			{
				if(!MethodJNI()) return -90001;
				wchar temp_UTF16[1024];
				char result_str1024[1024];
				jboolean IsCopy = false;
				string UTFChars = ENV->GetStringUTFChars(param1_str1024, &IsCopy);
				BxUtil::UTF8ToUTF16(UTFChars, -1, temp_UTF16);
				ENV->ReleaseStringUTFChars(param1_str1024, UTFChars);
				BxUtil::UTF16ToCP949(temp_UTF16, -1, result_str1024, 1024);
				return (jint) MethodJNI()(result_str1024, (int) param2);
			}
		public:
			local_func bool GetClass(string classpath, string methodname)
			{
				if(ClassPath()[0] == '\0' || MethodName()[0] == '\0')
				{
					BxUtilGlobal::StrCpy(ClassPath(), classpath);
					BxUtilGlobal::StrCpy(MethodName(), methodname);
					return true;
				}
				return (BxUtilGlobal::StrCmp(ClassPath(), classpath) == same && BxUtilGlobal::StrCmp(MethodName(), methodname) == same);
			}

			local_func int SetRegister(JNIEnv* env, jclass jclass, string methodname, callback_jni methodCB)
			{
				MethodJNI() = methodCB;
				global_data const JNINativeMethod JMETHOD = {methodname, "(Ljava/lang/String;I)I", (void*) &CallbackMethod};
				if(env->RegisterNatives(jclass, &JMETHOD, 1))
					return -90004;
				return 0;
			}
		};
		/// @endcond
	}

	namespace Surface
	{
		/// @cond SECTION_NAME
		local_func inline const bool IsOSIPhone() {global_data const bool OSResult = (BxUtilGlobal::StrICmp(System::GetOSName(), "IPHONE") == same); return OSResult;}
		local_func inline const bool IsOSAndroid() {global_data const bool OSResult = (BxUtilGlobal::StrICmp(System::GetOSName(), "ANDROID") == same); return OSResult;}
		local_func inline const bool IsOSWindows() {global_data const bool OSResult = (BxUtilGlobal::StrICmp(System::GetOSName(), "WINDOWS") == same); return OSResult;}
		local_func inline bool& IsOffscreenSurfaceValid() {global_data bool IsValid = false; return IsValid;}
		local_func inline CIwGxSurface& OffscreenSurface() {global_data CIwGxSurface Surface; return Surface;}
		local_func void SetRenderMode(rendermode mode)
		{
			global_data rendermode LastRenderMode = rendermode_null;
			if(LastRenderMode != mode)
			{
				#ifdef __BX_OPENGL
					if(LastRenderMode != rendermode_null)
						IwGxFlush();
					if(mode == rendermode_2d)
						IwGxSetSortMode(IW_GX_SORT_NONE);
					else if(mode == rendermode_3d)
					{
						IwGxSetSortMode(IW_GX_SORT_NONE);
						IwGxClear(IW_GX_DEPTH_BUFFER_F);
					}
				#endif
				LastRenderMode = mode;
			}
		}
		/// @endcond
	}

	namespace Sound
	{
		/// @cond SECTION_NAME
		class SoundData : public BxAutoBuffer<autobuffer_sound>
		{
		private:
			bool IsAudio;
			string_rw FileName;
			int FileSize;
		public:
			short* _ref_ WaveData;
			uint WaveLength;
			uint SamplesPerSec;

		private:
			class Initializer
			{
			public:
				Initializer()
				{
					const int SoundCacheSize = BxCore::System::GetConfigNumber("Bx.Memory.Cache.Sound", 1024 * 1024 * 3);
					BufferPoolSize() = SoundCacheSize;
					SetMusicVolume(100);
					SetEffectVolume(100);
					for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
						System::AddCallback(EffectChannelCB, i, nullptr);
				}
				virtual ~Initializer() {}
				local_func void Run() {global_data Initializer Init;}
			};

			struct RIFFHeader
			{
				uint typeID;
				uint length;
				uint subTypeID;
			};
			struct RIFFChunkHeader
			{
				uint typeID;
				uint length;
			};
			struct WAVEFormatChunk
			{
				ushort formatTag;
				ushort channels;
				uint samplesPerSec;
				uint avgBytesPerSec;
				ushort blockAlign;
				ushort bitsPerSample;
			};
			struct WAVEFormatChunkADPCM
			{
				ushort formatTag;
				ushort channels;
				uint samplesPerSec;
				uint avgBytesPerSec;
				ushort blockAlign;
				ushort bitsPerSample;
				ushort cbSize;
				ushort samplesPerBlock;
			};
			void LoadWave(const byte* sounddata, uint length)
			{
				if(((RIFFHeader*) sounddata)->typeID != VAL4('R', 'I', 'F', 'F') || ((RIFFHeader*) sounddata)->subTypeID != VAL4('W', 'A', 'V', 'E'))
					return;
				const byte* DataEnd = sounddata + length;
				sounddata += sizeof(RIFFHeader);
				while(sounddata + sizeof(RIFFChunkHeader) <= DataEnd)
				{
					const byte* NextData = sounddata + sizeof(RIFFChunkHeader) + ((RIFFChunkHeader*) sounddata)->length;
					switch(((RIFFChunkHeader*) sounddata)->typeID)
					{
					case VAL4('f', 'm', 't', ' '):
						sounddata += sizeof(RIFFChunkHeader);
						if(((WAVEFormatChunkADPCM*) sounddata)->bitsPerSample == 16)
							SamplesPerSec = ((WAVEFormatChunkADPCM*) sounddata)->samplesPerSec;
						break;
					case VAL4('d', 'a', 't', 'a'):
						WaveData = (short*)(sounddata + sizeof(RIFFChunkHeader));
						WaveLength = ((RIFFChunkHeader*) sounddata)->length;
						break;
					case VAL4('f', 'a', 'c', 't'):
						sounddata += sizeof(RIFFChunkHeader);
						WaveLength = *((uint32*) sounddata);
						break;
					default:
						break;
					}
					sounddata = NextData;
				}
			}

		public:
			SoundData(string filename, bool audio, bool autoload)
			{
				Initializer::Run();
				IsAudio = audio;
				FileName = BxUtilGlobal::StrCpyWithAlloc(filename);
				FileSize = (int) s3eFileGetFileInt(FileName, S3E_FILE_SIZE);
				WaveData = nullptr;
				WaveLength = 0;
				SamplesPerSec = s3eSoundGetInt(S3E_SOUND_DEFAULT_FREQ);
				if(!autoload)
				{
					id_file File = File::Open(FileName, "rb");
					BxASSERT("BxCore::Sound<존재하지 않는 파일입니다>", File);
					File::Read(File, Alloc(FileSize, false, false), FileSize);
					File::Close(File);
					LoadWave((const byte*) GetBuffer(), FileSize);
				}
			}

			virtual ~SoundData()
			{
				FileName = BxUtilGlobal::StrFree(FileName);
			}

			short* LockSoundData()
			{
				if(!GetBuffer())
				{
					id_file File = File::Open(FileName, "rb");
					BxASSERT("BxCore::Sound<존재하지 않는 파일입니다>", File);
					File::Read(File, Alloc(FileSize, true, true), FileSize);
					File::Close(File);
					LoadWave((const byte*) GetBuffer(), FileSize);
				}
				else Lock();
				return (short*) GetBuffer();
			}

			inline void UnlockSoundData()
			{
				Unlock();
			}

			inline short* GetSoundData()
			{
				return (IsAudio)? (short*) GetBuffer() : WaveData;
			}

			inline int GetSoundDataSize()
			{
				return ((IsAudio)? FileSize : WaveLength);
			}

			inline int GetSoundDataCount()
			{
				return ((IsAudio)? FileSize : WaveLength) / sizeof(short);
			}

			inline bool IsSoundDataByAudio()
			{
				return IsAudio;
			}

			local_func s3eResult EffectChannelPlay(int channel, id_sound sound)
			{
				ChannelState(channel, false) = sound;
				ChannelState(channel, true) = nullptr;
				BxASSERT("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !((SoundData*) sound)->IsSoundDataByAudio());
				((SoundData*) sound)->LockSoundData();
				s3eSoundChannelSetInt(channel, S3E_CHANNEL_RATE, ((SoundData*) sound)->SamplesPerSec);
				return s3eSoundChannelPlay(channel, ((SoundData*) sound)->GetSoundData(), ((SoundData*) sound)->GetSoundDataCount(), 1, 0);
			}

			local_func void EffectChannelPlayReserve(int channel, id_sound sound)
			{
				SoundData* Sound = (SoundData*) ChannelState(channel, true);
				if(Sound)
				{
					BxASSERT("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !Sound->IsSoundDataByAudio());
					Sound->LockSoundData();
					s3eSoundChannelSetInt(channel, S3E_CHANNEL_RATE, Sound->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(channel, Sound->GetSoundData(), Sound->GetSoundDataCount(), 1, 0);
					BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				ChannelState(channel, false) = ChannelState(channel, true);
				ChannelState(channel, true) = sound;
			}

			local_func s3eResult EffectChannelStop(int channel)
			{
				SoundData* CurSound = (SoundData*) SoundData::ChannelState(channel, false);
				if(CurSound) CurSound->UnlockSoundData();
				ChannelState(channel, false) = nullptr;
				ChannelState(channel, true) = nullptr;
				return s3eSoundChannelStop(channel);
			}

			local_func id_sound& ChannelState(int channel, bool isReserve)
			{
				global_data id_sound Channel[32][2] = {{nullptr,},};
				return Channel[channel][isReserve];
			}

			local_func bool EffectChannelCB(int value, void*)
			{
				// 확인
				const int Channel = value;
				SoundData* CurSound = (SoundData*) SoundData::ChannelState(Channel, false);
				if(!CurSound) return true;
				if(s3eSoundChannelGetInt(Channel, S3E_CHANNEL_PAUSED)) return true;
				if(s3eSoundChannelGetInt(Channel, S3E_CHANNEL_STATUS)) return true;
				CurSound->UnlockSoundData();
				SoundData::ChannelState(Channel, false) = nullptr;
				// 처리
				SoundData* Sound = (SoundData*) ChannelState(Channel, true);
				if(Sound)
				{
					BxASSERT("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !Sound->IsSoundDataByAudio());
					Sound->LockSoundData();
					s3eSoundChannelSetInt(Channel, S3E_CHANNEL_RATE, Sound->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(Channel, Sound->GetSoundData(), Sound->GetSoundDataCount(), 1, 0);
					BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				ChannelState(Channel, false) = ChannelState(Channel, true);
				ChannelState(Channel, true) = nullptr;
				return true;
			}

			local_func inline int GetEffectChannelBegin()
			{
				return 1;
			}

			local_func inline int GetChannelCount()
			{
				global_data const int ChannelCount = s3eSoundGetInt(S3E_SOUND_NUM_CHANNELS);
				return ChannelCount;
			}

			local_func int GetFreeEffectChannel()
			{
				for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
					if(ChannelState(i, false) == nullptr && ChannelState(i, true) == nullptr)
						return i;
				return 0;
			}

			local_func int GetSameEffectChannel(id_sound sound)
			{
				for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
					if(ChannelState(i, false) == sound && ChannelState(i, true) == nullptr)
						return i;
				for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
					if(ChannelState(i, true) == sound)
						return i;
				return 0;
			}

			local_func int GetImminentEffectChannel()
			{
				int iBest = 1;
				bool IsReserveBest = true;
				int ValueBest = 0x7FFFFFFF;
				for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
				{
					SoundData* Sound1 = (SoundData*) ChannelState(i, false);
					SoundData* Sound2 = (SoundData*) ChannelState(i, false);
					bool IsReserve = (Sound2 != nullptr);
					int Value = ((Sound1)? Sound1->GetSoundDataSize() : 0) + ((Sound2)? Sound2->GetSoundDataSize() : 0);
					if(IsReserve < IsReserveBest || ((IsReserve == IsReserveBest) && Value < ValueBest))
					{
						iBest = i;
						IsReserveBest = IsReserve;
						ValueBest = Value;
					}
				}
				return iBest;
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class PanoramaList
		{
			id_sound Sound;
			PanoramaList* Next;
		public:
			enum Command {None, RemoveAll, GoNext, GoFirst};

			PanoramaList()
			{
				Sound = nullptr;
				Next = this;
				System::AddCallback(MusicChannelCB, 0, nullptr);
			}

			virtual ~PanoramaList()
			{
				RemoveList();
			}

			void RemoveList()
			{
				PanoramaList* Focus = Next;
				while(Focus != this)
				{
					PanoramaList* NextFocus = Focus->Next;
					BxFree(Focus);
					Focus = NextFocus;
				}
				Next = this;
			}

			int GetLength()
			{
				int Length = 0;
				PanoramaList* Focus = this;
				while((Focus = Focus->Next) != this)
					++Length;
				return Length;
			}

			void AddNext(id_sound sound)
			{
				PanoramaList* NewData = (PanoramaList*) BxAlloc(sizeof(PanoramaList));
				NewData->Sound = sound;
				NewData->Next = Next;
				Next = NewData;
			}

			local_func PanoramaList* Focus(Command cmd = None)
			{
				global_data PanoramaList ListBegin;
				global_data PanoramaList* ListFocus = &ListBegin;
				switch(cmd)
				{
				case RemoveAll:
					ListBegin.RemoveList();
					ListFocus = &ListBegin;
					break;
				case GoNext:
					ListFocus = ListFocus->Next;
					break;
				case GoFirst:
					ListFocus = ListBegin.Next;
					break;
				}
				return ListFocus;
			}

			local_func bool MusicChannelCB(int, void*)
			{
				// 확인
				SoundData* CurSound = (SoundData*) SoundData::ChannelState(0, false);
				if(!CurSound) return true;
				if(CurSound->IsSoundDataByAudio())
				{
					s3eAudioStatus Status = (s3eAudioStatus) s3eAudioGetInt(S3E_AUDIO_STATUS);
					if(Status == S3E_AUDIO_PAUSED) return true;
					if(Status == S3E_AUDIO_PLAYING) return true;
				}
				else
				{
					if(s3eSoundChannelGetInt(0, S3E_CHANNEL_PAUSED)) return true;
					if(s3eSoundChannelGetInt(0, S3E_CHANNEL_STATUS)) return true;
				}
				CurSound->UnlockSoundData();
				SoundData::ChannelState(0, false) = nullptr;
				// 처리
				if(!Focus(GoNext)->Sound) Focus(GoNext);
				if(Focus()->Sound)
				{
					SoundData::ChannelState(0, false) = Focus()->Sound;
					SoundData* CurSound = (SoundData*) Focus()->Sound;
					if(CurSound)
					{
						if(CurSound->IsSoundDataByAudio()) s3eAudioStop();
						else s3eSoundChannelStop(0);
					}
					CurSound->LockSoundData();
					if(CurSound->IsSoundDataByAudio())
					{
						s3eResult Result = s3eAudioPlayFromBuffer(CurSound->GetSoundData(), CurSound->GetSoundDataSize(), 1);
						BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
					}
					else
					{
						s3eSoundChannelSetInt(0, S3E_CHANNEL_RATE, CurSound->SamplesPerSec);
						s3eResult Result = s3eSoundChannelPlay(0, CurSound->GetSoundData(), CurSound->GetSoundDataCount(), 1, 0);
						BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
					}
				}
				return true;
			}
		};
		/// @endcond
	}

	namespace Socket
	{
		// 네트워크 제어
		/// @cond SECTION_NAME
		#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
			#define WSADESCRIPTION_LEN 256
			#define WSASYS_STATUS_LEN 128
			#define WSAEFAULT 10014L
			#define WSAEWOULDBLOCK 10035L
			#define WSAEINPROGRESS 10036L
			#define WSAENOTSOCK 10038L
			#define WSAECONNABORTED 10053L
			#define WSAECONNRESET 10054L
			#define WSAENOTCONN 10057L
			#define WSAESHUTDOWN 10058L
			typedef uint SOCKET;
			#define INVALID_SOCKET (SOCKET)(~0)
			#define SOCKET_ERROR (-1)
			#define SD_RECEIVE 0x00
			#define SD_SEND 0x01
			#define SD_BOTH 0x02
			#define IOCPARM_MASK (0x7f)
			#define IOC_IN (0x80000000)
			#define _IOW(x, y, t) (IOC_IN | (((int) sizeof(t) & IOCPARM_MASK) << 16) | ((x) << 8) | (y))
			#define FIONBIO (_IOW('f', 126, uint))
			typedef struct
			{
				uint fd_count;
				SOCKET fd_array[64];
			} BxDLL_fd_set;
			typedef struct
			{
				int tv_sec;
				int tv_usec;
			} BxDLL_timeval;
			typedef struct
			{
				ushort wVersion;
				ushort wHighVersion;
				char szDescription[WSADESCRIPTION_LEN+1];
				char szSystemStatus[WSASYS_STATUS_LEN+1];
				ushort iMaxSockets;
				ushort iMaxUdpDg;
				char* lpVendorInfo;
			} WSADATA, *LPWSADATA;
			typedef struct {ushort sa_family; char sa_data[14];} SOCKADDR, *LPSOCKADDR;
			typedef struct {union {struct {byte s_b1, s_b2, s_b3, s_b4;} S_un_b; struct {ushort s_w1, s_w2;} S_un_w; uint S_addr;} S_un;} IN_ADDR;
			typedef struct {ushort sin_family; ushort sin_port; IN_ADDR sin_addr; char sin_zero[8];} SOCKADDR_IN;
			typedef int (__stdcall *NetworkLib_WSAStartup)(ushort, LPWSADATA);
			typedef int (__stdcall *NetworkLib_WSACleanup)(void);
			typedef int (__stdcall *NetworkLib_WSAGetLastError)(void);
			typedef SOCKET (__stdcall *NetworkLib_socket)(int, int, int);
			typedef int (__stdcall *NetworkLib_closesocket)(SOCKET);
			typedef int (__stdcall *NetworkLib_connect)(SOCKET, LPSOCKADDR, int);
			typedef int (__stdcall *NetworkLib_shutdown)(SOCKET, int);
			typedef int (__stdcall *NetworkLib_send)(SOCKET, string, int, int);
			typedef int (__stdcall *NetworkLib_recv)(SOCKET, string_rw, int, int);
			typedef int (__stdcall *NetworkLib_sendto)(SOCKET, string, int, int, LPSOCKADDR, int);
			typedef int (__stdcall *NetworkLib_recvfrom)(SOCKET, string_rw, int, int, LPSOCKADDR, int*);
			typedef int (__stdcall *NetworkLib_setsockopt)(SOCKET, int, int, string, int);
			typedef int (__stdcall *NetworkLib_getsockopt)(SOCKET, int, int, char*, int*);
			typedef int (__stdcall *NetworkLib_ioctlsocket)(SOCKET, int, uint*);
			typedef int (__stdcall *NetworkLib_select)(int, BxDLL_fd_set*, BxDLL_fd_set*, BxDLL_fd_set*, BxDLL_timeval*);
			typedef int (__stdcall *NetworkLib_WSAFDIsSet)(SOCKET, BxDLL_fd_set*);
			local_data NetworkLib_WSAStartup BxDLL_WSAStartup = nullptr;
			local_data NetworkLib_WSACleanup BxDLL_WSACleanup = nullptr;
			local_data NetworkLib_WSAGetLastError BxDLL_WSAGetLastError = nullptr;
			local_data NetworkLib_socket BxDLL_socket = nullptr;
			local_data NetworkLib_closesocket BxDLL_closesocket = nullptr;
			local_data NetworkLib_connect BxDLL_connect = nullptr;
			local_data NetworkLib_shutdown BxDLL_shutdown = nullptr;
			local_data NetworkLib_send BxDLL_send = nullptr;
			local_data NetworkLib_recv BxDLL_recv = nullptr;
			local_data NetworkLib_sendto BxDLL_sendto = nullptr;
			local_data NetworkLib_recvfrom BxDLL_recvfrom = nullptr;
			local_data NetworkLib_setsockopt BxDLL_setsockopt = nullptr;
			local_data NetworkLib_getsockopt BxDLL_getsockopt = nullptr;
			local_data NetworkLib_ioctlsocket BxDLL_ioctlsocket = nullptr;
			local_data NetworkLib_select BxDLL_select = nullptr;
			local_data NetworkLib_WSAFDIsSet BxDLL_WSAFDIsSet = nullptr;
			local_func bool LoadNetworkLibMain()
			{
				id_library NetworkLib_WS2_32 = BxCore::Library::Open("ws2_32.dll");
				BxDLL_WSAStartup = (NetworkLib_WSAStartup) BxCore::Library::Link(NetworkLib_WS2_32, "WSAStartup");
				BxDLL_WSACleanup = (NetworkLib_WSACleanup) BxCore::Library::Link(NetworkLib_WS2_32, "WSACleanup");
				BxDLL_WSAGetLastError = (NetworkLib_WSAGetLastError) BxCore::Library::Link(NetworkLib_WS2_32, "WSAGetLastError");
				BxDLL_socket = (NetworkLib_socket) BxCore::Library::Link(NetworkLib_WS2_32, "socket");
				BxDLL_closesocket = (NetworkLib_closesocket) BxCore::Library::Link(NetworkLib_WS2_32, "closesocket");
				BxDLL_connect = (NetworkLib_connect) BxCore::Library::Link(NetworkLib_WS2_32, "connect");
				BxDLL_shutdown = (NetworkLib_shutdown) BxCore::Library::Link(NetworkLib_WS2_32, "shutdown");
				BxDLL_send = (NetworkLib_send) BxCore::Library::Link(NetworkLib_WS2_32, "send");
				BxDLL_recv = (NetworkLib_recv) BxCore::Library::Link(NetworkLib_WS2_32, "recv");
				BxDLL_sendto = (NetworkLib_sendto) BxCore::Library::Link(NetworkLib_WS2_32, "sendto");
				BxDLL_recvfrom = (NetworkLib_recvfrom) BxCore::Library::Link(NetworkLib_WS2_32, "recvfrom");
				BxDLL_setsockopt = (NetworkLib_setsockopt) BxCore::Library::Link(NetworkLib_WS2_32, "setsockopt");
				BxDLL_getsockopt = (NetworkLib_getsockopt) BxCore::Library::Link(NetworkLib_WS2_32, "getsockopt");
				BxDLL_ioctlsocket = (NetworkLib_ioctlsocket) BxCore::Library::Link(NetworkLib_WS2_32, "ioctlsocket");
				BxDLL_select = (NetworkLib_select) BxCore::Library::Link(NetworkLib_WS2_32, "select");
				BxDLL_WSAFDIsSet = (NetworkLib_WSAFDIsSet) BxCore::Library::Link(NetworkLib_WS2_32, "__WSAFDIsSet");
				return true;
			}
			local_data bool UnknownB = LoadNetworkLibMain();
		#endif
		/// @endcond

		/// @cond SECTION_NAME
		#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
			class TCPData
			{
				SOCKET Client;
				SOCKET Server;
				socketstate State;
			public:
				TCPData() : Client(INVALID_SOCKET), Server(INVALID_SOCKET), State(socketstate_null)
				{
					WSADATA WsaData;
					BxDLL_WSAStartup((2 << 8) | 2, &WsaData);
					ResetClient();
				}
				virtual ~TCPData()
				{
					Disconnect();
					BxDLL_WSACleanup();
				}
				inline socketstate GetState()
				{
					return State;
				}
				inline void SetState(socketstate state)
				{
					State = state;
				}
				inline SOCKET GetClient()
				{
					return Client;
				}
				inline void ResetClient()
				{
					Disconnect();
					Client = BxDLL_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if(Client == INVALID_SOCKET)
					{
						const int ErrorCode = BxDLL_WSAGetLastError();
						BxASSERT(BxString::Parse("<>:BxCore::Socket<<소켓생성 실패(<A>)>>", BxTHROW(ErrorCode)), false);
						State = socketstate_null;
					}
					else State = socketstate_created;
				}
				inline SOCKET GetServer()
				{
					return Server;
				}
				void SetServer(SOCKET server)
				{
					Server = server;
					if(Server != INVALID_SOCKET)
						State = socketstate_connected;
					else State = socketstate_disconnected;
				}
				inline void Disconnect()
				{
					if(Client != INVALID_SOCKET)
					{
						BxDLL_shutdown(Client, SD_BOTH);
						BxDLL_closesocket(Client);
					}
					Client = INVALID_SOCKET;
					Server = INVALID_SOCKET;
					State = socketstate_null;
				}
			};
		#else
			class TCPData
			{
				s3eSocket* Client;
				s3eSocket* Server;
				socketstate State;
			public:
				TCPData()
				{
					Client = nullptr;
					Server = nullptr;
					State = socketstate_null;
					ResetClient();
				}
				virtual ~TCPData()
				{
					Disconnect();
				}
				inline socketstate GetState()
				{
					return State;
				}
				inline void SetState(socketstate state)
				{
					State = state;
				}
				inline s3eSocket* GetClient()
				{
					return Client;
				}
				inline void ResetClient()
				{
					Disconnect();
					Client = s3eSocketCreate(S3E_SOCKET_TCP);
					if(!Client)
					{
						const int ErrorCode = (int) s3eSocketGetError();
						BxASSERT(BxString::Parse("<>:BxCore::Socket<<소켓생성 실패(<A>)>>", BxTHROW(ErrorCode)), false);
					}
					State = (Client)? socketstate_created : socketstate_null;
				}
				inline s3eSocket* GetServer()
				{
					return Server;
				}
				void SetServer(s3eSocket* server)
				{
					Server = server;
					if(Server) State = socketstate_connected;
					else State = socketstate_disconnected;
				}
				inline void Disconnect()
				{
					if(Client) s3eSocketClose(Client);
					Client = nullptr;
					Server = nullptr;
					State = socketstate_null;
				}
			};
		#endif
		/// @endcond

		/// @cond SECTION_NAME
		#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
			class ICMPData
			{
				enum ECHO:byte {ECHO_REPLY = 0, ECHO_REQUEST = 8};
				struct IPHEADER
				{
					byte ip_hl:4; // 헤더 길이
					byte ip_v:4; // 버전
					byte ip_tos; // 서비스 타입
					short ip_len; // 전체 길이
					ushort ip_id; // ID
					short ip_off; // Fragment 옵셋 필드
					byte ip_ttl; // TTL
					byte ip_p; // 프로토콜
					ushort ip_cksum; // 체크섬
					IN_ADDR ip_src; // 발송지
					IN_ADDR ip_dst; // 목적지
				};
				struct ICMPMESSAGE
				{
					ECHO icmp_type; // 메세지 타입
					byte icmp_code; // 서브 코드 타입
					ushort icmp_cksum; // 체크섬
					ushort icmp_id; // ID
					ushort icmp_seq; // 시퀀스 번호
				};
				SOCKET Server;
				uint LastTimeout;
				uhuge LastSendTime;
				ICMPMESSAGE Message;

				ushort MakeChecksum(ushort* buf, int len)
				{
					uint cksum = 0;
					ushort* ptr = buf;
					int left = len;
					while(1 < left)
					{
						cksum += *(ptr++);
						left -= sizeof(ushort);
					}
					if(left == 1)
						cksum += *((byte*) ptr);
					cksum = (cksum >> 16) + (cksum & 0xFFFF);
					cksum += (cksum >> 16);
					return (ushort) ~cksum;
				}
				bool ActivateSock(uint Timeout)
				{
					if(Server == INVALID_SOCKET)
					{
						Server = BxDLL_socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
						if(Server == INVALID_SOCKET)
						{
							const int ErrorCode = BxDLL_WSAGetLastError();
							BxASSERT(BxString::Parse("<>:BxCore::Socket<<소켓생성 실패(<A>)>>", BxTHROW(ErrorCode)), false);
						}
					}
					if(Server != INVALID_SOCKET && LastTimeout != Timeout)
					{
						LastTimeout = Timeout;
						BxDLL_setsockopt(Server, 0xffff, 0x1005, (string) &Timeout, sizeof(uint)); // SOL_SOCKET, SO_SNDTIMEO
						BxDLL_setsockopt(Server, 0xffff, 0x1006, (string) &Timeout, sizeof(uint)); // SOL_SOCKET, SO_RCVTIMEO
					}
					return (Server != INVALID_SOCKET);
				}

			public:
				ICMPData() : Server(INVALID_SOCKET), LastTimeout(0), LastSendTime(0)
				{
					WSADATA WsaData;
					BxDLL_WSAStartup((2 << 8) | 2, &WsaData);
					ActivateSock(1000);
					Message.icmp_type = ECHO_REQUEST;
					Message.icmp_code = 0;
					Message.icmp_cksum = 0;
					Message.icmp_id = (ushort) (BxCore::System::GetTimeMilliSecond() & 0xFFFF);
					Message.icmp_seq = 0;
				}
				virtual ~ICMPData()
				{
					if(Server != INVALID_SOCKET)
						BxDLL_closesocket(Server);
					BxDLL_WSACleanup();
				}
				int SendTo(uint Timeout, LPSOCKADDR SockAddr, int SockAddrLen)
				{
					if(ActivateSock(Timeout))
					{
						Message.icmp_cksum = 0;
						++Message.icmp_seq;
						Message.icmp_cksum = MakeChecksum((ushort*) &Message, sizeof(ICMPMESSAGE));
						LastSendTime = BxCore::System::GetTimeMilliSecond();
						return BxDLL_sendto(Server, (string) &Message, sizeof(ICMPMESSAGE), 0, SockAddr, SockAddrLen);
					}
					return SOCKET_ERROR;
				}
				int RecvFrom()
				{
					if(Server != INVALID_SOCKET)
					{
						byte RecvBuffer[1500];
						SOCKADDR_IN SockAddr;
						int SockAddrLen = sizeof(SOCKADDR_IN);
						int RecvValue = BxDLL_recvfrom(Server, (string_rw) RecvBuffer, 1500, 0, (LPSOCKADDR) &SockAddr, &SockAddrLen);
						if(RecvValue == SOCKET_ERROR) return SOCKET_ERROR;
						if(RecvValue < sizeof(IPHEADER)) return SOCKET_ERROR;
						IPHEADER* IPHdr = (IPHEADER*) RecvBuffer;
						const int IPHdrLen = IPHdr->ip_hl * 4;
						ICMPMESSAGE* ICMPMsg = (ICMPMESSAGE*) (RecvBuffer + IPHdrLen);
						if(RecvValue < IPHdrLen + (int) sizeof(ICMPMESSAGE)) return SOCKET_ERROR;
						if(ICMPMsg->icmp_id != Message.icmp_id) return SOCKET_ERROR;
						if(ICMPMsg->icmp_type != ECHO_REPLY) return SOCKET_ERROR;
						return (int) (BxCore::System::GetTimeMilliSecond() - LastSendTime);
					}
					return SOCKET_ERROR;
				}
			};
		#endif
		/// @endcond

		/// @cond SECTION_NAME
		#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
			int _GetErrorCode(id_socket sock)
			{
				const int ErrorCode = BxDLL_WSAGetLastError();
				if(ErrorCode == WSAEWOULDBLOCK) return 0;
				switch(ErrorCode)
				{
				case WSAEFAULT: BxTRACE("<>:##### Socket ErrorCode : WSAEFAULT #####<R><N>"); break;
				case WSAENOTSOCK: BxTRACE("<>:##### Socket ErrorCode : WSAENOTSOCK #####<R><N>"); break;
				case WSAESHUTDOWN: BxTRACE("<>:##### Socket ErrorCode : WSAESHUTDOWN #####<R><N>"); break;
				case WSAENOTCONN: BxTRACE("<>:##### Socket ErrorCode : WSAENOTCONN #####<R><N>"); break;
				case WSAECONNABORTED: BxTRACE("<>:##### Socket ErrorCode : WSAECONNABORTED #####<R><N>"); break;
				case WSAECONNRESET: BxTRACE("<>:##### Socket ErrorCode : WSAECONNRESET #####<R><N>"); break;
				default: BxTRACE("<>:##### Socket ErrorCode : <A> #####<R><N>", BxTHROW(ErrorCode)); break;
				}
				switch(ErrorCode)
				{
				case WSAEFAULT:
				case WSAENOTSOCK:
				case WSAESHUTDOWN: ((TCPData*) sock)->SetState(socketstate_null); break;
				case WSAENOTCONN:
				case WSAECONNABORTED:
				case WSAECONNRESET:
				default: ((TCPData*) sock)->SetState(socketstate_disconnected); break;
				}
				return -ErrorCode;
			}
		#else
			int32 _ConnectCB(s3eSocket* s, void* systemData, void* userData)
			{
				if(userData)
					((TCPData*) userData)->SetServer(s);
				return 0;
			}
			int _GetErrorCode(id_socket sock)
			{
				s3eSocketErrors ErrorCode = s3eSocketGetError();
				if(ErrorCode == S3E_SOCKET_ERR_WOULDBLOCK) return 0;
				switch(ErrorCode)
				{
				case S3E_SOCKET_ERR_PARAM: BxTRACE("<>:##### Socket ErrorCode : S3E_SOCKET_ERR_PARAM #####<R><N>"); break;
				case S3E_SOCKET_ERR_SHUTDOWN: BxTRACE("<>:##### Socket ErrorCode : S3E_SOCKET_ERR_SHUTDOWN #####<R><N>"); break;
				case S3E_SOCKET_ERR_NOTCONN: BxTRACE("<>:##### Socket ErrorCode : S3E_SOCKET_ERR_NOTCONN #####<R><N>"); break;
				default: BxTRACE("<>:##### Socket ErrorCode : <A> #####<R><N>", BxTHROW(ErrorCode)); break;
				}
				switch(ErrorCode)
				{
				case S3E_SOCKET_ERR_PARAM:
				case S3E_SOCKET_ERR_SHUTDOWN: ((TCPData*) sock)->SetState(socketstate_null); break;
				case S3E_SOCKET_ERR_NOTCONN:
				default: ((TCPData*) sock)->SetState(socketstate_disconnected); break;
				}
				return -ErrorCode;
			}
		#endif
		/// @endcond
	}

	namespace Font
	{
		/// @cond SECTION_NAME
		local_func int& RefCount() {global_data int Count = 0; return Count;}
		local_func CIwGxFont*& TempFont() {global_data CIwGxFont* Font = 0; return Font;} // GxFont버그회피용
		/// @endcond
	}

	namespace Thread
	{
		/// @cond SECTION_NAME
		class ThreadClass
		{
		private:
			#ifdef _WIN32
				void* ID;
			#else
				pthread_t ID;
			#endif
			callback_thread CB;
			void* Data;
		protected:
			#ifdef _WIN32
				static unsigned long __stdcall Run(void* self)
			#else
				static void* Run(void* self)
			#endif
			{
				ThreadClass* This = (ThreadClass*) self;
				if(This->CB) This->CB(This->Data);
				BxSingleton::UnbindAll(true);
				BxPoolGlobal::UnbindAll();
				BxCore::Thread::UnbindStorageAll();
				return 0;
			}
		public:
			ThreadClass(callback_thread cb, void* data) : CB(cb), Data(data)
			{
				#ifdef _WIN32
					ID = CreateThread(0, 0, Run, this, 0, 0);
				#else
					pthread_create(&ID, nullptr, Run, this);
				#endif
			}
			virtual ~ThreadClass()
			{
			}
		public:
			void Kill()
			{
				#ifdef _WIN32
					TerminateThread(ID, 0);
				#else
					pthread_cancel(ID);
				#endif
			}
			void Wait()
			{
				#ifdef _WIN32
					while(true)
					{
						DWORD Result = 0;
						GetExitCodeThread(ID, &Result);
						if(Result != STILL_ACTIVE) break;
						BxCore::System::Sleep(10);
					}
				#else
					void* Result = nullptr;
					pthread_join(ID, &Result);
				#endif
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class MutexClass
		{
		private:
			#ifdef _WIN32
				HANDLE Event;
				LONG Count;
			#else
				pthread_mutex_t ID;
			#endif
		public:
			MutexClass()
			{
				#ifdef _WIN32
					Event = CreateEvent(0, 0, 0, 0);
					Count = 0;
				#else
					pthread_mutex_init(&ID, nullptr);
				#endif
			}
			virtual ~MutexClass()
			{
				#ifdef _WIN32
					if(Event)
					{
						CloseHandle(Event);
						Event = 0;
					}
				#else
					pthread_mutex_destroy(&ID);
				#endif
			}
		public:
			void Lock()
			{
				#ifdef _WIN32
					if(1 < InterlockedIncrement(&Count))
						WaitForSingleObject(Event, INFINITE);
				#else
					pthread_mutex_lock(&ID);
				#endif
			}
			void Unlock()
			{
				#ifdef _WIN32
					if(0 < InterlockedDecrement(&Count))
						SetEvent(Event);
				#else
					pthread_mutex_unlock(&ID);
				#endif
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class StorageClass
		{
		public:
			void* Data;
		public:
			StorageClass() : Data(nullptr) {}
			~StorageClass() {BxCore::Util::Free(Data);}
			void* Init(int size)
			{
				Data = BxCore::Util::Alloc(size);
				BxCore::Util::MemSet(Data, 0, size);
				return Data;
			}
		public:
			global_func uint CurrentThreadID()
			{
				#ifdef _WIN32
					return (uint) GetCurrentThreadId();
				#else
					return (uint) (pthread_self() & 0xFFFFFFFF);
				#endif
			}
			global_func id_mutex& Mutex() {global_data id_mutex _ = OpenMutex(); return _;}
			global_func BxVarMap< BxVarMap<StorageClass> >& Map() {global_data BxVarMap< BxVarMap<StorageClass> > _; return _;}
		};
		/// @endcond
	}

	namespace OpenGL2D
	{
		// OpenGLForm
		/// @cond SECTION_NAME
		class OpenGLForm
		{
		public:
			enum OpenGLFormType {IMAGE, COLOR} Type;
			OpenGLForm(OpenGLFormType type) : Type(type) {}
			virtual ~OpenGLForm() {}
		};
		class TextureMaterial : public OpenGLForm
		{
		public:
			CIwTexture Texture;
			BxExpress::IntegerX ExpressWidth;
			BxExpress::IntegerX ExpressHeight;
			CIwMaterial Material;
			TextureMaterial() : OpenGLForm(IMAGE), ExpressWidth(BxInteger(0)), ExpressHeight(BxInteger(0)) {}
			virtual ~TextureMaterial() {}
		};
		class ColorAmbient : public OpenGLForm
		{
		public:
			color_x888 Color;
			CIwMaterial Material;
			ColorAmbient() : OpenGLForm(COLOR) {}
			virtual ~ColorAmbient() {}
		};
		/// @endcond

		// OpenGLOutline
		/// @cond SECTION_NAME
		class OpenGLOutline
		{
		public:
			enum OpenGLOutlineType {RECT, POLYGON, POLYGON3D} Type;
			OpenGLOutline(OpenGLOutlineType type) : Type(type) {}
			virtual ~OpenGLOutline() {}
		};
		class VertexUV : public OpenGLOutline
		{
		public:
			pointf Vertex[4];
			CIwSVec2 UV[4];
			VertexUV() : OpenGLOutline(RECT) {}
			virtual ~VertexUV() {}
		};
		class PolyVertexUV : public OpenGLOutline
		{
		public:
			int Count;
			pointf* Vertex;
			CIwSVec2* UV;
			PolyVertexUV(int count) : OpenGLOutline(POLYGON)
			{
				Count = count;
				Vertex = BxNew_Array(pointf, count);
				UV = BxNew_Array(CIwSVec2, count);
			}
			virtual ~PolyVertexUV()
			{
				BxDelete_Array(Vertex);
				BxDelete_Array(UV);
			}
		};
		class PolyVertexUV3D : public OpenGLOutline
		{
		public:
			int Count;
			CIwSVec3* Vertex;
			CIwSVec2* UV;
			PolyVertexUV3D(int count) : OpenGLOutline(POLYGON3D)
			{
				Count = count;
				Vertex = BxNew_Array(CIwSVec3, count);
				UV = BxNew_Array(CIwSVec2, count);
			}
			virtual ~PolyVertexUV3D()
			{
				BxDelete_Array(Vertex);
				BxDelete_Array(UV);
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class VBOCache
		{
		private:
			int PoolSize;
			int PoolFocus;
			CIwSVec2* VBOData;
			CIwGxStream* VBOPool;
		private:
			VBOCache()
			{
				PoolSize = BxCore::System::GetConfigNumber("Bx.Memory.Cache.VBO", 10000);
				PoolFocus = 0;
				VBOData = BxNew_Array(CIwSVec2, PoolSize);
				//VBOPool = BxNew_Param(CIwGxStream, CIwGxStream::SVEC2, VBOData, PoolSize);
				//VBOPool->Upload(true, false);
			}
			~VBOCache()
			{
				//VBOPool->Free();
				BxDelete(VBOData);
				//BxDelete(VBOPool);
			}
		public:
			global_func VBOCache& Pool() {global_data VBOCache VBO; return VBO;}
		public:
			void SetVertStream(CIwSVec2* Vertex, const int Length)
			{
				if(Length != 4 || PoolSize <= PoolFocus)
					IwGxSetVertStreamScreenSpace(Vertex, Length);
				else
				{
					//CIwGxStream ShareVBO(*VBOPool, CIwGxStream::SVEC2, sizeof(CIwSVec2) * PoolFocus);
					if(BxCore::Util::MemCmp(&VBOData[PoolFocus], Vertex, sizeof(CIwSVec2) * 4))
					{
						BxCore::Util::MemCpy(&VBOData[PoolFocus], Vertex, sizeof(CIwSVec2) * 4);
						//ShareVBO.Set(CIwGxStream::SVEC2, &VBOData[PoolFocus], 4, 0);
						//ShareVBO.Upload(true, false);
					}
					IwGxSetVertStreamScreenSpace(&VBOData[PoolFocus], 4);
					//IwGxSetVertStreamScreenSpace(ShareVBO);
					PoolFocus += 4;
				}
			}
			void SetUVStream(CIwSVec2* UV)
			{
				if(PoolSize <= PoolFocus)
					IwGxSetUVStream(UV, 0, 4);
				else
				{
					//CIwGxStream ShareVBO(*VBOPool, CIwGxStream::SVEC2, sizeof(CIwSVec2) * PoolFocus);
					if(BxCore::Util::MemCmp(&VBOData[PoolFocus], UV, sizeof(CIwSVec2) * 4))
					{
						BxCore::Util::MemCpy(&VBOData[PoolFocus], UV, sizeof(CIwSVec2) * 4);
						//ShareVBO.Set(CIwGxStream::SVEC2, &VBOData[PoolFocus], 4, 0);
						//ShareVBO.Upload(true, false);
					}
					IwGxSetUVStream(&VBOData[PoolFocus], 0, 4);
					//IwGxSetUVStream(ShareVBO);
					PoolFocus += 4;
				}
			}
			void Flush()
			{
				PoolFocus = 0;
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		void _RenderRect(OpenGLForm* TM, VertexUV* VUV, int x, int y, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_2d);
			// Material
			CIwMaterial* Material = nullptr;
			if(TM->Type == OpenGLForm::IMAGE) Material = &((TextureMaterial*) TM)->Material;
			else if(TM->Type == OpenGLForm::COLOR) Material = &((ColorAmbient*) TM)->Material;
			if(Material)
			{
				CIwColour& Color = Material->GetColAmbient();
				Color.a = opacity;
				Color.r = (color >> 16) & 0xFF;
				Color.g = (color >>  8) & 0xFF;
				Color.b = (color >>  0) & 0xFF;
				Material->SetColAmbient(Color);
				if(Material->GetAlphaMode() != CIwMaterial::ALPHA_BLEND)
					Material->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
				Material->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
				if(Material->GetCullMode() != CIwMaterial::CULL_NONE)
					Material->SetCullMode(CIwMaterial::CULL_NONE);
				IwGxSetMaterial(Material);
			}
			// Vertex
			CIwSVec2 Vertex[4];
			for(int i = 0; i < 4; ++i)
			{
				Vertex[i].x = FtoI(VUV->Vertex[i].x + 0x8000) + x;
				Vertex[i].y = FtoI(VUV->Vertex[i].y + 0x8000) + y;
			}
			VBOCache::Pool().SetVertStream(Vertex, 4);
			// UV
			if(TM->Type == OpenGLForm::IMAGE)
			{
				const int RateX = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetWidth() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressWidth;
				const int RateY = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetHeight() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressHeight;
				CIwSVec2 UV[4];
				for(int i = 0; i < 4; ++i)
				{
					UV[i].x = (VUV->UV[i].x - (1 << 11)) * RateX / BxIntegerX(1 << 11) + (1 << 11);
					UV[i].y = (VUV->UV[i].y - (1 << 11)) * RateY / BxIntegerX(1 << 11) + (1 << 11);
				}
				VBOCache::Pool().SetUVStream(UV);
			}
			else VBOCache::Pool().SetUVStream(VUV->UV);
			IwGxDrawPrims(IW_GX_QUAD_LIST, nullptr, 4);
		}
		/// @endcond

		/// @cond SECTION_NAME
		void _RenderPoly(OpenGLForm* TM, PolyVertexUV* PolyVUV, int x, int y, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_2d);
			// Material
			CIwMaterial* Material = nullptr;
			if(TM->Type == OpenGLForm::IMAGE) Material = &((TextureMaterial*) TM)->Material;
			else if(TM->Type == OpenGLForm::COLOR) Material = &((ColorAmbient*) TM)->Material;
			if(Material)
			{
				CIwColour& Color = Material->GetColAmbient();
				Color.a = opacity;
				Color.r = (color >> 16) & 0xFF;
				Color.g = (color >>  8) & 0xFF;
				Color.b = (color >>  0) & 0xFF;
				Material->SetColAmbient(Color);
				if(Material->GetAlphaMode() != CIwMaterial::ALPHA_BLEND)
					Material->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
				Material->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
				if(Material->GetCullMode() != CIwMaterial::CULL_NONE)
					Material->SetCullMode(CIwMaterial::CULL_NONE);
				IwGxSetMaterial(Material);
			}
			// Vertex
			CIwSVec2* Vertex = IW_GX_ALLOC(CIwSVec2, PolyVUV->Count);
			for(int i = 0; i < PolyVUV->Count; ++i)
			{
				Vertex[i].x = FtoI(PolyVUV->Vertex[i].x + 0x8000) + x;
				Vertex[i].y = FtoI(PolyVUV->Vertex[i].y + 0x8000) + y;
			}
			VBOCache::Pool().SetVertStream(Vertex, PolyVUV->Count);
			// UV
			if(TM->Type == OpenGLForm::IMAGE)
			{
				const int RateX = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetWidth() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressWidth;
				const int RateY = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetHeight() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressHeight;
				CIwSVec2* UV = IW_GX_ALLOC(CIwSVec2, PolyVUV->Count);
				for(int i = 0; i < PolyVUV->Count; ++i)
				{
					UV[i].x = (PolyVUV->UV[i].x - (1 << 11)) * RateX / BxIntegerX(1 << 11) + (1 << 11);
					UV[i].y = (PolyVUV->UV[i].y - (1 << 11)) * RateY / BxIntegerX(1 << 11) + (1 << 11);
				}
				VBOCache::Pool().SetUVStream(UV);
			}
			else VBOCache::Pool().SetUVStream(PolyVUV->UV);
			IwGxDrawPrims(IW_GX_QUAD_LIST, nullptr, PolyVUV->Count);
		}
		/// @endcond

		/// @cond SECTION_NAME
		void _RenderPoly3D(OpenGLForm* TM, PolyVertexUV3D* PolyVUV3D, int x, int y, int z, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_3d);
			// Material
			CIwMaterial* Material = nullptr;
			if(TM->Type == OpenGLForm::IMAGE) Material = &((TextureMaterial*) TM)->Material;
			else if(TM->Type == OpenGLForm::COLOR) Material = &((ColorAmbient*) TM)->Material;
			if(Material)
			{
				CIwColour& Color = Material->GetColAmbient();
				Color.a = opacity;
				Color.r = (color >> 16) & 0xFF;
				Color.g = (color >>  8) & 0xFF;
				Color.b = (color >>  0) & 0xFF;
				Material->SetColAmbient(Color);
				if(Material->GetAlphaMode() != CIwMaterial::ALPHA_NONE)
					Material->SetAlphaMode(CIwMaterial::ALPHA_NONE);
				Material->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_NORMAL);
				if(Material->GetCullMode() != CIwMaterial::CULL_FRONT)
					Material->SetCullMode(CIwMaterial::CULL_FRONT);
				IwGxSetMaterial(Material);
			}
			// Vertex
			CIwFVec3* Move = IW_GX_ALLOC(CIwFVec3, 1);
			Move[0].x = (float) x;
			Move[0].y = (float) y;
			Move[0].z = (float) z;
			CIwFMat ModelMatrix;
			ModelMatrix.SetRotX(90 * Pi() / 180 / (float) 0x10000, true, true);
			ModelMatrix.SetTrans(Move[0]);
			IwGxSetModelMatrix(&ModelMatrix);
			CIwFVec3* Vertex = IW_GX_ALLOC(CIwFVec3, PolyVUV3D->Count);
			for(int i = 0; i < PolyVUV3D->Count; ++i)
			{
				Vertex[i].x = PolyVUV3D->Vertex[i].x;
				Vertex[i].y = PolyVUV3D->Vertex[i].y;
				Vertex[i].z = PolyVUV3D->Vertex[i].z;
			}
			IwGxSetVertStreamModelSpace(Vertex, PolyVUV3D->Count);
			// UV
			if(TM->Type == OpenGLForm::IMAGE)
			{
				const int RateX = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetWidth() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressWidth;
				const int RateY = ((int)((1 << 11) * (((TextureMaterial*) TM)->Texture.GetHeight() - GetTextureMargin() * 2))) / ((TextureMaterial*) TM)->ExpressHeight;
				CIwSVec2* UV = IW_GX_ALLOC(CIwSVec2, PolyVUV3D->Count);
				for(int i = 0; i < PolyVUV3D->Count; ++i)
				{
					UV[i].x = (PolyVUV3D->UV[i].x - (1 << 11)) * RateX / BxIntegerX(1 << 11) + (1 << 11);
					UV[i].y = (PolyVUV3D->UV[i].y - (1 << 11)) * RateY / BxIntegerX(1 << 11) + (1 << 11);
				}
				IwGxSetUVStream(UV, 0, PolyVUV3D->Count);
			}
			else IwGxSetUVStream(PolyVUV3D->UV, 0, PolyVUV3D->Count);
			IwGxDrawPrims(IW_GX_QUAD_LIST, nullptr, PolyVUV3D->Count);
		}
		/// @endcond
	}
}
