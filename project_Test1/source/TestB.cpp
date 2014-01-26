#include <BxScene.hpp>
#include "TestB.hpp"

FRAMEWORK_SCENE(TestBClass, "Test-B")

syseventresult OnEvent(TestBClass& Data, const sysevent& Event)
{
	if(Event.type == syseventtype_touch && Event.touch.type == systouchtype_down)
	{
		BxScene::SubRequest("Test-B");
		BxScene::AddRequest("Test-List");
		return syseventresult_done;
	}
	return syseventresult_pass;
}

sysupdateresult OnUpdate(TestBClass& Data)
{
	return sysupdateresult_do_render;
}

void OnRender(TestBClass& Data, BxDraw& Draw)
{
	Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), Draw.Height()), COLOR(128, 192, 128));
}
