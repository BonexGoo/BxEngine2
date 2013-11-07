#include <BxScene.hpp>
#include "BxKeyboard.hpp"

FRAMEWORK_SCENE(BxKeyboard, "BxKeyboard")

local_data bool IsButtonShift = false;
local_data bool IsButtonKorean = false;
local_data bool IsButtonPushed[41] = {false,};
local_data string ButtonChar[2][2][41] =
{
	{
		{
			"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
			"q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
			"a",    "s", "d", "f", "g", "h", "j", "k",  "l",
			"↑",   "z", "x", "c", "v", "b", "n", "m",  "←",
			"Ko",                  " ",                 "▼"
		},
		{
			"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", "0",
			"ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ",
			"ㅁ",   "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ",  "ㅣ",
			"↑",   "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ",  "←",
			"En",                     " ",                    "▼"
		}
	},
	{
		{
			".", ":", "?", "/", "@", "&", "-", "=", "(", ")",
			"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
			"A",    "S", "D", "F", "G", "H", "J", "K",  "L",
			"↓",   "Z", "X", "C", "V", "B", "N", "M",  "←",
			"Ko",                  " ",                 "▼"
		},
		{
			".",  ":",  "?",  "/",  "@",  "&",  "-",  "=",  "(", ")",
			"ㅃ", "ㅉ", "ㄸ", "ㄲ", "ㅆ", "ㅛ", "ㅕ", "ㅑ", "ㅒ", "ㅖ",
			"ㅁ",   "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ",  "ㅣ",
			"↓",   "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ",  "←",
			"En",                     " ",                    "▼"
		}
	}
};
local_data const int ButtonX[41] =
{
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
	0,   3, 5, 7, 9, 11, 13, 15, 17,
	0,   3, 5, 7, 9, 11, 13, 15, 17,
	0,   3,                      17
};
local_data const int ButtonY[41] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2,   2, 2, 2, 2, 2, 2, 2, 2,
	3,   3, 3, 3, 3, 3, 3, 3, 3,
	4,   4,                   4
};
local_data const int ButtonW[41] =
{
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3,   2, 2, 2, 2, 2, 2, 2, 3,
	3,   2, 2, 2, 2, 2, 2, 2, 3,
	3,   14,                  3
};
local_data const int ButtonH = 44;

syseventresult OnEvent(BxKeyboard& Data, const sysevent& Event)
{
	int Index = -1;
	bool IsUp = false;
	bool IsDown = false;
	bool IsDrag = false;
	if(Event.type == syseventtype_button)
	{
		if(StrSameCount(Event.button.name, "KEY-") == 4)
		{
			Index = AtoI(&Event.button.name[4]);
			if(Event.button.type == sysbuttontype_down) IsDown = true;
			else if(Event.button.type == sysbuttontype_drag) IsDrag = true;
			else if(Event.button.type == sysbuttontype_up) IsUp = true;
		}
		else if(StrSameCount(Event.button.name, "BxKeyboard:0x") == 13)
		{
			if(Event.button.type == sysbuttontype_up)
			{
				Data.UserString = (BxString*)(void*) HtoI(Event.button.name + 13);
				Data.IMEChars = *Data.UserString;
				Data.IMETails = "";
				Data.IMETemps = "";
			}
			return syseventresult_done;
		}
	}
	else if(Event.type == syseventtype_key)
	{
		switch(Event.key.code)
		{
		case keykind_1: Index = 0; break; case keykind_2: Index = 1; break; case keykind_3: Index = 2; break;
		case keykind_4: Index = 3; break; case keykind_5: Index = 4; break; case keykind_6: Index = 5; break;
		case keykind_7: Index = 6; break; case keykind_8: Index = 7; break; case keykind_9: Index = 8; break;
		case keykind_0: Index = 9; break; case keykind_q: Index = 10; break; case keykind_w: Index = 11; break;
		case keykind_e: Index = 12; break; case keykind_r: Index = 13; break; case keykind_t: Index = 14; break;
		case keykind_y: Index = 15; break; case keykind_u: Index = 16; break; case keykind_i: Index = 17; break;
		case keykind_o: Index = 18; break; case keykind_p: Index = 19; break; case keykind_a: Index = 20; break;
		case keykind_s: Index = 21; break; case keykind_d: Index = 22; break; case keykind_f: Index = 23; break;
		case keykind_g: Index = 24; break; case keykind_h: Index = 25; break; case keykind_j: Index = 26; break;
		case keykind_k: Index = 27; break; case keykind_l: Index = 28; break; case keykind_shift: Index = 29; break;
		case keykind_z: Index = 30; break; case keykind_x: Index = 31; break; case keykind_c: Index = 32; break;
		case keykind_v: Index = 33; break; case keykind_b: Index = 34; break; case keykind_n: Index = 35; break;
		case keykind_m: Index = 36; break; case keykind_bspace: Index = 37; break; case keykind_ctrl: Index = 38; break;
		case keykind_space: Index = 39; break; case keykind_enter: Index = 40; break;
		}
		if(-1 < Index)
		{
			if(Event.key.type == syskeytype_down) IsDown = true;
			else if(Event.key.type == syskeytype_repeat) {IsUp = true; IsDown = true;} // 현재 마멀레이드 미지원
			else if(Event.key.type == syskeytype_up) IsUp = true;
		}
	}
	else if(Event.type == syseventtype_touch)
	{
		Data.UserString = null;
		Data.IMEChars = "";
		Data.IMETails = "";
		Data.IMETemps = "";
		return syseventresult_done;
	}
	const bool IsCharKey = !(Index == 29 || Index == 37 || Index == 38 || Index == 40);
	if(IsUp)
	{
		if(IsButtonPushed[Index])
		{
			IsButtonPushed[Index] = false;
			if(IsCharKey)
				Data.UpdateIME(true);
			else switch(Index)
			{
			case 29: // Shift
				IsButtonShift = !IsButtonShift;
				break;
			case 37: // BackSpace
				if(1 < Data.IMETemps.GetLength())
				{
					Data.IMETemps.DeleteRight(1 + Data.IMETemps.IsLastCharExtend());
					Data.UpdateIME(false);
				}
				else if(0 < Data.IMEChars.GetLength())
				{
					Data.IMEChars.DeleteRight(1 + Data.IMEChars.IsLastCharExtend());
					if(Data.UserString)
						*Data.UserString = Data.IMEChars + Data.IMETails;
				}
				break;
			case 38: // 한글/영어
				IsButtonKorean = !IsButtonKorean;
				break;
			case 40: // 입력
				Data.UserString = null;
				Data.IMEChars = "";
				Data.IMETails = "";
				Data.IMETemps = "";
				break;
			}
			if(Index != 29)
				IsButtonShift = false;
		}
	}
	if(IsDown)
	{
		if(IsCharKey && !IsButtonPushed[Index])
		{
			Data.IMETemps += ButtonChar[IsButtonShift][IsButtonKorean][Index];
			Data.UpdateIME(false);
		}
		IsButtonPushed[Index] = true;
	}
	if(IsDrag)
	{
		const bool IsInRect = (0 - 20 <= Event.button.x && 0 - 20 <= Event.button.y
			&& Event.button.x < Event.button.field.r - Event.button.field.l + 20
			&& Event.button.y < Event.button.field.b - Event.button.field.t + 20);
		if(IsCharKey && IsButtonPushed[Index] != IsInRect)
		{
			if(!IsButtonPushed[Index])
				Data.IMETemps += ButtonChar[IsButtonShift][IsButtonKorean][Index];
			else Data.IMETemps.DeleteRight(1 + Data.IMETemps.IsLastCharExtend());
			Data.UpdateIME(false);
		}
		IsButtonPushed[Index] = IsInRect;
	}
	if(IsUp || IsDown || IsDrag)
		return syseventresult_done;
	return syseventresult_pass;
}

sysupdateresult OnUpdate(BxKeyboard& Data)
{
	sysupdateresult Result = sysupdateresult_no_render_fixed_frame;
	const point Target = (Data.UserString)? XY(0, 0) : XY(0, ButtonH * 5);
	if(Data.UIPos.x != Target.x || Data.UIPos.y != Target.y)
	{
		Result = (sysupdateresult) (Result | sysupdateresult_do_render);
		Data.UIPos.x = (Data.UIPos.x * 9 + Target.x * 1) / 10;
		if(Data.UIPos.x < Target.x) ++Data.UIPos.x;
		else if(Data.UIPos.x > Target.x) --Data.UIPos.x;
		Data.UIPos.y = (Data.UIPos.y * 9 + Target.y * 1) / 10;
		if(Data.UIPos.y < Target.y) ++Data.UIPos.y;
		else if(Data.UIPos.y > Target.y) --Data.UIPos.y;
	}
	else if(Data.UserString)
		Result = (sysupdateresult) (Result | sysupdateresult_do_render);
	return Result;
}

void OnRender(BxKeyboard& Data, BxDraw& Draw)
{
	// Keyboard
	const int Gap = 2;
	const int RateX = Draw.Width() / 20;
	const int BeginX = (Draw.Width() - RateX * 20) / 2;
	const int BeginY = Draw.Height() - ButtonH * 5;
	for(int i = 0; i < 41; ++i)
	{
		const int iX = BeginX + RateX * ButtonX[i];
		const int iY = BeginY + ButtonH * ButtonY[i];
		const int iW = RateX * ButtonW[i];
		const int iH = ButtonH;
		try(Draw, CLIP(XYWH(iX + Data.UIPos.x, iY + Data.UIPos.y, iW, iH)), BxString("KEY-%02d", null, i))
		{
			try(Draw, CLIP(XYWH(Gap, Gap, Draw.Width() - Gap * 2, Draw.Height() - Gap * 2)))
			{
				Data.DrawButton(Draw, IsButtonPushed[i]);
				BxCore::Font::SetSort(fontsort_pad5);
				BxCore::Font::Draw(Data.buttonfont, ButtonChar[IsButtonShift][IsButtonKorean][i],
					XY(iX + Data.UIPos.x + Gap, iY + Data.UIPos.y + Gap), WH(Draw.Width(), Draw.Height()), RGB32(255, 255, 200), 192);
			}
		}
	}

	// IME
	if(Data.UserString)
	try(Draw, CLIP(XYWH(0, 0, Draw.Width(), BeginY)))
	{
		const int CharsWidth = BxCore::Font::GetSize(Data.IMEfont, Data.IMEChars).w;
		const int TailsWidth = BxCore::Font::GetSize(Data.IMEfont, Data.IMETails).w;
		const int CursorWidth = 8;
		const int TotalWidth = CharsWidth + TailsWidth + CursorWidth;
		const int TotalHeight = BxCore::Font::GetSize(Data.IMEfont, "O").h;
		const int X = (Draw.Width() - TotalWidth < 10)? Draw.Width() - 10 - TotalWidth : (Draw.Width() - TotalWidth) / 2;
		const int Y = (BeginY - TotalHeight) / 2;
		if(Data.IMEChars.GetLength() || Data.IMETails.GetLength())
		{
			Draw.Rectangle(FILL, XYWH(X, Y, CharsWidth + TailsWidth, TotalHeight), COLOR(255, 255, 255) >> OPACITY(64));
			BxCore::Font::SetSort(fontsort_pad7);
			BxCore::Font::Draw(Data.IMEfont, Data.IMEChars, XY(X, Y), WH(size::max, size::max), RGB32(0, 0, 0), 128);
			BxCore::Font::Draw(Data.IMEfont, Data.IMETails, XY(X + CharsWidth, Y), WH(size::max, size::max), RGB32(64, 48, 64), 128);
		}
		if((BxCore::Main::GetCurrentRenderCount() / 8) & 1)
			Draw.Rectangle(FILL, XYWH(X + TotalWidth - CursorWidth, Y, CursorWidth, TotalHeight), COLOR(64, 48, 64) >> OPACITY(192));
	}
}
