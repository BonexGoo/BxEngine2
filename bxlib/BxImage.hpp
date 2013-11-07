#pragma once
#include <BxDraw.hpp>

//! \brief 이미지 운영
class BxImage : public BxDrawForm
{
public:
	enum type {BMP, B16, B32, PNG, JPG, GIF};
	enum hotspot {KEEP = -1, PAD1 = 0, PAD2, PAD3, PAD4, PAD5, PAD6, PAD7, PAD8, PAD9};

	global_func int& ScaleNumber() {global_data int Scale = 1; return Scale;}

protected:
	/// @cond SECTION_NAME
	pixel_src* Bits;
	/// @endcond

public:
	/*!
	\brief 기본생성자
	*/
	BxImage() {Bits = null;}

	/*!
	\brief 소멸자
	*/
	virtual ~BxImage() {Release();}

	/*!
	\brief 이미지 로드
	\param Resource : 메모리리소스
	\param Length : 메모리리소스의 바이트길이
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\return 로딩성공여부
	*/
	bool Load(const byte* Resource, const int Length, type Type)
	{
		switch(Type)
		{
		case BMP:
			{
				const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Resource, ScaleNumber());
				bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Resource);
				BxCore::AddOn::ReleaseBMP(ScaledBmp);
				return Result;
			}
			break;
		case B16: return _LoadB16(Resource);
		case B32: return _LoadB32(Resource);
		case PNG: return _LoadPNG(Resource);
		case JPG: return _LoadJPG(Resource);
		case GIF: return _LoadGIF(Resource, Length);
		}
		return false;
	}

	/*!
	\brief 이미지 로드
	\param FileName : 파일명
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\return 로딩성공여부
	*/
	bool Load(string FileName, type Type)
	{
		id_file File = BxCore::File::Open(FileName, "rb");
		int Length = BxCore::File::GetSize(File);
		byte* Resource = null;
		if(Type == BMP)
		{
			Length += 2;
			Resource = BxNew_Array(byte, Length);
			Resource[0] = 'B';
			Resource[1] = 'x';
			BxCore::File::Read(File, &Resource[2], Length - 2);
			BxCore::File::Close(File);
		}
		else
		{
			Resource = BxNew_Array(byte, Length);
			BxCore::File::Read(File, Resource, Length);
			BxCore::File::Close(File);
		}
		bool Result = Load(Resource, Length, Type);
		BxDelete_Array(Resource);
		return Result;
	}

	/*!
	\brief 이미지 부분로드
	\param FileName : 파일명
	\param Type : 리소스타입(BMP, B16, B32, PNG, JPG, GIF)
	\param X : 잘라낼 위치X
	\param Y : 잘라낼 위치Y
	\param Width : 잘라낼 너비
	\param Height : 잘라낼 높이
	\param Hotspot : 초기중점(NumPad방식)
	\return 로딩성공여부
	*/
	bool Load(string FileName, type Type, const int X, const int Y, const int Width, const int Height, hotspot Hotspot = PAD7)
	{
		BxImage Src;
		if(!Src.Load(FileName, Type))
			return false;
		Create(Src, X, Y, Width, Height, Hotspot);
		return true;
	}

	/*!
	\brief 이미지 존재여부
	\return 존재여부
	\see Load, Create
	*/
	bool IsExist()
	{
		#ifdef __BX_OPENGL
			return (Bits != null || FormGL != null);
		#else
			return (Bits != null);
		#endif
	}

	/*!
	\brief 빈 이미지 생성
	\param Width : 생성낼 너비
	\param Height : 생성낼 높이
	\param HotspotX : 중점X
	\param HotspotY : 중점Y
	*/
	void Create(const int Width, const int Height, const int HotspotX, const int HotspotY)
	{
		Area.w = Width;
		Area.h = Height;
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = HotspotX;
		Area.hy = HotspotY;
		Flag = blendflag_null;
		Page = 1;
		FrameBegin = 0;
		const int AreaWidth = BxDrawForm::CalcTextureSize(Width);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Height);
		#ifdef __BX_OPENGL
		if(FormGL)
		{
			BxCore::OpenGL2D::FreeForm(FormGL);
			FormGL = null;
		}
		#endif
		BxDelete_Array(Bits);
		Bits = BxNew_Array(pixel_src, AreaWidth * AreaHeight);
		BxCore::Util::MemSet(Bits, 0, sizeof(pixel_src) * AreaWidth * AreaHeight);
	}

	/*!
	\brief 복사식 이미지 생성
	\param Src : 소스이미지
	\param X : 복사할 위치X
	\param Y : 복사할 위치Y
	\param Width : 복사할 너비
	\param Height : 복사할 높이
	\param Hotspot : 초기중점(NumPad방식)
	*/
	void Create(BxImage& Src, const int X, const int Y, const int Width, const int Height, hotspot Hotspot = PAD7)
	{
		int HotspotX = Src.Area.hx - X;
		int HotspotY = Src.Area.hy - Y;
		switch(Hotspot)
		{
		case PAD1: HotspotX = 0;         HotspotY = Height; break;
		case PAD2: HotspotX = Width / 2; HotspotY = Height; break;
		case PAD3: HotspotX = Width;     HotspotY = Height; break;
		case PAD4: HotspotX = 0;         HotspotY = Height / 2; break;
		case PAD5: HotspotX = Width / 2; HotspotY = Height / 2; break;
		case PAD6: HotspotX = Width;     HotspotY = Height / 2; break;
		case PAD7: HotspotX = 0;         HotspotY = 0; break;
		case PAD8: HotspotX = Width / 2; HotspotY = 0; break;
		case PAD9: HotspotX = Width;     HotspotY = 0; break;
		}
		Create(Width, Height, HotspotX, HotspotY);
		CopyFrom(0, 0, Src, X, Y, Width, Height);
	}

	/*!
	\brief 이미지 제거
	\see Load, Create
	*/
	void Release()
	{
		#ifdef __BX_OPENGL
		if(FormGL)
		{
			BxCore::OpenGL2D::FreeForm(FormGL);
			FormGL = null;
		}
		#endif
		BxDelete_Array(Bits);
	}

	/*!
	\brief 이미지복사
	\param X : 복사될 위치X
	\param Y : 복사될 위치Y
	\param Src : 소스이미지
	\param SrcX : 복사할 소스위치X
	\param SrcY : 복사할 소스위치Y
	\param SrcWidth : 복사할 소스너비
	\param SrcHeight : 복사할 소스높이
	*/
	void CopyFrom(int X, int Y, BxImage& Src, int SrcX, int SrcY, const int SrcWidth, const int SrcHeight)
	{
		if(Area.w <= 0 || Area.h <= 0 || Src.Area.w <= 0 || Src.Area.h <= 0) return;
		int SrcX2 = BxUtilGlobal::Min(SrcX + SrcWidth, Src.Area.w);
		int SrcY2 = BxUtilGlobal::Min(SrcY + SrcHeight, Src.Area.h);
		if(X < 0) SrcX -= X, X = 0;
		if(Y < 0) SrcY -= Y, Y = 0;
		if(SrcX < 0) X -= SrcX, SrcX = 0;
		if(SrcY < 0) Y -= SrcY, SrcY = 0;
		SrcX2 = BxUtilGlobal::Min(SrcX2 - SrcX, Area.w - X) + SrcX;
		SrcY2 = BxUtilGlobal::Min(SrcY2 - SrcY, Area.h - Y) + SrcY;
		if(SrcX2 - SrcX <= 0 || SrcY2 - SrcY <= 0) return;

		Flag = (blendflag)(Flag | Src.Flag);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaWidthSrc = BxDrawForm::CalcTextureSize(Src.Area.w);
		const int CopyLength = sizeof(pixel_src) * (SrcX2 - SrcX);
		for(int y = Y, srcy = SrcY; srcy < SrcY2; ++y, ++srcy)
			BxCore::Util::MemMove(&Bits[X + y * AreaWidth], &Src.Bits[SrcX + srcy * AreaWidthSrc], CopyLength);
	}

	/*!
	\brief 폼모듈용 콜백함수 얻기
	\param operate : 마스크조건
	\param flag : 쓰일 마스크기호
	\return 폼모듈용 콜백함수
	*/
	virtual OnFormMethod GetFormMethod(maskoperate operate, maskflag flag)
	{
		switch(operate)
		{
		case maskoperate_all_print:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_all_print, maskflag_write_0>();
			break;
		case maskoperate_no_print:
			return OnFormNoPrint;
		case maskoperate_print_by_1:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_print_by_1, maskflag_write_0>();
			break;
		case maskoperate_print_by_0:
			if(flag == maskflag_no_write)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_no_write>();
			else if(flag == maskflag_write_1)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_write_1>();
			else if(flag == maskflag_write_0)
				return GetFormMethodSwitch<maskoperate_print_by_0, maskflag_write_0>();
			break;
		}
		return null;
	}

protected:
	/// @cond SECTION_NAME
	template<maskoperate MASKOP, maskflag MASK, blendflag BLEND>
	global_func void OnForm(BxDrawForm& srcClass, const rect srcRect, pixel_dst* rowBuffer, const uint rowLength,
		const fint x, const fint y, const fint xVec, const fint yVec, const byte* blendSpriteTable, const byte* blendAlphaTable)
	{
		const pixel_dst* RowBufferEnd = rowBuffer + rowLength;
		const int AreaWidth = BxDrawForm::CalcTextureSize(srcClass.Width());
		const int AreaHeight = BxDrawForm::CalcTextureSize(srcClass.Height());
		const int SrcRowShift = BxDrawForm::CalcTextureLevel(AreaWidth);
		const int SrcWidthMask = AreaWidth - 1;
		const int SrcHeightMask = AreaHeight - 1;
		const pixel_src* SrcBits = ((BxImage*) &srcClass)->Bits + srcRect.l + srcRect.t * AreaWidth;
		fint XFocus = x + (AreaWidth << (16 + 6)), YFocus = y + (AreaHeight << (16 + 6));
		if(BLEND & blendflag_alpha)
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					if(TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)])\
					{\
						*rowBuffer = ((TempColor & _OPA_BIT_) == _OPA_BIT_)?\
							BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable)\
							: BxDrawGlobal::AlphaBlending(*rowBuffer, TempColor, blendAlphaTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
		else if(BLEND & blendflag_sprite)
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					if(TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)])\
					{\
						*rowBuffer = BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
		else
		{
			pixel_src TempColor;
			switch(~(rowLength - 1) & 0x3F)
			while(rowBuffer < RowBufferEnd)
				CASE64(\
					if(MASKOP == maskoperate_all_print || (MASKOP == maskoperate_print_by_1 && (*rowBuffer & _MASK_BIT_)) || (MASKOP == maskoperate_print_by_0 && (~*rowBuffer & _MASK_BIT_)))\
					{\
						TempColor = SrcBits[(FtoI(XFocus) & SrcWidthMask) | ((FtoI(YFocus) & SrcHeightMask) << SrcRowShift)];\
						*rowBuffer = BxDrawGlobal::SpriteBlending(*rowBuffer, TempColor, blendSpriteTable);\
						if(MASK == maskflag_write_1) *rowBuffer |= _MASK_BIT_;\
						if(MASK == maskflag_write_0) *rowBuffer &= ~_MASK_BIT_;\
					}\
					++rowBuffer;\
					XFocus += xVec;\
					YFocus += yVec;)
		}
	}
	/// @endcond

	/// @cond SECTION_NAME
	template<maskoperate MASKOP, maskflag MASK>
	inline OnFormMethod GetFormMethodSwitch()
	{
		switch(Flag)
		{
		case blendflag_null: return OnForm<MASKOP, MASK, blendflag_null>;
		case blendflag_alpha: return OnForm<MASKOP, MASK, blendflag_alpha>;
		case blendflag_sprite: return OnForm<MASKOP, MASK, blendflag_sprite>;
		case blendflag_alpha | blendflag_sprite: return OnForm<MASKOP, MASK, (blendflag) 3>;
		}
		return null;
	}
	/// @endcond

	/// @cond SECTION_NAME
	#ifdef __BX_OPENGL
	virtual void _UpdateFormGL()
	{
		if(FormGL) return;
		const int WidthByMargin = BxCore::OpenGL2D::GetTextureMargin() + Width() + BxCore::OpenGL2D::GetTextureMargin();
		const int HeightByMargin = BxCore::OpenGL2D::GetTextureMargin() + Height() + BxCore::OpenGL2D::GetTextureMargin();
		color_a888* TextureImage = BxNew_Array(color_a888, WidthByMargin * HeightByMargin);
		BxCore::Util::MemSet(TextureImage, 0, sizeof(color_a888) * WidthByMargin * HeightByMargin);
		const int TextureRow = WidthByMargin;
		const int TextureMargin = BxCore::OpenGL2D::GetTextureMargin() + TextureRow * BxCore::OpenGL2D::GetTextureMargin();
		const int BitsRow = BxDrawForm::CalcTextureSize(Width());
		// 마진고려 복사
		for(int y = 0; y < Height(); ++y)
		for(int x = 0; x < Width(); ++x)
		{
			const pixel_src OnePixel = Bits[x + y * BitsRow];
			#ifdef __BX_PIXEL16
				TextureImage[TextureMargin + x + y * TextureRow] = BxDrawGlobal::ColorM555A444ToA888(OnePixel);
			#else
				TextureImage[TextureMargin + x + y * TextureRow] = OnePixel;
			#endif
		}
		BxDelete_Array(Bits);
		// 투명픽셀 보간
		if(BxCore::OpenGL2D::DoTextureInterpolation())
		{
			const color_a888 MaskPixel = 0x00F8F8F8;
			for(uint i = 0, ib = 0, iend = (uint) BxUtilGlobal::Max(1, BxCore::OpenGL2D::GetTextureMargin()); i < iend; ib = (++i & 1))
			for(int y = 0; y < HeightByMargin; ++y)
			{
				const bool IsYB = (0 < y), IsYE = (y < HeightByMargin - 1);
				for(int x = 0; x < WidthByMargin; ++x)
				{
					const int XM = x + 0, YM = (y + 0) * TextureRow;
					if(TextureImage[XM + YM]) continue;
					const bool IsXB = (0 < x), IsXE = (x < WidthByMargin - 1);
					const int XL = x - 1, XR = x + 1;
					const int YT = (y - 1) * TextureRow, YB = (y + 1) * TextureRow;
					int Check = 0;
					color_a888 OnePixelA = 0, Sample = 0;
					if(IsXB && IsYB && (Sample = TextureImage[XL + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(        IsYB && (Sample = TextureImage[XM + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE && IsYB && (Sample = TextureImage[XR + YT]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXB         && (Sample = TextureImage[XL + YM]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE         && (Sample = TextureImage[XR + YM]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXB && IsYE && (Sample = TextureImage[XL + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(        IsYE && (Sample = TextureImage[XM + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(IsXE && IsYE && (Sample = TextureImage[XR + YB]) && ((Sample & 0xFF000000) || ((Sample & 1) == ib)) && ++Check) OnePixelA += (Sample & MaskPixel) >> 3;
					if(!Check) continue;
					const color_a888 OnePixelB = (const color_a888)
						(((((OnePixelA >> 16) & 0xFF) << 3) / Check) << 16) |
						(((((OnePixelA >> 8) & 0xFF) << 3) / Check) << 8) |
						(((((OnePixelA >> 0) & 0xFF) << 3) / Check) << 0);
					TextureImage[XM + YM] = (OnePixelB & 0xFFFFFFFE) | (~ib & 1);
				}
			}
		}
		FormGL = BxCore::OpenGL2D::MakeForm_TextureMaterial(TextureImage, WidthByMargin, HeightByMargin, TextureRow);
		BxDelete_Array(TextureImage);
	}
	#endif
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadBMP(const byte* Resource)
	{
		typedef struct {uint Size; ushort Reserved1; ushort Reserved2; uint OffBits;} BITMAPFILEHEADER;
		typedef struct {uint Size; int Width; int Height; ushort Planes; ushort BitCount; uint Compression;
			uint SizeImage; int XPelsPerMeter; int YPelsPerMeter; uint ClrUsed; uint ClrImportant;} BITMAPINFOHEADER;
		global_data const byte DevicePalette[3 * 16] = {
			0, 0, 0, 128, 0, 0, 0, 128, 0, 128, 128, 0,
			0, 0, 128, 128, 0, 128, 0, 128, 128, 128, 128, 128,
			192, 192, 192, 255, 0, 0, 0, 255, 0, 255, 255, 0,
			0, 0, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255};

		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "BxBM"))
			return false;

		// FILEHEADER/INFOHEADER/PALETTE 로드
		color_bgra BitmapPalette[256] = {{0,},};
		BITMAPFILEHEADER BitmapFileHeader;
		BITMAPINFOHEADER BitmapInfoHeader;
		BxCore::Util::MemMove(&BitmapFileHeader, BxUtilGlobal::LoadType(ResourcePtr, sizeof(BITMAPFILEHEADER)), sizeof(BITMAPFILEHEADER));
		BxCore::Util::MemMove(&BitmapInfoHeader, BxUtilGlobal::LoadType(ResourcePtr, sizeof(BITMAPINFOHEADER)), sizeof(BITMAPINFOHEADER));
		if(0 < BitmapInfoHeader.ClrUsed)
			BxUtilGlobal::LoadBytes(ResourcePtr, BitmapPalette, BitmapInfoHeader.ClrUsed * sizeof(color_bgra));
		else if(0 < BitmapFileHeader.OffBits - 54)
		{
			BitmapInfoHeader.ClrUsed = (BitmapFileHeader.OffBits - 54) / sizeof(color_bgra);
			BxUtilGlobal::LoadBytes(ResourcePtr, BitmapPalette, BitmapFileHeader.OffBits - 54);
		}

		// PALETTE 오류극복
		if(BitmapInfoHeader.ClrUsed == 0 && BitmapInfoHeader.BitCount < 8)
		{
			switch(BitmapInfoHeader.BitCount)
			{
			case 1:
				BitmapInfoHeader.ClrUsed = 2;
				BitmapPalette[0].r = 0;
				BitmapPalette[0].g = 0;
				BitmapPalette[0].b = 0;
				BitmapPalette[1].r = 255;
				BitmapPalette[1].g = 255;
				BitmapPalette[1].b = 255;
				break;
			case 4:
				{
					BitmapInfoHeader.ClrUsed = 16;
					for(int i = 0, iend = BitmapInfoHeader.ClrUsed; i < iend; ++i)
					{
						BitmapPalette[i].r = DevicePalette[i * 3 + 0];
						BitmapPalette[i].g = DevicePalette[i * 3 + 1];
						BitmapPalette[i].b = DevicePalette[i * 3 + 2];
					}
				}
				break;
			}
		}

		// PIXEL 로드
		if(0 < BitmapInfoHeader.Width && 0 < BitmapInfoHeader.Height)
		{
			const int RowLength = (((BitmapInfoHeader.Width * BitmapInfoHeader.BitCount + 7) / 8 + 3) & ~3);
			byte* ImageRow = (byte*) Resource + 2 + BitmapFileHeader.OffBits;
			const int AreaWidth = BxDrawForm::CalcTextureSize(BitmapInfoHeader.Width);
			const int AreaHeight = BxDrawForm::CalcTextureSize(BitmapInfoHeader.Height);
			Create(BitmapInfoHeader.Width, BitmapInfoHeader.Height, 0, 0);
			for(int y = Area.h - 1; 0 <= y; --y)
			{
				byte* ImageRowNext = ImageRow + RowLength;
				for(int x = 0, xbit = 0; x < Area.w; ++x)
				{
					const uint Color = BxUtilGlobal::LoadBits(ImageRow, xbit, BitmapInfoHeader.BitCount);
					switch(BitmapInfoHeader.BitCount)
					{
					case 16:
						if(Color == 0xF81F)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else Bits[x + y * AreaWidth] = BxDrawGlobal::Color565ToNativeSrc((color_565) Color);
						break;
					case 24:
						if(Color == 0xFF00FF)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else Bits[x + y * AreaWidth] = BxDrawGlobal::ColorX888ToNativeSrc((color_x888) Color);
						break;
					case 32:
						if((Color & 0xFF000000) == 0x00000000)
						{
							Flag = (blendflag)(Flag | blendflag_sprite);
							Bits[x + y * AreaWidth] = 0;
						}
						else
						{
							if((Color & 0xFF000000) != 0xFF000000) Flag = (blendflag)(Flag | blendflag_alpha);
							Bits[x + y * AreaWidth] = BxDrawGlobal::ColorA888ToNativeSrc((color_a888) Color);
						}
						break;
					default:
						{
							const color_bgra Palette = BitmapPalette[Color];
							if(Palette.r == 0xFF && Palette.g == 0x00 && Palette.b == 0xFF)
							{
								Flag = (blendflag)(Flag | blendflag_sprite);
								Bits[x + y * AreaWidth] = 0;
							}
							else Bits[x + y * AreaWidth] = BxDrawGlobal::ColorX888ToNativeSrc(BxDrawGlobal::RGB32(Palette.r, Palette.g, Palette.b));
						}
						break;
					}
				}
				ImageRow = ImageRowNext;
			}
			return true;
		}
		return false;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadB16(const byte* Resource)
	{
		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "B16h"))
			return false;
		BxUtilGlobal::LoadUint32(ResourcePtr); // skip HASH
		Area.w = BxUtilGlobal::LoadUint16(ResourcePtr);
		Area.h = BxUtilGlobal::LoadUint16(ResourcePtr);
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = BxUtilGlobal::LoadInt16(ResourcePtr);
		Area.hy = BxUtilGlobal::LoadInt16(ResourcePtr);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Area.h);
		Create(Area.w, Area.h, Area.hx, Area.hy);
		color_m555_a444* ImagePtr = (color_m555_a444*) ResourcePtr;
		for(int i = 0, iend = Area.w * Area.h; i < iend && Flag != (blendflag_alpha | blendflag_sprite); ++i)
		{
			if(!ImagePtr[i]) Flag = (blendflag)(Flag | blendflag_sprite);
			else if(!(ImagePtr[i] & 0x8000)) Flag = (blendflag)(Flag | blendflag_alpha);
		}
		for(int y = 0; y < Area.h; ++y)
		for(int x = 0; x < Area.w; ++x)
			Bits[x + AreaWidth * y] = BxDrawGlobal::ColorM555A444ToNativeSrc(ImagePtr[x + Area.w * y]);
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadB32(const byte* Resource)
	{
		byte* ResourcePtr = (byte*) Resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "B32h"))
			return false;
		BxUtilGlobal::LoadUint32(ResourcePtr); // skip HASH
		Area.w = BxUtilGlobal::LoadUint16(ResourcePtr);
		Area.h = BxUtilGlobal::LoadUint16(ResourcePtr);
		ExpressAreaW.Reset(BxInteger(0) + (int) Area.w);
		ExpressAreaH.Reset(BxInteger(0) + (int) Area.h);
		Area.hx = BxUtilGlobal::LoadInt16(ResourcePtr);
		Area.hy = BxUtilGlobal::LoadInt16(ResourcePtr);
		const int AreaWidth = BxDrawForm::CalcTextureSize(Area.w);
		const int AreaHeight = BxDrawForm::CalcTextureSize(Area.h);
		Create(Area.w, Area.h, Area.hx, Area.hy);
		color_a888* ImagePtr = (color_a888*) ResourcePtr;
		for(int i = 0, iend = Area.w * Area.h; i < iend && Flag != (blendflag_alpha | blendflag_sprite); ++i)
		{
			if(!ImagePtr[i]) Flag = (blendflag)(Flag | blendflag_sprite);
			else if((ImagePtr[i] & 0xFF000000) != 0xFF000000) Flag = (blendflag)(Flag | blendflag_alpha);
		}
		for(int y = 0; y < Area.h; ++y)
		for(int x = 0; x < Area.w; ++x)
			Bits[x + AreaWidth * y] = BxDrawGlobal::ColorA888ToNativeSrc(ImagePtr[x + Area.w * y]);
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadPNG(const byte* Resource)
	{
		PngToBmp PngDecoder;
		const byte* Bmp = PngDecoder.GetResource(Resource, 1 < ScaleNumber());
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::ReleaseBMP(ScaledBmp);
		return Result;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadJPG(const byte* Resource)
	{
		const byte* Bmp = BxCore::AddOn::JPGToBMP(Resource);
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::ReleaseBMP(ScaledBmp);
		BxCore::AddOn::ReleaseBMP(Bmp);
		return Result;
	}
	/// @endcond

	/// @cond SECTION_NAME
	bool _LoadGIF(const byte* Resource, const int Length)
	{
		int NumPage = 1;
		const byte* Bmp = BxCore::AddOn::GIFToBMP(Resource, Length, NumPage);
		const byte* ScaledBmp = BxCore::AddOn::HQXToBMP(Bmp, ScaleNumber());
		bool Result = _LoadBMP((ScaledBmp)? ScaledBmp : Bmp);
		BxCore::AddOn::ReleaseBMP(ScaledBmp);
		BxCore::AddOn::ReleaseBMP(Bmp);
		Page = NumPage;
		return Result;
	}
	/// @endcond

public:
	/// @cond SECTION_NAME
	class PngToBmp
	{
	public:
		PngToBmp(bool autodelete = true)
		{
			AutoDelete = autodelete;
			LZWindow = null;
			LZWindowSize = 0;
			LZWindowPos = 0;
			CopyPos = 0;
			CopyCount = 0;
			PNG = null;
			IsFinalBlock = false;
			BlockMode = 0xff;
			LiteralLength = 0;
			BMPData = null;
			BMPOffset = 0;
			BMPSize = 0;
		}
		virtual ~PngToBmp()
		{
			if(AutoDelete)
				DeleteResource();
		}

		// PNG메모리를 BMP메모리로 변환함
		byte* GetResource(const byte* pngsource, bool ismust32 = false)
		{
			DeleteResource();
			PNG = (byte*) pngsource;
			PNGByteOffset = 0;
			PNGBitOffset = 0;
			NextByteOffset = 8;
			byte* BMPPalette = null;
			byte* BMPPaletteAlpha = null;
			uint ChunkName = 0;

			// IHDR 청크
			while(ChunkName != 0x49484452)
			{
				PNGByteOffset = NextByteOffset;
				NextByteOffset += GetUInt32() + 12;
				ChunkName = GetUInt32();
			}

			uint ImageWidth = (uint) GetUInt32();
			uint ImageHeight = (uint) GetUInt32();
			uint ImageDepth = (uint) GetByte();
			uint ImageColorType = (uint) GetByte();
			GetByte(); GetByte(); // Compression Method, Filter Method 생략

			// MGSPng클래스는 인터레이스를 지원하지 않는다
			if(0 < GetByte())
			{
				BxAssert("BxImage<인터레이스가 들어간 PNG는 지원하지 않습니다>", false);
				return null;
			}

			// PLTE 청크
			uint PaletteSize = 0;
			while(ChunkName != 0x504c5445)
			{
				PNGByteOffset = NextByteOffset;
				NextByteOffset += (PaletteSize = GetUInt32()) + 12;
				ChunkName = GetUInt32();
				if(ChunkName == 0x49454e44)
				{
					NextByteOffset = 8;
					break;
				}
			}
			if(0 < PaletteSize)
			{
				BMPPalette = BxNew_Array(byte, PaletteSize);
				for(uint p = 0; p < PaletteSize; p++)
					BMPPalette[p] = GetByte();
			}

			// tRNS 청크
			uint TransparencySize = 0;
			while(ChunkName != 0x74524e53)
			{
				PNGByteOffset = NextByteOffset;
				NextByteOffset += (TransparencySize = GetUInt32()) + 12;
				ChunkName = GetUInt32();
				if(ChunkName == 0x49454e44)
				{
					NextByteOffset = 8;
					break;
				}
			}
			if(0 < TransparencySize)
			{
				BMPPaletteAlpha = BxNew_Array(byte, TransparencySize);
				for(uint a = 0; a < TransparencySize; a++)
					BMPPaletteAlpha[a] = GetByte();
			}

			// 비트맵구성
			ushort BytePerPixel = (ImageColorType == 4 || ImageColorType == 6 || (ImageColorType == 3 && 0 < TransparencySize) || ismust32)? 4 : 3;
			uint BMPDataWidth = (ImageWidth * BytePerPixel + 3) & ~3;
			BMPSize = 56 + ImageHeight * BMPDataWidth;
			BMPData = BxNew_Array(byte, BMPSize);
			BxCore::Util::MemSet(BMPData, 0xff, BMPSize);
			SetBMPInfo(BMPData, ImageWidth, ImageHeight, BytePerPixel);

			// IDAT 청크
			while(ChunkName != 0x49444154)
			{
				PNGByteOffset = NextByteOffset;
				NextByteOffset += GetUInt32() + 12;
				ChunkName = GetUInt32();
			}

			// LZWindow윈도우 만들기
			LZWindowPos = CopyPos = CopyCount = 0;
			LZWindowSize = 1 << ((GetByte() >> 4) + 8); // Compression Method 생략
			LZWindow = BxNew_Array(byte, LZWindowSize);
			GetByte(); // Compression Level, Preset Dictionary 생략

			// 디코딩된 픽셀정보 임시저장소
			uint RowBufferWidth = 0, FilterOffset = 0;
			switch(ImageColorType)
			{
			case 0: // 흑백
			case 3: // 팔레트
				RowBufferWidth = ((ImageWidth * ImageDepth) + 7) / 8;
				FilterOffset = 1;
				break;
			case 4: // 투명흑백
				RowBufferWidth = 2 * ImageWidth * ImageDepth / 8;
				FilterOffset = 2 * ImageDepth / 8;
				break;
			case 2: // RGB
				RowBufferWidth = 3 * ImageWidth * ImageDepth / 8;
				FilterOffset = 3 * ImageDepth / 8;
				break;
			case 6: // 투명RGB
				RowBufferWidth = 4 * ImageWidth * ImageDepth / 8;
				FilterOffset = 4 * ImageDepth / 8;
				break;
			}
			byte* RowBuffers[2];
			RowBuffers[0] = BxNew_Array(byte, RowBufferWidth);
			RowBuffers[1] = BxNew_Array(byte, RowBufferWidth);
			BxCore::Util::MemSet(RowBuffers[0], 0x00, RowBufferWidth);
			BxCore::Util::MemSet(RowBuffers[1], 0x00, RowBufferWidth);

			// 첫 블럭헤더 디코딩
			DecodeBlockHeader();

			// 행단위 디코딩
			int RowBufferFocus = 0;
			for(uint y = 0; y < ImageHeight; ++y)
			{
				byte Filter = DecodeByte();
				for(uint x = 0; x < RowBufferWidth; ++x)
					RowBuffers[RowBufferFocus][x] = DecodeByte();

				// 필터분석
				uint Col;
				switch(Filter)
				{
				case 1: // Sub필터
					for(Col = FilterOffset; Col < RowBufferWidth; ++Col)
						RowBuffers[RowBufferFocus][Col] += RowBuffers[RowBufferFocus][Col - FilterOffset];
					break;
				case 2: // Up필터
					for(Col = 0; Col < RowBufferWidth; ++Col)
						RowBuffers[RowBufferFocus][Col] += RowBuffers[1 - RowBufferFocus][Col];
					break;
				case 3: // Average필터
					for(Col = 0; Col < RowBufferWidth; ++Col)
						RowBuffers[RowBufferFocus][Col] += (((Col < FilterOffset)? 0 : RowBuffers[RowBufferFocus][Col - FilterOffset])
							+ RowBuffers[1 - RowBufferFocus][Col]) / 2;
					break;
				case 4: // Paeth필터
					for(Col = 0; Col < RowBufferWidth; ++Col)
					{
						// 죄측, 윗열, 윗좌측의 값들의 평균값에 가까운 필터를 사용
						// 전체압축율에 손실을 입힐 우려가 있는 필터를 배제하기 위한 방편
						const byte Left = (Col < FilterOffset)? 0 : RowBuffers[RowBufferFocus][Col - FilterOffset];
						const byte Above = RowBuffers[1 - RowBufferFocus][Col];
						const byte Aboveleft = (Col < FilterOffset)? 0 : RowBuffers[1 - RowBufferFocus][Col - FilterOffset];
						const int PaethPredictor = Left + Above - Aboveleft;
						const int PaethA = (Left < PaethPredictor)? PaethPredictor - Left : Left - PaethPredictor;
						const int PaethB = (Above < PaethPredictor)? PaethPredictor - Above : Above - PaethPredictor;
						const int PaethC = (Aboveleft < PaethPredictor)? PaethPredictor - Aboveleft : Aboveleft - PaethPredictor;
						RowBuffers[RowBufferFocus][Col] += (PaethA <= PaethB && PaethA <= PaethC)? Left
							: ((PaethB <= PaethC)? Above : Aboveleft);
					}
					break;
				}

				// 이미지쓰기
				uint ImageOffset = 56 + (ImageHeight - y - 1) * BMPDataWidth, AlphaOffset, PaletteOffset;
				uint TransOffset = (ImageHeight - y - 1) * ImageWidth;
				byte ColorValue;
				switch(ImageColorType)
				{
				case 0: // 흑백
					{
						for(uint x = 0; x < ImageWidth; ++x, ImageOffset += BytePerPixel)
						{
							ColorValue = (byte) (GetBits(RowBuffers[RowBufferFocus], x * ImageDepth, ImageDepth) * 255 / ((1 << ImageDepth) - 1));
							BMPData[ImageOffset] = ColorValue;
							BMPData[ImageOffset + 1] = ColorValue;
							BMPData[ImageOffset + 2] = ColorValue;
						}
					}
					break;
				case 3: // 팔레트
					{
						for(uint x = 0; x < ImageWidth; ++x, ImageOffset += BytePerPixel)
						{
							AlphaOffset = GetBits(RowBuffers[RowBufferFocus], x * ImageDepth, ImageDepth);
							PaletteOffset = AlphaOffset * 3;
							BMPData[ImageOffset] = BMPPalette[PaletteOffset + 2];
							BMPData[ImageOffset + 1] = BMPPalette[PaletteOffset + 1];
							BMPData[ImageOffset + 2] = BMPPalette[PaletteOffset];
							if(BytePerPixel == 4)
							{
								if(BMPPaletteAlpha)
									BMPData[ImageOffset + 3] = BMPPaletteAlpha[AlphaOffset];
								else BMPData[ImageOffset + 3] = 0xFF;
							}
						}
					}
					break;
				case 4: // 투명흑백
					{
						for(uint x = 0; x < ImageWidth; ++x, ImageOffset += BytePerPixel)
						{
							BMPData[ImageOffset] = RowBuffers[RowBufferFocus][x * 2];
							BMPData[ImageOffset + 1] = RowBuffers[RowBufferFocus][x * 2];
							BMPData[ImageOffset + 2] = RowBuffers[RowBufferFocus][x * 2];
							if(BytePerPixel == 4)
								BMPData[ImageOffset + 3] = RowBuffers[RowBufferFocus][x * 2 + 1];
						}
					}
					break;
				case 2: // RGB
					{
						for(uint x = 0; x < ImageWidth; ++x, ImageOffset += BytePerPixel)
						{
							BMPData[ImageOffset] = RowBuffers[RowBufferFocus][x * 3 + 2];
							BMPData[ImageOffset + 1] = RowBuffers[RowBufferFocus][x * 3 + 1];
							BMPData[ImageOffset + 2] = RowBuffers[RowBufferFocus][x * 3];
						}
					}
					break;
				case 6: // 투명RGB
					{
						for(uint x = 0; x < ImageWidth; ++x, ImageOffset += BytePerPixel)
						{
							BMPData[ImageOffset] = RowBuffers[RowBufferFocus][x * 4 + 2];
							BMPData[ImageOffset + 1] = RowBuffers[RowBufferFocus][x * 4 + 1];
							BMPData[ImageOffset + 2] = RowBuffers[RowBufferFocus][x * 4];
							if(BytePerPixel == 4)
								BMPData[ImageOffset + 3] = RowBuffers[RowBufferFocus][x * 4 + 3];
						}
					}
					break;
				}

				RowBufferFocus = 1 - RowBufferFocus;
			}

			BxDelete_Array(LZWindow);
			BxDelete_Array(BMPPalette);
			BxDelete_Array(BMPPaletteAlpha);
			BxDelete_Array(RowBuffers[0]);
			BxDelete_Array(RowBuffers[1]);
			return BMPData;
		}

		// 비트맵의 사이즈
		inline uint GetResourceSize()
		{
			return BMPSize;
		}

		// 비트맵리소스를 메모리상에서 해제
		inline void DeleteResource()
		{
			BxDelete_Array(BMPData);
			BMPData = null;
		}

	protected:
		bool AutoDelete;
		byte* LZWindow; // LZ77윈도우 포인터
		int LZWindowSize;//  LZ77윈도우 사이즈
		uint LZWindowPos;
		uint CopyPos;
		uint CopyCount;

		byte* PNG; // PNG파일메모리 포인터
		int PNGByteOffset; // PNG파일메모리 바이트단위 오프셋
		int PNGBitOffset; // PNG파일메모리 비트단위 오프셋
		int NextByteOffset; // PNG파일메모리 바이트단위 오프셋 넥스트
		bool IsFinalBlock; // IDAT의 블럭헤더 디코딩중 현재 블럭이 마지막인지를 나타냄
		char BlockMode; // IDAT의 블럭헤더 디코딩중 현재 블럭이 어떠한 블럭인지를 나타냄
		int LiteralLength; // 비압축블럭의 사이즈
		byte* BMPData; // 생산된 비트맵리소스
		uint BMPOffset; // 비트맵의 바이트단위 오프셋
		uint BMPSize; // 디코딩된 비트맵의 사이즈

		int MaxCode[3][15]; // Literal테이블 : 0, Distance테이블 : 1, Lengths테이블 : 2
		int MinCode[3][15];
		ushort ValueArray[3][15];
		ushort HuffValues[3][288];

	protected:
		// 확장IDAT 체크
		inline void CheckExtendIDAT()
		{
			if(PNGByteOffset == NextByteOffset - 4)
			{
				PNGBitOffset = 0;
				GetUInt32(false); // CRC코드 생략
				NextByteOffset += GetUInt32(false) + 12;
				GetUInt32(false); // 청크명 생략
			}
		}

		// 1비트를 읽음
		inline uint GetBit(bool DoCheck = true)
		{
			if(DoCheck) CheckExtendIDAT();
			uint RValue = (PNG[PNGByteOffset] >> PNGBitOffset) & 1;
			PNGByteOffset += (PNGBitOffset + 1) >> 3;
			PNGBitOffset = (PNGBitOffset + 1) & 7;
			return RValue;
		}

		// len비트를 읽음, len허용범위 : 0-32
		uint GetBits(const int len, bool DoCheck = true)
		{
			uint RValue = 0;
			for(int i = 0; i < len; ++i)
				RValue |= GetBit(DoCheck) << i;
			return RValue;
		}

		// buf의 bitoff부터 len비트를 읽음
		global_func uint GetBits(const byte* buf, int bitoff, const int len)
		{
			if(len == 0) return 0;
			uint RValue = 0;
			buf += bitoff >> 3;
			bitoff = (bitoff + len) & 7;
			if(len == 1)
				RValue = (*buf >> bitoff) & 1;
			else
			{
				for(int s = ((bitoff + len + 7) >> 3) - 1; 0 <= s; --s)
					RValue = (RValue << 8) | buf[s];
				RValue = (RValue >> bitoff) & ~(0xffffffff << len);
			}
			return RValue;
		}

		// 1바이트를 읽음, GetBits()로 읽다 남은것이 있다면 해당 바이트 건너뜀
		inline byte GetByte(bool DoCheck = true)
		{
			// 비트단위로 읽던것은 넘어감
			if(0 < PNGBitOffset)
			{
				PNGBitOffset = 0;
				++PNGByteOffset;
			}
			if(DoCheck) CheckExtendIDAT();
			return PNG[PNGByteOffset++];
		}

		// 4바이트를 읽음
		inline uint GetUInt32(bool DoCheck = true)
		{
			uint RValue = (GetByte(DoCheck) & 0xff) << 24;
			RValue |= (GetByte(DoCheck) & 0xff) << 16;
			RValue |= (GetByte(DoCheck) & 0xff) << 8;
			RValue |= GetByte(DoCheck) & 0xff;
			return RValue;
		}

		// 허프만테이블에서 값을 찾음
		ushort Decode(const uint tablenum)
		{
			int CodeLength;
			int Code = GetBit();
			// 먼저 1바이트를 읽고 MaxCode와 비교를 하기 때문에 Code가 더 클 수 있음
			for(CodeLength = 0; MaxCode[tablenum][CodeLength] < Code && CodeLength < 15; ++CodeLength)
				Code = (Code << 1) | GetBit(); // 1비트씩 거꾸로 읽음
			return HuffValues[tablenum][ValueArray[tablenum][CodeLength] + (Code - MinCode[tablenum][CodeLength])];
		}

		// IDAT를 허프만방식으로 읽음
		inline byte DecodeByte()
		{
			if(BlockMode == 0) return DecodeLiteralByte();
			return DecodeCompressedByte();
		}

		inline byte DecodeLiteralByte()
		{
			if(LiteralLength == 0)
			{
				if(!IsFinalBlock)
					DecodeBlockHeader();
				return DecodeByte();
			}
			--LiteralLength;
			return GetByte();
		}

		byte DecodeCompressedByte()
		{
			global_data const int LengthBase[29] = {
				 3,   4,   5,   6,   7,   8,   9,  10,  11,  13,
				15,  17,  19,  23,  27,  31,  35,  43,  51,  59,
				67,  83,  99, 115, 131, 163, 195, 227, 258};
			global_data const int LengthExtra[29] = {
				0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
				1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
				4, 4, 4, 4, 5, 5, 5, 5, 0};
			global_data const int DistanceBase[30] = {
				   1,     2,     3,     4,     5,     7,     9,    13,    17,    25,
				  33,    49,    65,    97,   129,   193,   257,   385,   513,   769,
				1025,  1537,  2049,  3073,  4097,  6145,  8193, 12289, 16385, 24577};
			global_data const int DistanceExtra[30] = {
				0,  0,  0,  0,  1,  1,  2,  2,  3,  3,
				4,  4,  5,  5,  6,  6,  7,  7,  8,  8,
				9,  9, 10, 10, 11, 11, 12, 12, 13, 13};

			// LZ윈도우의 복사거리동안 디코딩은 쉼
			if(0 < CopyCount)
			{
				byte Value8 = LZWindow[CopyPos];
				LZWindow[LZWindowPos] = Value8;
				--CopyCount;
				CopyPos = (CopyPos + 1) & 0x7fff;
				LZWindowPos = (LZWindowPos + 1) & 0x7fff;
				return Value8;
			}

			// 허프만트리에 입각하여 하나의 값을 읽어냄
			uint Value32 = Decode(0);

			if(Value32 == 256)
			{
				if(!IsFinalBlock)
					DecodeBlockHeader();
				return DecodeByte();
			}
			else if(Value32 < 256) // LZ77상의 기호토큰인 경우
			{
				LZWindow[LZWindowPos] = (byte) Value32;
				LZWindowPos = (LZWindowPos + 1) & 0x7fff;
				return (byte) Value32;
			}

			// LZ77상의 구를 이루는 토큰인 경우
			uint Length = LengthBase[Value32 - 257] + GetBits(LengthExtra[Value32 - 257]);
			Value32 = Decode(1);
			uint Distance = DistanceBase[Value32] + GetBits(DistanceExtra[Value32]);

			CopyPos = (32768 + LZWindowPos - Distance) & 0x7fff;
			CopyCount = Length;

			return DecodeCompressedByte();
		}

		// 블럭헤더의 Length를 읽음
		void DecodeLengths(uint tablenum, uint* lengths, uint lengthcount)
		{
			// 동적허프만테이블인 경우 이미지정보값과 LZ윈도우거리값의 배열이
			// 따로 기록되어 있으니 디코딩해야 하며 RLE압축방식을 쓴다
			uint Index = 0;
			while(Index < lengthcount)
			{
				uint Command = Decode(tablenum);
				if(Command < 16) lengths[Index++] = Command;
				else
				{
					uint Count = 0;
					switch(Command)
					{
					case 16: Count = GetBits(2) + 3; break; // 카운트만큼 이전과 같음
					case 17: Count = GetBits(3) + 3; break; // 카운트만큼 0값
					case 18: Count = GetBits(7) + 11; break; // 카운트만큼 0값
					}
					for(uint i = 0; i < Count; ++i)
					{
						if(Command == 16)
						{
							const uint Index2 = Index - 1;
							lengths[Index++] = lengths[Index2];
						}
						else lengths[Index++] = 0;
					}
				}
			}
		}

		// IDAT의 블럭헤더를 디코딩함
		void DecodeBlockHeader()
		{
			global_data const byte PngLengthOrder[19] =
				{16, 17, 18,  0,  8,  7,  9,  6, 10,  5, 11,  4, 12,  3, 13,  2, 14,  1, 15};
			// 압축블럭은 이미지 행단위가 아니라 행속에서의 효율단위이다
			IsFinalBlock = (GetBit() != 0);

			switch(GetBits(2))
			{
			case 0: // 비압축블럭
				{
					BlockMode = 0;
					LiteralLength = GetByte();
					LiteralLength |= ((int) GetByte()) << 8;
					GetByte(); GetByte(); // Not Length 생략
				}
				break;
			case 1: // 고정허프만테이블
				if(BlockMode != 1)
				{
					BlockMode = 1;
					BuildFixedTable();
				}
				break;
			case 2: // 동적허프만테이블
				{
					BlockMode = 2;
					const uint LiteralsNum = GetBits(5) + 257;
					const uint DistancesNum = GetBits(5) + 1;
					const uint LengthsNum = GetBits(4) + 4;
					uint Lengths[19];
					BxCore::Util::MemSet(Lengths, 0, sizeof(uint) * 19);
					for(uint i = 0; i < LengthsNum; ++i)
						Lengths[PngLengthOrder[i]] = GetBits(3);
					BuildTable(2, 19, Lengths);

					uint DynamicLiterals[288], DynamicDistances[32];
					DecodeLengths(2, DynamicLiterals, LiteralsNum);
					DecodeLengths(2, DynamicDistances, DistancesNum);

					BuildTable(0, LiteralsNum, DynamicLiterals);
					BuildTable(1, DistancesNum, DynamicDistances);
				}
				break;
			}
		}

		// 고정허프만테이블 구성
		void BuildFixedTable()
		{
			for(int j = 0; j < 2; j++)
			for(int i = 0; i < 15; ++i)
			{
				MaxCode[j][i] = -1;
				MinCode[j][i] = 0x0fffffff;
				ValueArray[j][i] = 0;
			}

			MaxCode[0][6] = 23, MaxCode[0][7] = 199, MaxCode[0][8] = 511;
			MinCode[0][6] = 0, MinCode[0][7] = 48, MinCode[0][8] = 400;
			ValueArray[0][6] = 0, ValueArray[0][7] = 24, ValueArray[0][8] = 176;
			uint valuesfocus = 0;
			for(ushort f = 256; f <= 279; ++f) HuffValues[0][valuesfocus++] = f;
			for(ushort f = 0; f <= 143; ++f) HuffValues[0][valuesfocus++] = f;
			for(ushort f = 280; f <= 287; ++f) HuffValues[0][valuesfocus++] = f;
			for(ushort f = 144; f <= 255; ++f) HuffValues[0][valuesfocus++] = f;

			MaxCode[1][4] = 31, MinCode[1][4] = 0, ValueArray[1][4] = 0;
			valuesfocus = 0;
			for(ushort f = 0; f <= 31; ++f) HuffValues[1][valuesfocus++] = f;
		}

		// 동적허프만테이블 구성
		void BuildTable(uint tablenum, uint vcount, const uint* codelengths)
		{
			uint LengthArray[288];
			// 배열번호는 그 자체로 최종적으로 디코딩시 얻는 이미지정보등의 값이며
			// Length는 그 값을 표현하기 위한 비트수이다
			for(uint i = 0; i < vcount; ++i)
			{
				HuffValues[tablenum][i] = (ushort) i;
				LengthArray[i] = codelengths[i];
			}

			// 배열의 뒤로 갈수록 비트수가 많아지게 정렬한다
			for(uint i = 0; i < vcount - 1; ++i)
			{
				uint MinIndex = i;
				for(uint j = i + 1; j < vcount; ++j)
					if(LengthArray[j] < LengthArray[MinIndex]
						|| (LengthArray[j] == LengthArray[MinIndex] && HuffValues[tablenum][j] < HuffValues[tablenum][MinIndex]))
							MinIndex = j;
				if(MinIndex != i)
				{
					ushort Temp1 = HuffValues[tablenum][MinIndex];
					HuffValues[tablenum][MinIndex] = HuffValues[tablenum][i];
					HuffValues[tablenum][i] = Temp1;
					uint Temp2 = LengthArray[MinIndex];
					LengthArray[MinIndex] = LengthArray[i];
					LengthArray[i] = Temp2;
				}
			}

			ushort HuffCodes[288];
			// 해당비트수로 표현할 수 있는 최소수부터 1씩 올라가며 차례차례 기록한다
			uint LastLen = 0, Code = 0;
			for(uint i = 0; i < vcount; ++i)
			{
				while(LastLen < LengthArray[i])
				{
					++LastLen;
					Code <<= 1;
				}
				if(LastLen != 0)
					HuffCodes[i] = (ushort) Code++;
			}

			// 비트수는 보통 5, 7, 8, 9만을 쓰기 때문에 초기화함
			for(uint i = 0; i < 15; ++i)
			{
				MaxCode[tablenum][i] = -1;
				MinCode[tablenum][i] = 0x0fffffff;
				ValueArray[tablenum][i] = 0;
			}

			// 비트수에 따른 최소값과 최대값, 이미지정보기본값을 저장한다
			uint LastLength = 0;
			for(uint i = 0; i < vcount; ++i)
			{
				if(LastLength < LengthArray[i])
				{
					LastLength = LengthArray[i];
					ValueArray[tablenum][LastLength - 1] = (ushort) i;
					MinCode[tablenum][LastLength - 1] = HuffCodes[i];
				}
				if(0 < LastLength)
					MaxCode[tablenum][LastLength - 1] = HuffCodes[i];
			}
		}

		// BMP 헤더정보 채우기
		inline void SetBMPInfo(byte* bmp, uint w, uint h, ushort byteperpixel)
		{
			BMPOffset = 0;
			// 비트맵정보구성
			SetBMPUInt32(bmp, *((uint*) "BxBM"));
			SetBMPUInt32(bmp, 54 + ((w * byteperpixel + 3) & ~3) * h);
			SetBMPUInt16(bmp, 0);
			SetBMPUInt16(bmp, 0);
			SetBMPUInt32(bmp, 54);
			SetBMPUInt32(bmp, 40);
			SetBMPUInt32(bmp, w);
			SetBMPUInt32(bmp, h);
			SetBMPUInt16(bmp, 1);
			SetBMPUInt16(bmp, 8 * byteperpixel);
			SetBMPUInt32(bmp, 0);
			SetBMPUInt32(bmp, ((w * byteperpixel + 3) & ~3) * h);
			SetBMPUInt32(bmp, 3780);
			SetBMPUInt32(bmp, 3780);
			SetBMPUInt32(bmp, 0);
			SetBMPUInt32(bmp, 0);
		}

		inline void SetBMPUInt16(byte* bmp, ushort value)
		{
			bmp[BMPOffset] = (byte) (value & 0xff);
			bmp[BMPOffset + 1] = (byte) ((value >> 8) & 0xff);
			BMPOffset += 2;
		}

		inline void SetBMPUInt32(byte* bmp, uint value)
		{
			bmp[BMPOffset] = (byte) (value & 0xff);
			bmp[BMPOffset + 1] = (byte) ((value >> 8) & 0xff);
			bmp[BMPOffset + 2] = (byte) ((value >> 16) & 0xff);
			bmp[BMPOffset + 3] = (byte) ((value >> 24) & 0xff);
			BMPOffset += 4;
		}
	};
	/// @endcond
};
