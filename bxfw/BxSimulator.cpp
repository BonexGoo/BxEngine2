#include <BxScene.hpp>
#include "BxSimulator.hpp"

FRAMEWORK_SCENE(BxSimulator, "BxSimulator")

syseventresult OnEvent(BxSimulator& Data, const sysevent& Event)
{
	if(Event.type == syseventtype_button)
	{
		if(StrCmp(Event.button.name, "SimulatorFrame:TitleBar") == same)
		{
			if(Event.button.type == sysbuttontype_down)
			{
				Data.NCWindowPos = BxCore::System::GetSimulatorWindowPos();
				Data.NCClickPos = BxCore::System::GetSimulatorCursorPos();
			}
			else if(Event.button.type == sysbuttontype_drag)
			{
				const point NewClickPos = BxCore::System::GetSimulatorCursorPos();
				BxCore::System::SetSimulatorWindowPos(Data.NCWindowPos.x + NewClickPos.x - Data.NCClickPos.x,
					Data.NCWindowPos.y + NewClickPos.y - Data.NCClickPos.y);
			}
			return syseventresult_done;
		}
		else if(StrCmp(Event.button.name, "SimulatorFrame:Min") == same)
		{
			if(Event.button.type == sysbuttontype_down)
			{
				Data.IsDown[0] = true;
			}
			else if(Event.button.type == sysbuttontype_up)
			{
				Data.IsDown[0] = false;
				if(0 <= Event.button.x && Event.button.x < Event.button.field.r - Event.button.field.l
					&& 0 <= Event.button.y && Event.button.y < Event.button.field.b - Event.button.field.t)
					BxCore::System::DoSimulatorMinimize();
			}
			return syseventresult_done;
		}
		else if(StrCmp(Event.button.name, "SimulatorFrame:Exit") == same)
		{
			if(Event.button.type == sysbuttontype_down)
			{
				Data.IsDown[1] = true;
			}
			else if(Event.button.type == sysbuttontype_up)
			{
				Data.IsDown[1] = false;
				if(0 <= Event.button.x && Event.button.x < Event.button.field.r - Event.button.field.l
					&& 0 <= Event.button.y && Event.button.y < Event.button.field.b - Event.button.field.t)
					BxCore::System::DoQuit();
			}
			return syseventresult_done;
		}
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(BxSimulator& Data)
{
	sysupdateresult Result = sysupdateresult_no_render_fixed_frame;
	if(0 < Data.LicenseFrameCount)
	{
		Result = (sysupdateresult) (Result | sysupdateresult_do_render);
		--Data.LicenseFrameCount;
	}
	const point WindowPos = BxCore::System::GetSimulatorWindowPos();
	const point CursorPos = BxCore::System::GetSimulatorCursorPos();
	Data.CursorPos.x = CursorPos.x - WindowPos.x;
	Data.CursorPos.y = CursorPos.y - WindowPos.y;
	// 업데이트 속도계산
	global_data uhuge LastTime = BxCore::System::GetTimeMilliSecond();
	global_data int UpdateTime[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	global_data int UpdateTimeFocus = -1;
	const uhuge ThisTime = BxCore::System::GetTimeMilliSecond();
	UpdateTime[UpdateTimeFocus = (UpdateTimeFocus + 1) % 16] = Max(1, (int)(ThisTime - LastTime));
	LastTime = ThisTime;
	const int AvgUpdateTime = Max(1, (UpdateTime[0] + UpdateTime[1] + UpdateTime[2] + UpdateTime[3] + UpdateTime[4]
		+ UpdateTime[5] + UpdateTime[6] + UpdateTime[7] + UpdateTime[8] + UpdateTime[9] + UpdateTime[10]
		+ UpdateTime[11] + UpdateTime[12] + UpdateTime[13] + UpdateTime[14] + UpdateTime[15]) / 16);
	const int AvgFrameTime = 1000 / AvgUpdateTime;
	const int AvgFrameRate = 100 * BxCore::Main::GetCurrentFrameTime(true) / AvgUpdateTime;
	if(Data.AvgFrameTime != AvgFrameTime || Data.AvgFrameRate != AvgFrameRate)
	{
		Result = (sysupdateresult) (Result | sysupdateresult_do_render_fixed_frame);
		Data.AvgFrameTime = AvgFrameTime;
		Data.AvgFrameRate = AvgFrameRate;
	}
	return Result;
}

void OnRender(BxSimulator& Data, BxDraw& Draw)
{
	// 상위
	const int TitleHeight = 38;
	try(Draw, CLIP(XYWH(0, 0, Draw.Width(), TitleHeight)), "SimulatorFrame:TitleBar")
	{
		for(int x = 0; x < Draw.Width(); x += 64)
			Draw.Area(x, 0, FORM(&Data.GUIImage, Data.GUIImageRect[1]));
		Draw.Area(0, 0, FORM(&Data.GUIImage, Data.GUIImageRect[0]));
		Draw.Area(Draw.Width() - Data.GUIImageRect[2].width(), 0, FORM(&Data.GUIImage, Data.GUIImageRect[2]));
		// 최소화버튼
		try(Draw, CLIP(XYWH(Draw.Width() - 39 - 27, 10, 27, 20)), "SimulatorFrame:Min")
		{
			const bool IsFocus = (BxDrawGlobal::_DrawOption::CurClipRect().r <= Data.CursorPos.x
				|| BxDrawGlobal::_DrawOption::CurClipRect().b <= Data.CursorPos.y
				|| Data.CursorPos.x < BxDrawGlobal::_DrawOption::CurClipRect().l
				|| Data.CursorPos.y < BxDrawGlobal::_DrawOption::CurClipRect().t)? false : true;
			Draw.Area(0, 0, FORM(&Data.GUIImage, Data.GUIImageRect[Data.IsDown[0]? 10 : (IsFocus? 9 : 8)]));
		}
		// 종료버튼
		try(Draw, CLIP(XYWH(Draw.Width() - 9 - 27, 10, 27, 20)), "SimulatorFrame:Exit")
		{
			const bool IsFocus = (BxDrawGlobal::_DrawOption::CurClipRect().r <= Data.CursorPos.x
				|| BxDrawGlobal::_DrawOption::CurClipRect().b <= Data.CursorPos.y
				|| Data.CursorPos.x < BxDrawGlobal::_DrawOption::CurClipRect().l
				|| Data.CursorPos.y < BxDrawGlobal::_DrawOption::CurClipRect().t)? false : true;
			Draw.Area(0, 0, FORM(&Data.GUIImage, Data.GUIImageRect[Data.IsDown[1]? 13 : (IsFocus? 12 : 11)]));
		}
	}
	// 프레임수
	BxCore::Font::SetSort(fontsort_pad6);
	BxCore::Font::Draw(Data.GUIFontMini, BxString("%dF, %d%%", nullptr, Data.AvgFrameTime, Data.AvgFrameRate),
		XY(Draw.Width() - Data.GUIImageRect[2].width() - BxCore::Main::GetCurrentGUIMargin().r - size::max, 0 - BxCore::Main::GetCurrentGUIMargin().t),
		WH(size::max, TitleHeight), RGB32(96, 64, 64));
	// 제목
	BxCore::Font::SetSort(fontsort_pad4);
	BxCore::Font::Draw(Data.GUIFont, BxString("%s.%s", nullptr, BxCore::System::GetConfigString("Bx.Currently.Title", "Untitle"), BxScene::GetName(BxScene::GetCount() - 1)),
		XY(Data.GUIImageRect[0].width() - BxCore::Main::GetCurrentGUIMargin().l, 0 - BxCore::Main::GetCurrentGUIMargin().t), WH(size::max, TitleHeight), RGB32(255, 0, 0));
	// 중간
	for(int y = TitleHeight; y < Draw.Height(); y += 64)
	{
		Draw.Area(0, y, FORM(&Data.GUIImage, Data.GUIImageRect[3]));
		Draw.Area(Draw.Width() - Data.GUIImageRect[4].width(), y, FORM(&Data.GUIImage, Data.GUIImageRect[4]));
	}
	// 아래
	for(int x = 0; x < Draw.Width(); x += 64)
		Draw.Area(x, Draw.Height() - Data.GUIImageRect[6].height(), FORM(&Data.GUIImage, Data.GUIImageRect[6]));
	Draw.Area(0, Draw.Height() - Data.GUIImageRect[5].height(), FORM(&Data.GUIImage, Data.GUIImageRect[5]));
	Draw.Area(Draw.Width() - Data.GUIImageRect[7].width(), Draw.Height() - Data.GUIImageRect[7].height(), FORM(&Data.GUIImage, Data.GUIImageRect[7]));

	if(0 < Data.LicenseFrameCount)
	try(Draw, OPACITY(Data.LicenseFrameCount * 0xFF / Data.LicenseFrameCountMax))
	{
		// 워터마크
		Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&Data.GUIWaterMark));
		// 라이센스
		Draw.Area(Draw.Width() / 2, Draw.Height() - 10, FORM(&Data.GUILicense));
	}
}
