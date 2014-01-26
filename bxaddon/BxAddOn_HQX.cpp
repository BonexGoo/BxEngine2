// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxMemory.hpp>
#include <BxSingleton.hpp>

namespace BxCore
{
	namespace AddOn
	{
		void* _inout_ ForExtend_HQXToBMP();
	}
}
local_func const byte* HQXToBMP(const byte* bmp, int scale);
local_func bool BindingCore() {BxCore::AddOn::ForExtend_HQXToBMP() = (void*) HQXToBMP; return true;}
local_data bool Unknown = BindingCore();
// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

#include <stdlib.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void hqxInit(void);
void hq2x_32( uint32_t * src, uint32_t * dest, int width, int height );
void hq3x_32( uint32_t * src, uint32_t * dest, int width, int height );
void hq4x_32( uint32_t * src, uint32_t * dest, int width, int height );

void hq2x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );
void hq3x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );
void hq4x_32_rb( uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );

#define MASK_2     0x0000FF00
#define MASK_13    0x00FF00FF
#define MASK_RGB   0x00FFFFFF
#define MASK_ALPHA 0xFF000000

#define Ymask 0x00FF0000
#define Umask 0x0000FF00
#define Vmask 0x000000FF
#define trY   0x00300000
#define trU   0x00000700
#define trV   0x00000006

// YUV계산 정밀도
#define DETAIL_MASK_SIZE 6
#define DETAIL_R_SHIFT ((8 - DETAIL_MASK_SIZE) * 3)
#define DETAIL_G_SHIFT ((8 - DETAIL_MASK_SIZE) * 2)
#define DETAIL_B_SHIFT ((8 - DETAIL_MASK_SIZE) * 1)
#if (DETAIL_MASK_SIZE == 6)
	#define DETAIL_R_MASK 0x0003F000
	#define DETAIL_G_MASK 0x00000FC0
	#define DETAIL_B_MASK 0x0000003F
#elif (DETAIL_MASK_SIZE == 7)
	#define DETAIL_R_MASK 0x001FC000
	#define DETAIL_G_MASK 0x00003F80
	#define DETAIL_B_MASK 0x0000007F
#endif

static uint32_t* RGBtoYUV = nullptr;
static uint32_t YUV1 = 0;
static uint32_t YUV2 = 0;

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
const byte* HQXToBMP(const byte* bmp, int scale)
{
	// 비트맵구성
	if(scale < 2 || 4 < scale) return nullptr;
	typedef struct {uint Size; ushort Reserved1; ushort Reserved2; uint OffBits;} BITMAPFILEHEADER;
	typedef struct {uint Size; int Width; int Height; ushort Planes; ushort BitCount; uint Compression;
		uint SizeImage; int XPelsPerMeter; int YPelsPerMeter; uint ClrUsed; uint ClrImportant;} BITMAPINFOHEADER;

	// Source정보
	if(bmp[0] != 'B' || bmp[1] != 'x' || bmp[2] != 'B' || bmp[3] != 'M') return nullptr;
	BITMAPINFOHEADER* SrcBmpInfoHeader = (BITMAPINFOHEADER*)(bmp + 4 + sizeof(BITMAPFILEHEADER));
	if(SrcBmpInfoHeader->BitCount != 32) return nullptr;
	const int SrcWidth = SrcBmpInfoHeader->Width;
	const int SrcHeight = SrcBmpInfoHeader->Height;

	// 비트맵구성
	const int DstWidth = SrcWidth * scale;
	const int DstHeight = SrcHeight * scale;
	const int BmpBytesPerPixel = 4;
	const int DstRow = (BmpBytesPerPixel * DstWidth + 3) & ~3;
	byte* Bmp = (byte*) BxAlloc(4 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + DstRow * DstHeight);
	Bmp[0] = 'B';
	Bmp[1] = 'x';
	Bmp[2] = 'B';
	Bmp[3] = 'M';
	BITMAPFILEHEADER* BmpFileHeader = (BITMAPFILEHEADER*)(Bmp + 4);
	BmpFileHeader->Size = 54 + DstRow * DstHeight;
	BmpFileHeader->Reserved1 = 0;
	BmpFileHeader->Reserved2 = 0;
	BmpFileHeader->OffBits = 54;
	BITMAPINFOHEADER* BmpInfoHeader = (BITMAPINFOHEADER*)(Bmp + 4 + sizeof(BITMAPFILEHEADER));
	BmpInfoHeader->Size = 40;
	BmpInfoHeader->Width = DstWidth;
	BmpInfoHeader->Height = DstHeight;
	BmpInfoHeader->Planes = 1;
	BmpInfoHeader->BitCount = 8 * BmpBytesPerPixel;
	BmpInfoHeader->Compression = 0;
	BmpInfoHeader->SizeImage = DstRow * DstHeight;
	BmpInfoHeader->XPelsPerMeter = 3780;
	BmpInfoHeader->YPelsPerMeter = 3780;
	BmpInfoHeader->ClrUsed = 0;
	BmpInfoHeader->ClrImportant = 0;

	hqxInit();
	uint32_t* SrcBits = (uint32_t*)(bmp + 4 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	uint32_t* DstBits = (uint32_t*)(Bmp + 4 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	switch(scale)
	{
	case 2: hq2x_32(SrcBits, DstBits, SrcWidth, SrcHeight); break;
	case 3: hq3x_32(SrcBits, DstBits, SrcWidth, SrcHeight); break;
	case 4: hq4x_32(SrcBits, DstBits, SrcWidth, SrcHeight); break;
	}
	return Bmp;
}
// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

static inline uint32_t rgb_to_yuv(uint32_t c)
{
    // Mask against MASK_RGB to discard the alpha channel
    return RGBtoYUV[((c >> DETAIL_R_SHIFT) & DETAIL_R_MASK) | ((c >> DETAIL_G_SHIFT) & DETAIL_G_MASK) | ((c >> DETAIL_B_SHIFT) & DETAIL_B_MASK)];
}

/* Test if there is difference in color */
static inline int yuv_diff(uint32_t yuv1, uint32_t yuv2) {
    return (( abs((yuv1 & Ymask) - (yuv2 & Ymask)) > trY ) ||
            ( abs((yuv1 & Umask) - (yuv2 & Umask)) > trU ) ||
            ( abs((yuv1 & Vmask) - (yuv2 & Vmask)) > trV ) );
}

static inline int Diff(uint32_t c1, uint32_t c2)
{
    return yuv_diff(rgb_to_yuv(c1), rgb_to_yuv(c2));
}

/* Interpolate functions */
static inline uint32_t Interpolate_2(uint32_t c1, int w1, uint32_t c2, int w2, int s)
{
    if (c1 == c2) {
        return c1;
    }
    return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2) >> s) & MASK_2)	+
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2) >> s) & MASK_13);
}

static inline uint32_t Interpolate_3(uint32_t c1, int w1, uint32_t c2, int w2, uint32_t c3, int w3, int s)
{
    return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2 + ((c3 & MASK_ALPHA) >> 24) * w3) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2 + (c3 & MASK_2) * w3) >> s) & MASK_2) +
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2 + (c3 & MASK_13) * w3) >> s) & MASK_13);
}

static inline void Interp1(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*3+c2) >> 2;
    *pc = Interpolate_2(c1, 3, c2, 1, 2);
}

static inline void Interp2(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+c2+c3) >> 2;
    *pc = Interpolate_3(c1, 2, c2, 1, c3, 1, 2);
}

static inline void Interp3(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*7+c2)/8;
    *pc = Interpolate_2(c1, 7, c2, 1, 3);
}

static inline void Interp4(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+(c2+c3)*7)/16;
    *pc = Interpolate_3(c1, 2, c2, 7, c3, 7, 4);
}

static inline void Interp5(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1+c2) >> 1;
    *pc = Interpolate_2(c1, 1, c2, 1, 1);
}

static inline void Interp6(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*5+c2*2+c3)/8;
    *pc = Interpolate_3(c1, 5, c2, 2, c3, 1, 3);
}

static inline void Interp7(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*6+c2+c3)/8;
    *pc = Interpolate_3(c1, 6, c2, 1, c3, 1, 3);
}

static inline void Interp8(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*pc = (c1*5+c2*3)/8;
    *pc = Interpolate_2(c1, 5, c2, 3, 3);
}

static inline void Interp9(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*2+(c2+c3)*3)/8;
    *pc = Interpolate_3(c1, 2, c2, 3, c3, 3, 3);
}

static inline void Interp10(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*pc = (c1*14+c2+c3)/16;
    *pc = Interpolate_3(c1, 14, c2, 1, c3, 1, 4);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void hqxInit(void)
{
	if(RGBtoYUV) return;
	BxSINGLETON(RGBtoYUV, (DETAIL_R_MASK | DETAIL_G_MASK | DETAIL_B_MASK) + 1);
    /* Initalize RGB to YUV lookup table */
    uint32_t c, r, g, b, y, u, v;
    for (c = 0; c < (DETAIL_R_MASK | DETAIL_G_MASK | DETAIL_B_MASK) + 1; c++) {
        r = (c & DETAIL_R_MASK) >> (16 - DETAIL_R_SHIFT);
        g = (c & DETAIL_G_MASK) >> (8 - DETAIL_G_SHIFT);
        b = (c & DETAIL_B_MASK) << DETAIL_B_SHIFT;
		r = r | (r >> DETAIL_MASK_SIZE);
		g = g | (g >> DETAIL_MASK_SIZE);
		b = b | (b >> DETAIL_MASK_SIZE);
        y = (uint32_t)(0.299*r + 0.587*g + 0.114*b);
        u = (uint32_t)(-0.169*r - 0.331*g + 0.5*b) + 128;
        v = (uint32_t)(0.5*r - 0.419*g - 0.081*b) + 128;
        RGBtoYUV[c] = (y << 16) + (u << 8) + v;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PIXEL_2X_00_0     *dp = w[5];
#define PIXEL_2X_00_10    Interp1(dp, w[5], w[1]);
#define PIXEL_2X_00_11    Interp1(dp, w[5], w[4]);
#define PIXEL_2X_00_12    Interp1(dp, w[5], w[2]);
#define PIXEL_2X_00_20    Interp2(dp, w[5], w[4], w[2]);
#define PIXEL_2X_00_21    Interp2(dp, w[5], w[1], w[2]);
#define PIXEL_2X_00_22    Interp2(dp, w[5], w[1], w[4]);
#define PIXEL_2X_00_60    Interp6(dp, w[5], w[2], w[4]);
#define PIXEL_2X_00_61    Interp6(dp, w[5], w[4], w[2]);
#define PIXEL_2X_00_70    Interp7(dp, w[5], w[4], w[2]);
#define PIXEL_2X_00_90    Interp9(dp, w[5], w[4], w[2]);
#define PIXEL_2X_00_100   Interp10(dp, w[5], w[4], w[2]);
#define PIXEL_2X_01_0     *(dp+1) = w[5];
#define PIXEL_2X_01_10    Interp1(dp+1, w[5], w[3]);
#define PIXEL_2X_01_11    Interp1(dp+1, w[5], w[2]);
#define PIXEL_2X_01_12    Interp1(dp+1, w[5], w[6]);
#define PIXEL_2X_01_20    Interp2(dp+1, w[5], w[2], w[6]);
#define PIXEL_2X_01_21    Interp2(dp+1, w[5], w[3], w[6]);
#define PIXEL_2X_01_22    Interp2(dp+1, w[5], w[3], w[2]);
#define PIXEL_2X_01_60    Interp6(dp+1, w[5], w[6], w[2]);
#define PIXEL_2X_01_61    Interp6(dp+1, w[5], w[2], w[6]);
#define PIXEL_2X_01_70    Interp7(dp+1, w[5], w[2], w[6]);
#define PIXEL_2X_01_90    Interp9(dp+1, w[5], w[2], w[6]);
#define PIXEL_2X_01_100   Interp10(dp+1, w[5], w[2], w[6]);
#define PIXEL_2X_10_0     *(dp+dpL) = w[5];
#define PIXEL_2X_10_10    Interp1(dp+dpL, w[5], w[7]);
#define PIXEL_2X_10_11    Interp1(dp+dpL, w[5], w[8]);
#define PIXEL_2X_10_12    Interp1(dp+dpL, w[5], w[4]);
#define PIXEL_2X_10_20    Interp2(dp+dpL, w[5], w[8], w[4]);
#define PIXEL_2X_10_21    Interp2(dp+dpL, w[5], w[7], w[4]);
#define PIXEL_2X_10_22    Interp2(dp+dpL, w[5], w[7], w[8]);
#define PIXEL_2X_10_60    Interp6(dp+dpL, w[5], w[4], w[8]);
#define PIXEL_2X_10_61    Interp6(dp+dpL, w[5], w[8], w[4]);
#define PIXEL_2X_10_70    Interp7(dp+dpL, w[5], w[8], w[4]);
#define PIXEL_2X_10_90    Interp9(dp+dpL, w[5], w[8], w[4]);
#define PIXEL_2X_10_100   Interp10(dp+dpL, w[5], w[8], w[4]);
#define PIXEL_2X_11_0     *(dp+dpL+1) = w[5];
#define PIXEL_2X_11_10    Interp1(dp+dpL+1, w[5], w[9]);
#define PIXEL_2X_11_11    Interp1(dp+dpL+1, w[5], w[6]);
#define PIXEL_2X_11_12    Interp1(dp+dpL+1, w[5], w[8]);
#define PIXEL_2X_11_20    Interp2(dp+dpL+1, w[5], w[6], w[8]);
#define PIXEL_2X_11_21    Interp2(dp+dpL+1, w[5], w[9], w[8]);
#define PIXEL_2X_11_22    Interp2(dp+dpL+1, w[5], w[9], w[6]);
#define PIXEL_2X_11_60    Interp6(dp+dpL+1, w[5], w[8], w[6]);
#define PIXEL_2X_11_61    Interp6(dp+dpL+1, w[5], w[6], w[8]);
#define PIXEL_2X_11_70    Interp7(dp+dpL+1, w[5], w[6], w[8]);
#define PIXEL_2X_11_90    Interp9(dp+dpL+1, w[5], w[6], w[8]);
#define PIXEL_2X_11_100   Interp10(dp+dpL+1, w[5], w[6], w[8]);

void hq2x_32_rb( uint32_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres )
{
    int  i, j, k;
    int  prevline, nextline;
    uint32_t  w[10];
    int dpL = (drb >> 2);
    int spL = (srb >> 2);
    uint8_t *sRowP = (uint8_t *) sp;
    uint8_t *dRowP = (uint8_t *) dp;
    uint32_t yuv1, yuv2;

    //   +----+----+----+
    //   |    |    |    |
    //   | w1 | w2 | w3 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w4 | w5 | w6 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w7 | w8 | w9 |
    //   +----+----+----+

    for (j=0; j<Yres; j++)
    {
        if (j>0)      prevline = -spL; else prevline = 0;
        if (j<Yres-1) nextline =  spL; else nextline = 0;

        for (i=0; i<Xres; i++)
        {
            w[2] = *(sp + prevline);
            w[5] = *sp;
            w[8] = *(sp + nextline);

            if (i>0)
            {
                w[1] = *(sp + prevline - 1);
                w[4] = *(sp - 1);
                w[7] = *(sp + nextline - 1);
            }
            else
            {
                w[1] = w[2];
                w[4] = w[5];
                w[7] = w[8];
            }

            if (i<Xres-1)
            {
                w[3] = *(sp + prevline + 1);
                w[6] = *(sp + 1);
                w[9] = *(sp + nextline + 1);
            }
            else
            {
                w[3] = w[2];
                w[6] = w[5];
                w[9] = w[8];
            }

            int pattern = 0;
            int flag = 1;

            yuv1 = rgb_to_yuv(w[5]);

            for (k=1; k<=9; k++)
            {
                if (k==5) continue;

                if ( w[k] != w[5] )
                {
                    yuv2 = rgb_to_yuv(w[k]);
                    if (yuv_diff(yuv1, yuv2))
                        pattern |= flag;
                }
                flag <<= 1;
            }

            switch (pattern)
            {
                case 0:
                case 1:
                case 4:
                case 32:
                case 128:
                case 5:
                case 132:
                case 160:
                case 33:
                case 129:
                case 36:
                case 133:
                case 164:
                case 161:
                case 37:
                case 165:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 2:
                case 34:
                case 130:
                case 162:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_21
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 16:
                case 17:
                case 48:
                case 49:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 64:
                case 65:
                case 68:
                case 69:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_21
                        PIXEL_2X_11_22
                        break;
                    }
                case 8:
                case 12:
                case 136:
                case 140:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 3:
                case 35:
                case 131:
                case 163:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_21
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 6:
                case 38:
                case 134:
                case 166:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_12
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 20:
                case 21:
                case 52:
                case 53:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_11
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 144:
                case 145:
                case 176:
                case 177:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        PIXEL_2X_10_20
                        PIXEL_2X_11_12
                        break;
                    }
                case 192:
                case 193:
                case 196:
                case 197:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_21
                        PIXEL_2X_11_11
                        break;
                    }
                case 96:
                case 97:
                case 100:
                case 101:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_12
                        PIXEL_2X_11_22
                        break;
                    }
                case 40:
                case 44:
                case 168:
                case 172:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        PIXEL_2X_10_11
                        PIXEL_2X_11_20
                        break;
                    }
                case 9:
                case 13:
                case 137:
                case 141:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_20
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 18:
                case 50:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 80:
                case 81:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 72:
                case 76:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 10:
                case 138:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 66:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_21
                        PIXEL_2X_10_21
                        PIXEL_2X_11_22
                        break;
                    }
                case 24:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 7:
                case 39:
                case 135:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 148:
                case 149:
                case 180:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_11
                        PIXEL_2X_10_20
                        PIXEL_2X_11_12
                        break;
                    }
                case 224:
                case 228:
                case 225:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 41:
                case 169:
                case 45:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_20
                        PIXEL_2X_10_11
                        PIXEL_2X_11_20
                        break;
                    }
                case 22:
                case 54:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 208:
                case 209:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 104:
                case 108:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 11:
                case 139:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 19:
                case 51:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_00_11
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_00_60
                            PIXEL_2X_01_90
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 146:
                case 178:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                            PIXEL_2X_11_12
                        }
                        else
                        {
                            PIXEL_2X_01_90
                            PIXEL_2X_11_61
                        }
                        PIXEL_2X_10_20
                        break;
                    }
                case 84:
                case 85:
                    {
                        PIXEL_2X_00_20
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_01_11
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_01_60
                            PIXEL_2X_11_90
                        }
                        PIXEL_2X_10_21
                        break;
                    }
                case 112:
                case 113:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_10_12
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_10_61
                            PIXEL_2X_11_90
                        }
                        break;
                    }
                case 200:
                case 204:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                            PIXEL_2X_11_11
                        }
                        else
                        {
                            PIXEL_2X_10_90
                            PIXEL_2X_11_60
                        }
                        break;
                    }
                case 73:
                case 77:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_00_12
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_00_61
                            PIXEL_2X_10_90
                        }
                        PIXEL_2X_01_20
                        PIXEL_2X_11_22
                        break;
                    }
                case 42:
                case 170:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                            PIXEL_2X_10_11
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_10_60
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_11_20
                        break;
                    }
                case 14:
                case 142:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                            PIXEL_2X_01_12
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_01_61
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 67:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_21
                        PIXEL_2X_10_21
                        PIXEL_2X_11_22
                        break;
                    }
                case 70:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_12
                        PIXEL_2X_10_21
                        PIXEL_2X_11_22
                        break;
                    }
                case 28:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 152:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 194:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_21
                        PIXEL_2X_10_21
                        PIXEL_2X_11_11
                        break;
                    }
                case 98:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_21
                        PIXEL_2X_10_12
                        PIXEL_2X_11_22
                        break;
                    }
                case 56:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 25:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 26:
                case 31:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 82:
                case 214:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 88:
                case 248:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 74:
                case 107:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 27:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_10
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 86:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_21
                        PIXEL_2X_11_10
                        break;
                    }
                case 216:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        PIXEL_2X_10_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 106:
                    {
                        PIXEL_2X_00_10
                        PIXEL_2X_01_21
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 30:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 210:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_10
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 120:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_10
                        break;
                    }
                case 75:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_10_10
                        PIXEL_2X_11_22
                        break;
                    }
                case 29:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        PIXEL_2X_10_22
                        PIXEL_2X_11_21
                        break;
                    }
                case 198:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_12
                        PIXEL_2X_10_21
                        PIXEL_2X_11_11
                        break;
                    }
                case 184:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_22
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 99:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_21
                        PIXEL_2X_10_12
                        PIXEL_2X_11_22
                        break;
                    }
                case 57:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 71:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_21
                        PIXEL_2X_11_22
                        break;
                    }
                case 156:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 226:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_21
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 60:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 195:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_21
                        PIXEL_2X_10_21
                        PIXEL_2X_11_11
                        break;
                    }
                case 102:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_12
                        PIXEL_2X_10_12
                        PIXEL_2X_11_22
                        break;
                    }
                case 153:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 58:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 83:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 92:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 202:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        PIXEL_2X_01_21
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 78:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 154:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 114:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 89:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 90:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 55:
                case 23:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_00_11
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_00_60
                            PIXEL_2X_01_90
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_21
                        break;
                    }
                case 182:
                case 150:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                            PIXEL_2X_11_12
                        }
                        else
                        {
                            PIXEL_2X_01_90
                            PIXEL_2X_11_61
                        }
                        PIXEL_2X_10_20
                        break;
                    }
                case 213:
                case 212:
                    {
                        PIXEL_2X_00_20
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_01_11
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_01_60
                            PIXEL_2X_11_90
                        }
                        PIXEL_2X_10_21
                        break;
                    }
                case 241:
                case 240:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_22
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_10_12
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_10_61
                            PIXEL_2X_11_90
                        }
                        break;
                    }
                case 236:
                case 232:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                            PIXEL_2X_11_11
                        }
                        else
                        {
                            PIXEL_2X_10_90
                            PIXEL_2X_11_60
                        }
                        break;
                    }
                case 109:
                case 105:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_00_12
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_00_61
                            PIXEL_2X_10_90
                        }
                        PIXEL_2X_01_20
                        PIXEL_2X_11_22
                        break;
                    }
                case 171:
                case 43:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                            PIXEL_2X_10_11
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_10_60
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_11_20
                        break;
                    }
                case 143:
                case 15:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                            PIXEL_2X_01_12
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_01_61
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_20
                        break;
                    }
                case 124:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_10
                        break;
                    }
                case 203:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        PIXEL_2X_10_10
                        PIXEL_2X_11_11
                        break;
                    }
                case 62:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 211:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_10
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 118:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_12
                        PIXEL_2X_11_10
                        break;
                    }
                case 217:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        PIXEL_2X_10_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 110:
                    {
                        PIXEL_2X_00_10
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 155:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_10
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 188:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 185:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 61:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 157:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 103:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_12
                        PIXEL_2X_11_22
                        break;
                    }
                case 227:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_21
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 230:
                    {
                        PIXEL_2X_00_22
                        PIXEL_2X_01_12
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 199:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_21
                        PIXEL_2X_11_11
                        break;
                    }
                case 220:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 158:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 234:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        PIXEL_2X_01_21
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 242:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 59:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 121:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 87:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 79:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 122:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 94:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 218:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 91:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 229:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_20
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 167:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_20
                        PIXEL_2X_11_20
                        break;
                    }
                case 173:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_20
                        PIXEL_2X_10_11
                        PIXEL_2X_11_20
                        break;
                    }
                case 181:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_11
                        PIXEL_2X_10_20
                        PIXEL_2X_11_12
                        break;
                    }
                case 186:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 115:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 93:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 206:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 205:
                case 201:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_10
                        }
                        else
                        {
                            PIXEL_2X_10_70
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 174:
                case 46:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_10
                        }
                        else
                        {
                            PIXEL_2X_00_70
                        }
                        PIXEL_2X_01_12
                        PIXEL_2X_10_11
                        PIXEL_2X_11_20
                        break;
                    }
                case 179:
                case 147:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_10
                        }
                        else
                        {
                            PIXEL_2X_01_70
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_12
                        break;
                    }
                case 117:
                case 116:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_11
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_10
                        }
                        else
                        {
                            PIXEL_2X_11_70
                        }
                        break;
                    }
                case 189:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 231:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_12
                        PIXEL_2X_10_12
                        PIXEL_2X_11_11
                        break;
                    }
                case 126:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_10
                        break;
                    }
                case 219:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_10
                        PIXEL_2X_10_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 125:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_00_12
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_00_61
                            PIXEL_2X_10_90
                        }
                        PIXEL_2X_01_11
                        PIXEL_2X_11_10
                        break;
                    }
                case 221:
                    {
                        PIXEL_2X_00_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_01_11
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_01_60
                            PIXEL_2X_11_90
                        }
                        PIXEL_2X_10_10
                        break;
                    }
                case 207:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                            PIXEL_2X_01_12
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_01_61
                        }
                        PIXEL_2X_10_10
                        PIXEL_2X_11_11
                        break;
                    }
                case 238:
                    {
                        PIXEL_2X_00_10
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                            PIXEL_2X_11_11
                        }
                        else
                        {
                            PIXEL_2X_10_90
                            PIXEL_2X_11_60
                        }
                        break;
                    }
                case 190:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                            PIXEL_2X_11_12
                        }
                        else
                        {
                            PIXEL_2X_01_90
                            PIXEL_2X_11_61
                        }
                        PIXEL_2X_10_11
                        break;
                    }
                case 187:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                            PIXEL_2X_10_11
                        }
                        else
                        {
                            PIXEL_2X_00_90
                            PIXEL_2X_10_60
                        }
                        PIXEL_2X_01_10
                        PIXEL_2X_11_12
                        break;
                    }
                case 243:
                    {
                        PIXEL_2X_00_11
                        PIXEL_2X_01_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_10_12
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_10_61
                            PIXEL_2X_11_90
                        }
                        break;
                    }
                case 119:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_00_11
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_00_60
                            PIXEL_2X_01_90
                        }
                        PIXEL_2X_10_12
                        PIXEL_2X_11_10
                        break;
                    }
                case 237:
                case 233:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_20
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 175:
                case 47:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        PIXEL_2X_01_12
                        PIXEL_2X_10_11
                        PIXEL_2X_11_20
                        break;
                    }
                case 183:
                case 151:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_20
                        PIXEL_2X_11_12
                        break;
                    }
                case 245:
                case 244:
                    {
                        PIXEL_2X_00_20
                        PIXEL_2X_01_11
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 250:
                    {
                        PIXEL_2X_00_10
                        PIXEL_2X_01_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 123:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_10
                        break;
                    }
                case 95:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_10
                        PIXEL_2X_11_10
                        break;
                    }
                case 222:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 252:
                    {
                        PIXEL_2X_00_21
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 249:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_22
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 235:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_21
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 111:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_22
                        break;
                    }
                case 63:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_21
                        break;
                    }
                case 159:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_22
                        PIXEL_2X_11_12
                        break;
                    }
                case 215:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_21
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 246:
                    {
                        PIXEL_2X_00_22
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 254:
                    {
                        PIXEL_2X_00_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 253:
                    {
                        PIXEL_2X_00_12
                        PIXEL_2X_01_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 251:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        PIXEL_2X_01_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 239:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        PIXEL_2X_01_12
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        PIXEL_2X_11_11
                        break;
                    }
                case 127:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_20
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_20
                        }
                        PIXEL_2X_11_10
                        break;
                    }
                case 191:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_11
                        PIXEL_2X_11_12
                        break;
                    }
                case 223:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_20
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_10
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_20
                        }
                        break;
                    }
                case 247:
                    {
                        PIXEL_2X_00_11
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        PIXEL_2X_10_12
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
                case 255:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_2X_00_0
                        }
                        else
                        {
                            PIXEL_2X_00_100
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_2X_01_0
                        }
                        else
                        {
                            PIXEL_2X_01_100
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_2X_10_0
                        }
                        else
                        {
                            PIXEL_2X_10_100
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_2X_11_0
                        }
                        else
                        {
                            PIXEL_2X_11_100
                        }
                        break;
                    }
            }
            sp++;
            dp += 2;
        }

        sRowP += srb;
        sp = (uint32_t *) sRowP;

        dRowP += drb * 2;
        dp = (uint32_t *) dRowP;
    }
}

void hq2x_32( uint32_t * sp, uint32_t * dp, int Xres, int Yres )
{
    uint32_t rowBytesL = Xres * 4;
    hq2x_32_rb(sp, rowBytesL, dp, rowBytesL * 2, Xres, Yres);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PIXEL_3X_00_1M  Interp1(dp, w[5], w[1]);
#define PIXEL_3X_00_1U  Interp1(dp, w[5], w[2]);
#define PIXEL_3X_00_1L  Interp1(dp, w[5], w[4]);
#define PIXEL_3X_00_2   Interp2(dp, w[5], w[4], w[2]);
#define PIXEL_3X_00_4   Interp4(dp, w[5], w[4], w[2]);
#define PIXEL_3X_00_5   Interp5(dp, w[4], w[2]);
#define PIXEL_3X_00_C   *dp   = w[5];

#define PIXEL_3X_01_1   Interp1(dp+1, w[5], w[2]);
#define PIXEL_3X_01_3   Interp3(dp+1, w[5], w[2]);
#define PIXEL_3X_01_6   Interp1(dp+1, w[2], w[5]);
#define PIXEL_3X_01_C   *(dp+1) = w[5];

#define PIXEL_3X_02_1M  Interp1(dp+2, w[5], w[3]);
#define PIXEL_3X_02_1U  Interp1(dp+2, w[5], w[2]);
#define PIXEL_3X_02_1R  Interp1(dp+2, w[5], w[6]);
#define PIXEL_3X_02_2   Interp2(dp+2, w[5], w[2], w[6]);
#define PIXEL_3X_02_4   Interp4(dp+2, w[5], w[2], w[6]);
#define PIXEL_3X_02_5   Interp5(dp+2, w[2], w[6]);
#define PIXEL_3X_02_C   *(dp+2) = w[5];

#define PIXEL_3X_10_1   Interp1(dp+dpL, w[5], w[4]);
#define PIXEL_3X_10_3   Interp3(dp+dpL, w[5], w[4]);
#define PIXEL_3X_10_6   Interp1(dp+dpL, w[4], w[5]);
#define PIXEL_3X_10_C   *(dp+dpL) = w[5];

#define PIXEL_3X_11     *(dp+dpL+1) = w[5];

#define PIXEL_3X_12_1   Interp1(dp+dpL+2, w[5], w[6]);
#define PIXEL_3X_12_3   Interp3(dp+dpL+2, w[5], w[6]);
#define PIXEL_3X_12_6   Interp1(dp+dpL+2, w[6], w[5]);
#define PIXEL_3X_12_C   *(dp+dpL+2) = w[5];

#define PIXEL_3X_20_1M  Interp1(dp+dpL+dpL, w[5], w[7]);
#define PIXEL_3X_20_1D  Interp1(dp+dpL+dpL, w[5], w[8]);
#define PIXEL_3X_20_1L  Interp1(dp+dpL+dpL, w[5], w[4]);
#define PIXEL_3X_20_2   Interp2(dp+dpL+dpL, w[5], w[8], w[4]);
#define PIXEL_3X_20_4   Interp4(dp+dpL+dpL, w[5], w[8], w[4]);
#define PIXEL_3X_20_5   Interp5(dp+dpL+dpL, w[8], w[4]);
#define PIXEL_3X_20_C   *(dp+dpL+dpL) = w[5];

#define PIXEL_3X_21_1   Interp1(dp+dpL+dpL+1, w[5], w[8]);
#define PIXEL_3X_21_3   Interp3(dp+dpL+dpL+1, w[5], w[8]);
#define PIXEL_3X_21_6   Interp1(dp+dpL+dpL+1, w[8], w[5]);
#define PIXEL_3X_21_C   *(dp+dpL+dpL+1) = w[5];

#define PIXEL_3X_22_1M  Interp1(dp+dpL+dpL+2, w[5], w[9]);
#define PIXEL_3X_22_1D  Interp1(dp+dpL+dpL+2, w[5], w[8]);
#define PIXEL_3X_22_1R  Interp1(dp+dpL+dpL+2, w[5], w[6]);
#define PIXEL_3X_22_2   Interp2(dp+dpL+dpL+2, w[5], w[6], w[8]);
#define PIXEL_3X_22_4   Interp4(dp+dpL+dpL+2, w[5], w[6], w[8]);
#define PIXEL_3X_22_5   Interp5(dp+dpL+dpL+2, w[6], w[8]);
#define PIXEL_3X_22_C   *(dp+dpL+dpL+2) = w[5];

void hq3x_32_rb( uint32_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres )
{
    int  i, j, k;
    int  prevline, nextline;
    uint32_t  w[10];
    int dpL = (drb >> 2);
    int spL = (srb >> 2);
    uint8_t *sRowP = (uint8_t *) sp;
    uint8_t *dRowP = (uint8_t *) dp;
    uint32_t yuv1, yuv2;

    //   +----+----+----+
    //   |    |    |    |
    //   | w1 | w2 | w3 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w4 | w5 | w6 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w7 | w8 | w9 |
    //   +----+----+----+

    for (j=0; j<Yres; j++)
    {
        if (j>0)      prevline = -spL; else prevline = 0;
        if (j<Yres-1) nextline =  spL; else nextline = 0;

        for (i=0; i<Xres; i++)
        {
            w[2] = *(sp + prevline);
            w[5] = *sp;
            w[8] = *(sp + nextline);

            if (i>0)
            {
                w[1] = *(sp + prevline - 1);
                w[4] = *(sp - 1);
                w[7] = *(sp + nextline - 1);
            }
            else
            {
                w[1] = w[2];
                w[4] = w[5];
                w[7] = w[8];
            }

            if (i<Xres-1)
            {
                w[3] = *(sp + prevline + 1);
                w[6] = *(sp + 1);
                w[9] = *(sp + nextline + 1);
            }
            else
            {
                w[3] = w[2];
                w[6] = w[5];
                w[9] = w[8];
            }

            int pattern = 0;
            int flag = 1;

            yuv1 = rgb_to_yuv(w[5]);

            for (k=1; k<=9; k++)
            {
                if (k==5) continue;

                if ( w[k] != w[5] )
                {
                    yuv2 = rgb_to_yuv(w[k]);
                    if (yuv_diff(yuv1, yuv2))
                        pattern |= flag;
                }
                flag <<= 1;
            }

            switch (pattern)
            {
                case 0:
                case 1:
                case 4:
                case 32:
                case 128:
                case 5:
                case 132:
                case 160:
                case 33:
                case 129:
                case 36:
                case 133:
                case 164:
                case 161:
                case 37:
                case 165:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 2:
                case 34:
                case 130:
                case 162:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 16:
                case 17:
                case 48:
                case 49:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 64:
                case 65:
                case 68:
                case 69:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 8:
                case 12:
                case 136:
                case 140:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 3:
                case 35:
                case 131:
                case 163:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 6:
                case 38:
                case 134:
                case 166:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 20:
                case 21:
                case 52:
                case 53:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 144:
                case 145:
                case 176:
                case 177:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 192:
                case 193:
                case 196:
                case 197:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 96:
                case 97:
                case 100:
                case 101:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 40:
                case 44:
                case 168:
                case 172:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 9:
                case 13:
                case 137:
                case 141:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 18:
                case 50:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1M
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 80:
                case 81:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 72:
                case 76:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1M
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 10:
                case 138:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 66:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 24:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 7:
                case 39:
                case 135:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 148:
                case 149:
                case 180:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 224:
                case 228:
                case 225:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 41:
                case 169:
                case 45:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 22:
                case 54:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 208:
                case 209:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 104:
                case 108:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 11:
                case 139:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 19:
                case 51:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_00_1L
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1M
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_01_6
                            PIXEL_3X_02_5
                            PIXEL_3X_12_1
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 146:
                case 178:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1M
                            PIXEL_3X_12_C
                            PIXEL_3X_22_1D
                        }
                        else
                        {
                            PIXEL_3X_01_1
                            PIXEL_3X_02_5
                            PIXEL_3X_12_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        break;
                    }
                case 84:
                case 85:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_02_1U
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                            PIXEL_3X_12_6
                            PIXEL_3X_21_1
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        break;
                    }
                case 112:
                case 113:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_20_1L
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_12_1
                            PIXEL_3X_20_2
                            PIXEL_3X_21_6
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        break;
                    }
                case 200:
                case 204:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1M
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1R
                        }
                        else
                        {
                            PIXEL_3X_10_1
                            PIXEL_3X_20_5
                            PIXEL_3X_21_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        break;
                    }
                case 73:
                case 77:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_00_1U
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1M
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_10_6
                            PIXEL_3X_20_5
                            PIXEL_3X_21_1
                        }
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 42:
                case 170:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1D
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_1
                            PIXEL_3X_10_6
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 14:
                case 142:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1R
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_6
                            PIXEL_3X_02_2
                            PIXEL_3X_10_1
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 67:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 70:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 28:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 152:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 194:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 98:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 56:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 25:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 26:
                case 31:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 82:
                case 214:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 88:
                case 248:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 74:
                case 107:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 27:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 86:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 216:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 106:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 30:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 210:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 120:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 75:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 29:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 198:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 184:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 99:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 57:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 71:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 156:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 226:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 60:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 195:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 102:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 153:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 58:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 83:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 92:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 202:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 78:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 154:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 114:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 89:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 90:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 55:
                case 23:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_00_1L
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_01_6
                            PIXEL_3X_02_5
                            PIXEL_3X_12_1
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 182:
                case 150:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                            PIXEL_3X_22_1D
                        }
                        else
                        {
                            PIXEL_3X_01_1
                            PIXEL_3X_02_5
                            PIXEL_3X_12_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        break;
                    }
                case 213:
                case 212:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_02_1U
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                            PIXEL_3X_12_6
                            PIXEL_3X_21_1
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        break;
                    }
                case 241:
                case 240:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_20_1L
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_1
                            PIXEL_3X_20_2
                            PIXEL_3X_21_6
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        break;
                    }
                case 236:
                case 232:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1R
                        }
                        else
                        {
                            PIXEL_3X_10_1
                            PIXEL_3X_20_5
                            PIXEL_3X_21_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        break;
                    }
                case 109:
                case 105:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_00_1U
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_10_6
                            PIXEL_3X_20_5
                            PIXEL_3X_21_1
                        }
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 171:
                case 43:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1D
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_1
                            PIXEL_3X_10_6
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 143:
                case 15:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1R
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_6
                            PIXEL_3X_02_2
                            PIXEL_3X_10_1
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 124:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 203:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 62:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 211:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 118:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 217:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 110:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 155:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 188:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 185:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 61:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 157:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 103:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 227:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 230:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 199:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 220:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 158:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 234:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1R
                        break;
                    }
                case 242:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1L
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 59:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 121:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 87:
                    {
                        PIXEL_3X_00_1L
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 79:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1R
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 122:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 94:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 218:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 91:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 229:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 167:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 173:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 181:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 186:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 115:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 93:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 206:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 205:
                case 201:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_1M
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 174:
                case 46:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_1M
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 179:
                case 147:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_1M
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 117:
                case 116:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_1M
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 189:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 231:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 126:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 219:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 125:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_00_1U
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_10_6
                            PIXEL_3X_20_5
                            PIXEL_3X_21_1
                        }
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 221:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_02_1U
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                            PIXEL_3X_12_6
                            PIXEL_3X_21_1
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        break;
                    }
                case 207:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_02_1R
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_6
                            PIXEL_3X_02_2
                            PIXEL_3X_10_1
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 238:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_1R
                        }
                        else
                        {
                            PIXEL_3X_10_1
                            PIXEL_3X_20_5
                            PIXEL_3X_21_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        break;
                    }
                case 190:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                            PIXEL_3X_22_1D
                        }
                        else
                        {
                            PIXEL_3X_01_1
                            PIXEL_3X_02_5
                            PIXEL_3X_12_6
                            PIXEL_3X_22_2
                        }
                        PIXEL_3X_00_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        break;
                    }
                case 187:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                            PIXEL_3X_20_1D
                        }
                        else
                        {
                            PIXEL_3X_00_5
                            PIXEL_3X_01_1
                            PIXEL_3X_10_6
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 243:
                    {
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_20_1L
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_1
                            PIXEL_3X_20_2
                            PIXEL_3X_21_6
                            PIXEL_3X_22_5
                        }
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        break;
                    }
                case 119:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_00_1L
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_01_6
                            PIXEL_3X_02_5
                            PIXEL_3X_12_1
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 237:
                case 233:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_2
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 175:
                case 47:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_2
                        break;
                    }
                case 183:
                case 151:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_2
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 245:
                case 244:
                    {
                        PIXEL_3X_00_2
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 250:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 123:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 95:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1M
                        break;
                    }
                case 222:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 252:
                    {
                        PIXEL_3X_00_1M
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 249:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 235:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 111:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 63:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1M
                        break;
                    }
                case 159:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_10_3
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 215:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 246:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 254:
                    {
                        PIXEL_3X_00_1M
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_4
                        }
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_4
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_21_3
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 253:
                    {
                        PIXEL_3X_00_1U
                        PIXEL_3X_01_1
                        PIXEL_3X_02_1U
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 251:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_01_3
                        }
                        PIXEL_3X_02_1M
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_10_C
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_10_3
                            PIXEL_3X_20_2
                            PIXEL_3X_21_3
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_12_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_12_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 239:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        PIXEL_3X_02_1R
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_1
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        PIXEL_3X_22_1R
                        break;
                    }
                case 127:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_01_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                            PIXEL_3X_01_3
                            PIXEL_3X_10_3
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_02_4
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_11
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                            PIXEL_3X_21_C
                        }
                        else
                        {
                            PIXEL_3X_20_4
                            PIXEL_3X_21_3
                        }
                        PIXEL_3X_22_1M
                        break;
                    }
                case 191:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1D
                        PIXEL_3X_21_1
                        PIXEL_3X_22_1D
                        break;
                    }
                case 223:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                            PIXEL_3X_10_C
                        }
                        else
                        {
                            PIXEL_3X_00_4
                            PIXEL_3X_10_3
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_01_C
                            PIXEL_3X_02_C
                            PIXEL_3X_12_C
                        }
                        else
                        {
                            PIXEL_3X_01_3
                            PIXEL_3X_02_2
                            PIXEL_3X_12_3
                        }
                        PIXEL_3X_11
                        PIXEL_3X_20_1M
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_21_C
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_21_3
                            PIXEL_3X_22_4
                        }
                        break;
                    }
                case 247:
                    {
                        PIXEL_3X_00_1L
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_1
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        PIXEL_3X_20_1L
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
                case 255:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_3X_00_C
                        }
                        else
                        {
                            PIXEL_3X_00_2
                        }
                        PIXEL_3X_01_C
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_3X_02_C
                        }
                        else
                        {
                            PIXEL_3X_02_2
                        }
                        PIXEL_3X_10_C
                        PIXEL_3X_11
                        PIXEL_3X_12_C
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_3X_20_C
                        }
                        else
                        {
                            PIXEL_3X_20_2
                        }
                        PIXEL_3X_21_C
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_3X_22_C
                        }
                        else
                        {
                            PIXEL_3X_22_2
                        }
                        break;
                    }
            }
            sp++;
            dp += 3;
        }

        sRowP += srb;
        sp = (uint32_t *) sRowP;

        dRowP += drb * 3;
        dp = (uint32_t *) dRowP;
    }
}

void hq3x_32( uint32_t * sp, uint32_t * dp, int Xres, int Yres )
{
    uint32_t rowBytesL = Xres * 4;
    hq3x_32_rb(sp, rowBytesL, dp, rowBytesL * 3, Xres, Yres);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define PIXEL_4X_00_0     *dp = w[5];
#define PIXEL_4X_00_11    Interp1(dp, w[5], w[4]);
#define PIXEL_4X_00_12    Interp1(dp, w[5], w[2]);
#define PIXEL_4X_00_20    Interp2(dp, w[5], w[2], w[4]);
#define PIXEL_4X_00_50    Interp5(dp, w[2], w[4]);
#define PIXEL_4X_00_80    Interp8(dp, w[5], w[1]);
#define PIXEL_4X_00_81    Interp8(dp, w[5], w[4]);
#define PIXEL_4X_00_82    Interp8(dp, w[5], w[2]);
#define PIXEL_4X_01_0     *(dp+1) = w[5];
#define PIXEL_4X_01_10    Interp1(dp+1, w[5], w[1]);
#define PIXEL_4X_01_12    Interp1(dp+1, w[5], w[2]);
#define PIXEL_4X_01_14    Interp1(dp+1, w[2], w[5]);
#define PIXEL_4X_01_21    Interp2(dp+1, w[2], w[5], w[4]);
#define PIXEL_4X_01_31    Interp3(dp+1, w[5], w[4]);
#define PIXEL_4X_01_50    Interp5(dp+1, w[2], w[5]);
#define PIXEL_4X_01_60    Interp6(dp+1, w[5], w[2], w[4]);
#define PIXEL_4X_01_61    Interp6(dp+1, w[5], w[2], w[1]);
#define PIXEL_4X_01_82    Interp8(dp+1, w[5], w[2]);
#define PIXEL_4X_01_83    Interp8(dp+1, w[2], w[4]);
#define PIXEL_4X_02_0     *(dp+2) = w[5];
#define PIXEL_4X_02_10    Interp1(dp+2, w[5], w[3]);
#define PIXEL_4X_02_11    Interp1(dp+2, w[5], w[2]);
#define PIXEL_4X_02_13    Interp1(dp+2, w[2], w[5]);
#define PIXEL_4X_02_21    Interp2(dp+2, w[2], w[5], w[6]);
#define PIXEL_4X_02_32    Interp3(dp+2, w[5], w[6]);
#define PIXEL_4X_02_50    Interp5(dp+2, w[2], w[5]);
#define PIXEL_4X_02_60    Interp6(dp+2, w[5], w[2], w[6]);
#define PIXEL_4X_02_61    Interp6(dp+2, w[5], w[2], w[3]);
#define PIXEL_4X_02_81    Interp8(dp+2, w[5], w[2]);
#define PIXEL_4X_02_83    Interp8(dp+2, w[2], w[6]);
#define PIXEL_4X_03_0     *(dp+3) = w[5];
#define PIXEL_4X_03_11    Interp1(dp+3, w[5], w[2]);
#define PIXEL_4X_03_12    Interp1(dp+3, w[5], w[6]);
#define PIXEL_4X_03_20    Interp2(dp+3, w[5], w[2], w[6]);
#define PIXEL_4X_03_50    Interp5(dp+3, w[2], w[6]);
#define PIXEL_4X_03_80    Interp8(dp+3, w[5], w[3]);
#define PIXEL_4X_03_81    Interp8(dp+3, w[5], w[2]);
#define PIXEL_4X_03_82    Interp8(dp+3, w[5], w[6]);
#define PIXEL_4X_10_0     *(dp+dpL) = w[5];
#define PIXEL_4X_10_10    Interp1(dp+dpL, w[5], w[1]);
#define PIXEL_4X_10_11    Interp1(dp+dpL, w[5], w[4]);
#define PIXEL_4X_10_13    Interp1(dp+dpL, w[4], w[5]);
#define PIXEL_4X_10_21    Interp2(dp+dpL, w[4], w[5], w[2]);
#define PIXEL_4X_10_32    Interp3(dp+dpL, w[5], w[2]);
#define PIXEL_4X_10_50    Interp5(dp+dpL, w[4], w[5]);
#define PIXEL_4X_10_60    Interp6(dp+dpL, w[5], w[4], w[2]);
#define PIXEL_4X_10_61    Interp6(dp+dpL, w[5], w[4], w[1]);
#define PIXEL_4X_10_81    Interp8(dp+dpL, w[5], w[4]);
#define PIXEL_4X_10_83    Interp8(dp+dpL, w[4], w[2]);
#define PIXEL_4X_11_0     *(dp+dpL+1) = w[5];
#define PIXEL_4X_11_30    Interp3(dp+dpL+1, w[5], w[1]);
#define PIXEL_4X_11_31    Interp3(dp+dpL+1, w[5], w[4]);
#define PIXEL_4X_11_32    Interp3(dp+dpL+1, w[5], w[2]);
#define PIXEL_4X_11_70    Interp7(dp+dpL+1, w[5], w[4], w[2]);
#define PIXEL_4X_12_0     *(dp+dpL+2) = w[5];
#define PIXEL_4X_12_30    Interp3(dp+dpL+2, w[5], w[3]);
#define PIXEL_4X_12_31    Interp3(dp+dpL+2, w[5], w[2]);
#define PIXEL_4X_12_32    Interp3(dp+dpL+2, w[5], w[6]);
#define PIXEL_4X_12_70    Interp7(dp+dpL+2, w[5], w[6], w[2]);
#define PIXEL_4X_13_0     *(dp+dpL+3) = w[5];
#define PIXEL_4X_13_10    Interp1(dp+dpL+3, w[5], w[3]);
#define PIXEL_4X_13_12    Interp1(dp+dpL+3, w[5], w[6]);
#define PIXEL_4X_13_14    Interp1(dp+dpL+3, w[6], w[5]);
#define PIXEL_4X_13_21    Interp2(dp+dpL+3, w[6], w[5], w[2]);
#define PIXEL_4X_13_31    Interp3(dp+dpL+3, w[5], w[2]);
#define PIXEL_4X_13_50    Interp5(dp+dpL+3, w[6], w[5]);
#define PIXEL_4X_13_60    Interp6(dp+dpL+3, w[5], w[6], w[2]);
#define PIXEL_4X_13_61    Interp6(dp+dpL+3, w[5], w[6], w[3]);
#define PIXEL_4X_13_82    Interp8(dp+dpL+3, w[5], w[6]);
#define PIXEL_4X_13_83    Interp8(dp+dpL+3, w[6], w[2]);
#define PIXEL_4X_20_0     *(dp+dpL+dpL) = w[5];
#define PIXEL_4X_20_10    Interp1(dp+dpL+dpL, w[5], w[7]);
#define PIXEL_4X_20_12    Interp1(dp+dpL+dpL, w[5], w[4]);
#define PIXEL_4X_20_14    Interp1(dp+dpL+dpL, w[4], w[5]);
#define PIXEL_4X_20_21    Interp2(dp+dpL+dpL, w[4], w[5], w[8]);
#define PIXEL_4X_20_31    Interp3(dp+dpL+dpL, w[5], w[8]);
#define PIXEL_4X_20_50    Interp5(dp+dpL+dpL, w[4], w[5]);
#define PIXEL_4X_20_60    Interp6(dp+dpL+dpL, w[5], w[4], w[8]);
#define PIXEL_4X_20_61    Interp6(dp+dpL+dpL, w[5], w[4], w[7]);
#define PIXEL_4X_20_82    Interp8(dp+dpL+dpL, w[5], w[4]);
#define PIXEL_4X_20_83    Interp8(dp+dpL+dpL, w[4], w[8]);
#define PIXEL_4X_21_0     *(dp+dpL+dpL+1) = w[5];
#define PIXEL_4X_21_30    Interp3(dp+dpL+dpL+1, w[5], w[7]);
#define PIXEL_4X_21_31    Interp3(dp+dpL+dpL+1, w[5], w[8]);
#define PIXEL_4X_21_32    Interp3(dp+dpL+dpL+1, w[5], w[4]);
#define PIXEL_4X_21_70    Interp7(dp+dpL+dpL+1, w[5], w[4], w[8]);
#define PIXEL_4X_22_0     *(dp+dpL+dpL+2) = w[5];
#define PIXEL_4X_22_30    Interp3(dp+dpL+dpL+2, w[5], w[9]);
#define PIXEL_4X_22_31    Interp3(dp+dpL+dpL+2, w[5], w[6]);
#define PIXEL_4X_22_32    Interp3(dp+dpL+dpL+2, w[5], w[8]);
#define PIXEL_4X_22_70    Interp7(dp+dpL+dpL+2, w[5], w[6], w[8]);
#define PIXEL_4X_23_0     *(dp+dpL+dpL+3) = w[5];
#define PIXEL_4X_23_10    Interp1(dp+dpL+dpL+3, w[5], w[9]);
#define PIXEL_4X_23_11    Interp1(dp+dpL+dpL+3, w[5], w[6]);
#define PIXEL_4X_23_13    Interp1(dp+dpL+dpL+3, w[6], w[5]);
#define PIXEL_4X_23_21    Interp2(dp+dpL+dpL+3, w[6], w[5], w[8]);
#define PIXEL_4X_23_32    Interp3(dp+dpL+dpL+3, w[5], w[8]);
#define PIXEL_4X_23_50    Interp5(dp+dpL+dpL+3, w[6], w[5]);
#define PIXEL_4X_23_60    Interp6(dp+dpL+dpL+3, w[5], w[6], w[8]);
#define PIXEL_4X_23_61    Interp6(dp+dpL+dpL+3, w[5], w[6], w[9]);
#define PIXEL_4X_23_81    Interp8(dp+dpL+dpL+3, w[5], w[6]);
#define PIXEL_4X_23_83    Interp8(dp+dpL+dpL+3, w[6], w[8]);
#define PIXEL_4X_30_0     *(dp+dpL+dpL+dpL) = w[5];
#define PIXEL_4X_30_11    Interp1(dp+dpL+dpL+dpL, w[5], w[8]);
#define PIXEL_4X_30_12    Interp1(dp+dpL+dpL+dpL, w[5], w[4]);
#define PIXEL_4X_30_20    Interp2(dp+dpL+dpL+dpL, w[5], w[8], w[4]);
#define PIXEL_4X_30_50    Interp5(dp+dpL+dpL+dpL, w[8], w[4]);
#define PIXEL_4X_30_80    Interp8(dp+dpL+dpL+dpL, w[5], w[7]);
#define PIXEL_4X_30_81    Interp8(dp+dpL+dpL+dpL, w[5], w[8]);
#define PIXEL_4X_30_82    Interp8(dp+dpL+dpL+dpL, w[5], w[4]);
#define PIXEL_4X_31_0     *(dp+dpL+dpL+dpL+1) = w[5];
#define PIXEL_4X_31_10    Interp1(dp+dpL+dpL+dpL+1, w[5], w[7]);
#define PIXEL_4X_31_11    Interp1(dp+dpL+dpL+dpL+1, w[5], w[8]);
#define PIXEL_4X_31_13    Interp1(dp+dpL+dpL+dpL+1, w[8], w[5]);
#define PIXEL_4X_31_21    Interp2(dp+dpL+dpL+dpL+1, w[8], w[5], w[4]);
#define PIXEL_4X_31_32    Interp3(dp+dpL+dpL+dpL+1, w[5], w[4]);
#define PIXEL_4X_31_50    Interp5(dp+dpL+dpL+dpL+1, w[8], w[5]);
#define PIXEL_4X_31_60    Interp6(dp+dpL+dpL+dpL+1, w[5], w[8], w[4]);
#define PIXEL_4X_31_61    Interp6(dp+dpL+dpL+dpL+1, w[5], w[8], w[7]);
#define PIXEL_4X_31_81    Interp8(dp+dpL+dpL+dpL+1, w[5], w[8]);
#define PIXEL_4X_31_83    Interp8(dp+dpL+dpL+dpL+1, w[8], w[4]);
#define PIXEL_4X_32_0     *(dp+dpL+dpL+dpL+2) = w[5];
#define PIXEL_4X_32_10    Interp1(dp+dpL+dpL+dpL+2, w[5], w[9]);
#define PIXEL_4X_32_12    Interp1(dp+dpL+dpL+dpL+2, w[5], w[8]);
#define PIXEL_4X_32_14    Interp1(dp+dpL+dpL+dpL+2, w[8], w[5]);
#define PIXEL_4X_32_21    Interp2(dp+dpL+dpL+dpL+2, w[8], w[5], w[6]);
#define PIXEL_4X_32_31    Interp3(dp+dpL+dpL+dpL+2, w[5], w[6]);
#define PIXEL_4X_32_50    Interp5(dp+dpL+dpL+dpL+2, w[8], w[5]);
#define PIXEL_4X_32_60    Interp6(dp+dpL+dpL+dpL+2, w[5], w[8], w[6]);
#define PIXEL_4X_32_61    Interp6(dp+dpL+dpL+dpL+2, w[5], w[8], w[9]);
#define PIXEL_4X_32_82    Interp8(dp+dpL+dpL+dpL+2, w[5], w[8]);
#define PIXEL_4X_32_83    Interp8(dp+dpL+dpL+dpL+2, w[8], w[6]);
#define PIXEL_4X_33_0     *(dp+dpL+dpL+dpL+3) = w[5];
#define PIXEL_4X_33_11    Interp1(dp+dpL+dpL+dpL+3, w[5], w[6]);
#define PIXEL_4X_33_12    Interp1(dp+dpL+dpL+dpL+3, w[5], w[8]);
#define PIXEL_4X_33_20    Interp2(dp+dpL+dpL+dpL+3, w[5], w[8], w[6]);
#define PIXEL_4X_33_50    Interp5(dp+dpL+dpL+dpL+3, w[8], w[6]);
#define PIXEL_4X_33_80    Interp8(dp+dpL+dpL+dpL+3, w[5], w[9]);
#define PIXEL_4X_33_81    Interp8(dp+dpL+dpL+dpL+3, w[5], w[6]);
#define PIXEL_4X_33_82    Interp8(dp+dpL+dpL+dpL+3, w[5], w[8]);

void hq4x_32_rb( uint32_t * sp, uint32_t srb, uint32_t * dp, uint32_t drb, int Xres, int Yres )
{
    int  i, j, k;
    int  prevline, nextline;
    uint32_t w[10];
    int dpL = (drb >> 2);
    int spL = (srb >> 2);
    uint8_t *sRowP = (uint8_t *) sp;
    uint8_t *dRowP = (uint8_t *) dp;
    uint32_t yuv1, yuv2;

    //   +----+----+----+
    //   |    |    |    |
    //   | w1 | w2 | w3 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w4 | w5 | w6 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w7 | w8 | w9 |
    //   +----+----+----+

    for (j=0; j<Yres; j++)
    {
        if (j>0)      prevline = -spL; else prevline = 0;
        if (j<Yres-1) nextline =  spL; else nextline = 0;

        for (i=0; i<Xres; i++)
        {
            w[2] = *(sp + prevline);
            w[5] = *sp;
            w[8] = *(sp + nextline);

            if (i>0)
            {
                w[1] = *(sp + prevline - 1);
                w[4] = *(sp - 1);
                w[7] = *(sp + nextline - 1);
            }
            else
            {
                w[1] = w[2];
                w[4] = w[5];
                w[7] = w[8];
            }

            if (i<Xres-1)
            {
                w[3] = *(sp + prevline + 1);
                w[6] = *(sp + 1);
                w[9] = *(sp + nextline + 1);
            }
            else
            {
                w[3] = w[2];
                w[6] = w[5];
                w[9] = w[8];
            }

            int pattern = 0;
            int flag = 1;

            yuv1 = rgb_to_yuv(w[5]);

            for (k=1; k<=9; k++)
            {
                if (k==5) continue;

                if ( w[k] != w[5] )
                {
                    yuv2 = rgb_to_yuv(w[k]);
                    if (yuv_diff(yuv1, yuv2))
                        pattern |= flag;
                }
                flag <<= 1;
            }

            switch (pattern)
            {
                case 0:
                case 1:
                case 4:
                case 32:
                case 128:
                case 5:
                case 132:
                case 160:
                case 33:
                case 129:
                case 36:
                case 133:
                case 164:
                case 161:
                case 37:
                case 165:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 2:
                case 34:
                case 130:
                case 162:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 16:
                case 17:
                case 48:
                case 49:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 64:
                case 65:
                case 68:
                case 69:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 8:
                case 12:
                case 136:
                case 140:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 3:
                case 35:
                case 131:
                case 163:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 6:
                case 38:
                case 134:
                case 166:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 20:
                case 21:
                case 52:
                case 53:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 144:
                case 145:
                case 176:
                case 177:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 192:
                case 193:
                case 196:
                case 197:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 96:
                case 97:
                case 100:
                case 101:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 40:
                case 44:
                case 168:
                case 172:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 9:
                case 13:
                case 137:
                case 141:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 18:
                case 50:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_12_0
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 80:
                case 81:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 72:
                case 76:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_21_0
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 10:
                case 138:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 66:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 24:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 7:
                case 39:
                case 135:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 148:
                case 149:
                case 180:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 224:
                case 228:
                case 225:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 41:
                case 169:
                case 45:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 22:
                case 54:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 208:
                case 209:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 104:
                case 108:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 11:
                case 139:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 19:
                case 51:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_00_81
                            PIXEL_4X_01_31
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_00_12
                            PIXEL_4X_01_14
                            PIXEL_4X_02_83
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_21
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 146:
                case 178:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                            PIXEL_4X_23_32
                            PIXEL_4X_33_82
                        }
                        else
                        {
                            PIXEL_4X_02_21
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_83
                            PIXEL_4X_23_13
                            PIXEL_4X_33_11
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        break;
                    }
                case 84:
                case 85:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_03_81
                            PIXEL_4X_13_31
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_03_12
                            PIXEL_4X_13_14
                            PIXEL_4X_22_70
                            PIXEL_4X_23_83
                            PIXEL_4X_32_21
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 112:
                case 113:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_30_82
                            PIXEL_4X_31_32
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_70
                            PIXEL_4X_23_21
                            PIXEL_4X_30_11
                            PIXEL_4X_31_13
                            PIXEL_4X_32_83
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 200:
                case 204:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                            PIXEL_4X_32_31
                            PIXEL_4X_33_81
                        }
                        else
                        {
                            PIXEL_4X_20_21
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_83
                            PIXEL_4X_32_14
                            PIXEL_4X_33_12
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        break;
                    }
                case 73:
                case 77:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_00_82
                            PIXEL_4X_10_32
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_00_11
                            PIXEL_4X_10_13
                            PIXEL_4X_20_83
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_21
                        }
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 42:
                case 170:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                            PIXEL_4X_20_31
                            PIXEL_4X_30_81
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_21
                            PIXEL_4X_10_83
                            PIXEL_4X_11_70
                            PIXEL_4X_20_14
                            PIXEL_4X_30_12
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 14:
                case 142:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_02_32
                            PIXEL_4X_03_82
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_83
                            PIXEL_4X_02_13
                            PIXEL_4X_03_11
                            PIXEL_4X_10_21
                            PIXEL_4X_11_70
                        }
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 67:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 70:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 28:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 152:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 194:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 98:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 56:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 25:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 26:
                case 31:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 82:
                case 214:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 88:
                case 248:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 74:
                case 107:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 27:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 86:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 216:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 106:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 30:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 210:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 120:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 75:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 29:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 198:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 184:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 99:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 57:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 71:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 156:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 226:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 60:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 195:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 102:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 153:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 58:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 83:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 92:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 202:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 78:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 154:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 114:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        break;
                    }
                case 89:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 90:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 55:
                case 23:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_00_81
                            PIXEL_4X_01_31
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_12_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_00_12
                            PIXEL_4X_01_14
                            PIXEL_4X_02_83
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_21
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 182:
                case 150:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_12_0
                            PIXEL_4X_13_0
                            PIXEL_4X_23_32
                            PIXEL_4X_33_82
                        }
                        else
                        {
                            PIXEL_4X_02_21
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_83
                            PIXEL_4X_23_13
                            PIXEL_4X_33_11
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        break;
                    }
                case 213:
                case 212:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_03_81
                            PIXEL_4X_13_31
                            PIXEL_4X_22_0
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_03_12
                            PIXEL_4X_13_14
                            PIXEL_4X_22_70
                            PIXEL_4X_23_83
                            PIXEL_4X_32_21
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 241:
                case 240:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_0
                            PIXEL_4X_30_82
                            PIXEL_4X_31_32
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_22_70
                            PIXEL_4X_23_21
                            PIXEL_4X_30_11
                            PIXEL_4X_31_13
                            PIXEL_4X_32_83
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 236:
                case 232:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_21_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                            PIXEL_4X_32_31
                            PIXEL_4X_33_81
                        }
                        else
                        {
                            PIXEL_4X_20_21
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_83
                            PIXEL_4X_32_14
                            PIXEL_4X_33_12
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        break;
                    }
                case 109:
                case 105:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_00_82
                            PIXEL_4X_10_32
                            PIXEL_4X_20_0
                            PIXEL_4X_21_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_00_11
                            PIXEL_4X_10_13
                            PIXEL_4X_20_83
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_21
                        }
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 171:
                case 43:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                            PIXEL_4X_11_0
                            PIXEL_4X_20_31
                            PIXEL_4X_30_81
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_21
                            PIXEL_4X_10_83
                            PIXEL_4X_11_70
                            PIXEL_4X_20_14
                            PIXEL_4X_30_12
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 143:
                case 15:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_02_32
                            PIXEL_4X_03_82
                            PIXEL_4X_10_0
                            PIXEL_4X_11_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_83
                            PIXEL_4X_02_13
                            PIXEL_4X_03_11
                            PIXEL_4X_10_21
                            PIXEL_4X_11_70
                        }
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 124:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 203:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 62:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 211:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 118:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 217:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 110:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 155:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 188:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 185:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 61:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 157:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 103:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 227:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 230:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 199:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 220:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 158:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_12_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 234:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 242:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        break;
                    }
                case 59:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_11_0
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 121:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 87:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_0
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 79:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_11_0
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 122:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 94:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_12_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 218:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 91:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_11_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 229:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 167:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 173:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 181:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 186:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 115:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        break;
                    }
                case 93:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 206:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 205:
                case 201:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_10
                            PIXEL_4X_21_30
                            PIXEL_4X_30_80
                            PIXEL_4X_31_10
                        }
                        else
                        {
                            PIXEL_4X_20_12
                            PIXEL_4X_21_0
                            PIXEL_4X_30_20
                            PIXEL_4X_31_11
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 174:
                case 46:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_80
                            PIXEL_4X_01_10
                            PIXEL_4X_10_10
                            PIXEL_4X_11_30
                        }
                        else
                        {
                            PIXEL_4X_00_20
                            PIXEL_4X_01_12
                            PIXEL_4X_10_11
                            PIXEL_4X_11_0
                        }
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 179:
                case 147:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_10
                            PIXEL_4X_03_80
                            PIXEL_4X_12_30
                            PIXEL_4X_13_10
                        }
                        else
                        {
                            PIXEL_4X_02_11
                            PIXEL_4X_03_20
                            PIXEL_4X_12_0
                            PIXEL_4X_13_12
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 117:
                case 116:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_30
                            PIXEL_4X_23_10
                            PIXEL_4X_32_10
                            PIXEL_4X_33_80
                        }
                        else
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_11
                            PIXEL_4X_32_12
                            PIXEL_4X_33_20
                        }
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        break;
                    }
                case 189:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 231:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 126:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 219:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 125:
                    {
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_00_82
                            PIXEL_4X_10_32
                            PIXEL_4X_20_0
                            PIXEL_4X_21_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_00_11
                            PIXEL_4X_10_13
                            PIXEL_4X_20_83
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_21
                        }
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 221:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_03_81
                            PIXEL_4X_13_31
                            PIXEL_4X_22_0
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_03_12
                            PIXEL_4X_13_14
                            PIXEL_4X_22_70
                            PIXEL_4X_23_83
                            PIXEL_4X_32_21
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 207:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_02_32
                            PIXEL_4X_03_82
                            PIXEL_4X_10_0
                            PIXEL_4X_11_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_83
                            PIXEL_4X_02_13
                            PIXEL_4X_03_11
                            PIXEL_4X_10_21
                            PIXEL_4X_11_70
                        }
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 238:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_21_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                            PIXEL_4X_32_31
                            PIXEL_4X_33_81
                        }
                        else
                        {
                            PIXEL_4X_20_21
                            PIXEL_4X_21_70
                            PIXEL_4X_30_50
                            PIXEL_4X_31_83
                            PIXEL_4X_32_14
                            PIXEL_4X_33_12
                        }
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        break;
                    }
                case 190:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_12_0
                            PIXEL_4X_13_0
                            PIXEL_4X_23_32
                            PIXEL_4X_33_82
                        }
                        else
                        {
                            PIXEL_4X_02_21
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_83
                            PIXEL_4X_23_13
                            PIXEL_4X_33_11
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        break;
                    }
                case 187:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                            PIXEL_4X_11_0
                            PIXEL_4X_20_31
                            PIXEL_4X_30_81
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_21
                            PIXEL_4X_10_83
                            PIXEL_4X_11_70
                            PIXEL_4X_20_14
                            PIXEL_4X_30_12
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 243:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_22_0
                            PIXEL_4X_23_0
                            PIXEL_4X_30_82
                            PIXEL_4X_31_32
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_22_70
                            PIXEL_4X_23_21
                            PIXEL_4X_30_11
                            PIXEL_4X_31_13
                            PIXEL_4X_32_83
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 119:
                    {
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_00_81
                            PIXEL_4X_01_31
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_12_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_00_12
                            PIXEL_4X_01_14
                            PIXEL_4X_02_83
                            PIXEL_4X_03_50
                            PIXEL_4X_12_70
                            PIXEL_4X_13_21
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 237:
                case 233:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_60
                        PIXEL_4X_03_20
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_70
                        PIXEL_4X_13_60
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 175:
                case 47:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_70
                        PIXEL_4X_23_60
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_60
                        PIXEL_4X_33_20
                        break;
                    }
                case 183:
                case 151:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_60
                        PIXEL_4X_21_70
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_20
                        PIXEL_4X_31_60
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 245:
                case 244:
                    {
                        PIXEL_4X_00_20
                        PIXEL_4X_01_60
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_60
                        PIXEL_4X_11_70
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 250:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        break;
                    }
                case 123:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 95:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 222:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 252:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_61
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 249:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_61
                        PIXEL_4X_03_80
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        break;
                    }
                case 235:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_61
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 111:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_61
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 63:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_61
                        PIXEL_4X_33_80
                        break;
                    }
                case 159:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_80
                        PIXEL_4X_31_61
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 215:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_61
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 246:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_61
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 254:
                    {
                        PIXEL_4X_00_80
                        PIXEL_4X_01_10
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_10
                        PIXEL_4X_11_30
                        PIXEL_4X_12_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 253:
                    {
                        PIXEL_4X_00_82
                        PIXEL_4X_01_82
                        PIXEL_4X_02_81
                        PIXEL_4X_03_81
                        PIXEL_4X_10_32
                        PIXEL_4X_11_32
                        PIXEL_4X_12_31
                        PIXEL_4X_13_31
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 251:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_10
                        PIXEL_4X_03_80
                        PIXEL_4X_11_0
                        PIXEL_4X_12_30
                        PIXEL_4X_13_10
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        break;
                    }
                case 239:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        PIXEL_4X_02_32
                        PIXEL_4X_03_82
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_32
                        PIXEL_4X_13_82
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_31
                        PIXEL_4X_23_81
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        PIXEL_4X_32_31
                        PIXEL_4X_33_81
                        break;
                    }
                case 127:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_02_0
                            PIXEL_4X_03_0
                            PIXEL_4X_13_0
                        }
                        else
                        {
                            PIXEL_4X_02_50
                            PIXEL_4X_03_50
                            PIXEL_4X_13_50
                        }
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_20_0
                            PIXEL_4X_30_0
                            PIXEL_4X_31_0
                        }
                        else
                        {
                            PIXEL_4X_20_50
                            PIXEL_4X_30_50
                            PIXEL_4X_31_50
                        }
                        PIXEL_4X_21_0
                        PIXEL_4X_22_30
                        PIXEL_4X_23_10
                        PIXEL_4X_32_10
                        PIXEL_4X_33_80
                        break;
                    }
                case 191:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_31
                        PIXEL_4X_21_31
                        PIXEL_4X_22_32
                        PIXEL_4X_23_32
                        PIXEL_4X_30_81
                        PIXEL_4X_31_81
                        PIXEL_4X_32_82
                        PIXEL_4X_33_82
                        break;
                    }
                case 223:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                            PIXEL_4X_01_0
                            PIXEL_4X_10_0
                        }
                        else
                        {
                            PIXEL_4X_00_50
                            PIXEL_4X_01_50
                            PIXEL_4X_10_50
                        }
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_10
                        PIXEL_4X_21_30
                        PIXEL_4X_22_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_23_0
                            PIXEL_4X_32_0
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_23_50
                            PIXEL_4X_32_50
                            PIXEL_4X_33_50
                        }
                        PIXEL_4X_30_80
                        PIXEL_4X_31_10
                        break;
                    }
                case 247:
                    {
                        PIXEL_4X_00_81
                        PIXEL_4X_01_31
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_10_81
                        PIXEL_4X_11_31
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_82
                        PIXEL_4X_21_32
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        PIXEL_4X_30_82
                        PIXEL_4X_31_32
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
                case 255:
                    {
                        if (Diff(w[4], w[2]))
                        {
                            PIXEL_4X_00_0
                        }
                        else
                        {
                            PIXEL_4X_00_20
                        }
                        PIXEL_4X_01_0
                        PIXEL_4X_02_0
                        if (Diff(w[2], w[6]))
                        {
                            PIXEL_4X_03_0
                        }
                        else
                        {
                            PIXEL_4X_03_20
                        }
                        PIXEL_4X_10_0
                        PIXEL_4X_11_0
                        PIXEL_4X_12_0
                        PIXEL_4X_13_0
                        PIXEL_4X_20_0
                        PIXEL_4X_21_0
                        PIXEL_4X_22_0
                        PIXEL_4X_23_0
                        if (Diff(w[8], w[4]))
                        {
                            PIXEL_4X_30_0
                        }
                        else
                        {
                            PIXEL_4X_30_20
                        }
                        PIXEL_4X_31_0
                        PIXEL_4X_32_0
                        if (Diff(w[6], w[8]))
                        {
                            PIXEL_4X_33_0
                        }
                        else
                        {
                            PIXEL_4X_33_20
                        }
                        break;
                    }
            }
            sp++;
            dp += 4;
        }

        sRowP += srb;
        sp = (uint32_t *) sRowP;

        dRowP += drb * 4;
        dp = (uint32_t *) dRowP;
    }
}

void hq4x_32( uint32_t * sp, uint32_t * dp, int Xres, int Yres )
{
    uint32_t rowBytesL = Xres * 4;
    hq4x_32_rb(sp, rowBytesL, dp, rowBytesL * 4, Xres, Yres);
}
