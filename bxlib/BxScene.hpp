#pragma once
#include <BxString.hpp>
#include <BxDraw.hpp>
using namespace BxUtilGlobal;
using namespace BxDrawGlobal;

//! \brief 씬관리
class BxScene
{
public:
	/*!
	\brief 씬활성화 요청
	\param Name : 씬이름
	\param Order : 추가될 출력순번(-1은 최상단, -2는 GUI)
	\see SubRequest
	*/
	global_func void AddRequest(string Name, int Order = -1)
	{
		AllScene* FindedScene = __FindAllScene__(Name);
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", FindedScene != nullptr);
		BxAssert("BxScene<이미 활성화된 씬입니다>", __FindActiveScene__(Name) == nullptr);
		BxAssert("BxScene<해당 씬은 이미 요청중입니다>", __FindActiveSceneRequest__(FindedScene) == nullptr);
		// New Request(Add)
		ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_add, Order, BxNew_Param(ActiveScene, FindedScene));
		NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
		++__GetActiveSceneRequestCount__();
	}

	/*!
	\brief 활성씬 제거
	\param Name : 씬이름
	\see AddRequest
	*/
	global_func void SubRequest(string Name)
	{
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		BxAssert("BxScene<해당 이름으로 활성화된 씬이 없습니다>", FindedScene != nullptr);
		BxAssert("BxScene<해당 씬은 이미 요청중입니다>", __FindActiveSceneRequest__(FindedScene) == nullptr);
		// Find Loop
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while(OneActiveScene->Scene != FindedScene)
			OneActiveScene = OneActiveScene->Next;
		// New Request(Sub)
		ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_sub, 0, OneActiveScene);
		NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
		++__GetActiveSceneRequestCount__();
	}

	/*!
	\brief 활성씬 출력순번변경
	\param Name : 씬이름
	\param Order : 변경될 출력순번
	*/
	global_func void SetRequest(string Name, uint Order)
	{
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		BxAssert("BxScene<해당 이름으로 활성화된 씬이 없습니다>", FindedScene != nullptr);
		BxAssert("BxScene<해당 씬은 이미 요청중입니다>", __FindActiveSceneRequest__(FindedScene) == nullptr);
		// Find Loop
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while(OneActiveScene->Scene != FindedScene)
			OneActiveScene = OneActiveScene->Next;
		// New Request(Set)
		ActiveSceneRequest* NewActiveSceneRequest = BxNew_Param(ActiveSceneRequest, sysrequest_set, (int) Order, OneActiveScene);
		NewActiveSceneRequest->Next = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = NewActiveSceneRequest;
		++__GetActiveSceneRequestCount__();
	}

	typedef syseventresult (*EventCB)(void* Data, const sysevent& Event);
	typedef sysupdateresult (*UpdateCB)(void* Data);
	typedef void (*RenderCB)(void* Data, BxDraw& Draw);
	typedef void* (*GetDataCB)(sysmethod Mode);

	/// @cond SECTION_NAME
	class AllScene
	{
	public:
		string const Name;
		const EventCB Event;
		const UpdateCB Update;
		const RenderCB Render;
		const GetDataCB GetData;
		AllScene* Next;
		AllScene(string name, EventCB event, UpdateCB update, RenderCB render, GetDataCB getdata)
			: Name(name), Event(event), Update(update), Render(render), GetData(getdata), Next(nullptr) {}
		~AllScene() {BxDelete(Next);}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class ActiveScene
	{
	public:
		AllScene* _ref_ Scene;
		bool EnableKey;
		bool EnableTouch;
		ActiveScene* Next;
		ActiveScene(AllScene* _ref_ layer) : Scene(layer), EnableKey(false), EnableTouch(false), Next(nullptr) {}
		~ActiveScene() {BxDelete(Next);}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class ActiveSceneRequest
	{
	public:
		const sysrequest Type;
		const int Option;
		ActiveScene* _ref_ Data;
		ActiveSceneRequest* Next;
		ActiveSceneRequest(sysrequest type, int option, ActiveScene* _ref_ data) : Type(type), Option(option), Data(data), Next(nullptr) {}
		~ActiveSceneRequest() {BxDelete(Next);}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class EventQueue
	{
	public:
		sysevent Event;
		syseventset Set;
		EventQueue* Next;
		EventQueue() : Next(nullptr) {}
		~EventQueue() {delete Next;}
		global_func EventQueue* Top(bool doflip = false)
		{
			global_data EventQueue _Top[2];
			global_data int Focusing = 0;
			EventQueue* Result = &_Top[Focusing];
			if(doflip) Focusing = 1 - Focusing;
			return Result;
		}
	};
	/// @endcond

	/*!
	\brief 활성씬 수량얻기
	*/
	global_func uint GetCount()
	{
		return (uint)(__GetActiveSceneAllCount__() - __GetActiveSceneGUICount__());
	}

	/*!
	\brief 활성씬 출력순번얻기
	\param Name : 씬이름
	\return 출력순번
	*/
	global_func uint GetOrder(string Name)
	{
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		uint Count = (uint) __GetActiveSceneAllCount__() - 1;
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
		{
			if(BxUtilGlobal::StrCmp(OneActiveScene->Scene->Name, Name) == same)
				return Count;
			--Count;
		}
		return 0;
	}

	/*!
	\brief 해당출력순번의 활성씬 이름얻기
	\param Order : 출력순번
	\return 씬이름
	*/
	global_func string GetName(uint Order)
	{
		BxAssert("BxScene<검색할 씬의 순번이 너무 큽니다>", Order < (uint) __GetActiveSceneAllCount__());
		uint Count = (uint) __GetActiveSceneAllCount__() - 1;
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
		{
			if(Count == Order)
				return OneActiveScene->Scene->Name;
			--Count;
		}
		return nullptr;
	}

	/*!
	\brief 해당 씬의 활성화 상태얻기
	\param Name : 씬이름
	\return 활성화여부
	*/
	global_func bool IsActivity(string Name)
	{
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		return (FindedScene != nullptr);
	}

	/*!
	\brief 활성씬의 데이터얻기
	\param Name : 씬이름
	\return 데이터
	*/
	global_func void* GetData(string Name)
	{
		BxAssert("BxScene<해당 이름의 씬은 존재하지 않습니다>", __FindAllScene__(Name) != nullptr);
		AllScene* FindedScene = __FindActiveScene__(Name);
		BxAssert("BxScene<해당 이름으로 활성화된 씬이 없습니다>", FindedScene != nullptr);
		return FindedScene->GetData(sysmethod_get);
	}

	/// @cond SECTION_NAME
	global_func bool __Create__(string Name, EventCB Event, UpdateCB Update, RenderCB Render, GetDataCB GetData)
	{
		BxAssert("BxScene<같은 씬이름을 가진 파일이 이미 존재합니다>", __FindAllScene__(Name) == nullptr);
		AllScene* NewScene = BxNew_Param(AllScene, Name, Event, Update, Render, GetData);
		NewScene->Next = __GetAllScene__().Next;
		__GetAllScene__().Next = NewScene;
		return true;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func void __AddEvent__(sysevent& Event, syseventset Set)
	{
		EventQueue* NewEvent = new EventQueue();
		BxCore::Util::MemMove(&NewEvent->Event, &Event, sizeof(sysevent));
		BxCore::Util::MemMove(&NewEvent->Set, &Set, sizeof(syseventset));
		EventQueue* CurNode = EventQueue::Top();
		while(CurNode->Next) CurNode = CurNode->Next;
		CurNode->Next = NewEvent;
	}

	/// @cond SECTION_NAME
	global_func bool __OnEvent__(BxDraw& Draw)
	{
		bool DoEvent = false;
		EventQueue* TopNode = EventQueue::Top(true);
		EventQueue* CurNode = TopNode;
		while(CurNode = CurNode->Next)
		{
			DoEvent = true;
			sysevent& Event = CurNode->Event;
			syseventset Set = CurNode->Set;
			bool NeedRecall = true;
			while(NeedRecall)
			{
				NeedRecall = false;
				switch(Event.type)
				{
				case syseventtype_touch:
					//if(Event.touch.id == 0)
					{
						int SelectedButton = -1;
						if(Event.touch.type == systouchtype_down)
						{
							for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
							{
								if(Event.touch.x < Draw.ButtonEvent[i].Field.l || Event.touch.y < Draw.ButtonEvent[i].Field.t ||
									Draw.ButtonEvent[i].Field.r <= Event.touch.x || Draw.ButtonEvent[i].Field.b <= Event.touch.y)
									continue;
								SelectedButton = i;
								Draw.CapturedButtonEvent.SetEvent(Draw.ButtonEvent[i].Name, Draw.ButtonEvent[i].Field);
								break;
							}
						}
						else if(*Draw.CapturedButtonEvent.Name)
						{
							for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
							{
								if(BxUtilGlobal::StrCmp(Draw.CapturedButtonEvent.Name, Draw.ButtonEvent[i].Name) != same)
									continue;
								SelectedButton = i;
								break;
							}
							if(Event.touch.type != systouchtype_move)
							{
								Draw.CapturedButtonEvent.DeleteEvent();
								NeedRecall = true;
							}
						}
						if(-1 < SelectedButton)
						{
							Event.type = syseventtype_button;
							switch(Event.touch.type)
							{
							case systouchtype_down: Event.button.type = sysbuttontype_down; break;
							case systouchtype_move: Event.button.type = sysbuttontype_drag; break;
							case systouchtype_up:
							case systouchtype_cancel: Event.button.type = sysbuttontype_up; break;
							}
							Event.button.name = Draw.ButtonEvent[SelectedButton].Name;
							Event.button.field = Draw.ButtonEvent[SelectedButton].Field;
							Event.button.x = Event.touch.x - Draw.ButtonEvent[SelectedButton].Field.l;
							Event.button.y = Event.touch.y - Draw.ButtonEvent[SelectedButton].Field.t;
						}
					}
					break;
				case syseventtype_button:
					if(Event.button.type == sysbuttontype_up)
					{
						for(int i = Draw.ButtonLength - 1; 0 <= i; --i)
						{
							if(Event.touch.x < Draw.ButtonEvent[i].Field.l || Event.touch.y < Draw.ButtonEvent[i].Field.t ||
								Draw.ButtonEvent[i].Field.r <= Event.touch.x || Draw.ButtonEvent[i].Field.b <= Event.touch.y)
								continue;
							if(BxUtilGlobal::StrCmp(Event.button.name, Draw.ButtonEvent[i].Name) != same)
							{
								Event.button.type = sysbuttontype_drop;
								Event.button.name = Draw.ButtonEvent[i].Name;
								Event.button.field = Draw.ButtonEvent[i].Field;
								Event.button.x = Event.touch.x - Draw.ButtonEvent[i].Field.l;
								Event.button.y = Event.touch.y - Draw.ButtonEvent[i].Field.t;
							}
							break;
						}
						if(Event.button.type != sysbuttontype_drop)
						{
							NeedRecall = false;
							continue;
						}
					}
					break;
				}
				// 이벤트처리
				ActiveScene* OneActiveScene = &__GetActiveScene__();
				while((OneActiveScene = OneActiveScene->Next) != nullptr)
				{
					switch(Event.type)
					{
					case syseventtype_init: break;
					case syseventtype_quit: break;
					case syseventtype_resize: break;
					case syseventtype_key:
						if(Set == syseventset_need_enable && OneActiveScene->EnableKey == false) continue;
						if(Set == syseventset_do_enable) OneActiveScene->EnableKey = true;
						break;
					case syseventtype_touch:
					case syseventtype_button:
						if(Set == syseventset_need_enable && OneActiveScene->EnableTouch == false) continue;
						if(Set == syseventset_do_enable) OneActiveScene->EnableTouch = true;
						break;
					}
					if(OneActiveScene->Scene->Event(OneActiveScene->Scene->GetData(sysmethod_get), Event) == syseventresult_done)
					{
						BxAssert("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_init);
						BxAssert("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_quit);
						BxAssert("BxScene<해당 이벤트는 독점할 수 없습니다>", Event.type != syseventtype_resize);
						while((OneActiveScene = OneActiveScene->Next) != nullptr)
						{
							switch(Event.type)
							{
							case syseventtype_init: break;
							case syseventtype_quit: break;
							case syseventtype_resize: break;
							case syseventtype_key:
								OneActiveScene->EnableKey = false;
								break;
							case syseventtype_touch:
							case syseventtype_button:
								OneActiveScene->EnableTouch = false;
								break;
							}
						}
						break;
					}
				}
				__UpdateActiveScene__();
			}
		}
		delete TopNode->Next;
		TopNode->Next = nullptr;
		return DoEvent;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func bool __OnEventUpdateRender__(BxDraw& Draw)
	{
		// Event
		sysupdateresult UpdateResult = (BxScene::__OnEvent__(Draw))? sysupdateresult_do_render : sysupdateresult_no_render_fixed_frame;
		// Update
		ActiveScene* OneActiveScene = &__GetActiveScene__();
		while((OneActiveScene = OneActiveScene->Next) != nullptr)
			UpdateResult = (sysupdateresult) (UpdateResult | OneActiveScene->Scene->Update(OneActiveScene->Scene->GetData(sysmethod_get)));
		// Render
		const bool DoRender = (UpdateResult & sysupdateresult_do_render_fixed_frame) == sysupdateresult_do_render_fixed_frame;
		if(DoRender)
		{
			if((UpdateResult & sysupdateresult_do_render) == sysupdateresult_do_render)
				BxCore::Main::PullFrameDelay();
			const int ActiveSceneCount = __GetActiveSceneAllCount__();
			for(int i = ActiveSceneCount; 0 < i; --i)
			{
				ActiveScene* OneActiveScene = &__GetActiveScene__();
				for(int j = 0; j < i; ++j) OneActiveScene = OneActiveScene->Next;
				if(BxCore::Surface::SetMarginMode(__GetActiveSceneGUICount__() < i))
					Draw.AddStyle(CLIP(XYWH(
						BxCore::Main::GetCurrentGUIMargin().l,
						BxCore::Main::GetCurrentGUIMargin().t,
						BxCore::Surface::GetWidthHW(),
						BxCore::Surface::GetHeightHW())));
				OneActiveScene->Scene->Render(OneActiveScene->Scene->GetData(sysmethod_get), Draw);
				Draw.SubStyle(true);
			}
			BxCore::Surface::SetMarginMode(true);
		}
		else if((UpdateResult & sysupdateresult_no_render) == sysupdateresult_no_render)
			BxCore::Main::PushFrameDelay();
		__UpdateActiveScene__();
		return DoRender;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func void __UpdateActiveScene__()
	{
		// 요청정리
		const int ActiveSceneRequestCount = __GetActiveSceneRequestCount__();
		__GetActiveSceneRequestCount__() = 0;
		ActiveSceneRequest* OldActiveSceneRequest = __GetActiveSceneRequest__().Next;
		__GetActiveSceneRequest__().Next = nullptr;
		// 요청실행
		sysevent Event;
		BxCore::Util::MemSet(&Event, 0, sizeof(sysevent));
		for(int i = ActiveSceneRequestCount - 1; 0 <= i; --i)
		{
			ActiveSceneRequest* OneActiveSceneRequest = OldActiveSceneRequest;
			for(int j = 0; j < i; ++j) OneActiveSceneRequest = OneActiveSceneRequest->Next;
			switch(OneActiveSceneRequest->Type)
			{
			case sysrequest_null: break;
			case sysrequest_add:
				{
					// Add
					ActiveScene* NewActiveScene = OneActiveSceneRequest->Data;
					const int Order = (0 <= OneActiveSceneRequest->Option)? __GetActiveSceneAllCount__() - OneActiveSceneRequest->Option
						: ((OneActiveSceneRequest->Option == -2)? 0 : __GetActiveSceneGUICount__());
					BxAssert("BxScene<추가할 씬의 순번이 너무 큽니다>", 0 <= Order);
					ActiveScene* PrevNewActiveScene = &__GetActiveScene__();
					for(int i = 0; i < Order; ++i)
						PrevNewActiveScene = PrevNewActiveScene->Next;
					NewActiveScene->Next = PrevNewActiveScene->Next;
					PrevNewActiveScene->Next = NewActiveScene;
					++__GetActiveSceneAllCount__();
					if(OneActiveSceneRequest->Option == -2)
						++__GetActiveSceneGUICount__();
					// Scene Init(SceneData Create)
					Event.type = syseventtype_init;
					Event.init.w = BxCore::Surface::GetWidth();
					Event.init.h = BxCore::Surface::GetHeight();
					OneActiveSceneRequest->Data->Scene->Event(OneActiveSceneRequest->Data->Scene->GetData(sysmethod_get), Event);
				}
				break;
			case sysrequest_sub:
				{
					// Scene Quit
					Event.type = syseventtype_quit;
					OneActiveSceneRequest->Data->Scene->Event(OneActiveSceneRequest->Data->Scene->GetData(sysmethod_get), Event);
					// SceneData Remove
					OneActiveSceneRequest->Data->Scene->GetData(sysmethod_remove);
					// Sub
					ActiveScene* PrevDeleteActiveScene = &__GetActiveScene__();
					int Order = 0;
					while(PrevDeleteActiveScene->Next != OneActiveSceneRequest->Data)
					{
						PrevDeleteActiveScene = PrevDeleteActiveScene->Next;
						++Order;
					}
					ActiveScene* DeleteActiveScene = OneActiveSceneRequest->Data;
					PrevDeleteActiveScene->Next = DeleteActiveScene->Next;
					DeleteActiveScene->Next = nullptr;
					BxDelete(DeleteActiveScene);
					--__GetActiveSceneAllCount__();
					if(Order < __GetActiveSceneGUICount__())
						--__GetActiveSceneGUICount__();
				}
				break;
			case sysrequest_set:
				{
					// Set
					ActiveScene* CurActiveScene = OneActiveSceneRequest->Data;
					const int Order = __GetActiveSceneAllCount__() - OneActiveSceneRequest->Option;
					BxAssert("BxScene<추가할 씬의 순번이 너무 큽니다>", 0 <= Order);
					ActiveScene* PrevSetActiveScene = &__GetActiveScene__();
					for(int i = 0; i < Order; ++i)
						PrevSetActiveScene = PrevSetActiveScene->Next;
					if(PrevSetActiveScene != CurActiveScene && PrevSetActiveScene->Next != CurActiveScene)
					{
						ActiveScene* PrevCurActiveScene = &__GetActiveScene__();
						while(PrevCurActiveScene->Next != CurActiveScene)
							PrevCurActiveScene = PrevCurActiveScene->Next;
						PrevCurActiveScene->Next = CurActiveScene->Next;
						CurActiveScene->Next = PrevSetActiveScene->Next;
						PrevSetActiveScene->Next = CurActiveScene;
					}
				}
				break;
			}
		}
		BxDelete(OldActiveSceneRequest);
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func AllScene* __FindAllScene__(string Name)
	{
		AllScene* FindedScene = &__GetAllScene__();
		while((FindedScene = FindedScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(FindedScene->Name, Name) == same)
				return FindedScene;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func AllScene* __FindActiveScene__(string Name)
	{
		ActiveScene* FindedActiveScene = &__GetActiveScene__();
		while((FindedActiveScene = FindedActiveScene->Next) != nullptr)
			if(BxUtilGlobal::StrCmp(FindedActiveScene->Scene->Name, Name) == same)
				return FindedActiveScene->Scene;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	global_func ActiveScene* __FindActiveSceneRequest__(AllScene* Scene)
	{
		ActiveSceneRequest* FindedActiveSceneRequest = &__GetActiveSceneRequest__();
		while((FindedActiveSceneRequest = FindedActiveSceneRequest->Next) != nullptr)
			if(FindedActiveSceneRequest->Data->Scene == Scene)
				return FindedActiveSceneRequest->Data;
		return nullptr;
	}
	/// @endcond

	/// @cond SECTION_NAME
	// Scene
	global_func inline AllScene& __GetAllScene__() {global_data AllScene Begin("", nullptr, nullptr, nullptr, nullptr); return Begin;}
	global_func inline ActiveScene& __GetActiveScene__() {global_data ActiveScene Begin(nullptr); return Begin;}
	global_func inline int& __GetActiveSceneAllCount__() {global_data int AllCount = 0; return AllCount;}
	global_func inline int& __GetActiveSceneGUICount__() {global_data int GUICount = 0; return GUICount;}
	// Request
	global_func inline ActiveSceneRequest& __GetActiveSceneRequest__() {global_data ActiveSceneRequest Begin(sysrequest_null, 0, nullptr); return Begin;}
	global_func inline int& __GetActiveSceneRequestCount__() {global_data int RequestCount = 0; return RequestCount;}
	/// @endcond
};

#define FRAMEWORK_SCENE(DATA, STRING) \
	local_func macro syseventresult OnEvent(DATA& Data, const sysevent& Event); \
	local_func macro sysupdateresult OnUpdate(DATA& Data); \
	local_func macro void OnRender(DATA& Data, BxDraw& Draw); \
	local_func syseventresult __OnEvent__(void* Data, const sysevent& Event) {return OnEvent(*((DATA*) Data), Event);} \
	local_func sysupdateresult __OnUpdate__(void* Data) {return OnUpdate(*((DATA*) Data));} \
	local_func void __OnRender__(void* Data, BxDraw& Draw) {OnRender(*((DATA*) Data), Draw);} \
	local_func void* __GetData__(sysmethod Mode) \
	{ \
		global_data DATA* Data = nullptr; \
		if(Mode == sysmethod_remove) \
		{ \
			BxDelete(Data); \
			Data = nullptr; \
		} \
		else if(!Data) \
			Data = BxNew(DATA); \
		return (void*) Data; \
	} \
	local_data bool _Unknown = BxScene::__Create__(STRING, __OnEvent__, __OnUpdate__, __OnRender__, __GetData__);
