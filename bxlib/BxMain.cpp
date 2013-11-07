#include <BxScene.hpp>
#include <BxSingleton.hpp>
#include <BxTelex.hpp>

/// @cond SECTION_NAME
local_func bool Procedure();
local_data BxDraw* Draw = null;
local_data bool IsChildProcessRun = false;
callback_process ChildProcess = null;
/// @endcond

/// @cond SECTION_NAME
namespace BxCore
{
	namespace Main
	{
		int& ForMain_LetUpdateCount();
		int& ForMain_LetRenderCount();
		rect& ForMain_LetGUIMargin();
	}
	namespace System
	{
		void ForMain_CallbackRun();
	}
}
/// @endcond

/// @cond SECTION_NAME
int main()
{
	// Draw��ü �Ҵ�
	Draw = BxNew(BxDraw);

	// �̺�Ʈ���
	BxCore::Main::EventAttach(Draw);

	// Add GUIScene
	if(BxUtilGlobal::StrCmp(BxCore::System::GetOSName(), "WINDOWS") == same
		&& BxUtilGlobal::StrCmp(BxCore::System::GetArchitectureName(), "X86") == same
		&& BxCore::System::IsExistConfig(keyword_string, "Bx.Framework.GUIScene"))
	{
		BxScene::AddRequest(BxCore::System::GetConfigString("Bx.Framework.GUIScene", ""), -2);
		BxCore::Main::ForMain_LetGUIMargin().l = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Left", 0);
		BxCore::Main::ForMain_LetGUIMargin().t = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Top", 0);
		BxCore::Main::ForMain_LetGUIMargin().r = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Right", 0);
		BxCore::Main::ForMain_LetGUIMargin().b = BxCore::System::GetConfigNumber("Bx.Framework.GUIMargin.Bottom", 0);
	}

	// Add FirstScene
	BxScene::AddRequest(BxCore::System::GetConfigString("Bx.Framework.FirstScene", "FirstScene"));

	// Scene Init(SceneData Create)
	sysevent Event;
	Event.type = syseventtype_init;
	Event.init.w = BxCore::Surface::GetWidth();
	Event.init.h = BxCore::Surface::GetHeight();
	BxScene::__AddEvent__(Event, syseventset_null);
	BxScene::__OnEvent__(*Draw);

	uhuge CheckTime = BxCore::System::GetTimerMilliSecond();
	while(Procedure())
	{
		// �����ӽ���ó��
		const int SleepTime = BxCore::Main::GetCurrentFrameTime() - (int) (BxCore::System::GetTimerMilliSecond() - CheckTime) - 1;
		BxCore::System::Sleep(BxUtilGlobal::Max(0, SleepTime));
		while((int) (BxCore::System::GetTimerMilliSecond() - CheckTime) < BxCore::Main::GetCurrentFrameTime())
			BxCore::System::Sleep(1);
		CheckTime = BxCore::System::GetTimerMilliSecond();
	}

	// Scene Quit
	Event.type = syseventtype_quit;
	BxScene::__AddEvent__(Event, syseventset_null);
	BxScene::__OnEvent__(*Draw);
	// All SceneData Remove
	BxScene::AllScene* OneScene = &BxScene::__GetAllScene__();
	while((OneScene = OneScene->Next) != null)
		OneScene->GetData(sysmethod_remove);

	// �̺�Ʈ����
	BxCore::Main::EventDetach();

	// Draw��ü ����
	BxDelete(Draw);

	// ������ü ����
	BxSingleton::UnbindAll();

	// �޸�����Ȯ��
	while(BxMemory::DoMemLogger())
	{
		void* Test = BxAlloc(1); // �޸�����Ȯ����...
		BxFree(Test); // �޸�����Ȯ����...
		BxCore::System::Sleep(10);
	}
	return 0;
}
/// @endcond

/// @cond SECTION_NAME
local_func bool ProcedureWithYield(int sleep)
{
	global_data uhuge LastTime = BxCore::System::GetTimerMilliSecond();
	const uhuge LimitTime = BxCore::System::GetTimerMilliSecond() + sleep;
	if(BxCore::System::IsQuit()) return false;
	do
	{
		const uhuge CurTime = BxCore::System::GetTimerMilliSecond();
		if(LastTime + BxCore::Main::GetCurrentFrameTime() < CurTime)
		{
			LastTime = CurTime - BxUtilGlobal::Min((const int) (CurTime - (LastTime + BxCore::Main::GetCurrentFrameTime())),
				BxCore::Main::GetCurrentFrameTime());
			BxCore::System::Sleep(0);
			if(!Procedure()) return false;
		}
		if(sleep) BxCore::System::Sleep(1);
	}
	while(BxCore::System::GetTimerMilliSecond() < LimitTime);
	return true;
}
/// @endcond

/// @cond SECTION_NAME
bool Procedure()
{
	if(BxCore::System::IsQuit() || BxScene::__GetActiveSceneAllCount__() <= 0)
		return false;

	BxCore::Util::SetCallCount(0);

	const bool DoRender = BxScene::__OnEventUpdateRender__(*Draw);
	Draw->Flush();
	#ifdef __BX_OPENGL
	BxCore::OpenGL2D::Flush();
	#endif

	// �����ݹ�ó��
	BxCore::System::ForMain_CallbackRun();

	// �ڽ����μ����� ����
	if(!IsChildProcessRun && ChildProcess)
	{
		IsChildProcessRun = true;
		ChildProcess(ProcedureWithYield);
		ChildProcess = null;
		IsChildProcessRun = false;
	}

	// �ڷ��� Ʈ����ǽõ�
	BxTelex::HQ::TryTransaction();

	// ������ī��Ʈ ����
	++BxCore::Main::ForMain_LetUpdateCount();
	if(DoRender) ++BxCore::Main::ForMain_LetRenderCount();
	return true;
}
/// @endcond
