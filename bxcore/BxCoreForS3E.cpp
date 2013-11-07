// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
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
///////////////////////////
#include <BxAutoBuffer.hpp>
#include <BxCore.hpp>
#include <BxExpress.hpp>
#include <BxKeyword.hpp>
#include <BxMemory.hpp>
#include <BxScene.hpp>
#include <BxUtil.hpp>
#include <BxString.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// 윈도우일반
/// @cond SECTION_NAME
#ifdef I3D_ARCH_X86
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
	local_data WindowsLib_GetSystemTimeAsFileTime BxDLL_GetSystemTimeAsFileTime = null;
	local_data WindowsLib_FileTimeToLocalFileTime BxDLL_FileTimeToLocalFileTime = null;
	local_data WindowsLib_GetFocus BxDLL_GetFocus = null;
	local_data WindowsLib_FindWindowA BxDLL_FindWindowA = null;
	local_data WindowsLib_GetWindowLongA BxDLL_GetWindowLongA = null;
	local_data WindowsLib_SetWindowLongA BxDLL_SetWindowLongA = null;
	local_data WindowsLib_GetClassLongA BxDLL_GetClassLongA = null;
	local_data WindowsLib_SetClassLongA BxDLL_SetClassLongA = null;
	local_data WindowsLib_SetMenu BxDLL_SetMenu = null;
	local_data WindowsLib_MoveWindow BxDLL_MoveWindow = null;
	local_data WindowsLib_GetWindowRect BxDLL_GetWindowRect = null;
	local_data WindowsLib_GetCursorPos BxDLL_GetCursorPos = null;
	local_data WindowsLib_GetSystemMetrics BxDLL_GetSystemMetrics = null;
	local_data WindowsLib_SendMessageA BxDLL_SendMessageA = null;
	local_data WindowsLib_ShowWindow BxDLL_ShowWindow = null;
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

// 네트워크
/// @cond SECTION_NAME
#ifdef I3D_ARCH_X86
	#define WSADESCRIPTION_LEN 256
	#define WSASYS_STATUS_LEN 128
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
	local_data NetworkLib_WSAStartup BxDLL_WSAStartup = null;
	local_data NetworkLib_WSACleanup BxDLL_WSACleanup = null;
	local_data NetworkLib_WSAGetLastError BxDLL_WSAGetLastError = null;
	local_data NetworkLib_socket BxDLL_socket = null;
	local_data NetworkLib_closesocket BxDLL_closesocket = null;
	local_data NetworkLib_connect BxDLL_connect = null;
	local_data NetworkLib_shutdown BxDLL_shutdown = null;
	local_data NetworkLib_send BxDLL_send = null;
	local_data NetworkLib_recv BxDLL_recv = null;
	local_data NetworkLib_sendto BxDLL_sendto = null;
	local_data NetworkLib_recvfrom BxDLL_recvfrom = null;
	local_data NetworkLib_setsockopt BxDLL_setsockopt = null;
	local_data NetworkLib_getsockopt BxDLL_getsockopt = null;
	local_data NetworkLib_ioctlsocket BxDLL_ioctlsocket = null;
	local_data NetworkLib_select BxDLL_select = null;
	local_data NetworkLib_WSAFDIsSet BxDLL_WSAFDIsSet = null;
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
template<int ID>
class CallbackListJNI
{
	local_func string_rw ClassPath() {global_data char Name[256] = {'\0',}; return Name;}
	local_func string_rw MethodName() {global_data char Name[256] = {'\0',}; return Name;}

	local_func callback_jni& MethodJNI() {global_data callback_jni MethodJNI = null; return MethodJNI;}

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

/// @cond SECTION_NAME
extern callback_process ChildProcess;
/// @endcond

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
/// @cond SECTION_NAME
namespace BxCore
{
	namespace Main
	{
		local_data BxDraw* Draw = null;

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
				s3ePointerRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion, null);
			else s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion);
			return 0;
		}

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
				s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMotion_Multi, null);
			if(!EventData->m_Pressed && PressCount == 0)
				s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMotion_Multi);
			return 0;
		}

		void EventAttach(void* drawclass)
		{
			Draw = (BxDraw*) drawclass;
			s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyboard, null);
			if(BxCore::System::IsMultiTouchAvailable())
				s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, OnPointer_Multi, null);
			else s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, OnPointer, null);
		}

		void EventDetach()
		{
			Draw = null;
			s3eKeyboardUnRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyboard);
			if(BxCore::System::IsMultiTouchAvailable())
				s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, OnPointer_Multi);
			else s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, OnPointer);
		}

		int& ForMain_LetUpdateCount() {global_data int UpdateCount = 0; return UpdateCount;}
		int& ForMain_LetRenderCount() {global_data int RenderCount = 0; return RenderCount;}
		int& ForMain_LetFrameTime() {global_data int FrameTime = 50; return FrameTime;}
		int& ForMain_LetFrameDelay() {global_data int FrameDelay = 0; return FrameDelay;}
		rect& ForMain_LetGUIMargin() {global_data rect GUIMargin = {0, 0, 0, 0}; return GUIMargin;}

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

		uhuge GetTimerMilliSecond()
		{
			return (uhuge) s3eTimerGetUTC();
		}

		uhuge GetTimerNanoSecond()
		{
			#ifdef I3D_ARCH_X86
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

		uhuge GetTimerSimilarMicroSecond()
		{
			return GetTimerNanoSecond() >> 10;
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
					BxCore::Main::OnPointerMotion_Multi(&EventData, null);
				}
				else if(type != systouchtype_cancel)
				{
					s3ePointerTouchEvent EventData;
					EventData.m_TouchID = 0;
					EventData.m_Pressed = (type == systouchtype_down);
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointer_Multi(&EventData, null);
				}
			}
			else
			{
				if(type == systouchtype_move)
				{
					s3ePointerMotionEvent EventData;
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointerMotion(&EventData, null);
				}
				else if(type != systouchtype_cancel)
				{
					s3ePointerEvent EventData;
					EventData.m_Button = S3E_POINTER_BUTTON_SELECT;
					EventData.m_Pressed = (type == systouchtype_down);
					EventData.m_x = x;
					EventData.m_y = y;
					BxCore::Main::OnPointer(&EventData, null);
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

		void _CertifyConfigLoad()
		{
			global_data bool IsLoaded = false;
			if(IsLoaded) return;
			const bool LoadResult = BxKeyword::Load("config/default.key");
			BxAssert("BxCore::System<config/default.key파일을 로드할 수 없습니다>", LoadResult);
			IsLoaded = true;
		}

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
			#ifdef I3D_ARCH_X86
				if(BxDLL_WindowHandle)
					BxDLL_MoveWindow(BxDLL_WindowHandle, x, y, BxCore::Surface::GetWidthHW(false), BxCore::Surface::GetHeightHW(false), 1);
			#endif
		}

		point GetSimulatorWindowPos()
		{
			point SimulatorPos = point::zero();
			#ifdef I3D_ARCH_X86
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
			#ifdef I3D_ARCH_X86
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
			#ifdef I3D_ARCH_X86
				if(BxDLL_WindowHandle)
				{
					const int SW_MINIMIZE = 6;
					BxDLL_ShowWindow(BxDLL_WindowHandle, SW_MINIMIZE);
				}
			#endif
		}

		void Assert(string name, bool& IsIgnore, bool flag, string filename, int linenumber)
		{
			if(IsIgnore || flag) return;
			switch(IwDebugAssertShow(name, filename, linenumber, 1))
			{
			case S3E_ERROR_SHOW_STOP:
				do{if(s3eDebugIsDebuggerPresent()) IwDebugBreak(); else abort();} while(false);
				break;
			case S3E_ERROR_SHOW_IGNORE:
				IsIgnore = true;
				break;
			}
		}

		string GetOSName()
		{
			return s3eDeviceGetString(S3E_DEVICE_OS);
		}

		string GetArchitectureName()
		{
			return s3eDeviceGetString(S3E_DEVICE_ARCHITECTURE);
		}

		string GetPhoneNumber()
		{
			return s3eDeviceGetString(S3E_DEVICE_PHONE_NUMBER);
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
						BxCore::Util::MemMove(Data, data, datasize);
					}
					else Data = null;
				}
				else Data = data;
				Next = null;
			}
			virtual ~CallbackList()
			{
				if(!IsDataRef && Data) BxFree(Data);
				if(Next) BxDelete(Next);
			}
			global_func CallbackList* GetFirst()
			{
				global_data CallbackList First(null, 0, null, -1);
				return &First;
			}
		};

		void ForMain_CallbackRun()
		{
			CallbackList* List = CallbackList::GetFirst();
			while(List->Next)
			{
				if(!List->Next->Method(List->Next->Value, List->Next->Data))
				{
					CallbackList* DeleteList = List->Next;
					List->Next = DeleteList->Next;
					DeleteList->Next = null;
					BxDelete(DeleteList);
				}
				else List = List->Next;
			}
		}

		void AddCallback(callback_frame method, int value, void* data, int datasize)
		{
			CallbackList* List = (CallbackList*) CallbackList::GetFirst();
			while(List->Next) List = List->Next;
			List->Next = BxNew_Param(CallbackList, method, value, data, datasize);
		}

		bool BeginChildProcess(callback_process process)
		{
			if(ChildProcess) return false;
			ChildProcess = process;
			return true;
		}

		bool IsExistClassJNI(string classpath)
		{
			if(!s3eAndroidJNIAvailable()) return false;
			JavaVM* JVM = (JavaVM*) s3eAndroidJNIGetVM();
			if(!JVM) return false;
			JNIEnv* ENV = null;
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
			JNIEnv* ENV = null;
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
			JNIEnv* ENV = null;
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

		local_func inline callback_edk& _EDKClient() {global_data callback_edk EDKClient = null; return EDKClient;}

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

		callback_edk SetCallbackEDK(callback_edk methodCB)
		{
			_EDKClient() = methodCB;
			return _EDKReceiver;
		}
	}
	namespace Surface
	{
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
				s3eSurfaceSetup(Type, 0, null, Direction);
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
				IwGxSetColStream(null);
				IwGxSetColClear(0x00, 0x00, 0x00, 0xFF);
				BxCore::Surface::SetRenderMode(rendermode_2d);
				IwGxClear();
				if(GetWidth() != GetWidthHW() || GetHeight() != GetHeightHW())
				{
					if(!OffscreenSurface().CreateSurface(null, GetWidth(), GetHeight(),
						CIwGxSurface::EXACT_MATCH_F | CIwGxSurface::PERSISTENT_F))
						BxAssert("BxCore::Surface<FBO의 초기화에 실패하였습니다>", false);
					else
					{
						IsOffscreenSurfaceValid() = true;
						OffscreenSurface().MakeCurrent();
						IwGxLightingOff();
						//IwGxMipMappingOff();
						IwGxFogOff();
						IwGxSetColStream(null);
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
					IwGxDrawPrims(IW_GX_QUAD_LIST, null, 4);
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

		int GetWidth(bool* isfixedvalue, bool usemargin)
		{
			global_data int SurfaceWidth = -1;
			if(SurfaceWidth == -1)
			{
				if(BxUtilGlobal::StrCmp(BxCore::System::GetOSName(), "WINDOWS") != same
					&& BxUtilGlobal::StrCmp(BxCore::System::GetArchitectureName(), "X86") != same
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
					&& BxUtilGlobal::StrCmp(BxCore::System::GetArchitectureName(), "X86") != same
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
	namespace Sound
	{
		class SoundData : public BxAutoBuffer<autobuffer_sound>
		{
		private:
			bool IsAudio;
			string FileName;
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
						System::AddCallback(EffectChannelCB, i, null);
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
				WaveData = null;
				WaveLength = 0;
				SamplesPerSec = s3eSoundGetInt(S3E_SOUND_DEFAULT_FREQ);
				if(!autoload)
				{
					id_file File = File::Open(FileName, "rb");
					BxAssert("BxCore::Sound<존재하지 않는 파일입니다>", File);
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
					BxAssert("BxCore::Sound<존재하지 않는 파일입니다>", File);
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
				ChannelState(channel, true) = null;
				BxAssert("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !((SoundData*) sound)->IsSoundDataByAudio());
				((SoundData*) sound)->LockSoundData();
				s3eSoundChannelSetInt(channel, S3E_CHANNEL_RATE, ((SoundData*) sound)->SamplesPerSec);
				return s3eSoundChannelPlay(channel, ((SoundData*) sound)->GetSoundData(), ((SoundData*) sound)->GetSoundDataCount(), 1, 0);
			}

			local_func void EffectChannelPlayReserve(int channel, id_sound sound)
			{
				SoundData* Sound = (SoundData*) ChannelState(channel, true);
				if(Sound)
				{
					BxAssert("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !Sound->IsSoundDataByAudio());
					Sound->LockSoundData();
					s3eSoundChannelSetInt(channel, S3E_CHANNEL_RATE, Sound->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(channel, Sound->GetSoundData(), Sound->GetSoundDataCount(), 1, 0);
					BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				ChannelState(channel, false) = ChannelState(channel, true);
				ChannelState(channel, true) = sound;
			}

			local_func s3eResult EffectChannelStop(int channel)
			{
				SoundData* CurSound = (SoundData*) SoundData::ChannelState(channel, false);
				if(CurSound) CurSound->UnlockSoundData();
				ChannelState(channel, false) = null;
				ChannelState(channel, true) = null;
				return s3eSoundChannelStop(channel);
			}

			local_func id_sound& ChannelState(int channel, bool isReserve)
			{
				global_data id_sound Channel[32][2] = {{null,},};
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
				SoundData::ChannelState(Channel, false) = null;
				// 처리
				SoundData* Sound = (SoundData*) ChannelState(Channel, true);
				if(Sound)
				{
					BxAssert("BxCore::Sound<효과음은 Audio포맷이 지원불가합니다>", !Sound->IsSoundDataByAudio());
					Sound->LockSoundData();
					s3eSoundChannelSetInt(Channel, S3E_CHANNEL_RATE, Sound->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(Channel, Sound->GetSoundData(), Sound->GetSoundDataCount(), 1, 0);
					BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				ChannelState(Channel, false) = ChannelState(Channel, true);
				ChannelState(Channel, true) = null;
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
					if(ChannelState(i, false) == null && ChannelState(i, true) == null)
						return i;
				return 0;
			}

			local_func int GetSameEffectChannel(id_sound sound)
			{
				for(int i = GetEffectChannelBegin(); i < GetChannelCount(); ++i)
					if(ChannelState(i, false) == sound && ChannelState(i, true) == null)
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
					bool IsReserve = (Sound2 != null);
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

		class PanoramaList
		{
			id_sound Sound;
			PanoramaList* Next;
		public:
			enum Command {None, RemoveAll, GoNext, GoFirst};

			PanoramaList()
			{
				Sound = null;
				Next = this;
				System::AddCallback(MusicChannelCB, 0, null);
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
				SoundData::ChannelState(0, false) = null;
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
						BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
					}
					else
					{
						s3eSoundChannelSetInt(0, S3E_CHANNEL_RATE, CurSound->SamplesPerSec);
						s3eResult Result = s3eSoundChannelPlay(0, CurSound->GetSoundData(), CurSound->GetSoundDataCount(), 1, 0);
						BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
					}
				}
				return true;
			}
		};

		id_sound Create(string filename, bool autoload)
		{
			const int Length = BxUtilGlobal::StrLen(filename);
			BxAssert("BxCore::Sound<파일명이 너무 짧습니다>", 4 <= Length);
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
					BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
				}
				else
				{
					s3eSoundChannelSetInt(0, S3E_CHANNEL_RATE, ((SoundData*) sound)->SamplesPerSec);
					s3eResult Result = s3eSoundChannelPlay(0, ((SoundData*) sound)->GetSoundData(), ((SoundData*) sound)->GetSoundDataCount(), 1, 0);
					BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
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
			SoundData::ChannelState(0, false) = null;
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
				BxAssert("BxCore::Sound<해당 사운드를 출력할 수 없습니다>", Result != S3E_RESULT_ERROR);
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
	namespace Util
	{
		id_memory Malloc(int size)
		{
			return (id_memory) s3eMalloc(size);
		}

		void Free(id_memory item)
		{
			s3eFree((void*) item);
		}

		void* GetPtr(id_memory item)
		{
			return (void*) item;
		}

		void MemMove(const void* dst, const void* src, int length)
		{
			if(dst) memmove((void*) dst, src, length);
		}

		void MemSet(const void* dst, byte value, int length)
		{
			if(dst) memset((void*) dst, value, length);
		}

		int MemCmp(const void* dst, const void* src, int length)
		{
			return memcmp(dst, src, length);
		}

		string _tmp_ Print(string src, const BxThrow& args, int* length)
		{
			global_data int ResultSize = 1024;
			string_rw Result = null;
			BxSINGLETON(Result, ResultSize);
			string_rw Focus = Result;
			int DefaultArgID = 0;
			if(src--)
			while(*(++src) != '\0')
			{
				if(*src == '<' && *(++src) != '<')
				{
					int Count = 0;
					while(src[Count] != '\0' && src[Count] != '>') ++Count;
					if(src[Count] == '\0')
					{
						BxAssert("BxUtilGlobal::StrFmt<미완성 태그가 존재합니다>", false);
						break;
					}
					--Count;
					switch(*(src++))
					{
					case 'A':
						if(0 < args.Length())
						{
							// 인수매칭
							string SubSrc = src;
							while(*SubSrc != ':' && *SubSrc != '>') ++SubSrc;
							const int ArgID = ((SubSrc - src)? AtoI(src, SubSrc - src) : DefaultArgID) % args.Length();
							// 공간제약
							char SpaceCode[2] = {' ', ' '};
							int SpaceSize[2] = {-1, -1};
							if(*SubSrc == ':')
							{
								bool IsRight = false;
								while(*(++SubSrc) != '>')
								{
									if('0' <= *SubSrc && *SubSrc <= '9')
									{
										if(SpaceSize[IsRight] == -1)
										{
											SpaceSize[IsRight] = *SubSrc - '0';
											if(*SubSrc == '0') SpaceCode[IsRight] = '0';
										}
										else SpaceSize[IsRight] = SpaceSize[IsRight] * 10 + (*SubSrc - '0');
									}
									else if(*SubSrc == '.') IsRight = true;
									else SpaceCode[IsRight] = *SubSrc;
								}
							}
							// 인수수집
							void* ArgData = null;
							string ArgString[2] = {null, null};
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
							else if(ArgData = args.Access<double>(ArgID))
							{
								ArgString[0] = ItoA((int) *((double*) ArgData));
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
								ArgString[0] = (string) ArgData; // 특수화
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else if(ArgData = args.Access<BxString>(ArgID))
							{
								ArgString[0] = (string) *((BxString*) ArgData);
								ArgStringLength[0] = BxUtilGlobal::StrLen(ArgString[0]);
							}
							else BxAssert("BxUtilGlobal::StrFmt<지원하지 않는 타입의 인수입니다>", false);
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
									bool IsRight = (ArgString[1] != null);
									int SpaceCount = SpaceSize[1] - ArgStringLength[IsRight];
									if(0 < SpaceCount)
									{
										Focus += BxUtilGlobal::StrCpy(Focus, ArgString[IsRight], ArgStringLength[IsRight]);
										while(SpaceCount--) *(Focus++) = SpaceCode[1];
									}
									else Focus += BxUtilGlobal::StrCpy(Focus, ArgString[IsRight], ArgStringLength[IsRight] + SpaceCount);
								}
							}
							++DefaultArgID;
						}
						else BxAssert("BxUtilGlobal::StrFmt<참조할 인수가 없습니다>", false);
						break;
					case 'B': if(Count == 0) *(Focus++) = '\b'; break;
					case 'D': if(Count == 1 && *src == 'Q') *(Focus++) = '\"'; break;
					case 'F': if(Count == 1 && *src == 'S') *(Focus++) = '/'; break;
					case 'N': if(Count == 0) *(Focus++) = '\n'; break;
					case 'R': if(Count == 0) *(Focus++) = '\r'; else if(Count == 1 && *src == 'S') *(Focus++) = '\\'; break;
					case 'S': if(Count == 1 && *src == 'Q') *(Focus++) = '\''; break;
					case 'T': if(Count == 0) *(Focus++) = '\t'; break;
					case 'X':
						for(int i = 0; i + 1 < Count; i += 2)
						{
							if('0' <= src[i] && src[i] <= '9')
								*Focus = ((src[i] - '0') & 0x0F) << 4;
							else if('A' <= src[i] && src[i] <= 'F')
								*Focus = ((src[i] - 'A' + 10) & 0x0F) << 4;
							else *Focus = 0;
							if('0' <= src[i + 1] && src[i + 1] <= '9')
								*(Focus++) |= ((src[i + 1] - '0') & 0x0F);
							else if('A' <= src[i + 1] && src[i + 1] <= 'F')
								*(Focus++) |= ((src[i + 1] - 'A' + 10) & 0x0F);
							else ++Focus;
						}
						break;
					case 'Z': if(Count == 0) *(Focus++) = '\0'; break;
					default: BxAssert("BxUtilGlobal::StrFmt<알 수 없는 태그입니다>", false); break;
					}
					src += Count;
				}
				else *(Focus++) = *src;
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

		int Printf(string src, ...)
		{
			va_list List;
			va_start(List, src);
			int Result = vprintf(src, List);
			va_end(List);
			return Result;
		}

		int SnPrintf(string_rw dst, int length, string src, ...)
		{
			va_list List;
			va_start(List, src);
			const int Result = VsnPrintf(dst, length, src, List);
			va_end(List);
			return Result;
		}

		int VsnPrintf(string_rw dst, int length, string src, va_list list)
		{
			const int Result = vsnprintf(dst, length, src, list);
			BxAssert("BxCore::Util<DST의 길이가 부족합니다>", !dst || (0 <= Result && Result < length));
			BxAssert("BxCore::Util<DST의 예상길이를 얻는데 실패하였습니다>", dst || 0 <= Result);
			return Result;
		}

		uint __CallStack__[20480] = {0,};
		uint* __CallStackFocus__ = __CallStack__;
		uint __CallCount__ = 0;

		#if !defined(__GNUC__) && !defined(__ARMCC_VERSION)
		// 컴파일옵션필요(/Gh)
		extern "C" void __declspec(naked) _cdecl _penter(void)
		{
			_asm
			{
				push edi
				push esi
					mov edi, __CallStackFocus__
					mov esi, [esp] + 8
					mov [edi], esi
					mov esi, __CallCount__
					mov [edi + 4], esi
				pop esi
				pop edi
				inc __CallCount__
				add __CallStackFocus__, 8
				ret
			}
		}
		// 컴파일옵션필요(/GH)
		extern "C" void __declspec(naked) _cdecl _pexit(void)
		{
			_asm
			{
				sub __CallStackFocus__, 8
				ret
			}
		}
		#endif

		uint GetCurrentCallDepth()
		{
			if(!__CallCount__) return 0;
			return ((__CallStackFocus__ - __CallStack__) >> 1) - 2;
		}

		void* GetCurrentCallAddress()
		{
			if(!__CallCount__) return null;
			return (void*) __CallStackFocus__[-4];
		}

		uint GetCurrentCallCount()
		{
			if(!__CallCount__) return 0;
			return __CallStackFocus__[-3];
		}

		void SetCallCount(uint value)
		{
			if(!__CallCount__) return;
			__CallCount__ = value;
		}

		// 박스함수관련
		static int _function_cur_depth = 0;
		static int _function_log_depth = 0;
		static const int _function_log_depth_max = 32;
		static bool _function_is_before_endline = true;
		static const char* _function_depth_blank[_function_log_depth_max] = {
		"0...",
		"0...1...",
		"0...1...2...",
		"0...1...2...3...",
		"0...1...2...3...4...",
		"0...1...2...3...4...5...",
		"0...1...2...3...4...5...6...",
		"0...1...2...3...4...5...6...7...",
		"0...1...2...3...4...5...6...7...8...",
		"0...1...2...3...4...5...6...7...8...9...",
		"0...1...2...3...4...5...6...7...8...9..10...",
		"0...1...2...3...4...5...6...7...8...9..10...1...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...7...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...7...8...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...7...8...9...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...7...8...9..30...",
		"0...1...2...3...4...5...6...7...8...9..10...1...2...3...4...5...6...7...8...9..20...1...2...3...4...5...6...7...8...9..30...1..."};

		void PushCallDepthAndLog(string file, const int line, string func)
		{
			if(_function_cur_depth++ < _function_log_depth)
				Printf("%s%s%s                ........%s(%dLn), %I64ds\r%s{\r",
					(_function_is_before_endline)? "\r" : "",
					_function_depth_blank[_function_cur_depth - 1], func, file, line,
					BxCore::System::GetTimerNanoSecond(),
					_function_depth_blank[_function_cur_depth - 1]);
			_function_is_before_endline = false;
		}

		void PopCallDepthAndLog(string file, const int line)
		{
			if(--_function_cur_depth < _function_log_depth)
				Printf("%s}                ........%s(%dLn), %I64ds\r",
					_function_depth_blank[_function_cur_depth], file, line,
					BxCore::System::GetTimerNanoSecond());
			_function_is_before_endline = true;
		}
	}
	namespace AddOn
	{
		/*const byte* JPGToBMP(const byte* jpg)
		{
			typedef struct {uint Size; ushort Reserved1; ushort Reserved2; uint OffBits;} BITMAPFILEHEADER;
			typedef struct {uint Size; int Width; int Height; ushort Planes; ushort BitCount; uint Compression;
				uint SizeImage; int XPelsPerMeter; int YPelsPerMeter; uint ClrUsed; uint ClrImportant;} BITMAPINFOHEADER;

			struct jpeg_error_mgr jerr;
			struct jpeg_decompress_struct cinfo;
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);
			jpeg_mem_src(&cinfo, jpg, 15000);
			jpeg_read_header(&cinfo, true);

			const int _width = cinfo.image_width;
			const int _height = cinfo.image_height;
			cinfo.out_color_space = JCS_RGB;
			cinfo.out_color_components = 3;

			const int BmpRow = (3 * _width + 3) & ~3;
			byte* Bmp = (byte*) BxAlloc(2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BmpRow * _height);
			Bmp[0] = 'B';
			Bmp[1] = 'M';
			BITMAPFILEHEADER* BmpFileHeader = (BITMAPFILEHEADER*)(Bmp + 2);
			BmpFileHeader->Size = 54 + BmpRow * _height;
			BmpFileHeader->Reserved1 = 0;
			BmpFileHeader->Reserved2 = 0;
			BmpFileHeader->OffBits = 54;
			BITMAPINFOHEADER* BmpInfoHeader = (BITMAPINFOHEADER*)(Bmp + 2 + sizeof(BITMAPFILEHEADER));
			BmpInfoHeader->Size = 40;
			BmpInfoHeader->Width = _width;
			BmpInfoHeader->Height = _height;
			BmpInfoHeader->Planes = 1;
			BmpInfoHeader->BitCount = 24;
			BmpInfoHeader->Compression = 0;
			BmpInfoHeader->SizeImage = BmpRow * _height;
			BmpInfoHeader->XPelsPerMeter = 3780;
			BmpInfoHeader->YPelsPerMeter = 3780;
			BmpInfoHeader->ClrUsed = 0;
			BmpInfoHeader->ClrImportant = 0;

			jpeg_start_decompress(&cinfo);
			JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo,
				JPOOL_IMAGE, BmpRow, 1);
			byte* BmpFocus = Bmp + 2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
				+ BmpRow * (_height - 1);
			for(int y = _height; y--; BmpFocus -= BmpRow)
			{
				jpeg_read_scanlines(&cinfo, buffer, 1);
				for(int x = 0; x < BmpRow; x += 3)
				{
					BmpFocus[x + 0] = (*buffer)[x + 2];
					BmpFocus[x + 1] = (*buffer)[x + 1];
					BmpFocus[x + 2] = (*buffer)[x + 0];
				}
			}
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return Bmp;
		}*/

		typedef const byte* (*CoreJPGToBMP)(const byte* jpg);
		const byte* DefaultJPGToBMP(const byte* jpg)
		{BxAssert("BxCore::AddOn<해당 JPGToBMP확장함수가 없습니다>", false); return null;}
		void* _inout_ ForExtend_JPGToBMP() {static void* Function = (void*) DefaultJPGToBMP; return Function;}
		const byte* JPGToBMP(const byte* jpg)
		{
			return ((CoreJPGToBMP) ForExtend_JPGToBMP())(jpg);
		}

		typedef const byte* (*CoreGIFToBMP)(const byte* gif, const int length, int _out_ numpage);
		const byte* DefaultGIFToBMP(const byte* gif, const int length, int _out_ numpage)
		{BxAssert("BxCore::AddOn<해당 GIFToBMP확장함수가 없습니다>", false); return null;}
		void* _inout_ ForExtend_GIFToBMP() {static void* Function = (void*) DefaultGIFToBMP; return Function;}
		const byte* GIFToBMP(const byte* gif, const int length, int _out_ numpage)
		{
			return ((CoreGIFToBMP) ForExtend_GIFToBMP())(gif, length, numpage);
		}

		typedef const byte* (*CoreHQXToBMP)(const byte* bmp, int scale);
		const byte* DefaultHQXToBMP(const byte* bmp, int scale)
		{BxAssert("BxCore::AddOn<해당 HQXToBMP확장함수가 없습니다>", false); return null;}
		void* _inout_ ForExtend_HQXToBMP() {static void* Function = (void*) DefaultHQXToBMP; return Function;}
		const byte* HQXToBMP(const byte* bmp, int scale)
		{
			return ((CoreHQXToBMP) ForExtend_HQXToBMP())(bmp, scale);
		}

		void ReleaseBMP(const byte* bmp)
		{
			BxFree(bmp);
		}
	}
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
			BxAssert("BxCore::File<파일핸들이 null입니다>", file);
			s3eFileClose((s3eFile*) file);
		}

		uint Read(id_file file, void* buffer, uint length, callback_progress progress)
		{
			BxAssert("BxCore::File<파일핸들이 null입니다>", file);
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

		uint Write(id_file file, const void* buffer, uint length, callback_progress progress)
		{
			BxAssert("BxCore::File<파일핸들이 null입니다>", file);
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

		uint WriteString(id_file file, string buffer, callback_progress progress)
		{
			return Write(file, buffer, BxUtilGlobal::StrLen(buffer), progress);
		}

		int Skip(id_file file, uint length)
		{
			BxAssert("BxCore::File<파일핸들이 null입니다>", file);
			return s3eFileSeek((s3eFile*) file, length, S3E_FILESEEK_CUR);
		}

		int GetSize(id_file file)
		{
			BxAssert("BxCore::File<파일핸들이 null입니다>", file);
			return s3eFileGetSize((s3eFile*) file);
		}

		int GetSize(string filename)
		{
			s3eFile* File = s3eFileOpen(filename, "rb");
			const int Result = (File)? s3eFileGetSize(File) : 0;
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

		void RemoveFile(string filename, bool doRemoveBlankedDirectory)
		{
			if(s3eFileDelete(filename) != S3E_RESULT_SUCCESS)
				s3eFileGetError();
			else if(doRemoveBlankedDirectory)
			{
				BxString BlankedDirectoryName(filename);
				while(s3eFileDeleteDirectory(BlankedDirectoryName.GetFilePath()) == S3E_RESULT_SUCCESS)
				{
					BlankedDirectoryName = BlankedDirectoryName.GetFilePath();
					BlankedDirectoryName = BlankedDirectoryName.Left(BlankedDirectoryName.GetLength() - 1);
				}
			}
		}
	}
	namespace Socket
	{
		#ifdef I3D_ARCH_X86
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
						BxAssert(BxString("BxCore::Socket<소켓생성 실패(%d)>", null, ErrorCode), false);
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
					Client = null;
					Server = null;
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
						BxAssert(BxString("BxCore::Socket<소켓생성 실패(%d)>", null, ErrorCode), false);
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
					Client = null;
					Server = null;
					State = socketstate_null;
				}
			};
		#endif

		#ifdef I3D_ARCH_X86
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
							BxAssert(BxString("BxCore::Socket<소켓생성 실패(%d)>", null, ErrorCode), false);
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
					Message.icmp_id = (ushort) (BxCore::System::GetTimerMilliSecond() & 0xFFFF);
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
						LastSendTime = BxCore::System::GetTimerMilliSecond();
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
						return (int) (BxCore::System::GetTimerMilliSecond() - LastSendTime);
					}
					return SOCKET_ERROR;
				}
			};
		#endif

		id_socket Create()
		{
			TCPData* Data = BxNew(TCPData);
			if(Data->GetState() == socketstate_created)
				return (id_socket) Data;
			BxDelete(Data);
			return null;
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

		#ifdef I3D_ARCH_X86
			int _GetErrorCode(id_socket sock)
			{
				const int ErrorCode = BxDLL_WSAGetLastError();
				if(ErrorCode == WSAEWOULDBLOCK) return 0;
				switch(ErrorCode)
				{
				case WSAENOTSOCK: BxCore::Util::Printf("##### Socket ErrorCode : WSAENOTSOCK #####\r\n"); break;
				case WSAESHUTDOWN: BxCore::Util::Printf("##### Socket ErrorCode : WSAESHUTDOWN #####\r\n"); break;
				case WSAENOTCONN: BxCore::Util::Printf("##### Socket ErrorCode : WSAENOTCONN #####\r\n"); break;
				case WSAECONNABORTED: BxCore::Util::Printf("##### Socket ErrorCode : WSAECONNABORTED #####\r\n"); break;
				case WSAECONNRESET: BxCore::Util::Printf("##### Socket ErrorCode : WSAECONNRESET #####\r\n"); break;
				default: BxCore::Util::Printf("##### Socket ErrorCode : %d #####\r\n", ErrorCode); break;
				}
				switch(ErrorCode)
				{
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
				case S3E_SOCKET_ERR_PARAM: BxCore::Util::Printf("##### Socket ErrorCode : S3E_SOCKET_ERR_PARAM #####\r\n"); break;
				case S3E_SOCKET_ERR_SHUTDOWN: BxCore::Util::Printf("##### Socket ErrorCode : S3E_SOCKET_ERR_SHUTDOWN #####\r\n"); break;
				case S3E_SOCKET_ERR_NOTCONN: BxCore::Util::Printf("##### Socket ErrorCode : S3E_SOCKET_ERR_NOTCONN #####\r\n"); break;
				default: BxCore::Util::Printf("##### Socket ErrorCode : %d #####\r\n", ErrorCode); break;
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
			BxAssert("BxCore::Socket<알 수 없는 도메인입니다>", HostEnt);
			if(!HostEnt) return connect_error_param;
			#ifdef I3D_ARCH_X86
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
				BxDLL_select(FD + 1, null, &fdset, null, &tmval);
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
					uhuge LimitTime = BxCore::System::GetTimerMilliSecond() + timeout;
					while(BxCore::System::GetTimerMilliSecond() < LimitTime && ((TCPData*) sock)->GetState() == socketstate_connecting)
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
			#ifdef I3D_ARCH_X86
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
			#ifdef I3D_ARCH_X86
				int Result = BxDLL_recv(TCP->GetServer(), (string_rw) buffer, len, 0);
			#else
				int Result = s3eSocketRecv(TCP->GetServer(), (string_rw) buffer, len, 0);
			#endif
			if(0 <= Result) return Result;
			return _GetErrorCode(sock);
		}

		int RecvFully(id_socket sock, byte* buffer, int len, int timeout, bool cancelmode)
		{
			BxAssert("BxCore::Socket<파라미터 len은 1 이상이어야 합니다>", 1 <= len);
			if(timeout < 0) timeout = 5000;
			int RecvResult = 0, RecvedLength = 0, ErrorTime = 0, SleepTime = 0;
			while(0 <= (RecvResult = Recv(sock, buffer + RecvedLength, len - RecvedLength)))
			{
				RecvedLength += RecvResult;
				if(0 < RecvResult)
				{
					ErrorTime = 0;
					SleepTime >>= 1;
				}
				if(RecvedLength == len
					|| (RecvedLength == 0 && cancelmode)
					|| timeout < (ErrorTime += SleepTime))
					return RecvedLength;
				else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, timeout >> 6));
			}
			return RecvResult;
		}

		int Ping(string addr, uint timeout)
		{
			#ifdef I3D_ARCH_X86
				global_data ICMPData Sock;
				// IP조사
				hostent* HostEnt = gethostbyname(addr);
				BxAssert("BxCore::Socket<알 수 없는 도메인입니다>", HostEnt);
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
	namespace Font
	{
		local_func int& RefCount() {global_data int Count = 0; return Count;}
		local_func CIwGxFont*& TempFont() {global_data CIwGxFont* Font = 0; return Font;} // GxFont버그회피용

		id_font Open(string filename, const int pointsize, const int numCacheGlyphs)
		{
			#ifndef __BX_OPENGL
			return null;
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
			return null;
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
			global_data id_font LastFont = null;
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

	#ifdef __BX_OPENGL
	namespace OpenGL2D
	{
		// OpenGLForm
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

		// OpenGLOutline
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
						BxCore::Util::MemMove(&VBOData[PoolFocus], Vertex, sizeof(CIwSVec2) * 4);
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
						BxCore::Util::MemMove(&VBOData[PoolFocus], UV, sizeof(CIwSVec2) * 4);
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
			Result->Texture.CopyFromBuffer(width, height, CIwImage::ARGB_8888, sizeof(color_a888) * pitch, (uint8*) image, null);
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
			BxAssert("BxCore::OpenGL2D<파라미터 p의 count는 3 이상이어야 합니다>", 3 <= p.count);
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
				return null;
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
			BxAssert("BxCore::OpenGL2D<파라미터 v의 count는 3 이상이어야 합니다>", 3 <= v.count);
			BxAssert("BxCore::OpenGL2D<파라미터 v와 uv의 count는 동일해야 합니다>", v.count == uv.count);
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
			BxAssert("BxCore::OpenGL2D<파라미터가 ColorAmbient형이 아닙니다>", Form->Type == OpenGLForm::COLOR);
			return ((ColorAmbient*) Form)->Color;
		}

		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			OpenGLOutline* Outline = (OpenGLOutline*) outline;
			BxAssert("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
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
			BxAssert("BxCore::OpenGL2D<파라미터가 VertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::RECT);
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
			BxAssert("BxCore::OpenGL2D<파라미터가 PolyVertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::POLYGON);
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
			BxAssert("BxCore::OpenGL2D<파라미터가 PolyVertexUV형이 아닙니다>", Outline->Type == OpenGLOutline::POLYGON);
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

		void _RenderRect(OpenGLForm* TM, VertexUV* VUV, int x, int y, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_2d);
			// Material
			CIwMaterial* Material = null;
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
			IwGxDrawPrims(IW_GX_QUAD_LIST, null, 4);
		}

		void _RenderPoly(OpenGLForm* TM, PolyVertexUV* PolyVUV, int x, int y, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_2d);
			// Material
			CIwMaterial* Material = null;
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
			IwGxDrawPrims(IW_GX_QUAD_LIST, null, PolyVUV->Count);
		}

		void _RenderPoly3D(OpenGLForm* TM, PolyVertexUV3D* PolyVUV3D, int x, int y, int z, const byte opacity, const color_x888 color)
		{
			BxCore::Surface::SetRenderMode(rendermode_3d);
			// Material
			CIwMaterial* Material = null;
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
			IwGxDrawPrims(IW_GX_QUAD_LIST, null, PolyVUV3D->Count);
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
	#endif
}
/// @endcond
