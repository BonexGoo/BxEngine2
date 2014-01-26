#pragma once
#include <BxUtil.hpp>
#include <BxExpress.hpp>

// BxTRY-모듈
#define BxTRY(DRAW, ...) for(bool _ = DRAW.AddStyleByIf(__VA_ARGS__); _; _ = DRAW.SubStyle())

//! \brief 새로운 폼개발을 위한 인터페이스
class BxDrawForm
{
public:
	enum blendflag {blendflag_null = 0, blendflag_alpha = 1, blendflag_sprite = 2};
	typedef void (*OnFormMethod)(BxDrawForm& srcClass, const rect srcRect, pixel_dst* rowBuffer, const uint rowLength,
		const fint x, const fint y, const fint xVec, const fint yVec, const byte* blendSpriteTable, const byte* blendAlphaTable);

protected:
	area Area;
	BxExpress::IntegerX ExpressAreaW;
	BxExpress::IntegerX ExpressAreaH;
	blendflag Flag;
	int Page;
	int FrameBegin;
	#ifdef __BX_OPENGL
	id_opengl_form FormGL;
	#endif

	BxDrawForm() : ExpressAreaW(BxInteger(0)), ExpressAreaH(BxInteger(0))
	{
		Area.w = 0;
		Area.h = 0;
		Area.hx = 0;
		Area.hy = 0;
		Flag = blendflag_null;
		Page = 1;
		FrameBegin = 0;
		#ifdef __BX_OPENGL
		FormGL = nullptr;
		#endif
	}
	virtual ~BxDrawForm()
	{
		#ifdef __BX_OPENGL
		BxCore::OpenGL2D::FreeForm(FormGL);
		#endif
	}

public:
	inline int Width() {return Area.w;}
	inline int Height() {return Area.h;}
	inline const BxExpress::IntegerX& ExpressWidth() {return ExpressAreaW;}
	inline const BxExpress::IntegerX& ExpressHeight() {return ExpressAreaH;}
	inline const area* GetArea() {return &Area;}
	inline void SetHotspot(const int hx, const int hy) {Area.hx = hx; Area.hy = hy;}
	inline int GetPage() {return Page;}
	inline int GetFrameBegin() {return FrameBegin;}
	inline void SetAnimationFrame(int Frame) {FrameBegin = BxCore::Main::GetCurrentRenderCount() - Frame;}

	global_func const int CalcTextureSize(const int RealSize)
	{
		#ifdef __BX_OPENGL
		return RealSize;
		#else
		int Result = 1;
		while(Result < RealSize)
			Result <<= 1;
		return Result;
		#endif
	}

	global_func const int CalcTextureLevel(int TextureSize)
	{
		int Result = 0;
		while(TextureSize >>= 1)
			++Result;
		return Result;
	}

	virtual OnFormMethod GetFormMethod(maskoperate operate, maskflag flag) = 0;

	#ifdef __BX_OPENGL
	id_opengl_form GetFormGL()
	{
		_UpdateFormGL();
		return FormGL;
	}
	#endif

protected:
	global_func void OnFormNoPrint(BxDrawForm&, const rect, pixel_dst*, const uint, const fint,
		const fint, const fint, const fint, const byte*, const byte*)
	{
	}
	#ifdef __BX_OPENGL
	virtual void _UpdateFormGL() = 0;
	#endif
};

//! \brief BxDraw관련 글로벌함수
namespace BxDrawGlobal
{
	class StyleStack
	{
	protected:
		unsigned int Depth;
	public:
		StyleStack() {Depth = 0;}
		StyleStack(const StyleStack& RHS) {Depth = RHS.Depth;}
	protected:
		StyleStack(unsigned int RHS) {Depth = RHS;}
		StyleStack(const StyleStack& RHS1, const StyleStack& RHS2) {Depth = RHS1.Depth + RHS2.Depth;}
	public:
		inline StyleStack operator>>(const StyleStack& RHS) const {return StyleStack(*this, RHS);}
		inline const StyleStack& operator++() {++Depth; return *this;} // 전치연산
		inline bool operator>(unsigned int RHS) const {return Depth > RHS;}
		inline bool operator==(const StyleStack& RHS) const {return Depth == RHS.Depth;}
	public:
		global_func const StyleStack& Zero() {global_data StyleStack _Zero(0); return _Zero;}
		global_func const StyleStack& One() {global_data StyleStack _One(1); return _One;}
	};

	enum option {FILL, DRAW};

	/*!
	\brief 위치제작
	\param x : 위치X
	\param y : 위치Y
	\return 위치
	*/
	static inline const point XY(int x, int y)
	{
		const point Result = {x, y};
		return Result;
	}
	static inline const pointf XY(float x, float y)
	{
		const pointf Result = {(fint)(x * ItoF(1)), (fint)(y * ItoF(1))};
		return Result;
	}

	/*!
	\brief 크기제작
	\param w : 너비
	\param h : 높이
	\return 크기
	*/
	static inline const size WH(int w, int h)
	{
		const size Result = {w, h};
		return Result;
	}

	/*!
	\brief 영역제작
	\param x : 시작위치X
	\param y : 시작위치Y
	\param w : 너비
	\param h : 높이
	\return 영역
	*/
	static inline const rect XYWH(int x, int y, int w = size::max, int h = size::max)
	{
		const rect Result = {x, y, x + w, y + h};
		BxASSERT("BxDraw<가로길이가 MAXSIZE를 초과합니다>", Result.r - Result.l <= size::max);
		BxASSERT("BxDraw<세로길이가 MAXSIZE를 초과합니다>", Result.b - Result.t <= size::max);
		return Result;
	}

	/*!
	\brief 영역제작
	\param x1 : 시작위치X
	\param y1 : 시작위치Y
	\param x2 : 끝위치X
	\param y2 : 끝위치Y
	\return 영역
	*/
	static inline const rect XYXY(int x1, int y1, int x2, int y2)
	{
		const rect Result = {x1, y1, x2, y2};
		BxASSERT("BxDraw<가로길이가 MAXSIZE를 초과합니다>", Result.r - Result.l <= size::max);
		BxASSERT("BxDraw<세로길이가 MAXSIZE를 초과합니다>", Result.b - Result.t <= size::max);
		return Result;
	}

	/*!
	\brief 중점식 영역제작
	\param x : 중심위치X
	\param y : 중심위치Y
	\param r : 반지름
	\return 영역
	*/
	static inline const rect XYR(int x, int y, int r)
	{
		const rect Result = {x - r, y - r, x + r, y + r};
		BxASSERT("BxDraw<가로길이가 MAXSIZE를 초과합니다>", Result.r - Result.l <= size::max);
		BxASSERT("BxDraw<세로길이가 MAXSIZE를 초과합니다>", Result.b - Result.t <= size::max);
		return Result;
	}

	/*!
	\brief 중점식 영역제작
	\param x : 중심위치X
	\param y : 중심위치Y
	\param wr : 가로 반지름
	\param hr : 세로 반지름
	\return 영역
	*/
	static inline const rect XYRR(int x, int y, int wr, int hr)
	{
		const rect Result = {x - wr, y - hr, x + wr, y + hr};
		BxASSERT("BxDraw<가로길이가 MAXSIZE를 초과합니다>", Result.r - Result.l <= size::max);
		BxASSERT("BxDraw<세로길이가 MAXSIZE를 초과합니다>", Result.b - Result.t <= size::max);
		return Result;
	}

	/*!
	\brief 가변인자식 다각영역제작
	\param x : 꼭지점X
	\param y : 꼭지점Y
	\return 다각영역
	*/
	static inline const points& XYS(const BxThrow& x, const BxThrow& y)
	{
		BxASSERT("BxDraw<가변인자의 길이가 서로 다릅니다>", x.Length() == y.Length());
		thread_storage _ = sizeof(points);
		points& Result = *((points*) BxCore::Thread::BindStorage(&_));
		Result.count = x.Length();
		for(int i = 0; i < Result.count; ++i)
		{
			const int* ArgX = x.Access<int>(i);
			const int* ArgY = y.Access<int>(i);
			BxASSERT("BxDraw<가변인자 x에 int가 아닌 값이 존재합니다>", ArgX);
			BxASSERT("BxDraw<가변인자 y에 int가 아닌 값이 존재합니다>", ArgY);
			Result.pt[i].x = *ArgX;
			Result.pt[i].y = *ArgY;
		}
		return Result;
	}

	/*!
	\brief 가변인자식 3D다각영역제작
	\param x : 꼭지점X
	\param y : 꼭지점Y
	\param z : 꼭지점Z
	\return 3D다각영역
	*/
	static inline const vertexs& XYZS(const BxThrow& x, const BxThrow& y, const BxThrow& z)
	{
		BxASSERT("BxDraw<가변인자의 길이가 서로 다릅니다>", x.Length() == y.Length() && x.Length() == z.Length());
		thread_storage _ = sizeof(vertexs);
		vertexs& Result = *((vertexs*) BxCore::Thread::BindStorage(&_));
		Result.count = x.Length();
		for(int i = 0; i < Result.count; ++i)
		{
			const int* ArgX = x.Access<int>(i);
			const int* ArgY = y.Access<int>(i);
			const int* ArgZ = z.Access<int>(i);
			BxASSERT("BxDraw<가변인자 x에 int가 아닌 값이 존재합니다>", ArgX);
			BxASSERT("BxDraw<가변인자 y에 int가 아닌 값이 존재합니다>", ArgY);
			BxASSERT("BxDraw<가변인자 z에 int가 아닌 값이 존재합니다>", ArgZ);
			Result.vt[i].x = *ArgX;
			Result.vt[i].y = *ArgY;
			Result.vt[i].z = *ArgZ;
		}
		return Result;
	}

	/*!
	\brief 배열식 다각영역제작
	\param count : 꼭지점의 수량
	\param xy : 꼭지점배열
	\return 다각영역
	*/
	static inline const points& XYARRAY(uint count, int* xy)
	{
		global_data points Result = {0,};
		Result.count = count;
		for(uint i = 0; i < count; ++i)
		{
			Result.pt[i].x = *(xy++);
			Result.pt[i].y = *(xy++);
		}
		return Result;
	}

	/*!
	\brief 배열식 3D다각영역제작
	\param count : 꼭지점의 수량
	\param xy : 꼭지점배열
	\return 3D다각영역
	*/
	static inline const vertexs& XYZARRAY(uint count, int* xyz)
	{
		global_data vertexs Result = {0,};
		Result.count = count;
		for(uint i = 0; i < count; ++i)
		{
			Result.vt[i].x = *(xyz++);
			Result.vt[i].y = *(xyz++);
			Result.vt[i].z = *(xyz++);
		}
		return Result;
	}

	/*!
	\brief 32비트 색상제작
	\param r : 빨강
	\param g : 초록
	\param b : 파랑
	\return 색상
	*/
	static inline const color_x888 RGB32(const byte r, const byte g, const byte b)
	{
		return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
	}

	/*!
	\brief 16비트 색상제작
	\param r : 빨강
	\param g : 초록
	\param b : 파랑
	\return 색상
	*/
	static inline const color_565 RGB16(const byte r, const byte g, const byte b)
	{
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
	}

	/*!
	\brief 스프라이트 블렌딩
	\param dst : 바탕
	\param src : 투명색이 있는 소스
	\param table : 선계산 테이블
	\return 계산된 바탕색
	*/
	static inline const pixel_dst SpriteBlending(const pixel_dst dst, const pixel_src src, const byte* table)
	{
		#ifdef __BX_PIXEL16
			return table[(dst >> 6 & 0x03E0) | (src >> 10 & 0x001F)] << 11
				| table[(dst >> 1 & 0x03E0) | (src >> 5 & 0x001F)] << 6
				| table[(dst << 5 & 0x03E0) | (src & 0x001F)];
		#else
			return table[(dst >> 8 & 0xFF00) | (src >> 16 & 0x00FF)] << 16
				| table[(dst & 0xFF00) | (src >> 8 & 0x00FF)] << 8
				| table[(dst << 8 & 0xFF00) | (src & 0x00FF)];
		#endif
	}

	/*!
	\brief 알파 블렌딩
	\param dst : 바탕
	\param src : 투명도가 있는 소스
	\param table : 선계산 테이블
	\return 계산된 바탕색
	*/
	static inline const pixel_dst AlphaBlending(const pixel_dst dst, const pixel_src src, const byte* table)
	{
		#ifdef __BX_PIXEL16
			return table[(src >> 3 & 0x0E00) | (dst >> 7 & 0x01F0) | (src >> 8 & 0x000F)] << 11
				| table[(src >> 3 & 0x0E00) | (dst >> 2 & 0x01F0) | (src >> 4 & 0x000F)] << 6
				| table[(src >> 3 & 0x0E00) | (dst << 4 & 0x01F0) | (src & 0x000F)];
		#else
			return table[(src >> 13 & 0x070000) | (dst >> 8 & 0x00FF00) | (src >> 16 & 0x0000FF)] << 16
				| table[(src >> 13 & 0x070000) | (dst & 0x00FF00) | (src >> 8 & 0x0000FF)] << 8
				| table[(src >> 13 & 0x070000) | (dst << 8 & 0x00FF00) | (src & 0x0000FF)];
		#endif
	}

	/*!
	\brief 불투명도값 추출(M555_A444)
	\param color : 원본색상
	\return 불투명도
	*/
	static inline const color_m555_a444 OpacityBitByM555A444(const color_m555_a444 color)
	{
		return (color & 0x8000)? 0x8000 : (color & 0xF000);
	}

	/*!
	\brief 불투명도값 추출(A888)
	\param color : 원본색상
	\return 불투명도
	*/
	static inline const color_a888 OpacityBitByA888(const color_a888 color)
	{
		return 0xFF000000 & color;
	}

	/*!
	\brief 색상전환(RGB→HLS)
	\param r : 빨강
	\param g : 초록
	\param b : 파랑
	\return HLS색상
	*/
	static inline const color_x888 ColorRGBToHLS(const byte r, const byte g, const byte b)
	{
		const int MaxValue = (int) ((r < g)? ((g < b)? b : g) : ((r < b)? b : r));
		const int MinValue = (int) ((r > g)? ((g > b)? b : g) : ((r > b)? b : r));
		const fint AddValue = ItoF(MaxValue + MinValue);
		const fint SubValue = ItoF(MaxValue - MinValue);
		fint HValue = 0, LValue = AddValue / 510, SValue = 0;
		if(MaxValue != MinValue)
		{
			SValue = (fint) ((((huge) SubValue) << 16) / ((LValue <= 0x8000)? AddValue : (0x1FE0000 - AddValue)));
			const fint NormalR = (fint) (((MaxValue - (huge) r) << 32) / SubValue);
			const fint NormalG = (fint) (((MaxValue - (huge) g) << 32) / SubValue);
			const fint NormalB = (fint) (((MaxValue - (huge) b) << 32) / SubValue);
			if(r == MaxValue) HValue = (42 * (0x60000 + NormalB - NormalG)) & 0xFFFFFF;
			else if(g == MaxValue) HValue = (42 * (0x20000 + NormalR - NormalB)) & 0xFFFFFF;
			else HValue = (42 * (0x40000 + NormalG - NormalR)) & 0xFFFFFF;
		}
		return (((HValue >> 16) & 0xFF) << 16) | ((((LValue * 255) >> 16) & 0xFF) << 8) | (((SValue * 255) >> 16) & 0xFF);
	}

	/*!
	\brief 색상전환(HLS→RGB)
	\param h : 색조
	\param l : 휘도
	\param s : 채도
	\return RGB색상
	*/
	static inline const color_x888 ColorHLSToRGB(const byte h, const byte l, const byte s)
	{
		if(0 < s)
		{
			const fint HValue = ItoF(h & 0xFF), LValue = ItoF(l & 0xFF) / 255, SValue = ItoF(s & 0xFF) / 255;
			const fint XValue = LValue + (fint) ((LValue <= 0x8000)? (LValue * (huge) SValue) >> 16 : SValue - ((LValue * (huge) SValue) >> 16));
			const fint YValue = 2 * LValue - XValue;
			fint RValue = (HValue + 0x555555) & 0xFFFFFF, GValue = HValue, BValue = (HValue + 0xAAAAAB) & 0xFFFFFF;
			if(RValue < 0x2A0000) RValue = YValue + (((XValue - YValue) * (huge) RValue) >> 16) / 42;
			else if(RValue < 0x800000) RValue = XValue;
			else if(RValue < 0xAAAAAA) RValue = YValue + (((XValue - YValue) * (huge) (0xAAAAAA - RValue)) >> 16) / 42;
			else RValue = YValue;
			if(GValue < 0x2A0000) GValue = YValue + (((XValue - YValue) * (huge) GValue) >> 16) / 42;
			else if(GValue < 0x800000) GValue = XValue;
			else if(GValue < 0xAAAAAA) GValue = YValue + (((XValue - YValue) * (huge) (0xAAAAAA - GValue)) >> 16) / 42;
			else GValue = YValue;
			if(BValue < 0x2A0000) BValue = YValue + (((XValue - YValue) * (huge) BValue) >> 16) / 42;
			else if(BValue < 0x800000) BValue = XValue;
			else if(BValue < 0xAAAAAA) BValue = YValue + (((XValue - YValue) * (huge) (0xAAAAAA - BValue)) >> 16) / 42;
			else BValue = YValue;
			return ((((RValue * 255 + 0x8000) >> 16) & 0xFF) << 16) | ((((GValue * 255 + 0x8000) >> 16) & 0xFF) << 8) | (((BValue * 255 + 0x8000) >> 16) & 0xFF);
		}
		return ((l & 0xFF) << 16) | ((l & 0xFF) << 8) | (l & 0xFF);
	}

	/*!
	\brief 색상전환(M555_A444→565)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const color_565 ColorM555A444To565(const color_m555_a444 color)
	{
		return (color & 0x8000)? ((color << 1) & 0xFFC0) | ((color >> 4) & 0x0020) | (color & 0x001F)
			: ((color << 4) & 0xF000) | ((color >> 0) & 0x0800)
			| ((color << 3) & 0x0780) | ((color >> 1) & 0x0060)
			| ((color << 1) & 0x001E) | ((color >> 3) & 0x0001);
	}

	/*!
	\brief 색상전환(M555_A444→A888)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const color_a888 ColorM555A444ToA888(const color_m555_a444 color)
	{
		const color_565 c = ColorM555A444To565(color);
		const color_m555_a444 o = OpacityBitByM555A444(color);
		return ((o == 0x8000)? 0xFF000000 : (((o & 0xF000) << 16) | ((o & 0xF000) << 12))) |
			((c << 8) & 0xF80000) | ((c << 3) & 0x070000) |
			((c << 5) & 0x00FC00) | ((c >> 1) & 0x000300) |
			((c << 3) & 0x0000F8) | ((c >> 2) & 0x000007);
	}

	/*!
	\brief 색상전환(A888→X888)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const color_x888 ColorA888ToX888(const color_a888 color)
	{
		return 0x00FFFFFF & color;
	}

	/*!
	\brief 네이티브바탕으로 색상전환(565→NativeDst)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_dst Color565ToNativeDst(const color_565 color)
	{
		#ifdef __BX_PIXEL16
			return color;
		#else
			return ((color << 8) & 0xF80000) | ((color << 3) & 0x070000) |
				((color << 5) & 0x00FC00) | ((color >> 1) & 0x000300) |
				((color << 3) & 0x0000F8) | ((color >> 2) & 0x000007);
		#endif
	}

	/*!
	\brief 네이티브소스로 색상전환(565→NativeSrc)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_src Color565ToNativeSrc(const color_565 color, const color_m555_a444 opacity_bit = 0x8000)
	{
		#ifdef __BX_PIXEL16
			return (opacity_bit == 0x8000)?
				opacity_bit | ((color >> 1) & 0x7FE0) | (color & 0x001F)
				: opacity_bit | ((color >> 4) & 0x0F00) | ((color >> 3) & 0x00F0) | ((color >> 1) & 0x000F);
		#else
			return ((opacity_bit == 0x8000)? 0xFF000000 : (((opacity_bit & 0xF000) << 16) | ((opacity_bit & 0xF000) << 12))) |
				((color << 8) & 0xF80000) | ((color << 3) & 0x070000) |
				((color << 5) & 0x00FC00) | ((color >> 1) & 0x000300) |
				((color << 3) & 0x0000F8) | ((color >> 2) & 0x000007);
		#endif
	}

	/*!
	\brief 네이티브바탕으로 색상전환(M555_A444→NativeDst)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_dst ColorM555A444ToNativeDst(const color_m555_a444 color)
	{
		return Color565ToNativeDst(ColorM555A444To565(color));
	}

	/*!
	\brief 네이티브소스로 색상전환(M555_A444→NativeSrc)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_src ColorM555A444ToNativeSrc(const color_m555_a444 color)
	{
		return Color565ToNativeSrc(ColorM555A444To565(color), OpacityBitByM555A444(color));
	}

	/*!
	\brief 네이티브바탕으로 색상전환(X888→NativeDst)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_dst ColorX888ToNativeDst(const color_x888 color)
	{
		#ifdef __BX_PIXEL16
			return ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
		#else
			return color;
		#endif
	}

	/*!
	\brief 네이티브소스로 색상전환(X888→NativeSrc)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_src ColorX888ToNativeSrc(const color_x888 color, const color_a888 opacity_bit = 0xFF000000)
	{
		#ifdef __BX_PIXEL16
			return (opacity_bit == 0xFF000000)?
				0x8000 | ((color >> 9) & 0x7C00) | ((color >> 6) & 0x03E0) | ((color >> 3) & 0x001F)
				: ((opacity_bit >> 17) & 0x7000) | ((color >> 12) & 0x0F00) | ((color >> 8) & 0x00F0) | ((color >> 4) & 0x000F);
		#else
			return opacity_bit | color;
		#endif
	}

	/*!
	\brief 네이티브바탕으로 색상전환(A888→NativeDst)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_dst ColorA888ToNativeDst(const color_a888 color)
	{
		return ColorX888ToNativeDst(ColorA888ToX888(color));
	}

	/*!
	\brief 네이티브소스로 색상전환(A888→NativeSrc)
	\param color : 원본색상
	\return 결과색상
	*/
	static inline const pixel_src ColorA888ToNativeSrc(const color_a888 color)
	{
		return ColorX888ToNativeSrc(ColorA888ToX888(color), OpacityBitByA888(color));
	}

	/// @cond SECTION_NAME
	// 옵션관리
	class _DrawOption
	{
	public:
		// 테이블링
		global_func inline uint GetBlend(const drawblend blend, const uint dst, const uint src, const uint indexMax)
		{
			const uint fullLen = indexMax + 1, halfLen = fullLen / 2;
			switch(blend)
			{
			case blend_normal: return src;
			case blend_darken: return BxUtilGlobal::Min(dst, src);
			case blend_lighten: return BxUtilGlobal::Max(dst, src);
			case blend_multiply: return (dst * src) / indexMax;
			case blend_screen: return indexMax - ((indexMax - dst) * (indexMax - src) / indexMax);
			case blend_color_dodge: return BxUtilGlobal::Min(indexMax, (dst * indexMax) / (fullLen - src));
			case blend_color_burn: return BxUtilGlobal::Max(0, indexMax - (indexMax - dst) * indexMax / BxUtilGlobal::Max(1, src));
			case blend_linear_dodge: return BxUtilGlobal::Min(dst + src, indexMax);
			case blend_linear_burn: return BxUtilGlobal::Max(0, dst + src - indexMax);
			case blend_linear_light: return dst + src * 2 - indexMax;
			case blend_overlay: return (dst < halfLen)? BxUtilGlobal::Min(indexMax, src * dst * 2 / indexMax) : BxUtilGlobal::Max(0, indexMax - (indexMax - src) * (indexMax - dst) * 2 / indexMax);
			case blend_hard_light: return (halfLen < src)? BxUtilGlobal::Max(0, indexMax - (indexMax - src) * (indexMax - dst) * 2 / indexMax) : BxUtilGlobal::Min(indexMax, src * dst * 2 / indexMax);
			case blend_pin_light: return (dst < (src << 1) - indexMax)? BxUtilGlobal::Max(0, (src << 1) - indexMax) : (((src << 1) - indexMax < dst && dst < (src << 1))? dst : BxUtilGlobal::Min(indexMax, src << 1));
			case blend_hard_mix: return (src < indexMax - dst)? 0 : indexMax;
			case blend_defferent: return BxUtilGlobal::Max(src, dst) - BxUtilGlobal::Min(src, dst);
			case blend_exclusion: return BxUtilGlobal::Max(0, BxUtilGlobal::Min(indexMax, src + dst - src * dst * 2 / indexMax));
			case blend_gray: return 0;
			case blend_add: return BxUtilGlobal::Min(dst + src, indexMax);
			case blend_sub: return BxUtilGlobal::Max(0, src - dst);
			case blend_xor: return src ^ dst;
			}
			return 0;
		}
		#ifndef __BX_OPENGL
		global_func byte* GetSpriteTable(drawblend blend)
		{
			global_data byte* Table[blend_max] = {nullptr,};
			if(!Table[blend])
			{
				#ifdef __BX_PIXEL16
					Table[blend] = BxNew_Array(byte, 8 * 32 * 32);
					for(uint opa = 0; opa < 8; ++opa)
					for(uint dst = 0; dst < 32; ++dst)
					for(uint src = 0; src < 32; ++src)
					{
						const uint BlendedSrc = GetBlend(blend, dst, src, 0x1F);
						Table[blend][(opa << 10) | (dst << 5) | src] =
							((dst * (7 - opa)) + (BlendedSrc * (1 + opa))) / 8;
					}
				#else
					Table[blend] = BxNew_Array(byte, 8 * 256 * 256);
					for(uint opa = 0; opa < 8; ++opa)
					for(uint dst = 0; dst < 256; ++dst)
					for(uint src = 0; src < 256; ++src)
					{
						const uint BlendedSrc = GetBlend(blend, dst, src, 0xFF);
						Table[blend][(opa << 16) | (dst << 8) | src] =
							((dst * (7 - opa)) + (BlendedSrc * (1 + opa))) / 8;
					}
				#endif
			}
			return Table[blend];
		}
		global_func byte* GetAlphaTable(drawblend blend)
		{
			global_data byte* Table[blend_max] = {nullptr,};
			if(!Table[blend])
			{
				#ifdef __BX_PIXEL16
					Table[blend] = BxNew_Array(byte, 8 * 8 * 32 * 16);
					for(uint opa = 0; opa < 8; ++opa)
					for(uint src_opa = 0; src_opa < 8; ++src_opa)
					for(uint dst = 0; dst < 32; ++dst)
					for(uint src = 0; src < 16; ++src)
					{
						const uint BlendedSrc = GetBlend(blend, dst, (src << 1) | (src >> 3), 0x1F);
						const uint BlendedSrc2 = (dst * (7 - opa)) + (BlendedSrc * (1 + opa));
						Table[blend][(opa << 12) | (src_opa << 9) | (dst << 4) | src] =
							((dst * (8 - src_opa) * 8) + (BlendedSrc2 * src_opa)) / (8 * 8);
					}
				#else
					Table[blend] = BxNew_Array(byte, 8 * 8 * 256 * 256);
					for(uint opa = 0; opa < 8; ++opa)
					for(uint src_opa = 0; src_opa < 8; ++src_opa)
					for(uint dst = 0; dst < 256; ++dst)
					for(uint src = 0; src < 256; ++src)
					{
						const uint BlendedSrc = GetBlend(blend, dst, src, 0xFF);
						const uint BlendedSrc2 = (dst * (7 - opa)) + (BlendedSrc * (1 + opa));
						Table[blend][(opa << 19) | (src_opa << 16) | (dst << 8) | src] =
							((dst * (8 - src_opa) * 8) + (BlendedSrc2 * src_opa)) / (8 * 8);
					}
				#endif
			}
			return Table[blend];
		}
		#endif
		// 현재스타일
		global_func inline rect& CurClipRect() {global_data rect ClipRect; return ClipRect;}
		global_func inline int& CurHotspotX() {global_data int HotspotX; return HotspotX;}
		global_func inline int& CurHotspotY() {global_data int HotspotY; return HotspotY;}
		global_func inline int& CurAreaWidth() {global_data int AreaWidth; return AreaWidth;}
		global_func inline int& CurAreaHeight() {global_data int AreaHeight; return AreaHeight;}
		global_func inline maskoperate& CurMaskOperate() {global_data maskoperate Operate; return Operate;}
		global_func inline maskflag& CurMaskFlag() {global_data maskflag Flag; return Flag;}
		global_func inline drawturn& CurTurn() {global_data drawturn Turn; return Turn;}
		global_func inline byte& CurAngle() {global_data byte Angle; return Angle;}
		global_func inline fint& CurScaleHor() {global_data fint ScaleHor; return ScaleHor;}
		global_func inline fint& CurScaleVer() {global_data fint ScaleVer; return ScaleVer;}
		global_func inline color_x888& CurColor() {global_data color_x888 Color; return Color;}
		global_func inline int& CurOpacity7() {global_data int Opacity7; return Opacity7;} // -1:0%, 0:12.5%, 6:87.5%, 7:100%
		global_func inline drawblend& CurBlend() {global_data drawblend Blend; return Blend;}
		#ifndef __BX_OPENGL
		global_func inline byte*& CurSpriteTable() {global_data byte* Table; return Table;}
		global_func inline byte*& CurAlphaTable() {global_data byte* Table; return Table;}
		#endif
		global_func inline BxDrawForm*& CurForm() {global_data BxDrawForm* Form; return Form;}
		global_func inline rect& CurFormRect() {global_data rect FormRect; return FormRect;}
		global_func inline int& CurMoveX3D() {global_data int MoveX3D; return MoveX3D;}
		global_func inline int& CurMoveY3D() {global_data int MoveY3D; return MoveY3D;}
		global_func inline int& CurMoveZ3D() {global_data int MoveZ3D; return MoveZ3D;}
	};
	/// @endcond

	/// @cond SECTION_NAME
	// 디폴트 폼클래스
	class _BxColorMap : public BxDrawForm
	{
	public:
		global_func inline _BxColorMap* GetClass() {global_data _BxColorMap Class; return &Class;}

	protected:
		_BxColorMap()
		{
			Area.w = 16;
			Area.h = 16;
			ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
			ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
			Area.hx = 8;
			Area.hy = 8;
		}
		virtual ~_BxColorMap() {}

	public:
		virtual OnFormMethod GetFormMethod(maskoperate operate, maskflag flag)
		{
			switch(operate)
			{
			case maskoperate_all_print:
				if(flag == maskflag_no_write)
					return OnForm<maskoperate_all_print, maskflag_no_write>;
				else if(flag == maskflag_write_1)
					return OnForm<maskoperate_all_print, maskflag_write_1>;
				else if(flag == maskflag_write_0)
					return OnForm<maskoperate_all_print, maskflag_write_0>;
				break;
			case maskoperate_no_print:
				return OnFormNoPrint;
			case maskoperate_print_by_1:
				if(flag == maskflag_no_write)
					return OnForm<maskoperate_print_by_1, maskflag_no_write>;
				else if(flag == maskflag_write_1)
					return OnForm<maskoperate_print_by_1, maskflag_write_1>;
				else if(flag == maskflag_write_0)
					return OnForm<maskoperate_print_by_1, maskflag_write_0>;
				break;
			case maskoperate_print_by_0:
				if(flag == maskflag_no_write)
					return OnForm<maskoperate_print_by_0, maskflag_no_write>;
				else if(flag == maskflag_write_1)
					return OnForm<maskoperate_print_by_0, maskflag_write_1>;
				else if(flag == maskflag_write_0)
					return OnForm<maskoperate_print_by_0, maskflag_write_0>;
				break;
			}
			return nullptr;
		}

	protected:
		template<maskoperate MASKOP, maskflag MASK>
		global_func void OnForm(BxDrawForm& srcClass, const rect srcRect, pixel_dst* rowBuffer, const uint rowLength,
			const fint x, const fint y, const fint xVec, const fint yVec, const byte* blendSpriteTable, const byte* blendAlphaTable)
		{
			const pixel_dst* RowBufferEnd = rowBuffer + rowLength;
			if(_DrawOption::CurOpacity7() == 7 && _DrawOption::CurBlend() == blend_normal)
			{
				const pixel_dst Color = BxDrawGlobal::ColorX888ToNativeDst(_DrawOption::CurColor());
				switch(~(rowLength - 1) & 0x3F)
				while(rowBuffer < RowBufferEnd)
					CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					{\
						*rowBuffer = Color;\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;)
			}
			else
			{
				const pixel_src Color = BxDrawGlobal::ColorX888ToNativeSrc(_DrawOption::CurColor());
				switch(~(rowLength - 1) & 0x3F)
				while(rowBuffer < RowBufferEnd)
					CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					{\
						*rowBuffer = BxDrawGlobal::SpriteBlending(*rowBuffer, Color, blendSpriteTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;)
			}
		}

		#ifdef __BX_OPENGL
		virtual void _UpdateFormGL()
		{
			if(FormGL)
			{
				if(BxCore::OpenGL2D::Form_ColorAmbient_GetColor(FormGL) != _DrawOption::CurColor())
				{
					BxCore::OpenGL2D::FreeForm(FormGL);
					FormGL = BxCore::OpenGL2D::MakeForm_ColorAmbient(_DrawOption::CurColor());
				}
			}
			else
			{
				FormGL = BxCore::OpenGL2D::MakeForm_ColorAmbient(_DrawOption::CurColor());
			}
		}
		#endif
	};
	/// @endcond

	/// @cond SECTION_NAME
	// 스택관리
	class _DrawStack
	{
	public:
		enum {MAX = 256};
		StyleStack CountCurrently;
		StyleStack Count[MAX * drawstyle_max];
		int CountFocus;
		drawstyle Order[MAX * drawstyle_max];
		int OrderFocus;
		// 스타일
		struct {rect Rect; point Offset; size Size;} Clip[MAX];
		struct {maskoperate Operate; maskflag Flag;} Mask[MAX];
		struct {drawturn Turn; byte Angle;} Rotate[MAX];
		struct {fint Horizon; fint Vertical;} Scale[MAX];
		struct {byte R; byte G; byte B; uint Sum; color_x888 Color;} Color[MAX];
		byte Hue[MAX];
		struct {byte Opacity255; int Opacity7;} Opacity[MAX];
		drawblend Blend[MAX];
		struct {BxDrawForm* Form; rect Rect;} Form[MAX];
		vertex Move3D[MAX];
		int StyleFocus[drawstyle_max];
		// 초기화
		void Init(const int Width, const int Height)
		{
			// 스택관리
			CountFocus = -1;
			OrderFocus = -1;
			// 개별스택
			Clip[0].Rect.l = 0;
			Clip[0].Rect.t = 0;
			Clip[0].Rect.r = Width;
			Clip[0].Rect.b = Height;
			_DrawOption::CurClipRect() = Clip[0].Rect; // 현재클리핑
			Clip[0].Offset.x = _DrawOption::CurHotspotX() = 0; // 현재기준위치X
			Clip[0].Offset.y = _DrawOption::CurHotspotY() = 0; // 현재기준위치Y
			Clip[0].Size.w = _DrawOption::CurAreaWidth() = Width; // 현재영역사이즈W
			Clip[0].Size.h = _DrawOption::CurAreaHeight() = Height; // 현재영역사이즈H
			Mask[0].Operate = _DrawOption::CurMaskOperate() = maskoperate_all_print;
			Mask[0].Flag = _DrawOption::CurMaskFlag() = maskflag_no_write;
			Rotate[0].Turn = _DrawOption::CurTurn() = turn_0; // 현재턴
			Rotate[0].Angle = _DrawOption::CurAngle() = 0; // 현재각도
			Scale[0].Horizon = _DrawOption::CurScaleHor() = 0x10000; // 현재수평배율
			Scale[0].Vertical = _DrawOption::CurScaleVer() = 0x10000; // 현재수직배율
			Color[0].R = 0xFF;
			Color[0].G = 0xFF;
			Color[0].B = 0xFF;
			Color[0].Sum = 0;
			Color[0].Color = _DrawOption::CurColor() = RGB32(Color[0].R, Color[0].G, Color[0].B); // 현재색상
			Hue[0] = 0;
			Opacity[0].Opacity255 = 255;
			Opacity[0].Opacity7 = _DrawOption::CurOpacity7() = 7; // 현재불투명도
			Blend[0] = _DrawOption::CurBlend() = blend_normal; // 현재블랜드
			#ifndef __BX_OPENGL
			_DrawOption::CurSpriteTable() = _DrawOption::GetSpriteTable(blend_normal);
			_DrawOption::CurAlphaTable() = _DrawOption::GetAlphaTable(blend_normal);
			#endif
			Form[0].Form = _DrawOption::CurForm() = _BxColorMap::GetClass(); // 현재폼
			Form[0].Rect = _DrawOption::CurFormRect() = XYWH(0, 0, size::max, size::max); // 현재폼영역
			Move3D[0].x = _DrawOption::CurMoveX3D() = 0; // 현재이동위치X3D
			Move3D[0].y = _DrawOption::CurMoveY3D() = 0; // 현재이동위치Y3D
			Move3D[0].z = _DrawOption::CurMoveZ3D() = 0; // 현재이동위치Z3D
			// 스택포커스
			for(int i = 0; i < drawstyle_max; ++i)
				StyleFocus[i] = 0;
		}
		// 제거
		void Sub()
		{
			BxASSERT("BxDraw", 0 <= CountFocus);
			for(uint i = 0; Count[CountFocus] > i; ++i)
			{
				BxASSERT("BxDraw", 0 <= OrderFocus);
				BxASSERT("BxDraw", 0 < StyleFocus[Order[OrderFocus]]);
				const int Focus = --StyleFocus[Order[OrderFocus]];
				switch(Order[OrderFocus--])
				{
				case drawstyle_clip:
					_DrawOption::CurClipRect() = Clip[Focus].Rect; // 현재클리핑
					_DrawOption::CurHotspotX() = Clip[Focus].Offset.x; _DrawOption::CurHotspotY() = Clip[Focus].Offset.y; // 현재기준위치
					_DrawOption::CurAreaWidth() = Clip[Focus].Size.w; _DrawOption::CurAreaHeight() = Clip[Focus].Size.h; // 현재영역사이즈
					#ifdef __BX_OPENGL
					BxCore::OpenGL2D::Clip(_DrawOption::CurClipRect());
					#endif
					break;
				case drawstyle_mask:
					_DrawOption::CurMaskOperate() = Mask[Focus].Operate; // 현재마스킹조작계
					_DrawOption::CurMaskFlag() = Mask[Focus].Flag; // 현재마스킹플래그
					break;
				case drawstyle_rotate: _DrawOption::CurTurn() = Rotate[Focus].Turn; _DrawOption::CurAngle() = Rotate[Focus].Angle; break; // 현재턴, 현재각도
				case drawstyle_scale: _DrawOption::CurScaleHor() = Scale[Focus].Horizon; _DrawOption::CurScaleVer() = Scale[Focus].Vertical; break; // 현재배율
				case drawstyle_color: _DrawOption::CurColor() = Color[Focus].Color; break; // 현재색상
				case drawstyle_hue: break;
				case drawstyle_opacity: _DrawOption::CurOpacity7() = Opacity[Focus].Opacity7; break; // 현재불투명도
				case drawstyle_blend: _DrawOption::CurBlend() = Blend[Focus]; // 현재블랜드
					#ifndef __BX_OPENGL
					_DrawOption::CurSpriteTable() = _DrawOption::GetSpriteTable(Blend[Focus]);
					_DrawOption::CurAlphaTable() = _DrawOption::GetAlphaTable(Blend[Focus]);
					#endif
					break;
				case drawstyle_form:
					_DrawOption::CurForm() = Form[Focus].Form; // 현재폼
					_DrawOption::CurFormRect() = Form[Focus].Rect; // 현재폼영역
					break;
				case drawstyle_event: break;
				case drawstyle_move3d: // 현재이동위치3D
					_DrawOption::CurMoveX3D() = Move3D[Focus].x;
					_DrawOption::CurMoveY3D() = Move3D[Focus].y;
					_DrawOption::CurMoveZ3D() = Move3D[Focus].z;
					break;
				}
			}
			--CountFocus;
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	inline _DrawStack& Stack() {global_data _DrawStack _Stack; return _Stack;}
	/// @endcond

	/*!
	\brief 스타일-클리핑
	\param r : 영역
	\param hx : 영역기준 중점X
	\param hy : 영역기준 중점Y
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack CLIP(rect r, int hx = 0, int hy = 0)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_clip];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Clip[Focus].Rect.l = BxUtilGlobal::Max(Stack().Clip[Focus - 1].Rect.l, Stack().Clip[Focus - 1].Offset.x + r.l);
		Stack().Clip[Focus].Rect.t = BxUtilGlobal::Max(Stack().Clip[Focus - 1].Rect.t, Stack().Clip[Focus - 1].Offset.y + r.t);
		Stack().Clip[Focus].Rect.r = BxUtilGlobal::Min(Stack().Clip[Focus - 1].Rect.r, Stack().Clip[Focus - 1].Offset.x + r.r);
		Stack().Clip[Focus].Rect.b = BxUtilGlobal::Min(Stack().Clip[Focus - 1].Rect.b, Stack().Clip[Focus - 1].Offset.y + r.b);
		_DrawOption::CurClipRect() = Stack().Clip[Focus].Rect;
		Stack().Clip[Focus].Offset.x = _DrawOption::CurHotspotX() = Stack().Clip[Focus - 1].Offset.x + ((r.l != -size::max / 2)? r.l : 0) + hx; // 현재기준위치X
		Stack().Clip[Focus].Offset.y = _DrawOption::CurHotspotY() = Stack().Clip[Focus - 1].Offset.y + ((r.t != -size::max / 2)? r.t : 0) + hy; // 현재기준위치Y
		Stack().Clip[Focus].Size.w = _DrawOption::CurAreaWidth() = r.r - r.l; // 현재영역사이즈W
		Stack().Clip[Focus].Size.h = _DrawOption::CurAreaHeight() = r.b - r.t; // 현재영역사이즈H
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_clip;
		++Stack().CountCurrently;
		#ifdef __BX_OPENGL
		BxCore::OpenGL2D::Clip(_DrawOption::CurClipRect());
		#endif
		return StyleStack::One();
	}

	/*!
	\brief 스타일-중점이동
	\param hx : 중점X
	\param hy : 중점Y
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack HOTSPOT(int hx, int hy)
	{
		return CLIP(BxDrawGlobal::XYXY(-size::max / 2, -size::max / 2, size::max / 2, size::max / 2), hx, hy);
	}

	/*!
	\brief 스타일-마스크
	\param operate : 출력여부를 결정할 마스크값
	\param flag : 출력후 마스크를 기록하는 방법
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack MASK(maskoperate operate = maskoperate_default, maskflag flag = maskflag_default)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_mask];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Mask[Focus].Operate = _DrawOption::CurMaskOperate() = (operate != maskoperate_default)? operate : Stack().Mask[Focus - 1].Operate;
		Stack().Mask[Focus].Flag = _DrawOption::CurMaskFlag() = (flag != maskflag_default)? flag : Stack().Mask[Focus - 1].Flag;
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_mask;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-회전
	\param angle1024 : 1024도기준 회전값
	\param doFlip : 좌우대칭여부(회전보다 먼저 적용)
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack ROTATE(int angle1024, bool doFlip = false)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_rotate];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		angle1024 += (Stack().Rotate[Focus - 1].Turn / 2) * 256 + Stack().Rotate[Focus - 1].Angle;
		angle1024 = (angle1024 + (1024 << 16)) % 1024;
		Stack().Rotate[Focus].Turn = _DrawOption::CurTurn() = (drawturn)(angle1024 / 256 * 2 + ((Stack().Rotate[Focus - 1].Turn + doFlip) % 2)); // 현재턴
		Stack().Rotate[Focus].Angle = _DrawOption::CurAngle() = angle1024 & 0xFF;
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_rotate;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-확대율(fint)
	\param hor : 수평
	\param ver : 수직
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack SCALE(fint hor, fint ver)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_scale];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Scale[Focus].Horizon = _DrawOption::CurScaleHor() = (fint)((((huge) Stack().Scale[Focus - 1].Horizon) * hor) >> 16); // 현재수평배율
		Stack().Scale[Focus].Vertical = _DrawOption::CurScaleVer() = (fint)((((huge) Stack().Scale[Focus - 1].Vertical) * ver) >> 16); // 현재수직배율
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_scale;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-확대율(double)
	\param hor : 수평
	\param ver : 수직
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack SCALE(double hor, double ver)
	{
		return SCALE((int)(0x10000 * hor + 0.5), (int)(0x10000 * ver + 0.5));
	}

	/*!
	\brief 스타일-컬러링(byte)
	\param r : 빨강
	\param g : 초록
	\param b : 파랑
	\param rate : 이전 컬러링과 혼합을 위한 영향력값
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack COLOR(byte r, byte g, byte b, uint rate = 100)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_color];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Color[Focus].R = (Stack().Color[Focus - 1].R * Stack().Color[Focus - 1].Sum + r * rate) / (Stack().Color[Focus - 1].Sum + rate);
		Stack().Color[Focus].G = (Stack().Color[Focus - 1].G * Stack().Color[Focus - 1].Sum + g * rate) / (Stack().Color[Focus - 1].Sum + rate);
		Stack().Color[Focus].B = (Stack().Color[Focus - 1].B * Stack().Color[Focus - 1].Sum + b * rate) / (Stack().Color[Focus - 1].Sum + rate);
		Stack().Color[Focus].Sum = Stack().Color[Focus - 1].Sum + rate;
		Stack().Color[Focus].Color = _DrawOption::CurColor() = RGB32(Stack().Color[Focus].R, Stack().Color[Focus].G, Stack().Color[Focus].B); // 현재색상
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_color;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-컬러링(color_x888)
	\param rgb : 색상
	\param rate : 이전 컬러링과 혼합을 위한 영향력값
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack COLOR(color_x888 rgb, uint rate = 100)
	{
		return COLOR((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, rate);
	}

	/// @cond SECTION_NAME
	static StyleStack HUE(byte hue)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_hue];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Hue[Focus] = Stack().Hue[Focus - 1] + hue;
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_hue;
		++Stack().CountCurrently;
		return StyleStack::One();
	}
	/// @endcond

	/*!
	\brief 스타일-불투명도
	\param opa : 짙은 정도
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack OPACITY(byte opa)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_opacity];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Opacity[Focus].Opacity255 = Stack().Opacity[Focus - 1].Opacity255 * opa / 255;
		Stack().Opacity[Focus].Opacity7 = _DrawOption::CurOpacity7() = 8 * Stack().Opacity[Focus].Opacity255 / 255 - 1; // 현재불투명도
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_opacity;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-블렌딩
	\param blend : 블렌딩메소드(20가지)
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack BLEND(drawblend blend)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_blend];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Blend[Focus] = _DrawOption::CurBlend() = blend; // 현재블랜드
		#ifndef __BX_OPENGL
		_DrawOption::CurSpriteTable() = _DrawOption::GetSpriteTable(blend);
		_DrawOption::CurAlphaTable() = _DrawOption::GetAlphaTable(blend);
		#endif
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_blend;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-채우기
	\param form : From객체(BxDrawForm상속으로 사용자From 제작가능)
	\param r : 로드할 구역
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack FORM(BxDrawForm* form = nullptr, rect r = XYWH(0, 0, size::max, size::max))
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_form];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		if(!form) form = _BxColorMap::GetClass();
		// 애니메이션
		else if(1 < form->GetPage())
		{
			const int NumPage = form->GetPage();
			const int CurPage = BxUtilGlobal::Max(0, BxCore::Main::GetCurrentRenderCount() - form->GetFrameBegin()) % NumPage;
			const int PageOffset = form->Height() * CurPage / NumPage;
			const int PageHeight = BxUtilGlobal::Min(r.b - r.t, form->Height() / NumPage);
			r.b = (r.t += PageOffset) + PageHeight;
		}
		Stack().Form[Focus].Form = _DrawOption::CurForm() = form; // 현재폼
		Stack().Form[Focus].Rect = _DrawOption::CurFormRect() = r; // 현재폼영역
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_form;
		++Stack().CountCurrently;
		return StyleStack::One();
	}

	/*!
	\brief 스타일-3D모델이동
	\param x : 이동X
	\param y : 이동Y
	\param z : 이동Z
	\return 스택처리수량(무조건 1), 여러가지 스타일을 +로 복합사용하기 위함
	*/
	static StyleStack MOVE3D(int x, int y, int z)
	{
		const int Focus = ++Stack().StyleFocus[drawstyle_move3d];
		BxASSERT("BxDraw", Focus < Stack().MAX);
		Stack().Move3D[Focus].x = _DrawOption::CurMoveX3D() = Stack().Move3D[Focus - 1].x + x; // 현재이동위치3D
		Stack().Move3D[Focus].y = _DrawOption::CurMoveY3D() = Stack().Move3D[Focus - 1].y + y;
		Stack().Move3D[Focus].z = _DrawOption::CurMoveZ3D() = Stack().Move3D[Focus - 1].z + z;
		// 스택증가
		Stack().Order[++Stack().OrderFocus] = drawstyle_move3d;
		++Stack().CountCurrently;
		return StyleStack::One();
	}
}

//! \brief OpenGL/SW랜더링기반 그리기도구
class BxDraw
{
	global_func inline bool& IsActivated() {global_data bool isActivated = false; return isActivated;}

public:
	/*!
	\brief 기본생성자
	*/
	BxDraw() : FigureMapWidthFix(32), FigureMapWidthLevel(5)
	{
		BxASSERT("BxDraw<생성수량이 허용치인 1을 초과합니다>", !IsActivated());
		IsActivated() = true;
		#ifdef __BX_OPENGL
		BxCore::OpenGL2D::Init();
		#endif
		BxCore::Surface::Init();
		// 버튼이벤트초기화
		ButtonLength = 0;
		DoClearButtonEvent = false;
		// 도형버퍼초기화
		FigureMapHeight = 0;
		FigureMap = nullptr;
		// 백버퍼 초기화
		BackBufferWidth = 0;
		BackBufferHeight = 0;
		BackBufferPtr = nullptr;
		Flush(true);
	}

	/*!
	\brief 소멸자
	*/
	~BxDraw()
	{
		// 백버퍼 제거
		BxDelete_Array(BackBufferPtr);
		// 도형버퍼 제거
		FigureMapHeight = 0;
		BxDelete_Array(FigureMap);
		IsActivated() = false;
	}

	/*!
	\brief 현재 X축 기준좌표얻기
	\return X축 기준좌표
	*/
	inline int TranslateX()
	{
		return BxDrawGlobal::_DrawOption::CurHotspotX();
	}

	/*!
	\brief 현재 Y축 기준좌표얻기
	\return Y축 기준좌표
	*/
	inline int TranslateY()
	{
		return BxDrawGlobal::_DrawOption::CurHotspotY();
	}

	/*!
	\brief 현재 너비얻기
	\param isScreen : true-스크린, false-현재클리핑
	\return 너비
	*/
	inline int Width(bool isScreen = false)
	{
		if(isScreen) return BxCore::Surface::GetWidthHW();
		return BxDrawGlobal::_DrawOption::CurAreaWidth();
	}

	/*!
	\brief 현재 높이얻기
	\param isScreen : true-스크린, false-현재클리핑
	\return 높이
	*/
	inline int Height(bool isScreen = false)
	{
		if(isScreen) return BxCore::Surface::GetHeightHW();
		return BxDrawGlobal::_DrawOption::CurAreaHeight();
	}

	/*!
	\brief 백버퍼 주소얻기
	\return 백버퍼 메모리주소
	*/
	inline pixel_dst* GetBackBufferPtr() {return BackBufferPtr;}

	/*!
	\brief 백버퍼 피치정보얻기
	\return 가로 한줄의 배열길이(바이트길이가 아님)
	*/
	inline int GetBackBufferPitch() {return BackBufferWidth;}

	/*!
	\brief 형식없는 기본출력
	\param x : 출력위치X
	\param y : 출력위치Y
	\param count : 스타일옵션(이미지는 FORM으로 가능), 다중설정시 >>사용
	*/
	void Area(int x, int y, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		// 무조건 이관
		area FormArea = *BxDrawGlobal::_DrawOption::CurForm()->GetArea();
		rect FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
		FormArea.w = BxUtilGlobal::Min(FormArea.w, FormRect.r - FormRect.l);
		FormArea.h = BxUtilGlobal::Min(FormArea.h, FormRect.b - FormRect.t);
		x = FtoI(x * BxDrawGlobal::_DrawOption::CurScaleHor());
		y = FtoI(y * BxDrawGlobal::_DrawOption::CurScaleVer());
		const uint Angle = BxDrawGlobal::_DrawOption::CurAngle();
		const drawturn Turn = BxDrawGlobal::_DrawOption::CurTurn();
		if(0 < Angle || Turn != turn_0)
		{
			const int AngleValue = ((1024 << 16) + (Angle + Turn / 2 * 256)) % 1024;
			const huge AngleCos = BxUtil::Cos(AngleValue);
			const huge AngleSin = BxUtil::Sin(AngleValue);
			const int AngleFlip = (Turn % 2)? -1 : 1;
			const int TempX = FtoI(AngleFlip * x * AngleCos - y * AngleSin);
			const int TempY = FtoI(AngleFlip * x * AngleSin + y * AngleCos);
			x = TempX, y = TempY;
		}
		Rectangle(BxDrawGlobal::FILL, BxDrawGlobal::XYWH(-FormArea.hx, -FormArea.hy, FormArea.w, FormArea.h),
			count >> BxDrawGlobal::HOTSPOT(x, y));
	}

	/*!
	\brief 사각형출력
	\param opt : FILL-채움그리기, DRAW-틀그리기
	\param r : 출력영역
	\param count : 스타일옵션, 다중설정시 >>사용
	*/
	void Rectangle(BxDrawGlobal::option opt, rect r, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		#ifdef __BX_OPENGL
		// 조건 이관
		if(0 < BxDrawGlobal::_DrawOption::CurAngle())
		{
			PolygonSt(opt, BxDrawGlobal::XYS(BxTHROW(r.l, r.l, r.r, r.r), BxTHROW(r.t, r.b, r.b, r.t)), count);
			return;
		}
		// 모듈
		if(AddStyleByIf(count))
		{
			const BxExpress::IntegerX& FormWidth = BxDrawGlobal::_DrawOption::CurForm()->ExpressWidth();
			const BxExpress::IntegerX& FormHeight = BxDrawGlobal::_DrawOption::CurForm()->ExpressHeight();
			const rect& FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
			rect uvclip;
			uvclip.l = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.l, FormWidth) / FormWidth);
			uvclip.t = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.t, FormHeight) / FormHeight);
			uvclip.r = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.r, FormWidth) / FormWidth);
			uvclip.b = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.b, FormHeight) / FormHeight);
			id_opengl_outline Outline = BxCore::OpenGL2D::MakeOutline_VertexUV(r, uvclip);
			if(Outline)
			{
				const bool DoFlip = (BxDrawGlobal::_DrawOption::CurTurn() % 2 == 1);
				const int Angle = ((1024 << 16) + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256) % 1024;
				BxCore::OpenGL2D::Outline_VertexUV_SetScale(Outline, BxDrawGlobal::_DrawOption::CurScaleHor(), BxDrawGlobal::_DrawOption::CurScaleVer());
				BxCore::OpenGL2D::Outline_VertexUV_SetRotate(Outline, Angle, DoFlip);
				BxCore::OpenGL2D::Render(BxDrawGlobal::_DrawOption::CurForm()->GetFormGL(), Outline,
					BxDrawGlobal::_DrawOption::CurHotspotX(), BxDrawGlobal::_DrawOption::CurHotspotY(),
					255 * (BxDrawGlobal::_DrawOption::CurOpacity7() + 1) / 8,
					BxDrawGlobal::_DrawOption::CurColor());
				BxCore::OpenGL2D::FreeOutline(Outline);
			}
			SubStyle();
		}
		#else
		// 조건 이관
		if(0 < BxDrawGlobal::_DrawOption::CurAngle())
		{
			PolygonSt(opt, BxDrawGlobal::XYS(BxTHROW(r.l, r.r - 1, r.r - 1, r.l), BxTHROW(r.t, r.t, r.b, r.b)), count);
			return;
		}
		// 모듈
		if(!AddStyleByIf(count)) return;
		const drawturn Turn = BxDrawGlobal::_DrawOption::CurTurn();
		const bool IsFlip90 = (Turn == turn_90 || Turn == turn_x90 || Turn == turn_270 || Turn == turn_x270);
		const fint ScaleHor = (IsFlip90)? BxDrawGlobal::_DrawOption::CurScaleVer() : BxDrawGlobal::_DrawOption::CurScaleHor();
		const fint ScaleVer = (IsFlip90)? BxDrawGlobal::_DrawOption::CurScaleHor() : BxDrawGlobal::_DrawOption::CurScaleVer();
		const rect C = {r.l, r.t, r.r, r.b};
		switch(Turn)
		{
		case turn_0   :
		case turn_x0  : r.l = +C.l; r.t = +C.t; r.r = +C.r; r.b = +C.b; break;
		case turn_90  :
		case turn_x90 : r.l = -C.b; r.t = +C.l; r.r = -C.t; r.b = +C.r; break;
		case turn_180 :
		case turn_x180: r.l = -C.r; r.t = -C.b; r.r = -C.l; r.b = -C.t; break;
		case turn_270 :
		case turn_x270: r.l = +C.t; r.t = -C.r; r.r = +C.b; r.b = -C.l; break;
		}
		const int Left = FtoI(r.l * ScaleHor) + BxDrawGlobal::_DrawOption::CurHotspotX();
		const int Top = FtoI(r.t * ScaleVer) + BxDrawGlobal::_DrawOption::CurHotspotY();
		const int Right = FtoI(r.r * ScaleHor) + BxDrawGlobal::_DrawOption::CurHotspotX();
		const int Bottom = FtoI(r.b * ScaleVer) + BxDrawGlobal::_DrawOption::CurHotspotY();
		rect Rect = {
			BxUtilGlobal::Max(BxDrawGlobal::_DrawOption::CurClipRect().l, Left),
			BxUtilGlobal::Max(BxDrawGlobal::_DrawOption::CurClipRect().t, Top),
			BxUtilGlobal::Min(BxDrawGlobal::_DrawOption::CurClipRect().r, Right),
			BxUtilGlobal::Min(BxDrawGlobal::_DrawOption::CurClipRect().b, Bottom)};
		if(0 < Rect.r - Rect.l && 0 < Rect.b - Rect.t)
		{
			// 모듈링준비
			const drawturn Turn = BxDrawGlobal::_DrawOption::CurTurn();
			const area& FormArea = *BxDrawGlobal::_DrawOption::CurForm()->GetArea();
			const bool IsFlip90 = (Turn == turn_90 || Turn == turn_x90 || Turn == turn_270 || Turn == turn_x270);
			const fint DescaleX = (fint)(ox100000000 / ScaleHor);
			const fint DescaleY = (fint)(ox100000000 / ScaleVer);
			const fint XBegin = (IsFlip90)? (Rect.t - Top) * DescaleY : (Rect.l - Left) * DescaleX;
			const fint YBegin = (IsFlip90)? (Rect.l - Left) * DescaleX : (Rect.t - Top) * DescaleY;
			fint XFocus = (Turn == turn_0 || Turn == turn_90 || Turn == turn_x180 || Turn == turn_x270)? XBegin : ItoF(FormArea.w) - 1 - XBegin;
			fint YFocus = (Turn == turn_0 || Turn == turn_x0 || Turn == turn_270 || Turn == turn_x270)? YBegin : ItoF(FormArea.h) - 1 - YBegin;
			const fint XVec = (IsFlip90)? 0 : ((Turn == turn_0 || Turn == turn_x180)? DescaleX : -DescaleX);
			const fint YVec = (!IsFlip90)? 0 : ((Turn == turn_x270 || Turn == turn_270)? DescaleX : -DescaleX);
			const fint XAdd = (!IsFlip90)? 0 : ((Turn == turn_90 || Turn == turn_x270)? DescaleY : -DescaleY);
			const fint YAdd = (IsFlip90)? 0 : ((Turn == turn_0 || Turn == turn_x0)? DescaleY : -DescaleY);
			// 블렌딩준비
			pixel_dst* ScreenPtr = GetBackBufferPtr();
			pixel_dst* Dst = &ScreenPtr[Rect.l + Rect.t * GetBackBufferPitch()];
			const uint DstPitch = GetBackBufferPitch();
			const uint DstCopyLen = Rect.r - Rect.l;
			#ifdef __BX_PIXEL16
				const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (5 + 5)];
				const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 5 + 4)];
			#else
				const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (8 + 8)];
				const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 8 + 8)];
			#endif
			// 백버퍼카피
			BxDrawForm& FormClass = *BxDrawGlobal::_DrawOption::CurForm();
			rect FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
			BxDrawForm::OnFormMethod FormMethod = BxDrawGlobal::_DrawOption::CurForm()->GetFormMethod
				(BxDrawGlobal::_DrawOption::CurMaskOperate(), BxDrawGlobal::_DrawOption::CurMaskFlag());
			for(int y = 0, yend = Rect.b - Rect.t; y < yend; ++y)
			{
				FormMethod(FormClass, FormRect, Dst, DstCopyLen, XFocus, YFocus, XVec, YVec, BlendSpriteTable, BlendAlphaTable);
				XFocus += XAdd, YFocus += YAdd;
				Dst += DstPitch;
			}
		}
		SubStyle();
		#endif
	}

	/// @cond SECTION_NAME
	void Line(rect r, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		#ifdef __BX_OPENGL
		// 모듈
		if(AddStyleByIf(count))
		{
			/////////////////////////////////
			SubStyle();
		}
		#else
		// 모듈
		if(!AddStyleByIf(count)) return;
		const int PosX = BxDrawGlobal::_DrawOption::CurHotspotX();
		const int PosY = BxDrawGlobal::_DrawOption::CurHotspotY();
		const int X1 = PosX + r.l;
		const int Y1 = PosY + r.t;
		const int X2 = PosX + r.r;
		const int Y2 = PosY + r.b;
		// 수정중...///////////////////////////////////////////
		if(Y1 != Y2)
		{
			#ifdef __BX_PIXEL16
				const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (5 + 5)];
				const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 5 + 4)];
			#else
				const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (8 + 8)];
				const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 8 + 8)];
			#endif
			const int BeginX = (Y1 < Y2)? X1 : X2, BeginY = (Y1 < Y2)? Y1 : Y2;
			const int EndX = (Y1 > Y2)? X1 : X2, EndY = (Y1 > Y2)? Y1 : Y2;
			const int DistanceX = EndX - BeginX, DistanceY = EndY - BeginY;

			pixel_dst* ScreenPtr = GetBackBufferPtr();
			BxDrawForm& FormClass = *BxDrawGlobal::_DrawOption::CurForm();
			rect FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
			BxDrawForm::OnFormMethod FormMethod = BxDrawGlobal::_DrawOption::CurForm()->GetFormMethod
				(BxDrawGlobal::_DrawOption::CurMaskOperate(), BxDrawGlobal::_DrawOption::CurMaskFlag());
			for(int y = BxUtilGlobal::Max(BxDrawGlobal::_DrawOption::CurClipRect().t, BeginY), yend = BxUtilGlobal::Min(EndY, BxDrawGlobal::_DrawOption::CurClipRect().b); y < yend; ++y)
			{
				const int x1 = BeginX + DistanceX * (y - BeginY) / DistanceY;
				const int x2 = BeginX + DistanceX * (y + 1 - BeginY) / DistanceY;
				const int xbegin = BxUtilGlobal::Max((x1 < x2)? x1 : x2, BxDrawGlobal::_DrawOption::CurClipRect().l);
				const int xend = BxUtilGlobal::Min((x1 > x2)? x1 : x2, BxDrawGlobal::_DrawOption::CurClipRect().r - 1);
				if(xbegin <= xend)
				{
					// 블렌딩준비
					pixel_dst* Dst = &ScreenPtr[xbegin + y * GetBackBufferPitch()];
					// 백버퍼카피
					FormMethod(FormClass, FormRect, Dst, xend - xbegin, ItoF(xbegin), ItoF(y), ItoF(1), ItoF(0), BlendSpriteTable, BlendAlphaTable);
				}
			}
		}
		SubStyle();
		#endif
	}
	/// @endcond

	/// @cond SECTION_NAME
	void Ellipse(BxDrawGlobal::option opt, rect r, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		#ifdef __BX_OPENGL
		// 모듈
		if(AddStyleByIf(count))
		{
			/////////////////////////////////
			SubStyle();
		}
		#else
		// 모듈
		if(!AddStyleByIf(count)) return;
		SubStyle();
		#endif
	}
	/// @endcond

	/*!
	\brief 폴리곤출력
	\param opt : FILL-채움그리기, DRAW-틀그리기
	\param p : 출력영역(XYS, XYARRAY로 생성)
	\param count : 스타일옵션, 다중설정시 >>사용
	*/
	void PolygonSt(BxDrawGlobal::option opt, points p, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		#ifdef __BX_OPENGL
		// 모듈
		if(AddStyleByIf(count))
		{
			const int FormWidth = BxDrawGlobal::_DrawOption::CurForm()->Width();
			const int FormHeight = BxDrawGlobal::_DrawOption::CurForm()->Height();
			const rect& FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
			rect uvclip;
			uvclip.l = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.l, FormWidth) / FormWidth);
			uvclip.t = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.t, FormHeight) / FormHeight);
			uvclip.r = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.r, FormWidth) / FormWidth);
			uvclip.b = (short) ((1 << 12) * BxUtilGlobal::MinMax(0, FormRect.b, FormHeight) / FormHeight);
			id_opengl_outline Outline = BxCore::OpenGL2D::MakeOutline_PolyVertexUV(p, uvclip);
			if(Outline)
			{
				const bool DoFlip = (BxDrawGlobal::_DrawOption::CurTurn() % 2 == 1);
				const int Angle = ((1024 << 16) + (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
				BxCore::OpenGL2D::Outline_PolyVertexUV_SetScale(Outline, BxDrawGlobal::_DrawOption::CurScaleHor(), BxDrawGlobal::_DrawOption::CurScaleVer());
				BxCore::OpenGL2D::Outline_PolyVertexUV_SetRotate(Outline, Angle, DoFlip);
				BxCore::OpenGL2D::Render(BxDrawGlobal::_DrawOption::CurForm()->GetFormGL(), Outline,
					BxDrawGlobal::_DrawOption::CurHotspotX(), BxDrawGlobal::_DrawOption::CurHotspotY(),
					255 * (BxDrawGlobal::_DrawOption::CurOpacity7() + 1) / 8,
					BxDrawGlobal::_DrawOption::CurColor());
				BxCore::OpenGL2D::FreeOutline(Outline);
			}
			SubStyle();
		}
		#else
		// 모듈
		if(!AddStyleByIf(count)) return;
		const bool DoFlip = (BxDrawGlobal::_DrawOption::CurTurn() % 2 == 1);
		const int Angle1 = ((1024 << 16) + (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
		const int Angle2 = ((1024 << 16) - (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
		const huge Cos1 = BxUtil::Cos(Angle1);
		const huge Sin1 = BxUtil::Sin(Angle1);
		const huge Cos2 = BxUtil::Cos(Angle2);
		const huge Sin2 = BxUtil::Sin(Angle2);
		const fint Hor = BxDrawGlobal::_DrawOption::CurScaleHor();
		const fint Ver = BxDrawGlobal::_DrawOption::CurScaleVer();
		const int PosX = BxDrawGlobal::_DrawOption::CurHotspotX();
		const int PosY = BxDrawGlobal::_DrawOption::CurHotspotY();
		const fint Half = 0x10000 / 2;

		// 클리핑
		BxASSERT("BxDraw", p.count <= 256);
		point Vertex[256] = {{0,},};
		rect VertexArea = {0,};
		const int VertexCount = p.count;
		for(int i = 0; i < VertexCount; ++i)
		{
			const int OX = FtoI(p.pt[i].x * Hor + Half);
			const int OY = FtoI(p.pt[i].y * Ver + Half);
			Vertex[i].x = FtoI(OX * Cos1 - OY * Sin1 + Half);
			Vertex[i].y = FtoI(OX * Sin1 + OY * Cos1 + Half);
			VertexArea.l = (i == 0)? Vertex[i].x : BxUtilGlobal::Min(VertexArea.l, Vertex[i].x);
			VertexArea.t = (i == 0)? Vertex[i].y : BxUtilGlobal::Min(VertexArea.t, Vertex[i].y);
			VertexArea.r = (i == 0)? Vertex[i].x + 1 : BxUtilGlobal::Max(VertexArea.r, Vertex[i].x + 1);
			VertexArea.b = (i == 0)? Vertex[i].y + 1 : BxUtilGlobal::Max(VertexArea.b, Vertex[i].y + 1);
		}
		VertexArea.l = BxUtilGlobal::Max(PosX + VertexArea.l, BxDrawGlobal::_DrawOption::CurClipRect().l);
		VertexArea.t = BxUtilGlobal::Max(PosY + VertexArea.t, BxDrawGlobal::_DrawOption::CurClipRect().t);
		VertexArea.r = BxUtilGlobal::Min(PosX + VertexArea.r, BxDrawGlobal::_DrawOption::CurClipRect().r);
		VertexArea.b = BxUtilGlobal::Min(PosY + VertexArea.b, BxDrawGlobal::_DrawOption::CurClipRect().b);

		// 매핑초기화
		for(int v = VertexArea.t; v < VertexArea.b; ++v)
		{
			FigureMap[(v << FigureMapWidthLevel) | 0] = 3;
			FigureMap[(v << FigureMapWidthLevel) | 1] = 1;
			FigureMap[(v << FigureMapWidthLevel) | 2] = VertexArea.l;
			FigureMap[(v << FigureMapWidthLevel) | 3] = VertexArea.r - 1;
		}
		// 매핑
		for(int p = 0, last_index = 0; p < VertexCount; ++p)
		{
			const int next_p = (p + 1 < VertexCount)? p + 1 : last_index;
			const int X1 = PosX + Vertex[p].x;
			const int Y1 = PosY + Vertex[p].y;
			const int X2 = PosX + Vertex[next_p].x;
			const int Y2 = PosY + Vertex[next_p].y;
			if(Y1 != Y2)
			{
				const bool IsBigY2 = (Y1 < Y2);
				const int BeginX = (IsBigY2)? X1 : X2, BeginY = (IsBigY2)? Y1 : Y2;
				const int EndX = (IsBigY2)? X2 : X1, EndY = (IsBigY2)? Y2 : Y1;
				const int DistanceX = EndX - BeginX, DistanceY = EndY - BeginY;
				const int BeautifulValue = 1;
				for(int y = BxUtilGlobal::Max(VertexArea.t, BeginY), yend = BxUtilGlobal::Min(EndY, VertexArea.b); y < yend; ++y)
				{
					const int x = BeginX + DistanceX * (y - BeginY + BeautifulValue) / (DistanceY + BeautifulValue);
					if(x <= FigureMap[(y << FigureMapWidthLevel) | 2]) ++FigureMap[(y << FigureMapWidthLevel) | 1];
					else if(x < FigureMap[(y << FigureMapWidthLevel) | FigureMap[y << FigureMapWidthLevel]])
					for(int f = 3, fend = FigureMap[y << FigureMapWidthLevel] + 1; f < fend; ++f)
						if(x < FigureMap[(y << FigureMapWidthLevel) | f])
						{
							for(int s = FigureMap[y << FigureMapWidthLevel], sbegin = f; sbegin <= s; --s)
								FigureMap[(y << FigureMapWidthLevel) | (s + 1)] = FigureMap[(y << FigureMapWidthLevel) | s];
							FigureMap[(y << FigureMapWidthLevel) | f] = x;
							++FigureMap[y << FigureMapWidthLevel];
							break;
						}
				}
			}
			if(next_p != last_index && Vertex[next_p].x == Vertex[last_index].x && Vertex[next_p].y == Vertex[last_index].y)
				last_index = BxUtilGlobal::Min(++p + 1, VertexCount - 1);
		}

		// 출력
		const area& FormArea = *BxDrawGlobal::_DrawOption::CurForm()->GetArea();
		const huge DeHor = ox100000000 / Hor;
		const huge DeVer = ox100000000 / Ver;
		const fint DrawAddX = (fint)((DeHor * Cos2) >> 16) * ((DoFlip)? -1 : 1);
		const fint DrawAddY = (fint)((DeVer * Sin2) >> 16);
		const fint SrcWidth = BxUtilGlobal::Max(1, FormArea.w * Hor);
		const fint SrcHeight = BxUtilGlobal::Max(1, FormArea.h * Ver);
		const fint SrcHotspotX = FormArea.hx * Hor;
		const fint SrcHotspotY = FormArea.hy * Ver;
		const fint AreaWidth = BxUtilGlobal::Max(1, BxDrawForm::CalcTextureSize(FormArea.w) * Hor);
		const fint AreaHeight = BxUtilGlobal::Max(1, BxDrawForm::CalcTextureSize(FormArea.h) * Ver);
		#ifdef __BX_PIXEL16
			const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (5 + 5)];
			const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 5 + 4)];
		#else
			const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (8 + 8)];
			const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 8 + 8)];
		#endif
		pixel_dst* ScreenPtr = GetBackBufferPtr();
		BxDrawForm& FormClass = *BxDrawGlobal::_DrawOption::CurForm();
		rect FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
		BxDrawForm::OnFormMethod FormMethod = BxDrawGlobal::_DrawOption::CurForm()->GetFormMethod
			(BxDrawGlobal::_DrawOption::CurMaskOperate(), BxDrawGlobal::_DrawOption::CurMaskFlag());
		for(int y = VertexArea.t; y < VertexArea.b; ++y)
		for(int f = 2 + (FigureMap[(y << FigureMapWidthLevel) | 1] % 2), fend = FigureMap[y << FigureMapWidthLevel]; f < fend; f += 2)
		{
			const int Left = FigureMap[(y << FigureMapWidthLevel) | f];
			const int Right = FigureMap[(y << FigureMapWidthLevel) | (f + 1)] + 1;
			if(Left < Right)
			{
				const fint RotateX = (fint)((Left - PosX) * Cos2 - (y - PosY) * Sin2);
				const fint RotateY = (fint)((Left - PosX) * Sin2 + (y - PosY) * Cos2);
				const fint DrawX = (fint)(((GetBeginAdd(RotateX, -SrcHotspotX, AreaWidth) - ((DoFlip)? SrcWidth : 0)) * ((DoFlip)? -DeHor : DeHor)) >> 16);
				const fint DrawY = (fint)((GetBeginAdd(RotateY, -SrcHotspotY, AreaHeight) * DeVer) >> 16);
				FormMethod(FormClass, FormRect, &ScreenPtr[Left + y * GetBackBufferPitch()], Right - Left, DrawX, DrawY, DrawAddX, DrawAddY, BlendSpriteTable, BlendAlphaTable);
			}
		}
		SubStyle();
		#endif
	}

	/// @cond SECTION_NAME
	void PolygonBz(BxDrawGlobal::option opt, points p, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		#ifdef __BX_OPENGL
		// 모듈
		if(AddStyleByIf(count))
		{
			/////////////////////////////////
			SubStyle();
		}
		#else
		// 모듈
		if(!AddStyleByIf(count)) return;
		const bool DoFlip = (BxDrawGlobal::_DrawOption::CurTurn() % 2 == 1);
		const int Angle1 = ((1024 << 16) + (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
		const int Angle2 = ((1024 << 16) - (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
		const huge Cos1 = BxUtil::Cos(Angle1);
		const huge Sin1 = BxUtil::Sin(Angle1);
		const huge Cos2 = BxUtil::Cos(Angle2);
		const huge Sin2 = BxUtil::Sin(Angle2);
		const fint Hor = BxDrawGlobal::_DrawOption::CurScaleHor();
		const fint Ver = BxDrawGlobal::_DrawOption::CurScaleVer();
		const int PosX = BxDrawGlobal::_DrawOption::CurHotspotX();
		const int PosY = BxDrawGlobal::_DrawOption::CurHotspotY();
		const fint Half = 0x10000 / 2;

		// 클리핑
		BxASSERT("BxDraw", p.count <= 256);
		point Vertex[256] = {{0,},};
		rect VertexArea = {0,};
		const int VertexCount = p.count;
		for(int i = 0; i < VertexCount; ++i)
		{
			const int OX = FtoI(p.pt[i].x * Hor + Half);
			const int OY = FtoI(p.pt[i].y * Ver + Half);
			Vertex[i].x = FtoI(OX * Cos1 - OY * Sin1 + Half);
			Vertex[i].y = FtoI(OX * Sin1 + OY * Cos1 + Half);
			VertexArea.l = (i == 0)? Vertex[i].x : BxUtilGlobal::Min(VertexArea.l, Vertex[i].x);
			VertexArea.t = (i == 0)? Vertex[i].y : BxUtilGlobal::Min(VertexArea.t, Vertex[i].y);
			VertexArea.r = (i == 0)? Vertex[i].x + 1 : BxUtilGlobal::Max(VertexArea.r, Vertex[i].x + 1);
			VertexArea.b = (i == 0)? Vertex[i].y + 1 : BxUtilGlobal::Max(VertexArea.b, Vertex[i].y + 1);
		}
		VertexArea.l = BxUtilGlobal::Max(PosX + VertexArea.l, BxDrawGlobal::_DrawOption::CurClipRect().l);
		VertexArea.t = BxUtilGlobal::Max(PosY + VertexArea.t, BxDrawGlobal::_DrawOption::CurClipRect().t);
		VertexArea.r = BxUtilGlobal::Min(PosX + VertexArea.r, BxDrawGlobal::_DrawOption::CurClipRect().r);
		VertexArea.b = BxUtilGlobal::Min(PosY + VertexArea.b, BxDrawGlobal::_DrawOption::CurClipRect().b);

		// 매핑초기화
		for(int v = VertexArea.t; v < VertexArea.b; ++v)
		{
			FigureMap[(v << FigureMapWidthLevel) | 0] = 3;
			FigureMap[(v << FigureMapWidthLevel) | 1] = 1;
			FigureMap[(v << FigureMapWidthLevel) | 2] = VertexArea.l;
			FigureMap[(v << FigureMapWidthLevel) | 3] = VertexArea.r - 1;
		}
		// 매핑
		for(int l = 1, last_index = 0; l <= VertexCount; ++l)
		{
			if(l < VertexCount && (Vertex[l].x != Vertex[last_index].x || Vertex[l].y != Vertex[last_index].y))
				continue;
			if(2 < l - last_index)
			for(int m = last_index; m < l; ++m)
			{
				const int UnitHalf = 16, UnitMax = UnitHalf * 2, UnitMaxPow2 = 2 * UnitMax * UnitMax;
				const int IndexA = (last_index <= m - 1)? m - 1 : l - 1;
				const int IndexB = m;
				const int IndexC = (m + 1 < l)? m + 1 : last_index;
				int OldX = 0, OldY = 0;
				for(int n = 0; n <= UnitMax; ++n)
				{
					const int RateA = (UnitMax - n) * (UnitMax - n);
					const int RateB = UnitMaxPow2 - (((UnitHalf - n) * (UnitHalf - n)) << 2);
					const int RateC = n * n;
					const int RateSum = RateA + RateB + RateC;
					const int CurX = PosX + (Vertex[IndexA].x * RateA + Vertex[IndexB].x * RateB + Vertex[IndexC].x * RateC) / RateSum;
					const int CurY = PosY + (Vertex[IndexA].y * RateA + Vertex[IndexB].y * RateB + Vertex[IndexC].y * RateC) / RateSum;
					if(n && OldY != CurY)
					{
						const bool IsBigY2 = (OldY < CurY);
						const int BeginX = (IsBigY2)? OldX : CurX, BeginY = (IsBigY2)? OldY : CurY;
						const int EndX = (IsBigY2)? CurX : OldX, EndY = (IsBigY2)? CurY : OldY;
						const int DistanceX = EndX - BeginX, DistanceY = EndY - BeginY;
						const int BeautifulValue = 1;
						for(int y = BxUtilGlobal::Max(VertexArea.t, BeginY), yend = BxUtilGlobal::Min(EndY, VertexArea.b); y < yend; ++y)
						{
							const int x = BeginX + DistanceX * (y - BeginY + BeautifulValue) / (DistanceY + BeautifulValue);
							if(x <= FigureMap[(y << FigureMapWidthLevel) | 2]) ++FigureMap[(y << FigureMapWidthLevel) | 1];
							else if(x < FigureMap[(y << FigureMapWidthLevel) | FigureMap[y << FigureMapWidthLevel]])
							for(int f = 3, fend = FigureMap[y << FigureMapWidthLevel] + 1; f < fend; ++f)
								if(x < FigureMap[(y << FigureMapWidthLevel) | f])
								{
									for(int s = FigureMap[y << FigureMapWidthLevel], sbegin = f; sbegin <= s; --s)
										FigureMap[(y << FigureMapWidthLevel) | (s + 1)] = FigureMap[(y << FigureMapWidthLevel) | s];
									FigureMap[(y << FigureMapWidthLevel) | f] = x;
									++FigureMap[y << FigureMapWidthLevel];
									break;
								}
						}
					}
					OldX = CurX;
					OldY = CurY;
				}
			}
			last_index = ++l;
		}

		// 출력
		const area& FormArea = *BxDrawGlobal::_DrawOption::CurForm()->GetArea();
		const huge DeHor = ox100000000 / Hor;
		const huge DeVer = ox100000000 / Ver;
		const fint DrawAddX = (fint)((DeHor * Cos2) >> 16) * ((DoFlip)? -1 : 1);
		const fint DrawAddY = (fint)((DeVer * Sin2) >> 16);
		const fint SrcWidth = BxUtilGlobal::Max(1, FormArea.w * Hor);
		const fint SrcHeight = BxUtilGlobal::Max(1, FormArea.h * Ver);
		const fint SrcHotspotX = FormArea.hx * Hor;
		const fint SrcHotspotY = FormArea.hy * Ver;
		const fint AreaWidth = BxUtilGlobal::Max(1, BxDrawForm::CalcTextureSize(FormArea.w) * Hor);
		const fint AreaHeight = BxUtilGlobal::Max(1, BxDrawForm::CalcTextureSize(FormArea.h) * Ver);
		#ifdef __BX_PIXEL16
			const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (5 + 5)];
			const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 5 + 4)];
		#else
			const byte* BlendSpriteTable = &BxDrawGlobal::_DrawOption::CurSpriteTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (8 + 8)];
			const byte* BlendAlphaTable = &BxDrawGlobal::_DrawOption::CurAlphaTable()[BxDrawGlobal::_DrawOption::CurOpacity7() << (3 + 8 + 8)];
		#endif
		pixel_dst* ScreenPtr = GetBackBufferPtr();
		BxDrawForm& FormClass = *BxDrawGlobal::_DrawOption::CurForm();
		rect FormRect = BxDrawGlobal::_DrawOption::CurFormRect();
		BxDrawForm::OnFormMethod FormMethod = BxDrawGlobal::_DrawOption::CurForm()->GetFormMethod
			(BxDrawGlobal::_DrawOption::CurMaskOperate(), BxDrawGlobal::_DrawOption::CurMaskFlag());
		for(int y = VertexArea.t; y < VertexArea.b; ++y)
		for(int f = 2 + (FigureMap[(y << FigureMapWidthLevel) | 1] % 2), fend = FigureMap[y << FigureMapWidthLevel]; f < fend; f += 2)
		{
			const int Left = FigureMap[(y << FigureMapWidthLevel) | f];
			const int Right = FigureMap[(y << FigureMapWidthLevel) | (f + 1)] + 1;
			if(Left < Right)
			{
				const fint RotateX = (fint)((Left - PosX) * Cos2 - (y - PosY) * Sin2);
				const fint RotateY = (fint)((Left - PosX) * Sin2 + (y - PosY) * Cos2);
				const fint DrawX = (fint)(((GetBeginAdd(RotateX, -SrcHotspotX, AreaWidth) - ((DoFlip)? SrcWidth : 0)) * ((DoFlip)? -DeHor : DeHor)) >> 16);
				const fint DrawY = (fint)((GetBeginAdd(RotateY, -SrcHotspotY, AreaHeight) * DeVer) >> 16);
				FormMethod(FormClass, FormRect, &ScreenPtr[Left + y * GetBackBufferPitch()], Right - Left, DrawX, DrawY, DrawAddX, DrawAddY, BlendSpriteTable, BlendAlphaTable);
			}
		}
		SubStyle();
		#endif
	}
	/// @endcond

	/*!
	\brief 3D폴리곤출력
	\param v : 3차원 출력영역(XYZS, XYZARRAY로 생성)
	\param uv : 텍스쳐의 UV값(1 << 12 기준)
	\param count : 스타일옵션, 다중설정시 >>사용
	*/
	void Polygon3D(vertexs v, points uv, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		if(!AddStyleByIf(count)) return;
		#ifdef __BX_OPENGL
		id_opengl_outline Outline = BxCore::OpenGL2D::MakeOutline_PolyVertexUV3D(v, uv);
		if(Outline)
		{
			//const bool DoFlip = (BxDrawGlobal::_DrawOption::CurTurn() % 2 == 1);
			//const int Angle = ((1024 << 16) + (BxDrawGlobal::_DrawOption::CurAngle() + BxDrawGlobal::_DrawOption::CurTurn() / 2 * 256)) % 1024;
			//BxCore::OpenGL2D::Outline_PolyVertexUV_SetScale(Outline, BxDrawGlobal::_DrawOption::CurScaleHor(), BxDrawGlobal::_DrawOption::CurScaleVer());
			//BxCore::OpenGL2D::Outline_PolyVertexUV_SetRotate(Outline, Angle, DoFlip);
			BxCore::OpenGL2D::Render3D(BxDrawGlobal::_DrawOption::CurForm()->GetFormGL(), Outline,
				BxDrawGlobal::_DrawOption::CurMoveX3D(), BxDrawGlobal::_DrawOption::CurMoveY3D(), BxDrawGlobal::_DrawOption::CurMoveZ3D(),
				255 * (BxDrawGlobal::_DrawOption::CurOpacity7() + 1) / 8,
				BxDrawGlobal::_DrawOption::CurColor());
			BxCore::OpenGL2D::FreeOutline(Outline);
		}
		#endif
		SubStyle();
	}

	/*!
	\brief 텍스트출력
	\param font : 폰트ID
	\param str : 스트링
	\param r : 출력영역
	\param sort : 숫자키패드 방식
	\param count : 스타일옵션, 다중설정시 >>사용
	*/
	void Text(id_font font, string str, rect r, fontsort sort, BxDrawGlobal::StyleStack count = BxDrawGlobal::StyleStack::Zero())
	{
		if(!VisibleTest(count)) return;
		if(!AddStyleByIf(count)) return;
		// 모듈
		r.l += BxDrawGlobal::_DrawOption::CurClipRect().l;
		r.t += BxDrawGlobal::_DrawOption::CurClipRect().t;
		r.r = BxUtilGlobal::Min(r.r + BxDrawGlobal::_DrawOption::CurClipRect().l, BxDrawGlobal::_DrawOption::CurClipRect().r);
		r.b = BxUtilGlobal::Min(r.b + BxDrawGlobal::_DrawOption::CurClipRect().t, BxDrawGlobal::_DrawOption::CurClipRect().b);
		fontsort OldSort = BxCore::Font::SetSort(sort);
		BxCore::Font::Draw(font, str, BxDrawGlobal::XY(r.l - BxCore::Main::GetCurrentGUIMargin().l, r.t - BxCore::Main::GetCurrentGUIMargin().t),
			BxDrawGlobal::WH(r.width(), r.height()), BxDrawGlobal::_DrawOption::CurColor(), (BxDrawGlobal::_DrawOption::CurOpacity7() + 1) * 0xFF / 8);
		BxCore::Font::SetSort(OldSort);
		SubStyle();
	}

	/*!
	\brief 백버퍼를 화면에 출력
	\param initOnly : 출력없이 버퍼관련 초기화만 수행
	*/
	void Flush(bool initOnly = false)
	{
		if(!initOnly)
			BxCore::Surface::Flush(BackBufferPtr, sizeof(pixel_dst) * BackBufferWidth, BackBufferHeight, sizeof(pixel_dst) * GetBackBufferPitch());
		// 버튼이벤트 초기화지시
		DoClearButtonEvent = true;
		// 백버퍼 초기화
		if(BackBufferWidth != BxCore::Surface::GetWidth(nullptr, false) || BackBufferHeight != BxCore::Surface::GetHeight(nullptr, false))
		{
			BackBufferWidth = BxCore::Surface::GetWidth(nullptr, false);
			BackBufferHeight = BxCore::Surface::GetHeight(nullptr, false);
			#ifndef __BX_OPENGL
				BxDelete_Array(BackBufferPtr);
				BackBufferPtr = BxNew_Array(pixel_dst, BackBufferWidth * BackBufferHeight);
			#endif
		}
		// 도형버퍼 재할당
		if(FigureMapHeight != BackBufferHeight)
		{
			FigureMapHeight = BackBufferHeight;
			BxDelete_Array(FigureMap);
			FigureMap = BxNew_Array(int, FigureMapWidthFix * FigureMapHeight);
		}
		// 스택 초기화
		BxDrawGlobal::Stack().CountCurrently = BxDrawGlobal::StyleStack::Zero();
		SubStyle(true);
	}

	/*!
	\brief 스타일옵션 스택추가
	\param count : 스타일옵션, 다중설정시 >>사용
	*/
	void AddStyle(BxDrawGlobal::StyleStack count)
	{
		BxASSERT("BxDraw", count == BxDrawGlobal::Stack().CountCurrently);
		BxDrawGlobal::Stack().Count[++BxDrawGlobal::Stack().CountFocus] = count;
		BxDrawGlobal::Stack().CountCurrently = BxDrawGlobal::StyleStack::Zero();
	}

	/*!
	\brief 클리핑고려 스타일옵션 스택추가(if안에서 사용권장)
	\param count : 스타일옵션, 다중설정시 >>사용
	\param buttonName : 버튼명, 작성시 현재 클리핑영역으로 GUI이벤트 작동
	\return true-정상처리, false-클리핑되어 출력할 것이 없음(SubStyle 자동처리)
	*/
	bool AddStyleByIf(BxDrawGlobal::StyleStack count, string buttonName = nullptr)
	{
		AddStyle(count);
		if(BxDrawGlobal::_DrawOption::CurClipRect().r <= BxDrawGlobal::_DrawOption::CurClipRect().l
			|| BxDrawGlobal::_DrawOption::CurClipRect().b <= BxDrawGlobal::_DrawOption::CurClipRect().t)
		{
			SubStyle();
			return false;
		}
		if(buttonName != nullptr)
		{
			if(DoClearButtonEvent)
			{
				ButtonLength = 0;
				DoClearButtonEvent = false;
			}
			BxASSERT("BxDraw", ButtonLength < sizeof(ButtonEvent) / sizeof(ButtonEvent[0]));
			ButtonEvent[ButtonLength++].SetEvent(buttonName, BxDrawGlobal::_DrawOption::CurClipRect());
		}
		return true;
	}

	/*!
	\brief 스타일옵션 스택해제
	\param doAll : true-전체스택해제, false-1회해제
	\return TRY모듈을 위한 false전달
	*/
	bool SubStyle(bool doAll = false)
	{
		BxASSERT("BxDraw", BxDrawGlobal::Stack().CountCurrently == BxDrawGlobal::StyleStack::Zero());
		if(doAll) BxDrawGlobal::Stack().Init(BackBufferWidth, BackBufferHeight);
		else BxDrawGlobal::Stack().Sub();
		return false;
	}

public:
	/// @cond SECTION_NAME
	// 버튼이벤트
	class _ButtonEvent
	{
	public:
		char Name[32];
		rect Field;
	public:
		_ButtonEvent() {Name[0] = '\0';}
		virtual ~_ButtonEvent() {}
	public:
		void SetEvent(string name, rect field)
		{
			const int Length = BxUtilGlobal::StrLen(name);
			BxASSERT("BxDraw", Length + 1 <= 32);
			BxCore::Util::MemCpy(Name, name, Length + 1);
			Field.l = field.l - BxCore::Main::GetCurrentGUIMargin().l;
			Field.t = field.t - BxCore::Main::GetCurrentGUIMargin().t;
			Field.r = field.r - BxCore::Main::GetCurrentGUIMargin().l;
			Field.b = field.b - BxCore::Main::GetCurrentGUIMargin().t;
		}
		void DeleteEvent()
		{
			Name[0] = '\0';
		}
	} ButtonEvent[256], CapturedButtonEvent;
	/// @endcond

	/// @cond SECTION_NAME
	int ButtonLength;
	bool DoClearButtonEvent;
	/// @endcond

protected:
	/// @cond SECTION_NAME
	// 백버퍼정보
	int BackBufferWidth;
	int BackBufferHeight;
	pixel_dst* _arr_ BackBufferPtr;
	// 도형버퍼
	const int FigureMapWidthFix;
	const int FigureMapWidthLevel;
	int FigureMapHeight;
	int* FigureMap;
	/// @endcond

	/// @cond SECTION_NAME
	inline bool VisibleTest(BxDrawGlobal::StyleStack count)
	{
		if(BxDrawGlobal::_DrawOption::CurOpacity7() < 0
			|| BxDrawGlobal::_DrawOption::CurScaleHor() == 0
			|| BxDrawGlobal::_DrawOption::CurScaleVer() == 0)
		{
			AddStyle(count);
			SubStyle();
			return false;
		}
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	inline int GetBeginAdd(const int Outline, const int DstBegin, const int SrcSize)
	{
		const int BeginPos = DstBegin - Outline;
		if(BeginPos <= 0) return -BeginPos % SrcSize;
		return SrcSize - 1 - ((BeginPos - 1) % SrcSize);
	}
	/// @endcond
};
