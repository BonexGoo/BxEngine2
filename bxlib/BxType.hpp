#pragma once
/*!
\mainpage BxEngine 개요
\section developer 개발자
	- Bonex Gu
\section infomation 목적
	- 크로스플랫폼향 미들웨어 게임엔진
\section advenced 소개
	- Marmalade-SDK기반 게임엔진
	- OpenGL/SW랜더링 통합그래픽스
	- 독립형 Core-Interface 게임엔진
*/

// 컴파일러 특성
#ifndef nullptr
	#define nullptr (0)
#endif
#ifndef true
	#define true (1)
#endif
#ifndef false
	#define false (0)
#endif
#ifndef inline
	#define inline _inline
#endif
#ifndef _inline
	#define _inline inline
#endif

// 정의
#ifndef _BX_STATIC_DEFINE_
#define _BX_STATIC_DEFINE_
	#define _arr_ // 배열식으로 할당되고 삭제되는 포인터이다
	#define _ref_ // 원본을 참조하는 데이터이며 삭제를 금한다
	#define _tmp_ // 임시로 전달하는 데이터이며 저장, 삭제를 금한다
	#define _in_ // 오직 입력을 위한 파라미터이다
	#define _out_ & // 오직 출력을 위한 파라미터이다
	#define _inout_ & // 입력되고 출력되는 파라미터이다
	#define local_func static // 해당소스에서만 접근가능한 국지적 전역함수(CPP)
	#define local_data static // 해당소스에서만 접근가능한 국지적 전역변수(CPP)
	#define global_func static // 어디서든지 접근할 수 있는 클래스내 멤버함수(H)
	#define global_data static // 어디서든지 접근할 수 있는 함수내 정적변수(CPP/H)
	#define thread_storage static int // 스레드별 로컬스토리지(TLS) 연결변수(CPP/H)
	#if defined(__GNUC__) || defined(__ARMCC_VERSION)
		#define macro inline
	#else
		#define macro __forceinline
	#endif
#endif

// 타입-데이터
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef int fint; // Float-Int : 고정소수점(16/16)
typedef unsigned long mint; // Memory-Int : 메모리주소
typedef long long huge;
typedef unsigned long long uhuge;
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
	enum rsid:uhuge {rsid_null = 0x0000000000000000ULL, rsid_temp = 0xAAA00AAAAA000000ULL};
#else
	enum rsid:uhuge {rsid_null = 0x0000000000000000ui64, rsid_temp = 0xAAA00AAAAA000000ui64};
#endif

// 타입-색상
typedef unsigned short color_565;
typedef unsigned short color_m555_a444;
typedef unsigned int color_x888;
typedef unsigned int color_a888;
//! \brief color_bgra구조체 : b, g, r, a
typedef struct {byte b; byte g; byte r; byte a;} color_bgra;
#ifdef __BX_PIXEL16
	typedef color_565 pixel_dst;
	typedef color_m555_a444 pixel_src;
	#define _MASK_BIT_ 0x0020
	#define _OPA_BIT_ 0x8000
#else
	typedef color_x888 pixel_dst;
	typedef color_a888 pixel_src;
	#define _MASK_BIT_ 0x01000000
	#define _OPA_BIT_ 0xFF000000
#endif

// 열거
enum syseventtype:byte {syseventtype_init, syseventtype_quit, syseventtype_resize, syseventtype_key, syseventtype_touch, syseventtype_button};
enum syseventset:byte {syseventset_null, syseventset_do_enable, syseventset_need_enable};
enum syseventresult:byte {syseventresult_pass, syseventresult_done};
enum sysupdateresult:byte {sysupdateresult_no_render_fixed_frame = 0x01, sysupdateresult_no_render = 0x03, sysupdateresult_do_render_fixed_frame = 0x10, sysupdateresult_do_render = 0x30};
enum sysmethod:byte {sysmethod_get, sysmethod_remove};
enum sysrequest:byte {sysrequest_null, sysrequest_add, sysrequest_sub, sysrequest_set};
enum syskeytype:byte {syskeytype_down, syskeytype_repeat, syskeytype_up, syskeytype_cancel};
enum systouchtype:byte {systouchtype_down, systouchtype_move, systouchtype_up, systouchtype_cancel};
enum sysbuttontype:byte {sysbuttontype_down, sysbuttontype_drag, sysbuttontype_up, sysbuttontype_drop};
enum compare:int {param_is_less = -2, b_is_less = -1, same = 0, b_is_more = 1, param_is_more = 2};
enum keyword:int {keyword_null = -1, keyword_event, keyword_check, keyword_radio, keyword_number, keyword_string};
enum form:int {form_null = -1, form_sprite, form_gradation, form_stencil};
enum outline:int {outline_null = -1, outline_hotspot, outline_rectangle, outline_ellipse, outline_st_polygon, outline_bz_polygon};
enum motion:byte
{
	motion_null = 0, motion_frame, motion_sleep, motion_speed, motion_vector,
	motion_set, motion_if, motion_iffew, motion_ifrate, motion_ifcount, motion_else, motion_endif,
	motion_goto, motion_return, motion_end, motion_yield, motion_finish
};
enum layer:int {layer_null = -1, layer_image, layer_event, layer_frame};
enum maskoperate:int {maskoperate_default = -1, maskoperate_all_print = 0, maskoperate_no_print, maskoperate_print_by_1, maskoperate_print_by_0, maskoperate_max};
enum maskflag:int {maskflag_default = -1, maskflag_no_write = 0, maskflag_write_1, maskflag_write_0, maskflag_max};
enum drawturn:byte {turn_0, turn_x0, turn_90, turn_x90, turn_180, turn_x180, turn_270, turn_x270, turn_max};
enum drawblend:byte
{
	blend_normal = 0, blend_darken, blend_lighten, blend_multiply, blend_screen,
	blend_color_dodge, blend_color_burn, blend_linear_dodge, blend_linear_burn, blend_linear_light,
	blend_overlay, blend_hard_light, blend_pin_light, blend_hard_mix, blend_defferent,
	blend_exclusion, blend_gray, blend_add, blend_sub, blend_xor, blend_max
};
enum drawstyle:byte
{
	drawstyle_clip, drawstyle_mask, drawstyle_rotate, drawstyle_scale,
	drawstyle_color, drawstyle_hue, drawstyle_opacity, drawstyle_blend,
	drawstyle_form, drawstyle_event, drawstyle_move3d, drawstyle_max
};
enum surtype:byte {surtype_rgb565, surtype_rgb888};
enum dirtype:byte {dir_rot0, dir_rot90, dir_rot180, dir_rot270, dir_native};
enum keystate:byte {keystate_null = 0, keystate_down = 1, keystate_pressed = 2, keystate_released = 4};
enum keykind:byte
{
	keykind_null, keykind_esc, keykind_tab, keykind_space, keykind_bspace, keykind_enter,
	keykind_shift, keykind_ctrl, keykind_left, keykind_up, keykind_right, keykind_down,
	keykind_0, keykind_1, keykind_2, keykind_3, keykind_4,
	keykind_5, keykind_6, keykind_7, keykind_8, keykind_9,
	keykind_a, keykind_b, keykind_c, keykind_d, keykind_e,
	keykind_f, keykind_g, keykind_h, keykind_i, keykind_j,
	keykind_k, keykind_l, keykind_m, keykind_n, keykind_o,
	keykind_p, keykind_q, keykind_r, keykind_s, keykind_t,
	keykind_u, keykind_v, keykind_w, keykind_x, keykind_y, keykind_z,
	keykind_f1, keykind_f2, keykind_f3, keykind_f4, keykind_f5,
	keykind_f6, keykind_f7, keykind_f8, keykind_f9, keykind_f10,
	keykind_pad_0, keykind_pad_1, keykind_pad_2, keykind_pad_3, keykind_pad_4,
	keykind_pad_5, keykind_pad_6, keykind_pad_7, keykind_pad_8, keykind_pad_9,
	keykind_pad_plus, keykind_pad_minus, keykind_pad_enter,
	keykind_soft_left, keykind_soft_right,
	keykind_shoulder_left, keykind_shoulder_right,
	keykind_star, keykind_sharp, keykind_ok, keykind_clr,
	keykind_vol_up, keykind_vol_down, keykind_max
};
enum fonttype:byte {fonttype_kor, fonttype_eng, fonttype_both};
enum fontarea:byte {fontarea_fore, fontarea_bold, fontarea_both};
enum fontoption:byte {fontoption_style, fontoption_lineup, fontoption_code};
enum fontoptionvalue:byte
{
	fontoptionvalue_style_normal = 0, fontoptionvalue_style_wide = 1, fontoptionvalue_style_outline4 = 2, fontoptionvalue_style_outline8 = 3,
	fontoptionvalue_lineup_fix = 0, fontoptionvalue_lineup_each = 1,
	fontoptionvalue_code_euc_kr = 0, fontoptionvalue_code_utf_16be = 1
};
enum fontcomponent:byte {fontcomponent_gap_width, fontcomponent_blank_width, fontcomponent_eng_adjust_y};
enum fontkorgroup:byte {fontgroup_8_4_4 = 0, fontgroup_10_11_5 = 1, fontgroup_11_14_6 = 2};
enum fontencoding:byte {fontencoding_cp949, fontencoding_utf8, fontencoding_iso2022kr, fontencoding_iso2022jp};
enum fontsort:byte {fontsort_pad1, fontsort_pad2, fontsort_pad3, fontsort_pad4, fontsort_pad5, fontsort_pad6, fontsort_pad7, fontsort_pad8, fontsort_pad9};
enum socketstate:byte {socketstate_null, socketstate_created, socketstate_connecting, socketstate_connected, socketstate_disconnected};
enum connectresult:int {connect_connected = 2, connect_connecting = 1, connect_disconnected = 0,
	connect_error_param = -1, connect_error_already = -2, connect_error_netdown = -3,
	connect_error_timeout = -4, connect_error_refused = -5, connect_error_wrong_address = -6, connect_error_unknown = -7};
enum autobuffer:byte {autobuffer_sound, autobuffer_user};
enum autorelease:byte {autorelease_global, autorelease_user};
enum oskeyboardtype:byte {oskeyboardtype_base, oskeyboardtype_email, oskeyboardtype_number, oskeyboardtype_password, oskeyboardtype_url};
enum rendermode:byte {rendermode_null, rendermode_2d, rendermode_3d};
enum bindstate:byte {bindstate_binded, bindstate_bind, bindstate_entrust, bindstate_unbind};

// 타입-키워드
typedef const char* event;
typedef unsigned char check;
typedef unsigned char radio;
typedef int number;
typedef const char* string;
typedef char* string_rw;
typedef const ushort* wstring;
typedef ushort* wstring_rw;
typedef ushort wchar;
typedef const char* __restrict string_prm;
typedef char* __restrict string_rw_prm;
typedef const ushort* __restrict wstring_prm;
typedef ushort* __restrict wstring_rw_prm;
// 타입-좌표
//! \brief point구조체 : x, y
typedef struct point
{
	int x; int y;
	global_func inline const point& zero() {global_data const point p = {0, 0}; return p;}
	point& operator=(const point& rhs) {x = rhs.x; y = rhs.y; return *this;}
} point;
//! \brief pointf구조체 : x, y
typedef struct pointf {fint x; fint y;} pointf;
//! \brief size구조체 : w, h
typedef struct size
{
	int w; int h;
	enum {half = 1024, max = half * 2};
	global_func inline const size& zero() {global_data const size s = {0, 0}; return s;}
	global_func inline const size& full() {global_data const size s = {max, max}; return s;}
	size& operator=(const size& rhs) {w = rhs.w; h = rhs.h; return *this;}
} size;
//! \brief rect구조체 : l, t, r, b
typedef struct rect
{
	int l; int t; int r; int b;
	inline const int width() const {return r - l;}
	inline const int height() const {return b - t;}
	global_func inline const rect& zero() {global_data const rect r = {0, 0, 0, 0}; return r;}
	rect& operator=(const rect& rhs) {l = rhs.l; t = rhs.t; r = rhs.r; b = rhs.b; return *this;}
} rect;
//! \brief rectf구조체 : l, t, r, b
typedef struct rectf {fint l; fint t; fint r; fint b;} rectf;
//! \brief vertex구조체 : x, y, z, nouse(구조체2승수맞춤)
typedef struct vertex {int x; int y; int z; int nouse;} vertex;
//! \brief quater구조체 : x, y, z, angle1024
typedef struct quater {int x; int y; int z; int angle1024;} quater;
//! \brief uv구조체 : u, v
typedef struct uv {int u; int v;} uv;
//! \brief face구조체 : va, vb, vc, ua, ub, uc, nouse(구조체2승수맞춤)
typedef struct face {short va; short vb; short vc; short ua; short ub; short uc; int nouse;} face;
//! \brief matrix구조체 : m[4][4]
typedef struct matrix {fint m[4][4];} matrix;
//! \brief area구조체 : w, h, hx, hy
typedef struct area {short w; short h; short hx; short hy;} area;
//! \brief points구조체 : count, {x, y}, ...
typedef struct points {enum {max = 1020}; int count; point pt[max];} points;
//! \brief vertexs구조체 : count, {x, y, z}, ...
typedef struct vertexs {enum {max = 1020}; int count; vertex vt[max];} vertexs;
// 타입-ID/기타
typedef void* id_font;
typedef void* id_sound;
typedef void* id_file;
typedef void* id_socket;
typedef void* id_library;
typedef void* id_object;
typedef void* id_opengl_form;
typedef void* id_opengl_outline;
typedef void* id_static;
typedef void* id_zip;
typedef void* id_thread;
typedef void* id_mutex;
typedef bool delete_me;
//! \brief OnEvent()의 이벤트파라미터
typedef struct sysevent
{
	syseventtype type;
	struct {uint w; uint h;} init;
	struct {} quit;
	struct {uint w; uint h;} resize;
	struct {syskeytype type; keykind code;} key;
	struct {systouchtype type; uint id; int x; int y;} touch;
	struct {sysbuttontype type; string name; rect field; int x; int y;} button;
} sysevent;
// 콜백-프레임
typedef bool (*callback_frame)(int value, void* data);
// 콜백-이벤트
typedef void (*callback_event)(bool isautocall, string name, void* param);
// 콜백-JNI
typedef int (*callback_jni)(string param1_str256, int param2);
// 콜백-EDK
typedef void (*callback_edk)(string param1_str, int param2);
// 콜백-프로시저
typedef bool (*callback_procedure)(int sleep);
// 콜백-프로세스
typedef void (*callback_process)(callback_procedure yield);
// 콜백-프로그레스(pos가 -1이면 arg은 yield시간을 의미)
typedef bool (*callback_progress)(int pos, mint arg);
// 콜백-직렬화
typedef void (*callback_serialize)(bindstate state, string comment);
// 콜백-파일검색
typedef void (*callback_filesearch)(bool isdirname, string _tmp_ name, void* data);
// 콜백-스레드
typedef void (*callback_thread)(void* data);

// 상수
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
	#define ox100000000 0x100000000LL
#else
	#define ox100000000 0x100000000i64
#endif

// 문자열 상수
#ifndef _BX_STRING_LITERAL_
#define _BX_STRING_LITERAL_
	#if defined(_MSC_VER)
		#define MB(STR) ((const char* const) STR)
		#define W1(STR) BxCore::System::GetUTF8LiteralByCP949(STR, sizeof(STR) - 1)
		#define W2(STR) ((const ushort* const) L##STR)
		#define _W2(STR) L##STR
	#else
		#define MB(STR) BxCore::System::GetCP949LiteralByUTF8(STR, sizeof(STR) - 1)
		#define W1(STR) ((const char* const) STR)
		#define W2(STR) ((const ushort* const) u##STR)
		#define _W2(STR) u##STR
	#endif
#endif

// 응용 매크로
#define ItoF(A) ((fint) ((A) << 16))
#define FtoI(A) ((int) ((A) >> 16))
#define FtoR(A) ((int) (((A) + 0x8000) >> 16))
#define REV4(A) ((((A) & 0xFF000000) >> 24) | (((A) & 0x00FF0000) >> 8) | (((A) & 0x0000FF00) << 8) | (((A) & 0x000000FF) << 24))
#define VAL4(A, B, C, D) ((((A) & 0xFF) << 0) | (((B) & 0xFF) << 8) | (((C) & 0xFF) << 16) | (((D) & 0xFF) << 24))
#define CASE64(A) { \
	case 0x00: A case 0x01: A case 0x02: A case 0x03: A \
	case 0x04: A case 0x05: A case 0x06: A case 0x07: A \
	case 0x08: A case 0x09: A case 0x0A: A case 0x0B: A \
	case 0x0C: A case 0x0D: A case 0x0E: A case 0x0F: A \
	case 0x10: A case 0x11: A case 0x12: A case 0x13: A \
	case 0x14: A case 0x15: A case 0x16: A case 0x17: A \
	case 0x18: A case 0x19: A case 0x1A: A case 0x1B: A \
	case 0x1C: A case 0x1D: A case 0x1E: A case 0x1F: A \
	case 0x20: A case 0x21: A case 0x22: A case 0x23: A \
	case 0x24: A case 0x25: A case 0x26: A case 0x27: A \
	case 0x28: A case 0x29: A case 0x2A: A case 0x2B: A \
	case 0x2C: A case 0x2D: A case 0x2E: A case 0x2F: A \
	case 0x30: A case 0x31: A case 0x32: A case 0x33: A \
	case 0x34: A case 0x35: A case 0x36: A case 0x37: A \
	case 0x38: A case 0x39: A case 0x3A: A case 0x3B: A \
	case 0x3C: A case 0x3D: A case 0x3E: A case 0x3F: A}

// DEBUG관련
#ifdef __BX_DEBUG
	#define __DEBUG_MCR__ ,__FILE__,__LINE__,__FUNCTION__
	#define __DEBUG_PRM__ ,string file,const int line,string func
	#define __DEBUG_VAL__ string File;const int Line;string Func;
	#define __DEBUG_INT__ ,File(file),Line(line),Func(func)
	#define __DEBUG_ARG__ ,file,line,func
	#define __DEBUG_FILE__ file
	#define __DEBUG_LINE__ line
	#define __DEBUG_FUNC__ func
#else
	#define __DEBUG_MCR__
	#define __DEBUG_PRM__
	#define __DEBUG_VAL__
	#define __DEBUG_INT__
	#define __DEBUG_ARG__
	#define __DEBUG_FILE__ ""
	#define __DEBUG_LINE__ -1
	#define __DEBUG_FUNC__ ""
#endif

// ASSERT/TRACE관련
#ifdef __BX_DEBUG
	namespace BxCore {namespace System {bool Break(string, string __DEBUG_PRM__);}}
	#define BxASSERT(STRING, QUERY) \
		do \
		{ \
			global_data bool IsIgnore = false; \
			if(IsIgnore || (QUERY) != 0) break; \
			IsIgnore = !BxCore::System::Break(STRING, #QUERY __DEBUG_MCR__); \
		} while(false)
	#define BxTRACE(...) BxCore::System::Trace(__VA_ARGS__)
#else
	#define BxASSERT(STRING, CHECK) do{} while(false)
	#define BxTRACE(...) do{} while(false)
#endif

// NEW/DELETE관련
#ifndef _BX_OPERATOR_NEW_
#define _BX_OPERATOR_NEW_
	#ifdef __BX_USED_LONG_SIZET
		#define NEW_SIZE_T unsigned long
	#else
		#define NEW_SIZE_T unsigned int
	#endif
	inline void* operator new(NEW_SIZE_T, mint ptr) {return (void*) ptr;}
	inline void* operator new[](NEW_SIZE_T, mint ptr) {return (void*) ptr;}
	inline void operator delete(void*, mint) {}
	inline void operator delete[](void*, mint) {}
#endif

// 타입분석관련
#ifndef __BX_DECLARE_IsTypePointer
#define __BX_DECLARE_IsTypePointer
	template<typename TYPE>
	inline bool _IsTypePointer(TYPE&) {return false;}
	template<typename TYPE>
	inline bool _IsTypePointer(TYPE*&) {return true;}
	template<typename TYPE>
	inline bool IsTypePointer()
	{
		TYPE* Pointer = nullptr;
		TYPE& Variable = *Pointer;
		return _IsTypePointer(Variable);
	}
#endif
