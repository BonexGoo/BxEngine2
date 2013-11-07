#pragma once

#include <BxImage.hpp>
#include <BxWebContent.hpp>

class BxSimulator
{
public:
	point NCWindowPos;
	point NCClickPos;
	id_font GUIFont;
	id_font GUIFontMini;
	BxImage GUIImage;
	rect GUIImageRect[14];
	BxImage GUIWaterMark;
	BxImage GUILicense;
	int LicenseFrameCountMax;
	int LicenseFrameCount;
	bool IsDown[2];
	point CursorPos;
	int AvgFrameTime;
	int AvgFrameRate;

public:
	BxSimulator() : NCWindowPos(point::zero()), NCClickPos(point::zero()), GUIFont(null)
	{
		BxWebContent Web;
		if(Web.Cache("elgard.nowcdn.co.kr", 80, "labtest/r00000000n.TTF-01-BONEC-000000.NanumGothicExtraBold.ttf"))
		{
			GUIFont = BxCore::Font::Open(BxString("%s/%s", null, Web.GetCachePath(), "labtest/r00000000n.TTF-01-BONEC-000000.NanumGothicExtraBold.ttf"), 8);
			GUIFontMini = BxCore::Font::Open(BxString("%s/%s", null, Web.GetCachePath(), "labtest/r00000000n.TTF-01-BONEC-000000.NanumGothicExtraBold.ttf"), 6);
		}
		if(Web.Cache("elgard.nowcdn.co.kr", 80, "labtest/r00000000n.IMG-01-BONEC-000000.popup.png"))
		{
			GUIImage.Load(Web.GetBytes(), Web.GetDataLength(), BxImage::PNG);
			GUIImageRect[0] = XYWH(1, 1, 36, 38);
			GUIImageRect[1] = XYWH(38, 1, 64, 38);
			GUIImageRect[2] = XYWH(103, 1, 72, 38);
			GUIImageRect[3] = XYWH(1, 40, 7, 64);
			GUIImageRect[4] = XYWH(74, 40, 7, 64);
			GUIImageRect[5] = XYWH(1, 105, 7, 7);
			GUIImageRect[6] = XYWH(9, 105, 64, 7);
			GUIImageRect[7] = XYWH(74, 105, 7, 7);
			GUIImageRect[8] = XYWH(82, 40, 27, 20);
			GUIImageRect[9] = XYWH(110, 40, 27, 20);
			GUIImageRect[10] = XYWH(138, 40, 27, 20);
			GUIImageRect[11] = XYWH(82, 61, 27, 20);
			GUIImageRect[12] = XYWH(110, 61, 27, 20);
			GUIImageRect[13] = XYWH(138, 61, 27, 20);
		}
		if(Web.Cache("elgard.nowcdn.co.kr", 80, "labtest/r00000000n.IMG-01-BONEC-000000.WaterMark.png"))
		{
			GUIWaterMark.Load(Web.GetBytes(), Web.GetDataLength(), BxImage::PNG);
			GUIWaterMark.SetHotspot(GUIWaterMark.Width() / 2, GUIWaterMark.Height() / 2);
		}
		if(Web.Cache("elgard.nowcdn.co.kr", 80, "labtest/r00000000n.IMG-01-BONEC-000000.License.png"))
		{
			GUILicense.Load(Web.GetBytes(), Web.GetDataLength(), BxImage::PNG);
			GUILicense.SetHotspot(GUILicense.Width() / 2, GUILicense.Height());
		}
		LicenseFrameCountMax = 5000 / BxCore::Main::GetCurrentFrameTime();
		LicenseFrameCount = LicenseFrameCountMax;
		IsDown[0] = false;
		IsDown[1] = false;
		CursorPos = point::zero();
		AvgFrameTime = 0;
		AvgFrameRate = 0;
	}

	~BxSimulator()
	{
		BxCore::Font::Close(GUIFont);
		BxCore::Font::Close(GUIFontMini);
	}
};
