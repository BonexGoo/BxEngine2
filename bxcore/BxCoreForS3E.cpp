// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxCoreForS3E.hpp>
#include <BxCore.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxAutoBuffer.hpp>
#include <BxExpress.hpp>
#include <BxKeyword.hpp>
#include <BxMemory.hpp>
#include <BxScene.hpp>
#include <BxUtil.hpp>
#include <BxString.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
namespace BxCore
{
	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Main
	{
		local_data BxDraw* Draw = nullptr;

		void EventAttach(void* drawclass)
		{
			Draw = (BxDraw*) drawclass;
			s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyboard, nullptr);
			if(BxCore::System::IsMultiTouchAvailable())
				s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, OnPointer_Multi, nullptr);
			else s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, OnPointer, nullptr);
		}

		void EventDetach()
		{
			Draw = nullptr;
			s3eKeyboardUnRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyboard);
			if(BxCore::System::IsMultiTouchAvailable())
				s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, OnPointer_Multi);
			else s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, OnPointer);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace System
	{
		bool IsQuit()
		{
			return (s3eDeviceCheckQuitRequest() == S3E_TRUE);
		}

		void DoQuit()
		{
			s3eDeviceRequestQuit();
		}

		void Sleep(int ms)
		{
			s3eDeviceYield(ms);
		}

		uhuge GetTimeMilliSecond()
		{
			return (uhuge) s3eTimerGetUTC();
		}

		uhuge GetTimeNanoSecond()
		{
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				FILETIME sys;
				__asm push EAX
				__asm push EDX
				__asm __emit 0fh __asm __emit 031h
				__asm mov sys.dwLowDateTime, EAX
				__asm mov sys.dwHighDateTime, EDX
				__asm pop EDX
				__asm pop EAX
				return *(uhuge*)(&sys);
			#else
				return s3eTimerGetUTC() * (uhuge) 10000000;
			#endif
		}

		uhuge GetTimeSimilarMicroSecond()
		{
			return GetTimeNanoSecond() >> 10;
		}

		bool IsMultiTouchAvailable()
		{
			return (s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) != 0);
		}

		bool IsTouchDown()
		{
			s3ePointerState Result = (IsMultiTouchAvailable())? s3ePointerGetTouchState(0) : s3ePointerGetState(S3E_POINTER_BUTTON_SELECT);
			return (Result == S3E_POINTER_STATE_DOWN || Result == S3E_POINTER_STATE_PRESSED);
		}

		void MakeTouchEvent(systouchtype type, int x, int y)
		{
			global_data bool IsTestIOSDispScaleFactor = false;
			global_data int IOSDispScaleFactor = 100;
			if(!IsTestIOSDispScaleFactor)
			{
				IsTestIOSDispScaleFactor = true;
				const int Value = BxCore::System::GetPlatformConfigNumber("IOSDispScaleFactor");
				if(Value != -1) IOSDispScaleFactor = Value;
			}
			x = x * IOSDispScaleFactor / BxIntegerX(100);
			y = y * IOSDispScaleFactor / BxIntegerX(100);
			if(IsMultiTouchAvailable())
			{
				if(type == systouchtype_move)
				{
					s3ePointerTouchMotionEvent EventData;
					EventData.m_TouchID = 0;
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointerMotion_Multi(&EventData, nullptr);
				}
				else if(type != systouchtype_cancel)
				{
					s3ePointerTouchEvent EventData;
					EventData.m_TouchID = 0;
					EventData.m_Pressed = (type == systouchtype_down);
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointer_Multi(&EventData, nullptr);
				}
			}
			else
			{
				if(type == systouchtype_move)
				{
					s3ePointerMotionEvent EventData;
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointerMotion(&EventData, nullptr);
				}
				else if(type != systouchtype_cancel)
				{
					s3ePointerEvent EventData;
					EventData.m_Button = S3E_POINTER_BUTTON_SELECT;
					EventData.m_Pressed = (type == systouchtype_down);
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointer(&EventData, nullptr);
				}
			}
		}

		bool IsKeyDown(keykind key)
		{
			s3eKey Key = s3eKeyFirst;
			switch(key)
			{
			case keykind_esc: Key = s3eKeyEsc; break;
			case keykind_tab: Key = s3eKeyTab; break;
			case keykind_space: Key = s3eKeySpace; break;
			case keykind_bspace: Key = s3eKeyBackspace; break;
			case keykind_enter: Key = s3eKeyEnter; break;
			case keykind_shift: Key = s3eKeyLeftShift; break;
			case keykind_ctrl: Key = s3eKeyLeftControl; break;
			case keykind_left: Key = s3eKeyLeft; break;
			case keykind_up: Key = s3eKeyUp; break;
			case keykind_right: Key = s3eKeyRight; break;
			case keykind_down: Key = s3eKeyDown; break;
			case keykind_0: Key = s3eKey0; break;
			case keykind_1: Key = s3eKey1; break;
			case keykind_2: Key = s3eKey2; break;
			case keykind_3: Key = s3eKey3; break;
			case keykind_4: Key = s3eKey4; break;
			case keykind_5: Key = s3eKey5; break;
			case keykind_6: Key = s3eKey6; break;
			case keykind_7: Key = s3eKey7; break;
			case keykind_8: Key = s3eKey8; break;
			case keykind_9: Key = s3eKey9; break;
			case keykind_a: Key = s3eKeyA; break;
			case keykind_b: Key = s3eKeyB; break;
			case keykind_c: Key = s3eKeyC; break;
			case keykind_d: Key = s3eKeyD; break;
			case keykind_e: Key = s3eKeyE; break;
			case keykind_f: Key = s3eKeyF; break;
			case keykind_g: Key = s3eKeyG; break;
			case keykind_h: Key = s3eKeyH; break;
			case keykind_i: Key = s3eKeyI; break;
			case keykind_j: Key = s3eKeyJ; break;
			case keykind_k: Key = s3eKeyK; break;
			case keykind_l: Key = s3eKeyL; break;
			case keykind_m: Key = s3eKeyM; break;
			case keykind_n: Key = s3eKeyN; break;
			case keykind_o: Key = s3eKeyO; break;
			case keykind_p: Key = s3eKeyP; break;
			case keykind_q: Key = s3eKeyQ; break;
			case keykind_r: Key = s3eKeyR; break;
			case keykind_s: Key = s3eKeyS; break;
			case keykind_t: Key = s3eKeyT; break;
			case keykind_u: Key = s3eKeyU; break;
			case keykind_v: Key = s3eKeyV; break;
			case keykind_w: Key = s3eKeyW; break;
			case keykind_x: Key = s3eKeyX; break;
			case keykind_y: Key = s3eKeyY; break;
			case keykind_z: Key = s3eKeyZ; break;
			case keykind_f1: Key = s3eKeyF1; break;
			case keykind_f2: Key = s3eKeyF2; break;
			case keykind_f3: Key = s3eKeyF3; break;
			case keykind_f4: Key = s3eKeyF4; break;
			case keykind_f5: Key = s3eKeyF5; break;
			case keykind_f6: Key = s3eKeyF6; break;
			case keykind_f7: Key = s3eKeyF7; break;
			case keykind_f8: Key = s3eKeyF8; break;
			case keykind_f9: Key = s3eKeyF9; break;
			case keykind_f10: Key = s3eKeyF10; break;
			case keykind_pad_0: Key = s3eKeyNumPad0; break;
			case keykind_pad_1: Key = s3eKeyNumPad1; break;
			case keykind_pad_2: Key = s3eKeyNumPad2; break;
			case keykind_pad_3: Key = s3eKeyNumPad3; break;
			case keykind_pad_4: Key = s3eKeyNumPad4; break;
			case keykind_pad_5: Key = s3eKeyNumPad5; break;
			case keykind_pad_6: Key = s3eKeyNumPad6; break;
			case keykind_pad_7: Key = s3eKeyNumPad7; break;
			case keykind_pad_8: Key = s3eKeyNumPad8; break;
			case keykind_pad_9: Key = s3eKeyNumPad9; break;
			case keykind_pad_plus: Key = s3eKeyNumPadPlus; break;
			case keykind_pad_minus: Key = s3eKeyNumPadMinus; break;
			case keykind_pad_enter: Key = s3eKeyNumPadEnter; break;
			case keykind_soft_left: Key = s3eKeyLSK; break;
			case keykind_soft_right: Key = s3eKeyRSK; break;
			case keykind_shoulder_left: Key = s3eKeyLS; break;
			case keykind_shoulder_right: Key = s3eKeyRS; break;
			case keykind_star: Key = s3eKeyStar; break;
			case keykind_sharp: Key = s3eKeyHash; break;
			case keykind_ok: Key = s3eKeyOk; break;
			case keykind_clr: Key = s3eKeyCLR; break;
			case keykind_vol_up: Key = s3eKeyVolUp; break;
			case keykind_vol_down: Key = s3eKeyVolDown; break;
			}
			return !(!(s3eKeyboardGetState(Key) & S3E_KEY_STATE_DOWN));
		}

		int GetPlatformConfigNumber(string name)
		{
			int Result = -1;
			s3eConfigGetInt("s3e", name, &Result);
			return Result;
		}

		string _tmp_ GetPlatformConfigString(string name)
		{
			global_data char Result[256] = {'\0',};
			s3eConfigGetString("s3e", name, Result);
			return Result;
		}

		void SetSimulatorWindowPos(int x, int y)
		{
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				if(BxDLL_WindowHandle)
					BxDLL_MoveWindow(BxDLL_WindowHandle, x, y, BxCore::Surface::GetWidthHW(false), BxCore::Surface::GetHeightHW(false), 1);
			#endif
		}

		point GetSimulatorWindowPos()
		{
			point SimulatorPos = point::zero();
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				if(BxDLL_WindowHandle)
				{
					RECT WindowRect = {0, 0, 0, 0};
					BxDLL_GetWindowRect(BxDLL_WindowHandle, &WindowRect);
					SimulatorPos.x = WindowRect.left;
					SimulatorPos.y = WindowRect.top;
				}
			#endif
			return SimulatorPos;
		}

		point GetSimulatorCursorPos()
		{
			point SimulatorCursorPos = point::zero();
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				if(BxDLL_WindowHandle)
				{
					POINT CursorPos = {0, 0};
					BxDLL_GetCursorPos(&CursorPos);
					SimulatorCursorPos.x = CursorPos.x;
					SimulatorCursorPos.y = CursorPos.y;
				}
			#endif
			return SimulatorCursorPos;
		}

		void DoSimulatorMinimize()
		{
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				if(BxDLL_WindowHandle)
				{
					const int SW_MINIMIZE = 6;
					BxDLL_ShowWindow(BxDLL_WindowHandle, SW_MINIMIZE);
				}
			#endif
		}

		bool Break(string name, string query __DEBUG_PRM__)
		{
			switch(IwDebugAssertShow(name, __DEBUG_FILE__, __DEBUG_LINE__, 1))
			{
			case S3E_ERROR_SHOW_STOP:
				do{if(s3eDebugIsDebuggerPresent()) IwDebugBreak(); else abort();} while(false);
				break;
			case S3E_ERROR_SHOW_IGNORE:
				return false;
			}
			return true;
		}

		void Trace(string map, const BxThrow& args)
		{
			printf(BxCore::Util::Print(map, args));
			fflush(stdout);
		}

		string _tmp_ GetOSName()
		{
			return s3eDeviceGetString(S3E_DEVICE_OS);
		}

		string _tmp_ GetArchName()
		{
			return s3eDeviceGetString(S3E_DEVICE_ARCHITECTURE);
		}

		string _tmp_ GetDeviceID(int* integerid)
		{
			const int IntegerID = s3eDeviceGetInt(S3E_DEVICE_UNIQUE_ID);
			if(IntegerID == -1)
			{
				// 맥어드레스ID를 제작
				////////////////////////////////////////
				if(integerid) *integerid = 0;
				return "UNSUPPORTED";
			}
			if(integerid) *integerid = IntegerID;
			return s3eDeviceGetString(S3E_DEVICE_UNIQUE_ID);
		}

		string _tmp_ GetPhoneNumber()
		{
			return s3eDeviceGetString(S3E_DEVICE_PHONE_NUMBER);
		}

		string _tmp_ GetAppPackage()
		{
			return GetConfigString("Bx.Application.Package", "com.bx.game");
		}

		string _tmp_ GetAppPackageExt()
		{
			return ".apk";
		}

		string _tmp_ GetAppVersion()
		{
			return GetConfigString("Bx.Application.Version", "1.x.x");
		}

		bool IsExistApp(string packagename)
		{
			////////////////////////////////////////
			return false;
		}

		void PopupOSExecute(string url, bool exitme)
		{
			s3eOSExecExecute(url, exitme);
		}

		bool PopupOSKeyboard(string_rw result_str256, string default_str, string keyboard_name, oskeyboardtype type)
		{
			if(!s3eOSReadStringAvailable())
				return false;
			int Flag = 0;
			switch(type)
			{
			case oskeyboardtype_base: Flag = 0; break;
			case oskeyboardtype_email: Flag = S3E_OSREADSTRING_FLAG_EMAIL; break;
			case oskeyboardtype_number: Flag = S3E_OSREADSTRING_FLAG_NUMBER; break;
			case oskeyboardtype_password: Flag = S3E_OSREADSTRING_FLAG_PASSWORD; break;
			case oskeyboardtype_url: Flag = S3E_OSREADSTRING_FLAG_URL; break;
			}
			wchar temp_UTF16[1024];
			char default_str_UTF8[1024];
			char keyboard_name_UTF8[1024];
			BxUtil::CP949ToUTF16(default_str, -1, temp_UTF16, 1024);
			BxUtil::UTF16ToUTF8(temp_UTF16, -1, default_str_UTF8);
			BxUtil::CP949ToUTF16(keyboard_name, -1, temp_UTF16, 1024);
			BxUtil::UTF16ToUTF8(temp_UTF16, -1, keyboard_name_UTF8);
			string Result = s3eOSReadStringUTF8WithDefault(keyboard_name_UTF8, default_str_UTF8, Flag);
			if(!Result) return false;
			BxUtil::UTF8ToUTF16(Result, -1, temp_UTF16);
			BxUtil::UTF16ToCP949(temp_UTF16, -1, result_str256, 1024);
			return true;
		}

		bool IsExistClassJNI(string classpath)
		{
			if(!s3eAndroidJNIAvailable()) return false;
			JavaVM* JVM = (JavaVM*) s3eAndroidJNIGetVM();
			if(!JVM) return false;
			JNIEnv* ENV = nullptr;
			JVM->GetEnv((void**) &ENV, JNI_VERSION_1_6);
			jclass JCLASS = ENV->FindClass(classpath);
			if(!JCLASS) return false;
			ENV->DeleteLocalRef(JCLASS);
			return true;
		}

		int CallStaticMethodJNI(string classpath, string methodname, string param1_str1024, int param2)
		{
			if(!s3eAndroidJNIAvailable()) return -90001;
			JavaVM* JVM = (JavaVM*) s3eAndroidJNIGetVM();
			if(!JVM) return -90002;
			JNIEnv* ENV = nullptr;
			JVM->GetEnv((void**) &ENV, JNI_VERSION_1_6);
			jclass JCLASS = ENV->FindClass(classpath);
			if(!JCLASS) return -90003;

			jmethodID JMETHOD = ENV->GetStaticMethodID(JCLASS, methodname, "(Ljava/lang/String;I)I");
			wchar temp_UTF16[1024];
			char default_str_UTF8[1024];
			BxUtil::CP949ToUTF16(param1_str1024, -1, temp_UTF16, 1024);
			BxUtil::UTF16ToUTF8(temp_UTF16, -1, default_str_UTF8);
			jstring JSTRING = ENV->NewStringUTF(default_str_UTF8);
			jint JINT = ENV->CallStaticIntMethod(JCLASS, JMETHOD, JSTRING, param2);
			ENV->DeleteLocalRef(JCLASS);
			ENV->DeleteLocalRef(JSTRING);
			return JINT;
		}

		int AddCallbackJNI(string classpath, string methodname, callback_jni methodCB)
		{
			if(!s3eAndroidJNIAvailable()) return -90001;
			JavaVM* JVM = (JavaVM*) s3eAndroidJNIGetVM();
			if(!JVM) return -90002;
			JNIEnv* ENV = nullptr;
			JVM->GetEnv((void**) &ENV, JNI_VERSION_1_6);
			jclass JCLASS = ENV->FindClass(classpath);
			if(!JCLASS) return -90003;

			if(CallbackListJNI<0>::GetClass(classpath, methodname)) return CallbackListJNI<0>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<1>::GetClass(classpath, methodname)) return CallbackListJNI<1>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<2>::GetClass(classpath, methodname)) return CallbackListJNI<2>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<3>::GetClass(classpath, methodname)) return CallbackListJNI<3>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<4>::GetClass(classpath, methodname)) return CallbackListJNI<4>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<5>::GetClass(classpath, methodname)) return CallbackListJNI<5>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<6>::GetClass(classpath, methodname)) return CallbackListJNI<6>::SetRegister(ENV, JCLASS, methodname, methodCB);
			if(CallbackListJNI<7>::GetClass(classpath, methodname)) return CallbackListJNI<7>::SetRegister(ENV, JCLASS, methodname, methodCB);
			ENV->DeleteLocalRef(JCLASS);
			return -90005;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Surface
	{
		void Init()
		{
			// icf를 통한 방향성조사(에뮬은 비활성)
			dirtype dir = dir_rot0;
			#if defined(__GNUC__) || defined(__ARMCC_VERSION)
				string DispFixRot = BxCore::System::GetPlatformConfigString("DispFixRot");
				if(BxUtilGlobal::StrICmp(DispFixRot, "Portrait") == same || BxUtilGlobal::StrICmp(DispFixRot, "FixedPortrait") == same)
					dir = dir_rot0;
				else if(BxUtilGlobal::StrICmp(DispFixRot, "Landscape") == same || BxUtilGlobal::StrICmp(DispFixRot, "FixedLandscape") == same)
					dir = dir_rot90;
			#endif
			if(IsOSAndroid()) dir = dir_rot0;
			#ifndef __BX_OPENGL
				#ifdef __BX_PIXEL16
					s3eSurfacePixelType Type = S3E_SURFACE_PIXEL_TYPE_RGB565;
				#else
					s3eSurfacePixelType Type = S3E_SURFACE_PIXEL_TYPE_RGB888;
				#endif
				s3eSurfaceBlitDirection Direction = S3E_SURFACE_BLIT_DIR_NORMAL;
				switch(dir)
				{
				case dir_rot0: Direction = S3E_SURFACE_BLIT_DIR_NORMAL; break;
				case dir_rot90: Direction = S3E_SURFACE_BLIT_DIR_ROT90; break;
				case dir_rot180: Direction = S3E_SURFACE_BLIT_DIR_ROT180; break;
				case dir_rot270: Direction = S3E_SURFACE_BLIT_DIR_ROT270; break;
				case dir_native: Direction = S3E_SURFACE_BLIT_DIR_NATIVE; break;
				}
				s3eSurfaceSetup(Type, 0, nullptr, Direction);
			#else
				switch(dir)
				{
				case dir_rot0:
					IwGxSetScreenOrient(IW_GX_ORIENT_NONE);
					break;
				case dir_rot90:
					IwGxSetScreenOrient(IW_GX_ORIENT_90);
					break;
				case dir_rot180:
					IwGxSetScreenOrient(IW_GX_ORIENT_180);
					break;
				case dir_rot270:
					IwGxSetScreenOrient(IW_GX_ORIENT_270);
					break;
				}
				IwGxLightingOff();
				//IwGxMipMappingOff();
				IwGxFogOff();
				IwGxSetColStream(nullptr);
				IwGxSetColClear(0x00, 0x00, 0x00, 0xFF);
				BxCore::Surface::SetRenderMode(rendermode_2d);
				IwGxClear();
				if(GetWidth() != GetWidthHW() || GetHeight() != GetHeightHW())
				{
					if(!OffscreenSurface().CreateSurface(nullptr, GetWidth(), GetHeight(),
						CIwGxSurface::EXACT_MATCH_F | CIwGxSurface::PERSISTENT_F))
						BxASSERT("BxCore::Surface<FBO의 초기화에 실패하였습니다>", false);
					else
					{
						IsOffscreenSurfaceValid() = true;
						OffscreenSurface().MakeCurrent();
						IwGxLightingOff();
						//IwGxMipMappingOff();
						IwGxFogOff();
						IwGxSetColStream(nullptr);
						IwGxSetColClear(0x00, 0x00, 0x00, 0xFF);
						IwGxClear();
					}
				}
			#endif
			s3eDeviceBacklightOn();
		}

		void Flush(void* ptr, const int row, const int height, const int pitch)
		{
			#ifndef __BX_OPENGL
				void* ScreenPtr = s3eSurfacePtr();
				const int ScreenPitch = s3eSurfaceGetInt(S3E_SURFACE_PITCH);
				for(int y = 0; y < height; ++y)
				{
					memcpy(ScreenPtr, ptr, row);
					ScreenPtr = ((byte*) ScreenPtr) + ScreenPitch;
					ptr = ((byte*) ptr) + pitch;
				}
				s3eSurfaceShow();
			#else
				BxCore::Surface::SetRenderMode(rendermode_null);
				if(IsOffscreenSurfaceValid())
				{
					CIwGxSurface::MakeDisplayCurrent();
					CIwMaterial* MaterialFBO = IW_GX_ALLOC_MATERIAL();
					MaterialFBO->SetTexture(OffscreenSurface().GetTexture());
					MaterialFBO->SetModulateMode(CIwMaterial::MODULATE_RGB);
					MaterialFBO->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
					MaterialFBO->SetBlendMode(CIwMaterial::BLEND_MODULATE);
					MaterialFBO->SetColAmbient(0xFF, 0xFF, 0xFF, 0xFF);
					MaterialFBO->SetCullMode(CIwMaterial::CULL_NONE);
					IwGxSetMaterial(MaterialFBO);
					// Vertex
					CIwSVec2* Vertex = IW_GX_ALLOC(CIwSVec2, 4);
					Vertex[0].x = 0;
					Vertex[0].y = 0;
					Vertex[1].x = 0;
					Vertex[1].y = GetHeightHW();
					Vertex[2].x = GetWidthHW();
					Vertex[2].y = GetHeightHW();
					Vertex[3].x = GetWidthHW();
					Vertex[3].y = 0;
					// UV
					CIwSVec2* UV = IW_GX_ALLOC(CIwSVec2, 4);
					UV[0] = CIwSVec2(0, 0);
					UV[1] = CIwSVec2(0, (1 << 12) * GetHeight() / OffscreenSurface().GetHeight());
					UV[2] = CIwSVec2((1 << 12) * GetWidth() / OffscreenSurface().GetWidth(),
						(1 << 12) * GetHeight() / OffscreenSurface().GetHeight());
					UV[3] = CIwSVec2((1 << 12) * GetWidth() / OffscreenSurface().GetWidth(), 0);
					IwGxSetVertStreamScreenSpace(Vertex, 4);
					IwGxSetUVStream(UV, 0, 4);
					IwGxDrawPrims(IW_GX_QUAD_LIST, nullptr, 4);
					IwGxFlush();
				}
				IwGLSwapBuffers(); // for marmalade logo disabled
				if(IsOffscreenSurfaceValid())
					OffscreenSurface().MakeCurrent();
			#endif
			s3eDeviceYield();
			s3eKeyboardUpdate();
			s3ePointerUpdate();
			s3eDeviceBacklightOn();
		}

		void MiddleFlush()
		{
			#ifdef __BX_OPENGL
				IwGxFlush();
			#endif
		}

		bool& _MarginEnable() {global_data bool MarginMode = true; return MarginMode;}

		int GetWidthHW(bool usemargin)
		{
			int WidthHW = 0;
			#ifdef __BX_OPENGL
				if(IsOSIPhone()) WidthHW = IwGxGetScreenWidth();
				else WidthHW = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_WIDTH);
			#else
				WidthHW = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
			#endif
			if(usemargin && _MarginEnable())
				return WidthHW - BxCore::Main::GetCurrentGUIMargin().l - BxCore::Main::GetCurrentGUIMargin().r;
			return WidthHW;
		}

		int GetHeightHW(bool usemargin)
		{
			int HeightHW = 0;
			#ifdef __BX_OPENGL
				if(IsOSIPhone()) HeightHW = IwGxGetScreenHeight();
				else HeightHW = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_HEIGHT);
			#else
				HeightHW = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
			#endif
			if(usemargin && _MarginEnable())
				return HeightHW - BxCore::Main::GetCurrentGUIMargin().t - BxCore::Main::GetCurrentGUIMargin().b;
			return HeightHW;
		}

		bool SetMarginMode(bool enable)
		{
			return _MarginEnable() = enable;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Sound
	{
		id_sound Create(string filename, bool autoload)
		{
			const int Length = BxUtilGlobal::StrLen(filename);
			BxASSERT("BxCore::Sound<파일명이 너무 짧습니다>", 4 <= Length);
			const bool IsAudio = (BxUtilGlobal::StrICmp(filename + Length - 4, ".wav") != same);
			SoundData* Data = BxNew_Param(SoundData, filename, IsAudio, autoload);
			return (id_sound) Data;
		}

		void Release(id_sound sound)
		{
			BxDelete_ByType(SoundData, sound);
		}

		void PlayMusic(id_sound sound, bool interrupt, const BxThrow& panorama)
		{
			if(PanoramaList::Focus()->GetLength() == 0)
				interrupt = true;
			PanoramaList::Focus(PanoramaList::RemoveAll)->AddNext((SoundData*) sound);
			for(int i = 0; i < panorama.Length(); ++i)
			{
				SoundData* iSound = (SoundData*) *panorama.Access<id_sound>(i);
				PanoramaList::Focus(PanoramaList::GoNext)->AddNext(iSound);
			}
			PanoramaList::Focus(PanoramaList::GoFirst);
			if(interrupt)
			{
				PanoramaList::Focus(PanoramaList::GoNext);
				SoundData* CurSound = (SoundData*) SoundData::ChannelState(0, false);
				if(CurSound)
				{
					if(CurSound->IsSoundDataByAudio()) s3eAudioStop();
					else s3eSoundChannelStop(0);
				}
				SoundData::ChannelState(0, false) = sound;
				((SoundData*) sound)->LockSoundData();
				if(((SoundData*) sound)->IsSoundDataByAudio())
				{
					s3eResult Result = s3eAudioPlayFromBuffer(((SoundData*) sound)->GetSoundData(), ((SoundData*) sound)->GetSoundDataSize(), 1);
					BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				else
				{
					s3eSoundChannelSetInt(0, S3E_CHANNEL_RATE, ((SoundData*) sound)->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(0, ((SoundData*) sound)->GetSoundData(), ((SoundData*) sound)->GetSoundDataCount(), 1, 0);
					BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
			}
		}

		void StopMusic()
		{
			PanoramaList::Focus(PanoramaList::RemoveAll);
			SoundData* CurSound = (SoundData*) SoundData::ChannelState(0, false);
			if(CurSound)
			{
				if(CurSound->IsSoundDataByAudio()) s3eAudioStop();
				else s3eSoundChannelStop(0);
				CurSound->UnlockSoundData();
			}
			SoundData::ChannelState(0, false) = nullptr;
		}

		void SetMusicVolume(uint percent)
		{
			global_data const int DefaultVolumeSound = s3eSoundGetInt(S3E_SOUND_VOLUME_DEFAULT);
			int VolumeSound = DefaultVolumeSound * (int) percent / BxIntegerX(100);
			if(VolumeSound < 0) VolumeSound = 0;
			else if(S3E_SOUND_MAX_VOLUME < VolumeSound) VolumeSound = S3E_SOUND_MAX_VOLUME;
			s3eSoundChannelSetInt(0, S3E_CHANNEL_VOLUME, VolumeSound);
			// Audio
			global_data const int DefaultVolumeAudio = s3eAudioGetInt(S3E_AUDIO_VOLUME_DEFAULT);
			int VolumeAudio = DefaultVolumeAudio * (int) percent / BxIntegerX(100);
			if(VolumeAudio < 0) VolumeAudio = 0;
			else if(S3E_AUDIO_MAX_VOLUME < VolumeAudio) VolumeAudio = S3E_AUDIO_MAX_VOLUME;
			s3eAudioSetInt(S3E_AUDIO_VOLUME, VolumeAudio);
		}

		void PlayEffect(id_sound sound, bool interrupt)
		{
			int Channel = SoundData::GetFreeEffectChannel();
			if(Channel) interrupt = true;
			else if(!(Channel = SoundData::GetSameEffectChannel(sound)))
				Channel = SoundData::GetImminentEffectChannel();
			if(interrupt)
			{
				SoundData::EffectChannelStop(Channel);
				s3eResult Result = SoundData::EffectChannelPlay(Channel, sound);
				BxASSERT("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
			}
			else SoundData::EffectChannelPlayReserve(Channel, sound);
		}

		void StopEffect()
		{
			for(int i = 1; i < SoundData::GetChannelCount(); ++i)
				SoundData::EffectChannelStop(i);
		}

		void SetEffectVolume(uint percent)
		{
			global_data const int DefaultVolume = s3eSoundGetInt(S3E_SOUND_VOLUME_DEFAULT);
			int Volume = DefaultVolume * (int) percent / BxIntegerX(100);
			if(Volume < 0) Volume = 0;
			if(S3E_SOUND_MAX_VOLUME < Volume) Volume = S3E_SOUND_MAX_VOLUME;
			for(int i = 1; i < SoundData::GetChannelCount(); ++i)
				s3eSoundChannelSetInt(i, S3E_CHANNEL_VOLUME, Volume);
		}

		void PauseAll()
		{
			s3eSoundPauseAllChannels();
			s3eAudioPause();
		}

		void ResumeAll()
		{
			s3eSoundResumeAllChannels();
			s3eAudioResume();
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Util
	{
		void* Alloc(int size)
		{
			return s3eMalloc(size);
		}

		void Free(void* buf)
		{
			s3eFree(buf);
		}

		void MemSet(void* dst, byte value, int length)
		{
			if(dst) memset(dst, value, length);
		}

		void MemCpy(void* dst, const void* src, int length)
		{
			if(dst) memmove(dst, src, length);
		}

		int MemCmp(const void* dst, const void* src, int length)
		{
			return memcmp(dst, src, length);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace File
	{
		bool IsExist(string filename)
		{
			return (s3eFileCheckExists(filename) == true);
		}

		id_file Open(string filename, string mode)
		{
			return (id_file) s3eFileOpen(filename, mode);
		}

		void Close(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			s3eFileClose((s3eFile*) file);
		}

		uint Read(id_file file, void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
				return s3eFileRead(buffer, length, 1, (s3eFile*) file);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
				Result += s3eFileRead(&((byte*) buffer)[i], BxUtilGlobal::Min(length - i, 4096), 1, (s3eFile*) file);
				if(progress && !progress(-1, 0)) return Result; // 업데이트
			}
			return Result;
		}

		uint ReadVariable(id_file file, uhuge* variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			byte CurBits = 0x80;
			while((CurBits & 0x80) && Read(file, &CurBits, 1, progress))
			{
				CurBits += additionkey;
				*variable |= (CurBits & 0x7F) << (7 * Result);
				++Result;
			}
			return Result;
		}

		uint Write(id_file file, const void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
				return s3eFileWrite(buffer, length, 1, (s3eFile*) file);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
				Result += s3eFileWrite(&((const byte*) buffer)[i], BxUtilGlobal::Min(length - i, 4096), 1, (s3eFile*) file);
				if(progress && !progress(-1, 0)) return Result; // 업데이트
			}
			return Result;
		}

		uint WriteVariable(id_file file, const uhuge variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			uhuge AllBits = variable;
			do
			{
				byte CurBits = (AllBits & 0x7F);
				if(AllBits >>= 7) CurBits |= 0x80;
				CurBits += 0x100 - additionkey;
				Write(file, &CurBits, 1, progress);
				++Result;
			}
			while(AllBits);
			return Result;
		}

		uint WriteString(id_file file, string buffer, callback_progress progress)
		{
			return Write(file, buffer, BxUtilGlobal::StrLen(buffer), progress);
		}

		int Skip(id_file file, uint length)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			return s3eFileSeek((s3eFile*) file, length, S3E_FILESEEK_CUR);
		}

		int GetSize(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			return s3eFileGetSize((s3eFile*) file);
		}

		int GetSize(string filename)
		{
			s3eFile* File = s3eFileOpen(filename, "rb");
			const int Result = (File)? s3eFileGetSize(File) : -1;
			if(File) s3eFileClose(File);
			return Result;
		}

		uhuge GetDate(string filename)
		{
			return (uhuge) s3eFileGetLastWriteTime(filename);
		}

		bool Attach(string archivename)
		{
			return (((s3eResult) dzArchiveAttach(archivename)) == S3E_RESULT_SUCCESS);
		}

		void Detach()
		{
			dzArchiveDetach();
		}

		void MakeDirectory(string dirname, bool isFile)
		{
			char PathName[1024];
			PathName[0] = '\0';
			for(int i = 0; dirname[i]; ++i)
			{
				if(dirname[i] == '/' || dirname[i] == '\\')
				{
					if(*PathName) s3eFileMakeDirectory(PathName);
					PathName[i] = '/';
				}
				else PathName[i] = dirname[i];
				PathName[i + 1] = '\0';
			}
			if(*PathName && !isFile)
				s3eFileMakeDirectory(PathName);
		}

		uint SearchFiles(string dirname, callback_filesearch resultCB, void* data)
		{
			s3eFileList* List = s3eFileListDirectory(dirname);
			const int DirLength = BxUtilGlobal::StrLen(dirname);
			uint SearchCount = 0;
			char FileName[1024];
			BxUtilGlobal::StrCpy(FileName, dirname, DirLength);
			FileName[DirLength] = '/';
			while(s3eFileListNext(List, FileName + (DirLength + 1), 1024 - (DirLength + 1)) == S3E_RESULT_SUCCESS)
			{
				++SearchCount;
				if(resultCB) resultCB(!IsExist(FileName), FileName, data);
			}
			s3eFileListClose(List);
			return SearchCount;
		}

		bool RemoveFile(string filename, bool doRemoveBlankedDirectory)
		{
			if(s3eFileDelete(filename) != S3E_RESULT_SUCCESS)
			{
				s3eFileGetError();
				return false;
			}
			else if(doRemoveBlankedDirectory)
			{
				BxString::Parse DirName(filename);
				do
				{
					DirName.DeleteLast();
					while(DirName.GetLast() != '/' && DirName.GetLast() != '\\')
						DirName.DeleteLast();
				}
				while(s3eFileDeleteDirectory(DirName) == S3E_RESULT_SUCCESS);
			}
			return true;
		}

		bool RenameFile(string srcname, string dstname)
		{
			if(s3eFileRename(srcname, dstname) != S3E_RESULT_SUCCESS)
			{
				s3eFileGetError();
				return false;
			}
			return true;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Socket
	{
		id_socket Create()
		{
			TCPData* Data = BxNew(TCPData);
			if(Data->GetState() == socketstate_created)
				return (id_socket) Data;
			BxDelete(Data);
			return nullptr;
		}

		void Release(id_socket sock)
		{
			if(sock)
				BxDelete_ByType(TCPData, sock);
		}

		socketstate GetState(id_socket sock)
		{
			if(sock)
				return ((TCPData*) sock)->GetState();
			return socketstate_null;
		}

		connectresult Connect(id_socket sock, string addr, ushort port, uint timeout, callback_progress progress)
		{
			if(!sock) return connect_error_param;
			else if(((TCPData*) sock)->GetState() == socketstate_null)
			{
				((TCPData*) sock)->ResetClient();
				if(((TCPData*) sock)->GetState() != socketstate_created)
					return connect_error_param;
			}
			else if(((TCPData*) sock)->GetState() == socketstate_connecting) return connect_connecting;
			else if(((TCPData*) sock)->GetState() == socketstate_connected) return connect_connected;
			// IP조사
			hostent* HostEnt = gethostbyname(addr);
			BxASSERT("BxCore::Socket<알 수 없는 도메인입니다>", HostEnt);
			if(!HostEnt) return connect_error_param;
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				// 접속정보
				SOCKADDR_IN SockAddr;
				BxCore::Util::MemSet(&SockAddr, 0, sizeof(SOCKADDR_IN));
				SockAddr.sin_family = AF_INET;
				SockAddr.sin_addr.S_un.S_addr = *((uint*) HostEnt->h_addr_list[0]);
				SockAddr.sin_port = htons(port);
				// 접속
				((TCPData*) sock)->SetState(socketstate_connecting);
				SOCKET FD = ((TCPData*) sock)->GetClient();
				uint DoNonBlock = 1;
				BxDLL_ioctlsocket(FD, FIONBIO, &DoNonBlock);
				BxDLL_connect(FD, (LPSOCKADDR) &SockAddr, sizeof(SOCKADDR_IN));
				// 결과확인
				BxDLL_fd_set fdset = {1, {FD,}};
				BxDLL_timeval tmval = {(timeout + 999) / 1000, 0};
				BxDLL_select(FD + 1, nullptr, &fdset, nullptr, &tmval);
				if(!BxDLL_WSAFDIsSet(FD, &fdset))
					((TCPData*) sock)->Disconnect();
				else if(0 < fdset.fd_count)
					((TCPData*) sock)->SetServer(fdset.fd_array[0]);
			#else
				// 접속정보
				s3eInetAddress SockAddr = {0,};
				SockAddr.m_IPAddress = *((uint*) HostEnt->h_addr_list[0]);
				SockAddr.m_Port = ((port & 0x00FF) << 8) | ((port & 0xFF00) >> 8);
				// 접속
				((TCPData*) sock)->SetState(socketstate_connecting);
				s3eResult Result = s3eSocketConnect(((TCPData*) sock)->GetClient(), &SockAddr, _ConnectCB, sock);
				if(Result == S3E_RESULT_ERROR)
				{
					switch(s3eSocketGetError())
					{
					case S3E_SOCKET_ERR_PARAM: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_param;
					case S3E_SOCKET_ERR_ALREADY: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_already;
					case S3E_SOCKET_ERR_NETDOWN: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_netdown;
					case S3E_SOCKET_ERR_TIMEDOUT: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_timeout;
					case S3E_SOCKET_ERR_INPROGRESS: break;
					case S3E_SOCKET_ERR_CONNREFUSED: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_refused;
					default: ((TCPData*) sock)->SetState(socketstate_disconnected); return connect_error_unknown;
					}
				}
				// 결과확인
				if(0 < timeout)
				{
					uhuge LimitTime = BxCore::System::GetTimeMilliSecond() + timeout;
					while(BxCore::System::GetTimeMilliSecond() < LimitTime && ((TCPData*) sock)->GetState() == socketstate_connecting)
					{
						if(progress && !progress(-1, 0)) // 업데이트
							return connect_error_unknown;
						BxCore::System::Sleep(10);
					}
				}
				else BxCore::System::Sleep(0);
			#endif
			switch(((TCPData*) sock)->GetState())
			{
			case socketstate_connecting: return connect_connecting;
			case socketstate_connected: return connect_connected;
			case socketstate_disconnected: return connect_disconnected;
			}
			((TCPData*) sock)->SetState(socketstate_disconnected);
			return connect_error_unknown;
		}

		void Disconnect(id_socket sock)
		{
			if(!sock) return;
			((TCPData*) sock)->Disconnect();
		}

		int Send(id_socket sock, const void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected) return -2;
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				int Result = BxDLL_send(TCP->GetServer(), (string) buffer, len, 0);
			#else
				int Result = s3eSocketSend(TCP->GetServer(), (string) buffer, len, 0);
			#endif
			if(0 <= Result) return Result;
			return _GetErrorCode(sock);
		}

		int Recv(id_socket sock, void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected)
				return -2;
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				int Result = BxDLL_recv(TCP->GetServer(), (string_rw) buffer, len, 0);
			#else
				int Result = s3eSocketRecv(TCP->GetServer(), (string_rw) buffer, len, 0);
			#endif
			if(0 <= Result) return Result;
			return _GetErrorCode(sock);
		}

		int Ping(string addr, uint timeout)
		{
			#if defined(_MSC_VER) && defined(I3D_ARCH_X86)
				global_data ICMPData Sock;
				// IP조사
				hostent* HostEnt = gethostbyname(addr);
				BxASSERT("BxCore::Socket<알 수 없는 도메인입니다>", HostEnt);
				if(!HostEnt) return SOCKET_ERROR;
				// 접속정보
				SOCKADDR_IN SockAddr;
				BxCore::Util::MemSet(&SockAddr, 0, sizeof(SOCKADDR_IN));
				SockAddr.sin_family = AF_INET;
				SockAddr.sin_addr.S_un.S_addr = *((uint*) HostEnt->h_addr_list[0]);
				// 송수신
				if(Sock.SendTo(timeout, (LPSOCKADDR) &SockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
					return SOCKET_ERROR;
				return Sock.RecvFrom();
			#else
				return -1;
			#endif
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Font
	{
		id_font Open(string filename, const int pointsize, const int numCacheGlyphs)
		{
			#ifndef __BX_OPENGL
				return nullptr;
			#else
				if(++RefCount() == 1)
				{
					IwGxFontInit();
					IwGxFontResetState();
					SetEncoding(fontencoding_cp949);
					SetSort(fontsort_pad7);
					SetStyle(false, false);
					TempFont() = IwGxFontCreateTTFont(filename, 1, -1);
				}
				CIwGxFont* NewFont = IwGxFontCreateTTFont(filename, pointsize, numCacheGlyphs);
				if(!NewFont)
				{
					if(--RefCount() == 0)
					{
						IwGxFontDestroyTTFont(TempFont());
						IwGxFontTerminate();
					}
				}
				return (id_font) NewFont;
			#endif
		}

		id_font Open(const byte* buffer, const int buffersize, const int pointsize, const int numCacheGlyphs)
		{
			#ifndef __BX_OPENGL
				return nullptr;
			#else
				if(++RefCount() == 1)
				{
					IwGxFontInit();
					IwGxFontResetState();
					SetEncoding(fontencoding_cp949);
					SetSort(fontsort_pad7);
					SetStyle(false, false);
					TempFont() = IwGxFontCreateTTFontFromBuffer(buffer, buffersize, 1, -1);
				}
				CIwGxFont* NewFont = IwGxFontCreateTTFontFromBuffer(buffer, buffersize, pointsize, numCacheGlyphs);
				if(!NewFont)
				{
					if(--RefCount() == 0)
					{
						IwGxFontDestroyTTFont(TempFont());
						IwGxFontTerminate();
					}
				}
				return (id_font) NewFont;
			#endif
		}

		void Close(id_font font)
		{
			if(!font) return;
			IwGxFontDestroyTTFont((CIwGxFont*) font);
			if(--RefCount() == 0)
			{
				IwGxFontDestroyTTFont(TempFont());
				IwGxFontTerminate();
			}
		}

		void SetEncoding(fontencoding encoding)
		{
			global_data fontencoding oldvalue = (fontencoding) -1;
			if(oldvalue == encoding) return;
			oldvalue = encoding;
			#ifdef __BX_OPENGL
				switch(encoding)
				{
				case fontencoding_cp949: IwGxFontSetEncoding(IW_GX_FONT_CP949); break;
				case fontencoding_utf8: IwGxFontSetEncoding(IW_GX_FONT_UTF_8); break;
				case fontencoding_iso2022kr: IwGxFontSetEncoding(IW_GX_FONT_ISO_2022_KR); break;
				case fontencoding_iso2022jp: IwGxFontSetEncoding(IW_GX_FONT_ISO_2022_JP); break;
				}
				IwGxFontInitEncodings();
			#endif
		}

		fontsort SetSort(fontsort sort)
		{
			global_data fontsort oldvalue = (fontsort) -1;
			fontsort result = oldvalue;
			if(oldvalue != sort)
			{
				oldvalue = sort;
				#ifdef __BX_OPENGL
					switch(sort)
					{
					case fontsort_pad1: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_BOTTOM); break;
					case fontsort_pad2: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_BOTTOM); break;
					case fontsort_pad3: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_BOTTOM); break;
					case fontsort_pad4: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE); break;
					case fontsort_pad5: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE); break;
					case fontsort_pad6: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE); break;
					case fontsort_pad7: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_TOP); break;
					case fontsort_pad8: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_TOP); break;
					case fontsort_pad9: IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT); IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_TOP); break;
					}
				#endif
			}
			return result;
		}

		void SetStyle(bool underline, bool italic)
		{
			global_data bool oldvalue1 = false;
			global_data bool oldvalue2 = false;
			if(oldvalue1 == underline && oldvalue2 == italic) return;
			oldvalue1 = underline;
			oldvalue2 = italic;
			#ifdef __BX_OPENGL
				if(underline) IwGxFontSetFlags(IW_GX_FONT_UNDERLINE_F);
				else IwGxFontClearFlags(IW_GX_FONT_UNDERLINE_F);
				if(italic) IwGxFontSetFlags(IW_GX_FONT_ITALIC_F);
				else IwGxFontClearFlags(IW_GX_FONT_ITALIC_F);
			#endif
		}

		size GetSize(id_font font, string str, const size s)
		{
			size Size = {0, 0};
			if(!font || !str) return Size;
			CIwGxFontPreparedData Data;
			IwGxFontSetFont((CIwGxFont*) font);
			IwGxFontSetRect(CIwRect(0, 0, s.w, s.h));
			IwGxFontPrepareText(Data, str);
			Size.w = Data.GetWidth();
			Size.h = Data.GetHeight();
			return Size;
		}

		void Draw(id_font font, string str, const point p, const size s, const color_x888 color, const byte opacity)
		{
			if(!font || !str) return;
			global_data id_font LastFont = nullptr;
			global_data color_x888 LastColor = 0;
			global_data byte LastOpacity = 0xFF;
			if(LastFont != font || LastColor != color || LastOpacity != opacity)
			{
				LastFont = font;
				LastColor = color;
				LastOpacity = opacity;
				for(int i = 0, iend = ((CIwGxFont*) font)->GetNumberMaterials(); i < iend; ++i)
					((CIwGxFont*) font)->GetMaterial(i)->SetColAmbient((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, opacity);
			}
			IwGxFontSetFont((CIwGxFont*) font);
			IwGxFontSetRect(CIwRect(BxCore::Main::GetCurrentGUIMargin().l + p.x, BxCore::Main::GetCurrentGUIMargin().t + p.y, s.w, s.h));
			BxCore::Surface::SetRenderMode(rendermode_2d);
			IwGxFontDrawText(str);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Thread
	{
		id_thread Create(callback_thread threadCB, void* data)
		{
			ThreadClass* Class = new ThreadClass(threadCB, data);
			return (id_thread) Class;
		}

		void Release(id_thread thread, bool dokill, bool dowait)
		{
			if(!thread) return;
			ThreadClass* Class = (ThreadClass*) thread;
			if(dokill) Class->Kill();
			if(dowait) Class->Wait();
			delete Class;
		}

		id_mutex OpenMutex()
		{
			MutexClass* Mutex = new MutexClass();
			return (id_mutex) Mutex;
		}

		void CloseMutex(id_mutex mutex)
		{
			if(!mutex) return;
			delete (MutexClass*) mutex;
		}

		void Lock(id_mutex mutex)
		{
			if(!mutex) return;
			((MutexClass*) mutex)->Lock();
		}

		void Unlock(id_mutex mutex)
		{
			if(!mutex) return;
			((MutexClass*) mutex)->Unlock();
		}

		void* BindStorage(int* storagekey)
		{
			// 스레드별 저장소그룹 접근
			Lock(StorageClass::Mutex());
			BxVarMap<StorageClass>& StorageByThread = StorageClass::Map()[StorageClass::CurrentThreadID()];
			Unlock(StorageClass::Mutex());
			// 스레드내 개별저장소 접근
			StorageClass* Result = StorageByThread.Access((uint) storagekey);
			if(Result) return Result->Data;
			return StorageByThread[(uint) storagekey].Init(*storagekey);
		}

		void UnbindStorageAll()
		{
			Lock(StorageClass::Mutex());
			StorageClass::Map()[StorageClass::CurrentThreadID()].Reset();
			Unlock(StorageClass::Mutex());
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Library
	{
		id_library Open(string filename)
		{
			return (id_library) s3eExtLibraryOpen(filename);
		}

		void Close(id_library handle)
		{
			s3eExtLibraryClose((s3eDLLHandle*) handle);
		}

		void* Link(id_library handle, string name)
		{
			return s3eExtLibraryGetSymbol((s3eDLLHandle*) handle, name);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace OpenGL2D
	{
		void Init()
		{
			IwGLInit(); // for marmalade logo disabled
			IwGxInit();
			CIwFMat ViewMatrix = CIwFMat::g_Identity;
			ViewMatrix.SetZero();
			IwGxSetViewMatrix(&ViewMatrix);
			SetPerspective(500, 0x4000, 0x10);
		}

		void Quit()
		{
			IwGxTerminate();
			IwGLTerminate(); // for marmalade logo disabled
		}

		void Flush()
		{
			VBOCache::Pool().Flush();
		}

		void SetPerspective(const int mul, const int far, const int near)
		{
			IwGxSetPerspMul((float) mul);
			IwGxSetFarZNearZ((float) far, (float) near);
		}

		void SetOrthogonal(const int width, const int height, const int far, const int near)
		{
			IwGxSetOrtho(0, width, height, 0, (float) near, (float) far);
		}

		void AddViewTrans(int x, int y, int z)
		{
			CIwFMat ViewMatrix = IwGxGetViewMatrix();
			const CIwFVec3& OldVector = ViewMatrix.GetTrans();
			CIwFVec3 NewVector(OldVector.x + x, OldVector.y + y, OldVector.z + z);
			ViewMatrix.SetTrans(NewVector);
			IwGxSetViewMatrix(&ViewMatrix);
		}

		void AddViewRotX(fint angle)
		{
			CIwFMat ViewMatrix = IwGxGetViewMatrix();
			ViewMatrix.SetRotX(angle / (float) 0x10000, false, false);
			IwGxSetViewMatrix(&ViewMatrix);
		}

		void AddViewRotY(fint angle)
		{
			CIwFMat ViewMatrix = IwGxGetViewMatrix();
			ViewMatrix.SetRotY(angle / (float) 0x10000, false, false);
			IwGxSetViewMatrix(&ViewMatrix);
		}

		void AddViewRotZ(fint angle)
		{
			CIwFMat ViewMatrix = IwGxGetViewMatrix();
			ViewMatrix.SetRotZ(angle / (float) 0x10000, false, false);
			IwGxSetViewMatrix(&ViewMatrix);
		}

		id_opengl_form MakeForm_TextureMaterial(string filename)
		{
			TextureMaterial* Result = BxNew(TextureMaterial);
			Result->Texture.LoadFromFile(filename);
			Result->Texture.Upload();
			Result->ExpressWidth.Reset(BxInteger(0) + (int) Result->Texture.GetWidth());
			Result->ExpressHeight.Reset(BxInteger(0) + (int) Result->Texture.GetHeight());
			Result->Material.SetShadeMode(CIwMaterial::SHADE_FLAT);
			Result->Material.SetModulateMode(CIwMaterial::MODULATE_RGB);
			Result->Material.SetCullMode(CIwMaterial::CULL_NONE);
			Result->Material.SetInvisible(false);
			Result->Material.SetFiltering(false);
			Result->Material.SetAlphaTestMode(CIwMaterial::ALPHATEST_DISABLED);
			Result->Material.SetBlendMode(CIwMaterial::BLEND_BLEND);
			Result->Material.SetColAmbient(0xFF, 0xFF, 0xFF, 0xFF);
			Result->Material.SetTexture(&Result->Texture);
			return (id_opengl_form) Result;
		}

		id_opengl_form MakeForm_TextureMaterial(color_a888* image, const int width, const int height, const int pitch)
		{
			TextureMaterial* Result = BxNew(TextureMaterial);
			Result->Texture.CopyFromBuffer(width, height, CIwImage::ARGB_8888, sizeof(color_a888) * pitch, (uint8*) image, nullptr);
			Result->Texture.Upload();
			Result->ExpressWidth.Reset(BxInteger(0) + (int) Result->Texture.GetWidth());
			Result->ExpressHeight.Reset(BxInteger(0) + (int) Result->Texture.GetHeight());
			Result->Material.SetShadeMode(CIwMaterial::SHADE_FLAT);
			Result->Material.SetModulateMode(CIwMaterial::MODULATE_RGB);
			Result->Material.SetCullMode(CIwMaterial::CULL_NONE);
			Result->Material.SetInvisible(false);
			Result->Material.SetFiltering(false);
			Result->Material.SetAlphaTestMode(CIwMaterial::ALPHATEST_DISABLED);
			Result->Material.SetBlendMode(CIwMaterial::BLEND_BLEND);
			Result->Material.SetColAmbient(0xFF, 0xFF, 0xFF, 0xFF);
			Result->Material.SetTexture(&Result->Texture);
			return (id_opengl_form) Result;
		}

		id_opengl_form MakeForm_ColorAmbient(const color_x888 color)
		{
			static bool FirstInit = true;
			static ColorAmbient ResultData;
			ColorAmbient* Result = &ResultData;
			Result->Color = color;
			if(FirstInit)
			{
				FirstInit = false;
				Result->Material.SetShadeMode(CIwMaterial::SHADE_FLAT);
				Result->Material.SetModulateMode(CIwMaterial::MODULATE_RGB);
				Result->Material.SetCullMode(CIwMaterial::CULL_NONE);
				Result->Material.SetInvisible(false);
				Result->Material.SetFiltering(false);
				Result->Material.SetAlphaTestMode(CIwMaterial::ALPHATEST_DISABLED);
				Result->Material.SetBlendMode(CIwMaterial::BLEND_BLEND);
				Result->Material.SetColAmbient(0xFF, 0xFF, 0xFF, 0xFF);
			}
			return (id_opengl_form) Result;
		}

		void FreeForm(id_opengl_form form)
		{
			OpenGLForm* Form = (OpenGLForm*) form;
			if(Form && Form->Type != OpenGLForm::COLOR)
				BxDelete(Form);
		}

		id_opengl_outline MakeOutline_VertexUV(const rect& r, const rect& uvclip)
		{
			static VertexUV ResultData;
			VertexUV* Result = &ResultData;
			// Vertex
			Result->Vertex[0].x = ItoF(r.l);
			Result->Vertex[0].y = ItoF(r.t);
			Result->Vertex[1].x = ItoF(r.l);
			Result->Vertex[1].y = ItoF(r.b);
			Result->Vertex[2].x = ItoF(r.r);
			Result->Vertex[2].y = ItoF(r.b);
			Result->Vertex[3].x = ItoF(r.r);
			Result->Vertex[3].y = ItoF(r.t);
			// UV
			Result->UV[0] = CIwSVec2(uvclip.l, uvclip.t);
			Result->UV[1] = CIwSVec2(uvclip.l, uvclip.b);
			Result->UV[2] = CIwSVec2(uvclip.r, uvclip.b);
			Result->UV[3] = CIwSVec2(uvclip.r, uvclip.t);
			return (id_opengl_outline) Result;
		}

		id_opengl_outline MakeOutline_PolyVertexUV(const points& p, const rect& uvclip)
		{
			PolyVertexUV* Result = BxNew_Param(PolyVertexUV, p.count);
			BxASSERT("BxCore::OpenGL2D<파라미터 p의 count는 3 이상이어야 합니다>", 3 <= p.count);
			// Vertex
			rectf Bound = {0, 0, 0, 0};
			const bool IsClockwise = (BxUtil::GetClockwiseValue(p.pt[0], p.pt[1], p.pt[2]) < 0);
			for(int i = 0; i < p.count; ++i)
			{
				const int j = (IsClockwise)? (p.count - i) % p.count : i;
				Result->Vertex[i].x = ItoF(p.pt[j].x);
				Result->Vertex[i].y = ItoF(p.pt[j].y);
				Bound.l = (i == 0)? Result->Vertex[i].x : BxUtilGlobal::Min(Bound.l, Result->Vertex[i].x);
				Bound.t = (i == 0)? Result->Vertex[i].y : BxUtilGlobal::Min(Bound.t, Result->Vertex[i].y);
				Bound.r = (i == 0)? Result->Vertex[i].x : BxUtilGlobal::Max(Bound.r, Result->Vertex[i].x);
				Bound.b = (i == 0)? Result->Vertex[i].y : BxUtilGlobal::Max(Bound.b, Result->Vertex[i].y);
			}
			if(Bound.r <= Bound.l || Bound.b <= Bound.t)
			{
				BxDelete(Result);
				return nullptr;
			}
			// UV
			BxExpress::IntegerX BoundWidth(Bound.r - Bound.l + BxInteger(0));
			BxExpress::IntegerX BoundHeight(Bound.b - Bound.t + BxInteger(0));
			for(int i = 0; i < p.count; ++i)
				Result->UV[i] = CIwSVec2(
					uvclip.l + (short)((uvclip.r - uvclip.l) * (huge)(Result->Vertex[i].x - Bound.l) / BoundWidth),
					uvclip.t + (short)((uvclip.b - uvclip.t) * (huge)(Result->Vertex[i].y - Bound.t) / BoundHeight));
			return (id_opengl_outline) Result;
		}

		id_opengl_outline MakeOutline_PolyVertexUV3D(const vertexs& v, const points& uv)
		{
			PolyVertexUV3D* Result = BxNew_Param(PolyVertexUV3D, v.count);
			BxASSERT("BxCore::OpenGL2D<파라미터 v의 count는 3 이상이어야 합니다>", 3 <= v.count);
			BxASSERT("BxCore::OpenGL2D<파라미터 v와 uv의 count는 동일해야 합니다>", v.count == uv.count);
			// Vertex
			for(int i = 0; i < v.count; ++i)
			{
				Result->Vertex[i].x = v.vt[i].x;
				Result->Vertex[i].y = v.vt[i].y;
				Result->Vertex[i].z = v.vt[i].z;
			}
			// UV
			for(int i = 0; i < uv.count; ++i)
				Result->UV[i] = CIwSVec2(uv.pt[i].x, uv.pt[i].y);
			return (id_opengl_outline) Result;
		}

		void FreeOutline(id_opengl_outline outline)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			if(Outline && Outline->Type != OpenGLOutline::RECT)
				BxDelete(Outline);
		}

		color_x888 Form_ColorAmbient_GetColor(id_opengl_form form)
		{
			OpenGLForm* Form = (OpenGLForm*) form;
			BxASSERT("BxCore::OpenGL2D<파라미터가 ColorAmbient형이 아닙니다>", Form->Type == OpenGLForm::COLOR);
			return ((ColorAmbient*) Form)->Color;
		}

		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			BxASSERT("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
			if(hoz == 0x10000 && ver == 0x10000) return;
			VertexUV* VUV = (VertexUV*) Outline;
			for(int i = 0; i < 4; ++i)
			{
				VUV->Vertex[i].x = (fint)((VUV->Vertex[i].x * (huge) hoz + 0x8000) >> 16);
				VUV->Vertex[i].y = (fint)((VUV->Vertex[i].y * (huge) ver + 0x8000) >> 16);
			}
		}

		void Outline_VertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			BxASSERT("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
			if(angle1024 == 0 && !doFlip) return;
			VertexUV* VUV = (VertexUV*) Outline;
			const huge Cos = BxUtil::Cos(angle1024);
			const huge Sin = BxUtil::Sin(angle1024);
			fint MinX = 0, MaxX = 0;
			if(doFlip)
			for(int i = 0; i < 4; ++i)
			{
				MinX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Min(MinX, VUV->Vertex[i].x);
				MaxX = (i == 0)? VUV->Vertex[i].x : BxUtilGlobal::Max(MaxX, VUV->Vertex[i].x);
			}
			for(int i = 0; i < 4; ++i)
			{
				const fint TempX = (doFlip)? MinX + MaxX - VUV->Vertex[i].x : VUV->Vertex[i].x;
				VUV->Vertex[i].x = (fint)((TempX * Cos - VUV->Vertex[i].y * Sin + 0x8000) >> 16);
				VUV->Vertex[i].y = (fint)((TempX * Sin + VUV->Vertex[i].y * Cos + 0x8000) >> 16);
			}
		}

		void Outline_PolyVertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			BxASSERT("BxCore::OpenGL2D<파라미터가 PolyVertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::POLYGON);
			if(hoz == 0x10000 && ver == 0x10000) return;
			PolyVertexUV* PolyVUV = (PolyVertexUV*) Outline;
			for(int i = 0; i < PolyVUV->Count; ++i)
			{
				PolyVUV->Vertex[i].x = (fint)((PolyVUV->Vertex[i].x * (huge) hoz + 0x8000) >> 16);
				PolyVUV->Vertex[i].y = (fint)((PolyVUV->Vertex[i].y * (huge) ver + 0x8000) >> 16);
			}
		}

		void Outline_PolyVertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			BxASSERT("BxCore::OpenGL2D<파라미터가 PolyVertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::POLYGON);
			if(angle1024 == 0 && !doFlip) return;
			PolyVertexUV* PolyVUV = (PolyVertexUV*) Outline;
			const huge Cos = BxUtil::Cos(angle1024);
			const huge Sin = BxUtil::Sin(angle1024);
			fint MinX = 0, MaxX = 0;
			if(doFlip)
			for(int i = 0; i < PolyVUV->Count; ++i)
			{
				MinX = (i == 0)? PolyVUV->Vertex[i].x : BxUtilGlobal::Min(MinX, PolyVUV->Vertex[i].x);
				MaxX = (i == 0)? PolyVUV->Vertex[i].x : BxUtilGlobal::Max(MaxX, PolyVUV->Vertex[i].x);
			}
			for(int i = 0; i < PolyVUV->Count; ++i)
			{
				const fint TempX = (doFlip)? MinX + MaxX - PolyVUV->Vertex[i].x : PolyVUV->Vertex[i].x;
				PolyVUV->Vertex[i].x = (fint)((TempX * Cos - PolyVUV->Vertex[i].y * Sin + 0x8000) >> 16);
				PolyVUV->Vertex[i].y = (fint)((TempX * Sin + PolyVUV->Vertex[i].y * Cos + 0x8000) >> 16);
			}
		}

		void Render(id_opengl_form form, id_opengl_outline outline, int x, int y, const byte opacity, const color_x888 color)
		{
			OpenGLForm* Form = (OpenGLForm*) form;
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			if(Outline->Type == OpenGLOutline::RECT)
				_RenderRect(Form, (VertexUV*) Outline, x, y, opacity, color);
			else if(Outline->Type == OpenGLOutline::POLYGON)
				_RenderPoly(Form, (PolyVertexUV*) Outline, x, y, opacity, color);
		}

		void Render3D(id_opengl_form form, id_opengl_outline outline, int x, int y, int z, const byte opacity, const color_x888 color)
		{
			OpenGLForm* Form = (OpenGLForm*) form;
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			if(Outline->Type == OpenGLOutline::POLYGON3D)
				_RenderPoly3D(Form, (PolyVertexUV3D*) Outline, x, y, z, opacity, color);
		}

		void Clip(rect r)
		{
			if(0 < r.r - r.l && 0 < r.b - r.t)
				_IwGxSetViewRect(r.l, r.t, r.r - r.l, r.b - r.t);
		}
	}
}
