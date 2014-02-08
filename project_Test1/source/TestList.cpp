#include <BxScene.hpp>
#include "TestList.hpp"

FRAMEWORK_SCENE(TestListClass, "Test-List")

local_data string ButtonID[TestListClass::ButtonLength] = {"A", "B"};
local_data string LayerName[TestListClass::ButtonLength] = {"Test-A", "Test-B"};

syseventresult OnEvent(TestListClass& Data, const sysevent& Event)
{
	if(Event.type == syseventtype_button)
	{
		for(int i = 0; i < TestListClass::ButtonLength; ++i)
		{
			if(StrCmp(Event.button.name, ButtonID[i]) == same)
			{
				if(Event.button.type == sysbuttontype_down)
				{
					Data.IsButtonPushed[i] = true;
					BxCore::Sound::PlayEffect(Data.sound1);
				}
				else if(Event.button.type == sysbuttontype_up && Data.IsButtonPushed[i])
				{
					Data.IsButtonPushed[i] = false;
					if(!(Event.button.x < 0 || Event.button.y < 0 ||
						Event.button.field.r - Event.button.field.l <= Event.button.x || Event.button.field.b - Event.button.field.t <= Event.button.y))
					{
						BxCore::Sound::PlayEffect(Data.sound2);
						BxScene::SubRequest("Test-List");
						BxScene::AddRequest(LayerName[i]);
					}
				}
				return syseventresult_done;
			}
		}
		if(StrCmp(Event.button.name, "Exit") == same)
		{
			if(Event.button.type == sysbuttontype_down)
			{
				Data.IsExitButtonPushed = true;
				BxCore::Sound::PlayEffect(Data.sound1);
			}
			else if(Event.button.type == sysbuttontype_up && Data.IsExitButtonPushed)
			{
				Data.IsExitButtonPushed = false;
				if(!(Event.button.x < 0 || Event.button.y < 0 ||
					Event.button.field.r - Event.button.field.l <= Event.button.x || Event.button.field.b - Event.button.field.t <= Event.button.y))
				{
					BxCore::Sound::PlayEffect(Data.sound2);
					BxCore::System::DoQuit();
				}
			}
		}
		return syseventresult_done;
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(TestListClass& Data)
{
	return sysupdateresult_do_render;
}

void OnRender(TestListClass& Data, BxDraw& Draw)
{
	BxTRY(Draw, COLOR(160, 160, 192))
	{
		Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), Draw.Height()));

		BxCore::Font::SetSort(fontsort_pad5);
		BxCore::Font::Draw(Data.font1, "TEST LIST", XY(0, 0), WH(Draw.Width(), 50), RGB32(255, 255, 255), 0xFF);
		Draw.Rectangle(FILL, XYWH(10 + 0, 50 + 0, Draw.Width() - 20 - 0, Draw.Height() - 50 - 10 - 0), COLOR(255, 255, 255, 50));
		Draw.Rectangle(FILL, XYWH(10 + 0, 50 + 0, Draw.Width() - 20 - 1, Draw.Height() - 50 - 10 - 1), COLOR(0, 0, 0, 50));
		Draw.Rectangle(FILL, XYWH(10 + 1, 50 + 1, Draw.Width() - 20 - 2, Draw.Height() - 50 - 10 - 2), COLOR(0, 0, 0, 10));

		const int ButtonWidth = 400;
		const int ButtonHeight = 40;
		const int PosYBegin = 50 + (Draw.Height() - 50 - 10 - ((ButtonHeight + 10) * TestListClass::ButtonLength - 10)) / 2;
		for(int i = 0; i < TestListClass::ButtonLength; ++i)
		{
			int PosX = (Draw.Width() - ButtonWidth) / 2, PosY = PosYBegin + (ButtonHeight + 10) * i;
			BxTRY(Draw, CLIP(XYWH(PosX, PosY, ButtonWidth, ButtonHeight)), ButtonID[i])
			{
				Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width() - 0, Draw.Height() - 0), (Data.IsButtonPushed[i])? COLOR(255, 255, 255, 50) : COLOR(0, 0, 0, 50));
				Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width() - 1, Draw.Height() - 1), (Data.IsButtonPushed[i])? COLOR(0, 0, 0, 50) : COLOR(255, 255, 255, 50));
				Draw.Rectangle(FILL, XYWH(1, 1, Draw.Width() - 2, Draw.Height() - 2), (Data.IsButtonPushed[i])? COLOR(224, 224, 224, 50) : COLOR(192, 192, 192, 50));
				BxCore::Font::SetSort(fontsort_pad5);
				BxCore::Font::Draw(Data.font2, ButtonID[i], XY(PosX + ((Data.IsButtonPushed[i])? 1 : 0), PosY + ((Data.IsButtonPushed[i])? 1 : 0)),
					WH(Draw.Width(), Draw.Height()), RGB32(0, 0, 0), 192);
			}
		}

		BxCore::Font::SetSort(fontsort_pad5);
		BxString Date("<>:[<A>, <A>]", BxARG(__DATE__, __TIME__));
		BxCore::Font::Draw(Data.font2, Date, XY(0, Draw.Height() - 10 - 30),
			WH(Draw.Width(), 30), RGB32(255, 255, 0), 128);

		BxTRY(Draw, CLIP(XYWH(Draw.Width() - 10 - 30, (50 - 30) / 2, 30, 30)), "Exit")
		{
			Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width() - 0, Draw.Height() - 0), (Data.IsExitButtonPushed)? COLOR(255, 255, 255, 50) : COLOR(0, 0, 0, 50));
			Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width() - 1, Draw.Height() - 1), (Data.IsExitButtonPushed)? COLOR(0, 0, 0, 50) : COLOR(255, 255, 255, 50));
			Draw.Rectangle(FILL, XYWH(1, 1, Draw.Width() - 2, Draw.Height() - 2), COLOR(255, 0, 0, 100));
		}
	}
}
