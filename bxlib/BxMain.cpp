#include <BxScene.hpp>
#include <BxSingleton.hpp>
#include <BxTelex.hpp>

/// @cond SECTION_NAME
local_func bool Procedure();
local_data BxDraw* Draw = nullptr;
local_data bool IsChildProcessRun = false;
extern callback_process ChildProcess;
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
	// Draw객체 할당
	Draw = BxNew(BxDraw);

	// 이벤트등록
	BxCore::Main::EventAttach(Draw);

	// Add GUIScene
	if(BxUtilGlobal::StrCmp(BxCore::System::GetOSName(), "WINDOWS") == same
		&& BxUtilGlobal::StrCmp(BxCore::System::GetArchName(), "X86") == same
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

	uhuge CheckTime = BxCore::System::GetTimeMilliSecond();
	while(Procedure())
	{
		// 프레임슬립처리
		const int SleepTime = BxCore::Main::GetCurrentFrameTime() - (int) (BxCore::System::GetTimeMilliSecond() - CheckTime) - 1;
		BxCore::System::Sleep(BxUtilGlobal::Max(0, SleepTime));
		while((int) (BxCore::System::GetTimeMilliSecond() - CheckTime) < BxCore::Main::GetCurrentFrameTime())
			BxCore::System::Sleep(1);
		CheckTime = BxCore::System::GetTimeMilliSecond();
	}

	// Scene Quit
	Event.type = syseventtype_quit;
	BxScene::__AddEvent__(Event, syseventset_null);
	BxScene::__OnEvent__(*Draw);
	// All SceneData Remove
	BxScene::AllScene* OneScene = &BxScene::__GetAllScene__();
	while((OneScene = OneScene->Next) != nullptr)
		OneScene->GetData(sysmethod_remove);

	// 이벤트해제
	BxCore::Main::EventDetach();

	// Draw객체 해제
	BxDelete(Draw);

	// 전역객체 해제
	BxSingleton::UnbindAll();

	// 메모리해제확인
	while(BxMemory::DoMemLogger())
	{
		void* Test = BxAlloc(1); // 메모리해제확인중...
		BxFree(Test); // 메모리해제확인중...
		BxCore::System::Sleep(10);
	}
	return 0;
}
/// @endcond

/// @cond SECTION_NAME
local_func bool ProcedureWithYield(int sleep)
{
	global_data uhuge LastTime = BxCore::System::GetTimeMilliSecond();
	const uhuge LimitTime = BxCore::System::GetTimeMilliSecond() + sleep;
	if(BxCore::System::IsQuit()) return false;
	do
	{
		const uhuge CurTime = BxCore::System::GetTimeMilliSecond();
		if(LastTime + BxCore::Main::GetCurrentFrameTime() < CurTime)
		{
			LastTime = CurTime - BxUtilGlobal::Min((const int) (CurTime - (LastTime + BxCore::Main::GetCurrentFrameTime())),
				BxCore::Main::GetCurrentFrameTime());
			BxCore::System::Sleep(0);
			if(!Procedure()) return false;
		}
		if(sleep) BxCore::System::Sleep(1);
	}
	while(BxCore::System::GetTimeMilliSecond() < LimitTime);
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

	// 통합콜백처리
	BxCore::System::ForMain_CallbackRun();

	// 자식프로세스의 실행
	if(!IsChildProcessRun && ChildProcess)
	{
		IsChildProcessRun = true;
		ChildProcess(ProcedureWithYield);
		ChildProcess = nullptr;
		IsChildProcessRun = false;
	}

	// 텔렉스 트랜잭션시도
	BxTelex::HQ::TryTransaction();

	// 프레임카운트 진행
	++BxCore::Main::ForMain_LetUpdateCount();
	if(DoRender) ++BxCore::Main::ForMain_LetRenderCount();
	return true;
}
/// @endcond
