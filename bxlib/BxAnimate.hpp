#pragma once
#include <BxVarVector.hpp>
#include <BxImage.hpp>
#include <BxString.hpp>

//! \brief 메두사툴 애니메이션 운영
class BxAnimate
{
public:
	// 콜백
	typedef compare (*OnSort)(BxAnimate& anim, id_object objectA, id_object objectB);
	// 열거형
	enum {KEYWORD, FORM, OUTLINE, BEGIN_MAX};

public:
	/*!
	\brief 기본생성자
	*/
	BxAnimate()
	{
		Begin[KEYWORD] = Begin[FORM] = Begin[OUTLINE] = null;
	}

	/*!
	\brief 소멸자
	*/
	~BxAnimate()
	{
		ClearObjectAll();
	}

	/*!
	\brief 리소스폴더 경로지정
	\param path : 경로
	\return 저장된 경로스트링
	*/
	global_func BxString& SetResourcePath(string path = null)
	{
		global_data BxString Path("");
		if(path) Path = path;
		return Path;
	}

	/*!
	\brief 애니메이션 로드
	\param resource : 메모리리소스
	\return 로딩성공여부
	*/
	bool Load(byte* resource)
	{
		byte* ResourcePtr = (byte*) resource;
		if(BxUtilGlobal::LoadUint32(ResourcePtr) != *((uint*) "man1"))
			return false;
		Begin[KEYWORD] = Begin[FORM] = Begin[OUTLINE] = null;
		// Keyword
		for(int g = 0; g < 2; ++g)
		{
			const int NumKeyword = BxUtilGlobal::LoadInt16(ResourcePtr);
			Keyword[g].Reset();
			for(int i = 0; i < NumKeyword; ++i)
				Keyword[g][i].Load(&Begin[KEYWORD], ResourcePtr);
		}
		// Form
		const int NumForm = BxUtilGlobal::LoadInt16(ResourcePtr);
		Form.Reset();
		for(int i = 0; i < NumForm; ++i)
			Form[i].Load(&Begin[FORM], ResourcePtr);
		// Outline
		const int NumOutline = BxUtilGlobal::LoadInt16(ResourcePtr);
		Outline.Reset();
		for(int i = 0; i < NumOutline; ++i)
			Outline[i].Load(&Begin[OUTLINE], ResourcePtr);
		// Object
		const int NumObject = BxUtilGlobal::LoadInt16(ResourcePtr);
		Object.Reset();
		for(int i = 0; i < NumObject; ++i)
			Object[i].Load(&Begin[KEYWORD], ResourcePtr);
		return true;
	}

	/*!
	\brief 애니메이션 로드
	\param resource : 파일명
	\return 로딩성공여부
	*/
	bool Load(string filename)
	{
		id_file File = BxCore::File::Open(filename, "rb");
		int Length = BxCore::File::GetSize(File);
		byte* Resource = BxNew_Array(byte, Length);
		BxCore::File::Read(File, Resource, Length);
		BxCore::File::Close(File);
		bool Result = Load(Resource);
		BxDelete_Array(Resource);
		return Result;
	}

	/*!
	\brief 오브젝트 생존여부
	\param objectID : 오브젝트핸들
	\return 생존여부
	*/
	bool IsAliveObject(id_object objectID)
	{
		_Active* OneActive = (_Active*) objectID;
		return (OneActive)? OneActive->IsAlive : false;
	}

	/*!
	\brief 오브젝트 추가
	\param objectName : 오프젝트명
	\param x : 생성위치 X
	\param y : 생성위치 Y
	\return 오브젝트핸들
	\see SubObject
	*/
	id_object AddObject(string objectName, int x, int y)
	{
		BxAssert("BxAnimate", objectName);
		_Object* FindObject = null;
		for(int i = 0; i < Object.Length(); ++i)
			if(BxUtilGlobal::StrCmp(objectName, Object[i].Name) == same)
			{
				FindObject = &Object[i];
				break;
			}
		if(!FindObject) return null;
		_Active* NewActive = BxNew_Param(_Active, FindObject);
		NewActive->SetPos(x, y);
		NewActive->Play(&Begin[KEYWORD]);
		AliveActive.Insert(LAST, NewActive);
		return NewActive;
	}

	/*!
	\brief 오브젝트 제거
	\param objectID : 오브젝트핸들
	\see AddObject
	*/
	void SubObject(id_object _inout_ objectID)
	{
		_Active* OneActive = (_Active*) objectID;
		objectID = null;
		for(int i = 0; i < AliveActive.Length(); ++i)
		{
			if(&AliveActive[i] == OneActive)
			{
				BxAssert("BxAnimate<해당 오브젝트의 생존여부가 틀립니다>", OneActive->IsAlive);
				_Active* RemoveItem = null;
				AliveActive.Delete(i, &RemoveItem);
				BxDelete(RemoveItem);
				return;
			}
		}
		for(int i = 0; i < DeadActive.Length(); ++i)
		{
			if(&DeadActive[i] == OneActive)
			{
				BxAssert("BxAnimate<해당 오브젝트의 생존여부가 틀립니다>", !OneActive->IsAlive);
				_Active* RemoveItem = null;
				DeadActive.Delete(i, &RemoveItem);
				BxDelete(RemoveItem);
				return;
			}
		}
	}

	/*!
	\brief 모든 오브젝트 제거
	*/
	void ClearObjectAll()
	{
		for(int i = AliveActive.Length() - 1; 0 <= i; --i)
		{
			_Active* RemoveItem = null;
			AliveActive.Delete(i, &RemoveItem);
			BxDelete(RemoveItem);
		}
		for(int i = DeadActive.Length() - 1; 0 <= i; --i)
		{
			_Active* RemoveItem = null;
			DeadActive.Delete(i, &RemoveItem);
			BxDelete(RemoveItem);
		}
	}

	/*!
	\brief 오브젝트위치 얻기
	\param objectID : 오브젝트핸들
	\return 오브젝트위치
	*/
	inline point& ObjectPos(id_object objectID)
	{
		BxAssert("BxAnimate", objectID);
		_Active* OneActive = (_Active*) objectID;
		if(!OneActive->IsAlive)
		{
			global_data point NullPoint;
			NullPoint.x = 0;
			NullPoint.y = 0;
			return NullPoint;
		}
		return OneActive->GetPos();
	}

	/*!
	\brief 오브젝트에 이벤트전달
	\param objectID : 오브젝트핸들
	\param eventName : 이벤트명
	\param doInterrupt : 끼어들기여부
	*/
	void SendEvent(id_object objectID, string eventName, bool doInterrupt = false)
	{
		BxAssert("BxAnimate", objectID);
		BxAssert("BxAnimate", eventName);
		_Active* OneActive = (_Active*) objectID;
		if(!OneActive->IsAlive)
			return;
		for(int g = 0; g < 2; ++g)
		for(int i = 0; i < Keyword[g].Length(); ++i)
		{
			if(BxUtilGlobal::StrCmp(eventName, Keyword[g][i].Name) == same)
			{
				_Keyword* FindEvent = &Keyword[g][i];
				BxAssert("BxAnimate", FindEvent->Type == keyword_event);
				OneActive->SetEvent(FindEvent, doInterrupt);
				return;
			}
		}
	}

	/*!
	\brief 전체 오브젝트 그리기
	\param draw : Draw객체
	\param sortCB : 정렬용 콜백함수
	\see DefaultSort, NextAnimate
	*/
	void DrawAnimate(BxDraw& draw, OnSort sortCB = null)
	{
		if(sortCB)
		for(int i = 0; i < AliveActive.Length() - 1; ++i)
		{
			int iBest = i;
			for(int j = i + 1; j < AliveActive.Length(); ++j)
				if(sortCB(*this, &AliveActive[iBest], &AliveActive[j]) == b_is_less)
					iBest = j;
			if(iBest != i)
				AliveActive.SwapData(i, iBest);
		}
		for(int i = 0; i < AliveActive.Length(); ++i)
		{
			_Object::_Action::_Motion* OneMotion = AliveActive[i].GetMotion();
			if(OneMotion->Type != motion_frame)
			{
				if(AliveActive[i].Play(&Begin[KEYWORD]))
				{
					_Active* MoveItem = null;
					AliveActive.Delete(i, &MoveItem);
					if(MoveItem) MoveItem->IsAlive = false;
					DeadActive.Insert(LAST, MoveItem);
					--i;
					continue;
				}
			}
			if(OneMotion->Type == motion_frame)
			{
				_Object::_Action::_Motion::_Data::_Frame* OneFrame = (_Object::_Action::_Motion::_Data::_Frame*) OneMotion->Data.Frame;
				for(int l = 0; l < OneFrame->NumLayer; ++l)
				{
					_Object::_Action::_Motion::_Data::_Frame::_Layer* OneLayer = (_Object::_Action::_Motion::_Data::_Frame::_Layer*) &OneFrame->Layer[l];
					if(OneLayer->Type == layer_image)
					{
						_Object::_Action::_Motion::_Data::_Frame::_Layer::_Data::_Image* OneImage = (_Object::_Action::_Motion::_Data::_Frame::_Layer::_Data::_Image*) OneLayer->Data.Image;
						BxAssert("BxAnimate", OneImage->Form);
						BxAssert("BxAnimate", OneImage->Outline);
						const int X = AliveActive[i].GetPosX() + OneFrame->UnitSize.w * OneImage->Grid.x + OneImage->Detail.x;
						const int Y = AliveActive[i].GetPosY() + OneFrame->UnitSize.h * OneImage->Grid.y + OneImage->Detail.y;
						if(!OneImage->ShowTerms || (OneImage->ShowTerms->Type == keyword_check && OneImage->ShowTerms->Check) || (OneImage->ShowTerms->Type == keyword_radio && OneImage->ShowTerms->Radio))
						{
							const bool doFlip = ((OneImage->Pipe[2] % 2) == true);
							// 툴과 엔진의 드로잉모듈방식 차이
							int FlipXAdd = (doFlip)? OneImage->Form->GetForm()->GetArea()->hx * 2 - OneImage->Form->GetForm()->GetArea()->w : 0;
							switch(OneImage->Outline->CType)
							{
							case _Outline::_DataClass::ClassType_Point1:
								draw.Area(0, 0, BxDrawGlobal::FORM(OneImage->Form->GetForm())
									>> BxDrawGlobal::HOTSPOT(X + FlipXAdd, Y)
									>> BxDrawGlobal::ROTATE(OneImage->Pipe[2] / 2 * 256 + OneImage->Pipe[3], doFlip)
									>> BxDrawGlobal::OPACITY(OneImage->Pipe[6] * 255 / 32)
									>> BxDrawGlobal::SCALE(0x10000 * OneImage->Pipe[4] / 50, 0x10000 * OneImage->Pipe[4] / 50));
								break;
							case _Outline::_DataClass::ClassType_Point2:
								break;
							case _Outline::_DataClass::ClassType_Points:
								break;
							default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
							}
						}
					}
				}
			}
		}
	}

	/*!
	\brief 애니메이션 진행
	\see DrawAnimate
	*/
	void NextAnimate()
	{
		for(int i = 0; i < AliveActive.Length(); ++i)
		{
			if(AliveActive[i].Play(&Begin[KEYWORD]))
			{
				_Active* MoveItem = null;
				AliveActive.Delete(i, &MoveItem);
				if(MoveItem) MoveItem->IsAlive = false;
				DeadActive.Insert(LAST, MoveItem);
				--i;
			}
		}
	}

	/*!
	\brief OnSort정렬 기본제공함수
	\param anim : Anim객체
	\param objectA : 오브젝트A
	\param objectB : 오브젝트B
	\see DrawAnimate
	*/
	global_func compare DefaultSort(BxAnimate& anim, id_object objectA, id_object objectB)
	{
		if(anim.ObjectPos(objectB).y < anim.ObjectPos(objectA).y) return b_is_less;
		else if(anim.ObjectPos(objectA).y == anim.ObjectPos(objectB).y && anim.ObjectPos(objectB).x < anim.ObjectPos(objectA).x)
			return b_is_less;
		return same;
	}

	event SetEvent(string eventName, event* setValue = null)
	{
		BxAnimate::_Keyword* Node = BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 1);
		while(Node)
		{
			if(Node->Type == keyword_event && BxUtilGlobal::StrCmp(Node->Name, eventName) == same)
			{
				if(setValue)
				{
					Node->Event = BxUtilGlobal::StrFree(Node->Event);
					Node->Event = BxUtilGlobal::StrCpyWithAlloc(*setValue);
				}
				return Node->Event;
			}
			Node = Node->Link;
		}
		BxAssert("BxAnimate", Node);
		return Node->Event;
	}

	check SetCheck(string checkName, check* setValue = null)
	{
		BxAnimate::_Keyword* Node = BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 1);
		while(Node)
		{
			if(Node->Type == keyword_check && BxUtilGlobal::StrCmp(Node->Name, checkName) == same)
			{
				if(setValue)
					Node->Check = *setValue;
				return Node->Check;
			}
			Node = Node->Link;
		}
		BxAssert("BxAnimate", Node);
		return Node->Check;
	}

	radio SetRadio(string radioName, radio* setValue = null)
	{
		BxAnimate::_Keyword* Node = BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 1);
		while(Node)
		{
			if(Node->Type == keyword_radio && BxUtilGlobal::StrCmp(Node->Name, radioName) == same)
			{
				if(setValue)
					if(Node->Radio = *setValue)
						BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 0, null, Node);
				return Node->Radio;
			}
			Node = Node->Link;
		}
		BxAssert("BxAnimate", Node);
		return Node->Radio;
	}

	number SetNumber(string numberName, number* setValue = null)
	{
		BxAnimate::_Keyword* Node = BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 1);
		while(Node)
		{
			if(Node->Type == keyword_number && BxUtilGlobal::StrCmp(Node->Name, numberName) == same)
			{
				if(setValue)
					Node->Number = *setValue;
				return Node->Number;
			}
			Node = Node->Link;
		}
		BxAssert("BxAnimate", Node);
		return Node->Number;
	}

	string SetString(string stringName, string* setValue = null)
	{
		BxAnimate::_Keyword* Node = BxAnimate::_Keyword::GetKeyword(&Begin[KEYWORD], 1);
		while(Node)
		{
			if(Node->Type == keyword_string && BxUtilGlobal::StrCmp(Node->Name, stringName) == same)
			{
				if(setValue)
				{
					Node->String = BxUtilGlobal::StrFree(Node->String);
					Node->String = BxUtilGlobal::StrCpyWithAlloc(*setValue);
				}
				return Node->String;
			}
			Node = Node->Link;
		}
		BxAssert("BxAnimate", Node);
		return Node->String;
	}

	/*// 오브젝트 추가/삭제/위치정보/출력순서정보
	//////////////////identity_o AddObject(string objectName, string eventNameForAction);
	//////////////////void SubObject(identity_o objectID);
	//////////////////point& LetObjectPos(identity_o objectID);
	//////////////////uint& LetObjectOrder(identity_o objectID);

	// 이벤트 대기전달/강제전달
	//////////////void PostEvent(identity_o objectID, string eventName);
	//////////////void SendEvent(identity_o objectID, string eventName);

	// 영역 추가/삭제/영역정보/부모오브젝트정보
	identity_a AddArea(rect areaRect, string eventNameForTouch);
	void SubArea(identity_a areaID);
	rect& LetAreaRect(identity_a areaID);
	identity_o& LetAreaParentObject(identity_a areaID);

	// 키워드정보 이벤트/체크/라디오/숫자/단어
	string& LetEvent(identity_o objectID, string eventName);
	check& LetCheck(identity_o objectID, string checkName);
	radio& LetRadio(identity_o objectID, string radioName);
	number& LetNumber(identity_o objectID, string numberName);
	string& LetString(identity_o objectID, string stringName);

	// 콜백등록 사용자드로잉/이벤트/영역접촉
	void SetDrawCB(OnDraw drawCB);
	void SetEventCB(OnEvent eventCB);
	void SetContactCB(OnContact contactCB);

	// 애니메이션 출력/진행
	//////////////////void DrawAnimate(MuvDraw& draw);
	//////////////////uint NextAnimate();

	// 콜백
	typedef void (*OnDraw)(MuvDraw& draw, identity_a areaID);
	typedef void (*OnEvent)(identity_o objectID, string eventName);
	typedef void (*OnContact)(identity_a area1ID, identity_a area2ID, boolean isSeparate);*/

protected:
	/// @cond SECTION_NAME
	class _Keyword
	{
	public:
		keyword Type;
		string Name;
		int ParentID;
		union
		{
			event Event;
			check Check;
			radio Radio;
			number Number;
			string String;
		};
		_Keyword* _ref_ Link;
	public:
		_Keyword() : Type(keyword_null), Name(null), ParentID(0), Link(null) {}
		~_Keyword()
		{
			Name = BxUtilGlobal::StrFree(Name);
			switch(Type)
			{
			case keyword_event: Event = BxUtilGlobal::StrFree(Event); break;
			case keyword_string: String = BxUtilGlobal::StrFree(String); break;
			}
		}
	public:
		void Load(void** Begin, byte* _inout_ Resource)
		{
			Type = (keyword) BxUtilGlobal::LoadUint8(Resource);
			Name = BxUtilGlobal::LoadString(Resource);
			ParentID = BxUtilGlobal::LoadInt32(Resource);
			switch(Type)
			{
			case keyword_event: Event = BxUtilGlobal::LoadString(Resource); break;
			case keyword_check: Check = BxUtilGlobal::LoadUint8(Resource); break;
			case keyword_radio: Radio = BxUtilGlobal::LoadUint8(Resource); break;
			case keyword_number: Number = BxUtilGlobal::LoadInt32(Resource); break;
			case keyword_string: String = BxUtilGlobal::LoadString(Resource); break;
			default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
			}
			GetKeyword(Begin, 0, this);
		}
		global_func _Keyword* GetKeyword(void** Begin, int Index, _Keyword* Insert = null, _Keyword* SetRadio = null)
		{
			BxAnimate::_Keyword** KeywordBegin = (BxAnimate::_Keyword**) Begin;
			if(Insert)
			{
				Insert->Link = null;
				if(*KeywordBegin)
				{
					_Keyword* Node = *KeywordBegin;
					while(Node->Link) Node = Node->Link;
					Node->Link = Insert;
				}
				else *KeywordBegin = Insert;
				return null;
			}
			else if(SetRadio)
			{
				BxAssert("BxAnimate", SetRadio->Type == keyword_radio);
				_Keyword* Node = *KeywordBegin;
				while(Node)
				{
					if(Node->Type == keyword_radio && Node != SetRadio && Node->ParentID == SetRadio->ParentID)
						Node->Radio = false;
					Node = Node->Link;
				}
				return null;
			}
			if(Index-- <= 0) return null;
			_Keyword* Node = *KeywordBegin;
			while(0 < Index-- && Node) Node = Node->Link;
			BxAssert("BxAnimate", Node);
			return Node;
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class _Form
	{
	public:
		form Type;
		int Length;
		class _DataClass
		{
		public:
			enum ClassType {ClassType_Sprite, ClassType_Gradation, ClassType_Stencil};
			const ClassType CType;
			_DataClass(ClassType type) : CType(type) {}
			virtual BxDrawForm* GetForm()
			{
				BxAssert("BxAnimate<호출이 금지된 함수입니다>", false);
				return null;
			}
		};
		union _Data
		{
			class _Sprite : public _DataClass
			{
			public:
				uint FileID;
				BxImage* Image;
			public:
				_Sprite() : _DataClass(ClassType_Sprite), FileID(0), Image(null) {}
				~_Sprite() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Sprite); BxDelete(Image);}
			public:
				virtual BxDrawForm* GetForm()
				{
					if(!Image)
					{
						Image = BxNew(BxImage);
						BxString FileName;
						#ifdef __BX_PIXEL16
							FileName.Format("%s/%08x.b16", (string) SetResourcePath(), FileID);
							Image->Load(FileName, BxImage::B16);
						#else
							FileName.Format("%s/%08x.b32", (string) SetResourcePath(), FileID);
							Image->Load(FileName, BxImage::B32);
						#endif
						
					}
					return Image;
				}
			} *Sprite;
			class _Gradation : public _DataClass
			{
			public:
				uint Width;
				uint Height;
				int NumUnit;
				class _Unit
				{
				public:
					pixel_src Color;
					uint Pos;
				} *Unit;
				uint Angle1024;
			public:
				_Gradation() : _DataClass(ClassType_Gradation), Width(0), Height(0), NumUnit(0), Unit(null), Angle1024(0) {}
				~_Gradation() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Gradation); BxDelete_Array(Unit);}
			} *Gradation;
			class _Stencil : public _DataClass
			{
			public:
				uint Width;
				uint Height;
				byte* Bits;
			public:
				_Stencil() : _DataClass(ClassType_Stencil), Width(0), Height(0), Bits(null) {}
				~_Stencil() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Stencil); BxDelete_Array(Bits);}
			} *Stencil;
		} Data;
		_Form* _ref_ Link;
	public:
		_Form() : Type(form_null), Length(0), Link(null) {}
		~_Form()
		{
			switch(Type)
			{
			case form_sprite: BxDelete_Array(Data.Sprite); break;
			case form_gradation: BxDelete_Array(Data.Gradation); break;
			case form_stencil: BxDelete_Array(Data.Stencil); break;
			default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
			}
		}
	public:
		void Load(void** Begin, byte* _inout_ Resource)
		{
			Type = (form) BxUtilGlobal::LoadUint8(Resource);
			Length = BxUtilGlobal::LoadInt16(Resource);
			switch(Type)
			{
			case form_sprite:
				Data.Sprite = BxNew_Array(_Data::_Sprite, Length);
				for(int i = 0; i < Length; ++i)
					Data.Sprite[i].FileID = BxUtilGlobal::LoadUint32(Resource);
				break;
			case form_gradation:
				Data.Gradation = BxNew_Array(_Data::_Gradation, Length);
				for(int i = 0; i < Length; ++i)
				{
					Data.Gradation[i].Width = BxUtilGlobal::LoadUint16(Resource);
					Data.Gradation[i].Height = BxUtilGlobal::LoadUint16(Resource);
					Data.Gradation[i].NumUnit = BxUtilGlobal::LoadInt16(Resource);
					Data.Gradation[i].Unit = BxNew_Array(_Data::_Gradation::_Unit, Data.Gradation[i].NumUnit);
					for(int j = 0; j < Data.Gradation[i].NumUnit; ++j)
					{
						Data.Gradation[i].Unit[j].Color = BxDrawGlobal::Color565ToNativeSrc(BxUtilGlobal::LoadUint16(Resource));
						Data.Gradation[i].Unit[j].Pos = BxUtilGlobal::LoadUint8(Resource);
					}
					Data.Gradation[i].Angle1024 = BxUtilGlobal::LoadUint16(Resource);
				}
				break;
			case form_stencil:
				Data.Stencil = BxNew_Array(_Data::_Stencil, Length);
				for(int i = 0; i < Length; ++i)
				{
					Data.Stencil[i].Width = BxUtilGlobal::LoadUint16(Resource);
					Data.Stencil[i].Height = BxUtilGlobal::LoadUint16(Resource);
					const int NumBits = (Data.Stencil[i].Width * Data.Stencil[i].Height + 7) / 8;
					Data.Stencil[i].Bits = BxNew_Array(byte, NumBits);
					BxUtilGlobal::LoadBytes(Data.Stencil[i].Bits, Resource, NumBits);
				}
				break;
			default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
			}
			GetFormData(Begin, 0, this);
		}
		global_func _DataClass* GetFormData(void** Begin, int Index, _Form* Insert = null)
		{
			BxAnimate::_Form** FormBegin = (BxAnimate::_Form**) Begin;
			if(Insert)
			{
				Insert->Link = null;
				if(*FormBegin)
				{
					_Form* Node = *FormBegin;
					while(Node->Link) Node = Node->Link;
					Node->Link = Insert;
				}
				else *FormBegin = Insert;
				return null;
			}
			if(Index-- <= 0) return null;
			_Form* Node = *FormBegin;
			while(Node)
			{
				if(Index < (int) Node->Length)
				{
					switch(Node->Type)
					{
					case form_sprite: return &Node->Data.Sprite[Index];
					case form_gradation: return &Node->Data.Gradation[Index];
					case form_stencil: return &Node->Data.Stencil[Index];
					}
				}
				else Index -= Node->Length;
				Node = Node->Link;
			}
			BxAssert("BxAnimate", false);
			return null;
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class _Outline
	{
	public:
		outline Type;
		int Length;
		class _DataClass
		{
		public:
			enum ClassType {ClassType_Point1, ClassType_Point2, ClassType_Points};
			const ClassType CType;
			_DataClass(ClassType type) : CType(type) {}
		};
		union _Data
		{
			class _Point1 : public _DataClass
			{
			public:
				point Grid;
				point Detail;
				_Point1() : _DataClass(ClassType_Point1) {}
				~_Point1() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Point1);}
			} *Hotspot;
			class _Point2 : public _DataClass
			{
			public:
				point Grid[2];
				point Detail[2];
				_Point2() : _DataClass(ClassType_Point2) {}
				~_Point2() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Point2);}
			} *Rectangle, *Ellipse;
			class _Points : public _DataClass
			{
			public:
				int NumPoint;
				point* Grid;
				point* Detail;
			public:
				_Points() : _DataClass(ClassType_Points), NumPoint(0), Grid(null), Detail(null) {}
				~_Points()
				{
					BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Points);
					BxDelete_Array(Grid);
					BxDelete_Array(Detail);
				}
			} *StPolygon, *BzPolygon;
		} Data;
		_Outline* _ref_ Link;
	public:
		_Outline() : Type(outline_null), Length(0), Link(null) {}
		~_Outline()
		{
			switch(Type)
			{
			case outline_hotspot: BxDelete_Array(Data.Hotspot); break;
			case outline_rectangle: BxDelete_Array(Data.Rectangle); break;
			case outline_ellipse: BxDelete_Array(Data.Ellipse); break;
			case outline_st_polygon: BxDelete_Array(Data.StPolygon); break;
			case outline_bz_polygon: BxDelete_Array(Data.BzPolygon); break;
			default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
			}
		}
	public:
		void Load(void** Begin, byte* _inout_ Resource)
		{
			Type = (outline) BxUtilGlobal::LoadUint8(Resource);
			Length = BxUtilGlobal::LoadInt16(Resource);
			switch(Type)
			{
			case outline_hotspot:
				Data.Hotspot = BxNew_Array(_Data::_Point1, Length);
				for(int i = 0; i < Length; ++i)
				{
					Data.Hotspot[i].Grid.x = BxUtilGlobal::LoadInt16(Resource);
					Data.Hotspot[i].Grid.y = BxUtilGlobal::LoadInt16(Resource);
					Data.Hotspot[i].Detail.x = BxUtilGlobal::LoadInt16(Resource);
					Data.Hotspot[i].Detail.y = BxUtilGlobal::LoadInt16(Resource);
				}
				break;
			case outline_rectangle:
			case outline_ellipse:
				Data.Rectangle = BxNew_Array(_Data::_Point2, Length);
				for(int i = 0; i < Length; ++i)
				for(int j = 0; j < 2; ++j)
				{
					Data.Rectangle[i].Grid[j].x = BxUtilGlobal::LoadInt16(Resource);
					Data.Rectangle[i].Grid[j].y = BxUtilGlobal::LoadInt16(Resource);
					Data.Rectangle[i].Detail[j].x = BxUtilGlobal::LoadInt16(Resource);
					Data.Rectangle[i].Detail[j].y = BxUtilGlobal::LoadInt16(Resource);
				}
				break;
			case outline_st_polygon:
			case outline_bz_polygon:
				Data.StPolygon = BxNew_Array(_Data::_Points, Length);
				for(int i = 0; i < Length; ++i)
				{
					Data.StPolygon[i].NumPoint = BxUtilGlobal::LoadInt16(Resource);
					Data.StPolygon[i].Grid = BxNew_Array(point, Data.StPolygon[i].NumPoint);
					Data.StPolygon[i].Detail = BxNew_Array(point, Data.StPolygon[i].NumPoint);
					for(int j = 0; j < Data.StPolygon[i].NumPoint; ++j)
					{
						Data.StPolygon[i].Grid[j].x = BxUtilGlobal::LoadInt16(Resource);
						Data.StPolygon[i].Grid[j].y = BxUtilGlobal::LoadInt16(Resource);
						Data.StPolygon[i].Detail[j].x = BxUtilGlobal::LoadInt16(Resource);
						Data.StPolygon[i].Detail[j].y = BxUtilGlobal::LoadInt16(Resource);
					}
				}
				break;
			default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
			}
			GetOutlineData(Begin, 0, this);
		}
		global_func _DataClass* GetOutlineData(void** Begin, int Index, _Outline* Insert = null)
		{
			BxAnimate::_Outline** OutlineBegin = (BxAnimate::_Outline**) Begin;
			if(Insert)
			{
				if(*OutlineBegin)
				{
					Insert->Link = null;
					_Outline* Node = *OutlineBegin;
					while(Node->Link) Node = Node->Link;
					Node->Link = Insert;
				}
				else *OutlineBegin = Insert;
				return null;
			}
			if(Index-- <= 0) return null;
			_Outline* Node = *OutlineBegin;
			while(Node)
			{
				if(Index < (int) Node->Length)
				{
					switch(Node->Type)
					{
					case outline_hotspot: return &Node->Data.Hotspot[Index];
					case outline_rectangle: return &Node->Data.Rectangle[Index];
					case outline_ellipse: return &Node->Data.Ellipse[Index];
					case outline_st_polygon: return &Node->Data.StPolygon[Index];
					case outline_bz_polygon: return &Node->Data.BzPolygon[Index];
					default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
					}
				}
				else Index -= Node->Length;
				Node = Node->Link;
			}
			BxAssert("BxAnimate", false);
			return null;
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class _Object
	{
	public:
		string Name;
		int NumAction;
	public:
		_Object() : Name(null), NumAction(0), Action(null) {}
		~_Object() {Name = BxUtilGlobal::StrFree(Name); BxDelete_Array(Action);}
	public:
		void Load(void* BeginArray[3], byte* _inout_ Resource)
		{
			Name = BxUtilGlobal::LoadString(Resource);
			NumAction = BxUtilGlobal::LoadInt16(Resource);
			Action = BxNew_Array(_Action, NumAction);
			for(int i = 0; i < NumAction; ++i)
				Action[i].Load(BeginArray, Resource);
		}
	public:
		class _Action // 행동
		{
		public:
			string Name;
			BxAnimate::_Keyword* _ref_ Event;
			int NumMotion;
		public:
			_Action() : Name(null), Event(null), NumMotion(0), Motion(null) {}
			~_Action() {Name = BxUtilGlobal::StrFree(Name); BxDelete_Array(Motion);}
		public:
			void Load(void* BeginArray[3], byte* _inout_ Resource)
			{
				Name = BxUtilGlobal::LoadString(Resource);
				Event = BxAnimate::_Keyword::GetKeyword(&BeginArray[KEYWORD], BxUtilGlobal::LoadUint16(Resource));
				NumMotion = BxUtilGlobal::LoadInt16(Resource);
				Motion = BxNew_Array(_Motion, NumMotion);
				for(int i = 0; i < NumMotion; ++i)
					Motion[i].Load(BeginArray, Resource);
			}
		public:
			class _Motion // 동작
			{
			public:
				motion Type;
				class _DataClass
				{
				public:
					enum ClassType {ClassType_Frame, ClassType_Sleep, ClassType_Speed,
						ClassType_Vector, ClassType_Keyword, ClassType_Goto};
					const ClassType CType;
					_DataClass(ClassType type) : CType(type) {}
				};
				union _Data
				{
					class _Frame : public _DataClass // 장면
					{
					public:
						BxAnimate::_Keyword* _ref_ ShowEvent;
						BxAnimate::_Keyword* _ref_ TouchEvent;
						size UnitSize;
						int NumLayer;
						class _Layer
						{
						public:
							layer Type;
							union _Data
							{
								class _Image
								{
								public:
									BxAnimate::_Keyword* _ref_ ShowTerms;
									byte Pipe[8];
									BxAnimate::_Form::_DataClass* _ref_ Form;
									BxAnimate::_Outline::_DataClass* _ref_ Outline;
									point Grid;
									point Detail;
								public:
									void Load(void* BeginArray[3], byte* _inout_ Resource)
									{
										ShowTerms = BxAnimate::_Keyword::GetKeyword(&BeginArray[KEYWORD], BxUtilGlobal::LoadUint16(Resource));
										for(int i = 0; i < 8; ++i)
											Pipe[i] = BxUtilGlobal::LoadUint8(Resource);
										Form = BxAnimate::_Form::GetFormData(&BeginArray[FORM], BxUtilGlobal::LoadUint16(Resource));
										Outline = BxAnimate::_Outline::GetOutlineData(&BeginArray[OUTLINE], BxUtilGlobal::LoadUint16(Resource));
										BxAssert("BxAnimate", Form && Outline);
										Grid.x = BxUtilGlobal::LoadInt16(Resource);
										Grid.y = BxUtilGlobal::LoadInt16(Resource);
										Detail.x = BxUtilGlobal::LoadInt16(Resource);
										Detail.y = BxUtilGlobal::LoadInt16(Resource);
									}
								} *Image;
							} Data;
						public:
							_Layer() : Type(layer_null) {}
							~_Layer()
							{
								switch(Type)
								{
								case layer_image: BxPool<_Data::_Image>::FreeClass(Data.Image); break;
								}
							}
						public:
							void Load(void* BeginArray[3], byte* _inout_ Resource)
							{
								Type = (layer) BxUtilGlobal::LoadUint8(Resource);
								switch(Type)
								{
								case layer_image:
									Data.Image = BxPool<_Data::_Image>::MakeClass();
									Data.Image->Load(BeginArray, Resource);
									break;
								default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
								}
							}
						} *Layer;
					public:
						_Frame() : _DataClass(ClassType_Frame), ShowEvent(null), TouchEvent(null), NumLayer(0), Layer(null) {}
						~_Frame() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Frame); BxDelete_Array(Layer);}
					public:
						void Load(void* BeginArray[3], byte* _inout_ Resource)
						{
							ShowEvent = BxAnimate::_Keyword::GetKeyword(&BeginArray[KEYWORD], BxUtilGlobal::LoadUint16(Resource));
							TouchEvent = BxAnimate::_Keyword::GetKeyword(&BeginArray[KEYWORD], BxUtilGlobal::LoadUint16(Resource));
							UnitSize.w = BxUtilGlobal::LoadInt16(Resource);
							UnitSize.h = BxUtilGlobal::LoadInt16(Resource);
							NumLayer = BxUtilGlobal::LoadUint16(Resource);
							Layer = BxNew_Array(_Layer, NumLayer);
							for(int i = 0; i < NumLayer; ++i)
								Layer[i].Load(BeginArray, Resource);
						}
					} *Frame;
					class _Sleep : public _DataClass // 지시
					{
					public:
						uint Frame;
					public:
						_Sleep() : _DataClass(ClassType_Sleep), Frame(0) {}
						~_Sleep() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Sleep);}
					} *Sleep;
					class _Speed : public _DataClass // 지시
					{
					public:
						uint Frame;
					public:
						_Speed() : _DataClass(ClassType_Speed), Frame(1) {}
						~_Speed() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Speed);}
					} *Speed;
					class _Vector : public _DataClass // 지시
					{
					public:
						int X;
						int Y;
					public:
						_Vector() : _DataClass(ClassType_Vector), X(0), Y(0) {}
						~_Vector() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Vector);}
					} *Vector;
					class _Keyword : public _DataClass // 지시, 판단, 판단, 판단, 판단
					{
					public:
						BxAnimate::_Keyword* _ref_ Keyword;
						union _Compare
						{
							event Event;
							check Check;
							radio Radio;
							number Number;
							string String;
						} Compare;
					public:
						_Keyword() : _DataClass(ClassType_Keyword), Keyword(null) {}
						~_Keyword()
						{
							BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Keyword);
							if(Keyword)
							switch(Keyword->Type)
							{
							case keyword_event: Compare.Event = BxUtilGlobal::StrFree(Compare.Event); break;
							case keyword_string: Compare.String = BxUtilGlobal::StrFree(Compare.String); break;
							}
						}
					public:
						void Load(void** Begin, byte* _inout_ Resource, motion Type)
						{
							Keyword = BxAnimate::_Keyword::GetKeyword(Begin, BxUtilGlobal::LoadUint16(Resource));
							BxAssert("BxAnimate", Keyword);
							switch(Keyword->Type)
							{
							case keyword_event: Compare.Event = BxUtilGlobal::LoadString(Resource); break;
							case keyword_check: Compare.Check = BxUtilGlobal::LoadUint8(Resource); break;
							case keyword_radio: Compare.Radio = BxUtilGlobal::LoadUint8(Resource); break;
							case keyword_number:
								if(Type != motion_ifrate && Type != motion_ifcount)
									Compare.Number = BxUtilGlobal::LoadInt32(Resource);
								break;
							case keyword_string: Compare.String = BxUtilGlobal::LoadString(Resource); break;
							default: BxAssert("BxAnimate<잘못된 값입니다>", false); break;
							}
						}
					} *Set, *If, *IfFew, *IfRate, *IfCount;
					class _Goto : public _DataClass // 지시
					{
					public:
						int ActionID;
						int MotionID;
					public:
						_Goto() : _DataClass(ClassType_Goto), ActionID(0), MotionID(0) {}
						~_Goto() {BxAssert("BxAnimate<소멸되는 타입이 다릅니다>", CType == ClassType_Goto);}
					} *Goto;
				} Data;
			public:
				_Motion() : Type(motion_null) {}
				~_Motion()
				{
					switch(Type)
					{
					case motion_frame: BxPool<_Data::_Frame>::FreeClass(Data.Frame); break;
					case motion_sleep: BxPool<_Data::_Sleep>::FreeClass(Data.Sleep); break;
					case motion_speed: BxPool<_Data::_Speed>::FreeClass(Data.Speed); break;
					case motion_vector: BxPool<_Data::_Vector>::FreeClass(Data.Vector); break;
					case motion_set: BxPool<_Data::_Keyword>::FreeClass(Data.Set); break;
					case motion_if: BxPool<_Data::_Keyword>::FreeClass(Data.If); break;
					case motion_iffew: BxPool<_Data::_Keyword>::FreeClass(Data.IfFew); break;
					case motion_ifrate: BxPool<_Data::_Keyword>::FreeClass(Data.IfRate); break;
					case motion_ifcount: BxPool<_Data::_Keyword>::FreeClass(Data.IfCount); break;
					case motion_goto: BxPool<_Data::_Goto>::FreeClass(Data.Goto); break;
					}
				}
			public:
				void Load(void* BeginArray[3], byte* _inout_ Resource)
				{
					Type = (motion) BxUtilGlobal::LoadUint8(Resource);
					switch(Type)
					{
					case motion_frame:
						Data.Frame = BxPool<_Data::_Frame>::MakeClass();
						Data.Frame->Load(BeginArray, Resource);
						break;
					case motion_sleep:
						Data.Sleep = BxPool<_Data::_Sleep>::MakeClass();
						Data.Sleep->Frame = BxUtilGlobal::LoadUint16(Resource);
						break;
					case motion_speed:
						Data.Speed = BxPool<_Data::_Speed>::MakeClass();
						Data.Speed->Frame = BxUtilGlobal::LoadUint16(Resource);
						break;
					case motion_vector:
						Data.Vector = BxPool<_Data::_Vector>::MakeClass();
						Data.Vector->X = BxUtilGlobal::LoadInt16(Resource);
						Data.Vector->Y = BxUtilGlobal::LoadInt16(Resource);
						break;
					case motion_set:
						Data.Set = BxPool<_Data::_Keyword>::MakeClass();
						Data.Set->Load(&BeginArray[KEYWORD], Resource, Type);
						break;
					case motion_if:
						Data.If = BxPool<_Data::_Keyword>::MakeClass();
						Data.If->Load(&BeginArray[KEYWORD], Resource, Type);
						break;
					case motion_iffew:
						Data.IfFew = BxPool<_Data::_Keyword>::MakeClass();
						Data.IfFew->Load(&BeginArray[KEYWORD], Resource, Type);
						break;
					case motion_ifrate:
						Data.IfRate = BxPool<_Data::_Keyword>::MakeClass();
						Data.IfRate->Load(&BeginArray[KEYWORD], Resource, Type);
						break;
					case motion_ifcount:
						Data.IfCount = BxPool<_Data::_Keyword>::MakeClass();
						Data.IfCount->Load(&BeginArray[KEYWORD], Resource, Type);
						break;
					case motion_goto:
						Data.Goto = BxPool<_Data::_Goto>::MakeClass();
						Data.Goto->ActionID = BxUtilGlobal::LoadInt16(Resource);
						Data.Goto->MotionID = BxUtilGlobal::LoadInt16(Resource);
						break;
					}
				}
			} *Motion;
		} *Action;
	};
	/// @endcond

	/// @cond SECTION_NAME
	class _Active
	{
	protected:
		_Object* _ref_ Object;
		BxAnimate::_Keyword* _ref_ EventCur;
		BxAnimate::_Keyword* _ref_ EventNext;
		int ActionID;
		int MotionID;
		int CurrentAction;
		int CurrentMotion;
		point CurrentPos;
		uint SpeedFrame;
		uint SleepCount;
		point VectorValue;
		bool IsIfDone;
		class _GotoStack
		{
		public:
			int ActionID;
			int MotionID;
		public:
			_GotoStack() : ActionID(0), MotionID(0) {}
			~_GotoStack() {}
			void Init(int action, int motion)
			{
				ActionID = action;
				MotionID = motion;
			}
		};
		BxVarVector<_GotoStack, 256> Goto;
	public:
		bool IsAlive;
	public:
		_Active() {BxAssert("BxAnimate<잘못된 호출입니다>", false);}
		_Active(_Object* object) : Object(object), IsAlive(true)
		{
			EventCur = Object->Action[0].Event;
			EventNext = null;
			ActionID = 0;
			MotionID = 0;
			CurrentAction = 0;
			CurrentMotion = 0;
			CurrentPos.x = 0;
			CurrentPos.y = 0;
			Init();
		}
		~_Active() {}
	protected:
		void Init()
		{
			SpeedFrame = 1;
			SleepCount = 0;
			VectorValue.x = 0;
			VectorValue.y = 0;
			IsIfDone = false;
			Goto.Reset();
		}
		bool Push()
		{
			if(!EventNext) return false;
			for(int i = 0; i < Object->NumAction; ++i)
				if(Object->Action[i].Event == EventNext)
				{
					EventCur = EventNext;
					EventNext = null;
					ActionID = i;
					MotionID = 0;
					Init();
					return true;
				}
			EventNext = null;
			return false;
		}
	public:
		inline void SetEvent(BxAnimate::_Keyword* event, bool doInterrupt = false) {EventNext = event; if(doInterrupt) Push();}
		inline void SetPos(int x, int y) {CurrentPos.x = x; CurrentPos.y = y;}
		inline point& GetPos() {return CurrentPos;}
		inline const int GetPosX() {return CurrentPos.x;}
		inline const int GetPosY() {return CurrentPos.y;}
		inline _Object::_Action::_Motion* GetMotion() {return &Object->Action[CurrentAction].Motion[CurrentMotion];}
		delete_me Play(void** Begin) // local-keyword개념도입
		{
			do
			{
				while(ActionID < Object->NumAction && MotionID < Object->Action[ActionID].NumMotion)
				{
					_Object::_Action::_Motion& OneMotion = Object->Action[ActionID].Motion[MotionID];
					switch(OneMotion.Type)
					{
					case motion_frame:
						if(SleepCount == 0)
						{
							SleepCount = SpeedFrame;
							CurrentAction = ActionID;
							CurrentMotion = MotionID;
							CurrentPos.x += VectorValue.x;
							CurrentPos.y += VectorValue.y;
						}
						if(0 < SleepCount)
						{
							if(--SleepCount == 0)
								++MotionID;
						}
						return false;
					case motion_sleep:
						if(SleepCount == 0)
							SleepCount = OneMotion.Data.Sleep->Frame * SpeedFrame;
						if(0 < SleepCount)
						{
							CurrentPos.x += VectorValue.x;
							CurrentPos.y += VectorValue.y;
							if(--SleepCount == 0)
								++MotionID;
							return false;
						}
						++MotionID;
						break;
					case motion_speed:
						SpeedFrame = OneMotion.Data.Speed->Frame;
						++MotionID;
						break;
					case motion_vector:
						VectorValue.x = OneMotion.Data.Vector->X;
						VectorValue.y = OneMotion.Data.Vector->Y;
						++MotionID;
						break;
					case motion_set:
						switch(OneMotion.Data.Set->Keyword->Type)
						{
						case keyword_event:
							OneMotion.Data.Set->Keyword->Event = BxUtilGlobal::StrFree(OneMotion.Data.Set->Keyword->Event);
							OneMotion.Data.Set->Keyword->Event = BxUtilGlobal::StrCpyWithAlloc(OneMotion.Data.Set->Compare.Event);
							break;
						case keyword_check:
							OneMotion.Data.Set->Keyword->Check = OneMotion.Data.Set->Compare.Check;
							break;
						case keyword_radio:
							if(OneMotion.Data.Set->Keyword->Radio = OneMotion.Data.Set->Compare.Radio)
								BxAnimate::_Keyword::GetKeyword(Begin, 0, null, OneMotion.Data.Set->Keyword);
							break;
						case keyword_number:
							OneMotion.Data.Set->Keyword->Number = OneMotion.Data.Set->Compare.Number;
							break;
						case keyword_string:
							OneMotion.Data.Set->Keyword->String = BxUtilGlobal::StrFree(OneMotion.Data.Set->Keyword->String);
							OneMotion.Data.Set->Keyword->String = BxUtilGlobal::StrCpyWithAlloc(OneMotion.Data.Set->Compare.String);
							break;
						}
						++MotionID;
						break;
					case motion_if:
						if(!IsIfDone)
						{
							bool IsSuccess = false;
							switch(OneMotion.Data.If->Keyword->Type)
							{
							case keyword_event:
								BxAssert("BxAnimate", false);
								break;
							case keyword_check:
								IsSuccess = (OneMotion.Data.If->Keyword->Check == OneMotion.Data.If->Compare.Check);
								break;
							case keyword_radio:
								IsSuccess = (OneMotion.Data.If->Keyword->Radio == OneMotion.Data.If->Compare.Radio);
								break;
							case keyword_number:
								IsSuccess = (OneMotion.Data.If->Keyword->Number == OneMotion.Data.If->Compare.Number);
								break;
							case keyword_string:
								IsSuccess = (BxUtilGlobal::StrCmp(OneMotion.Data.If->Keyword->String, OneMotion.Data.If->Compare.String) == same);
								break;
							}
							if(IsSuccess)
							{
								IsIfDone = true;
								++MotionID;
								break;
							}
						}
						while(++MotionID < Object->Action[ActionID].NumMotion)
						{
							_Object::_Action::_Motion& FindMotion = Object->Action[ActionID].Motion[MotionID];
							if(motion_if <= FindMotion.Type && FindMotion.Type <= motion_endif) break;
						}
						break;
					case motion_iffew:
						if(!IsIfDone)
						{
							bool IsSuccess = false;
							switch(OneMotion.Data.IfFew->Keyword->Type)
							{
							case keyword_event:
								BxAssert("BxAnimate", false);
								break;
							case keyword_check:
								BxAssert("BxAnimate", false);
								break;
							case keyword_radio:
								BxAssert("BxAnimate", false);
								break;
							case keyword_number:
								IsSuccess = (OneMotion.Data.IfFew->Keyword->Number < OneMotion.Data.IfFew->Compare.Number);
								break;
							case keyword_string:
								BxAssert("BxAnimate", false);
								break;
							}
							if(IsSuccess)
							{
								IsIfDone = true;
								++MotionID;
								break;
							}
						}
						while(++MotionID < Object->Action[ActionID].NumMotion)
						{
							_Object::_Action::_Motion& FindMotion = Object->Action[ActionID].Motion[MotionID];
							if(motion_if <= FindMotion.Type && FindMotion.Type <= motion_endif) break;
						}
						break;
					case motion_ifrate:
						if(!IsIfDone)
						{
							bool IsSuccess = false;
							switch(OneMotion.Data.IfRate->Keyword->Type)
							{
							case keyword_event:
								BxAssert("BxAnimate", false);
								break;
							case keyword_check:
								BxAssert("BxAnimate", false);
								break;
							case keyword_radio:
								BxAssert("BxAnimate", false);
								break;
							case keyword_number:
								IsSuccess = ((((int) BxUtil::GetRandom()) % 100) < OneMotion.Data.IfRate->Keyword->Number);
								break;
							case keyword_string:
								BxAssert("BxAnimate", false);
								break;
							}
							if(IsSuccess)
							{
								IsIfDone = true;
								++MotionID;
								break;
							}
						}
						while(++MotionID < Object->Action[ActionID].NumMotion)
						{
							_Object::_Action::_Motion& FindMotion = Object->Action[ActionID].Motion[MotionID];
							if(motion_if <= FindMotion.Type && FindMotion.Type <= motion_endif) break;
						}
						break;
					case motion_ifcount:
						if(!IsIfDone)
						{
							bool IsSuccess = false;
							switch(OneMotion.Data.IfCount->Keyword->Type)
							{
							case keyword_event:
								BxAssert("BxAnimate", false);
								break;
							case keyword_check:
								BxAssert("BxAnimate", false);
								break;
							case keyword_radio:
								BxAssert("BxAnimate", false);
								break;
							case keyword_number:
								if(0 < OneMotion.Data.IfCount->Keyword->Number)
								{
									--OneMotion.Data.IfCount->Keyword->Number;
									IsSuccess = true;
								}
								break;
							case keyword_string:
								BxAssert("BxAnimate", false);
								break;
							}
							if(IsSuccess)
							{
								IsIfDone = true;
								++MotionID;
								break;
							}
						}
						while(++MotionID < Object->Action[ActionID].NumMotion)
						{
							_Object::_Action::_Motion& FindMotion = Object->Action[ActionID].Motion[MotionID];
							if(motion_if <= FindMotion.Type && FindMotion.Type <= motion_endif) break;
						}
						break;
					case motion_else:
						if(!IsIfDone)
						{
							IsIfDone = true;
							++MotionID;
						}
						else while(++MotionID < Object->Action[ActionID].NumMotion)
						{
							_Object::_Action::_Motion& FindMotion = Object->Action[ActionID].Motion[MotionID];
							if(motion_if <= FindMotion.Type && FindMotion.Type <= motion_endif) break;
						}
						break;
					case motion_endif:
						{
							IsIfDone = false;
							++MotionID;
						}
						break;
					case motion_goto:
						if(OneMotion.Data.Goto->ActionID < Object->NumAction && OneMotion.Data.Goto->MotionID < Object->Action[OneMotion.Data.Goto->ActionID].NumMotion)
						{
							Goto[LAST].Init(ActionID, MotionID);
							while(100 < Goto.Length()) Goto.Delete(0);
							ActionID = OneMotion.Data.Goto->ActionID;
							MotionID = OneMotion.Data.Goto->MotionID;
							IsIfDone = false;
						}
						else return true;
						break;
					case motion_return:
						if(0 < Goto.Length())
						{
							ActionID = Goto[END].ActionID;
							MotionID = Goto[END].MotionID;
							Goto.Delete(END);
						}
						++MotionID;
						break;
					case motion_end:
						if(!Push())
						{
							EventCur = null;
							return false;
						}
						break;
					case motion_yield:
						if(!Push())
							++MotionID;
						break;
					case motion_finish:
						EventCur = null;
						return true;
					default:
						++MotionID;
						break;
					}
				}
			}
			while(Push());
			return false;
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	void* _ref_ Begin[BEGIN_MAX];
	BxVarVector<_Keyword, 256> Keyword[2];
	BxVarVector<_Form, 256> Form;
	BxVarVector<_Outline, 256> Outline;
	BxVarVector<_Object, 256> Object;
	BxVarVector<_Active, 256> AliveActive;
	BxVarVector<_Active, 256> DeadActive;
	/// @endcond
};
