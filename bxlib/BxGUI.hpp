#pragma once
#include <BxXml.hpp>
#include <BxVarMap.hpp>
#include <BxDraw.hpp>
#include <BxWebContent.hpp>
#include <BxImage.hpp>

//! \brief GUI객체 운영
class BxGUI
{
	enum
	{
		TAG_GUI = 0,
		TAG_LAYOUT,
		TAG_LAYOUT_SPLIT,
		TAG_LAYOUT_OUTLINE,
		TAG_LAYOUT_EXTRA,
		TAG_LAYOUT_VIEW,
		TAG_CONTENT,
		TAG_CONTENT_COLOR,
		TAG_CONTENT_EDGE,
		TAG_CONTENT_IMAGE,
		TAG_CONTENT_SPRITE,
		TAG_CONTENT_FONT,
		TAG_CONTENT_TEXT,
		TAG_CONTENT_BUTTON,
		TAG_CONTENT_EDITBOX
	};
	enum
	{
		ATTB_GUI_VERSION = 0,
		ATTB_LAYOUT_NAME = 0,
		ATTB_LAYOUT_SPLIT_NAME = 0,
		ATTB_LAYOUT_SPLIT_POS,
		ATTB_LAYOUT_SPLIT_SIZE,
		ATTB_LAYOUT_SPLIT_FILL,
		ATTB_LAYOUT_OUTLINE_NAME = 0,
		ATTB_LAYOUT_OUTLINE_SIZE,
		ATTB_LAYOUT_OUTLINE_FILL,
		ATTB_LAYOUT_EXTRA_NAME = 0,
		ATTB_LAYOUT_EXTRA_WIDTH,
		ATTB_LAYOUT_EXTRA_HEIGHT,
		ATTB_LAYOUT_EXTRA_FILL,
		ATTB_LAYOUT_VIEW_NAME = 0,
		ATTB_LAYOUT_VIEW_POSX,
		ATTB_LAYOUT_VIEW_POSY,
		ATTB_LAYOUT_VIEW_WIDTH,
		ATTB_LAYOUT_VIEW_HEIGHT,
		ATTB_LAYOUT_VIEW_SCROLL,
		ATTB_LAYOUT_VIEW_FILL,
		ATTB_CONTENT_NAME = 0,
		ATTB_CONTENT_COLOR_NAME = 0,
		ATTB_CONTENT_COLOR_ARGB,
		ATTB_CONTENT_EDGE_NAME = 0,
		ATTB_CONTENT_EDGE_COLORS,
		ATTB_CONTENT_IMAGE_NAME = 0,
		ATTB_CONTENT_IMAGE_URL,
		ATTB_CONTENT_SPRITE_NAME = 0,
		ATTB_CONTENT_SPRITE_IMAGES,
		ATTB_CONTENT_SPRITE_DELAY,
		ATTB_CONTENT_FONT_NAME = 0,
		ATTB_CONTENT_FONT_SIZE,
		ATTB_CONTENT_FONT_URL,
		ATTB_CONTENT_TEXT_NAME = 0,
		ATTB_CONTENT_TEXT_FORMAT,
		ATTB_CONTENT_TEXT_FONT,
		ATTB_CONTENT_TEXT_COLOR,
		ATTB_CONTENT_TEXT_VECTOR,
		ATTB_CONTENT_TEXT_LINE,
		ATTB_CONTENT_BUTTON_NAME = 0,
		ATTB_CONTENT_BUTTON_IMAGES,
		ATTB_CONTENT_BUTTON_EDGES,
		ATTB_CONTENT_BUTTON_EVENT,
		ATTB_CONTENT_BUTTON_LINK,
		ATTB_CONTENT_EDITBOX_NAME = 0,
		ATTB_CONTENT_EDITBOX_TEXT,
		ATTB_CONTENT_EDITBOX_COLORS
	};

public:
	global_func BxString& Domain() {global_data BxString _Domain; return _Domain;}
	global_func BxString& Subject() {global_data BxString _Subject; return _Subject;}
	global_func int& Version() {global_data int _Version; return _Version;}
	global_func bool& IsValidDownload() {global_data bool _IsValidDownload = true; return _IsValidDownload;}
	global_func callback_event& Callback() {global_data callback_event _Callback; return _Callback;}
	global_func bool DownloadCache(string url, BxWebContent _out_ Content, int _out_ FileSize, string _out_ FilePath, bool DoLoad)
	{
		BxString QueryArg("subject=%s&file=%s&version=%d", nullptr, (string) BxGUI::Subject(), url, BxGUI::Version());
		BxWebContent Web;
		if(Web.Query(BxGUI::Domain(), 80, "update/res_download_cdn.aspx", QueryArg))
		{
			string QueryResult = BxUtilGlobal::StrCpyWithAlloc((string) Web.GetBytes(), Web.GetTextLength());
			BxVar<BxString> QueryResults;
			BxString::ParseDivision(QueryResult, QueryResults);
			if(QueryResults.Length() == 3 && BxUtilGlobal::AtoI(QueryResults[0]) == 1)
			{
				FileSize = BxUtilGlobal::AtoI(QueryResults[1]);
				BxString FilePathString("update/%s", nullptr, (string) QueryResults[2]);
				FilePath = BxUtilGlobal::StrCpyWithAlloc(FilePathString);
				if(Content.Cache(BxGUI::Domain(), 80, FilePath, DoLoad))
				{
					QueryResult = BxUtilGlobal::StrFree(QueryResult);
					return true;
				}
				FilePath = BxUtilGlobal::StrFree(FilePath);
			}
			QueryResult = BxUtilGlobal::StrFree(QueryResult);
		}
		return false;
	}

public:
	BxGUI() {Init(-1);}
	virtual ~BxGUI() {Quit(-1);}
	void Init(int index)
	{
		CurSection = NoSection;
	}
	void Quit(int index)
	{
	}

public:
	global_func void GetParser(BxXml& XMLParser)
	{
		XMLParser.DefineTag(TAG_GUI, "GUI");
		XMLParser.DefineTag(TAG_LAYOUT, "LAYOUT");
		XMLParser.DefineTag(TAG_LAYOUT_SPLIT, "SPLIT");
		XMLParser.DefineTag(TAG_LAYOUT_OUTLINE, "OUTLINE");
		XMLParser.DefineTag(TAG_LAYOUT_EXTRA, "EXTRA");
		XMLParser.DefineTag(TAG_LAYOUT_VIEW, "VIEW");
		XMLParser.DefineTag(TAG_CONTENT, "CONTENT");
		XMLParser.DefineTag(TAG_CONTENT_COLOR, "COLOR");
		XMLParser.DefineTag(TAG_CONTENT_EDGE, "EDGE");
		XMLParser.DefineTag(TAG_CONTENT_IMAGE, "IMAGE");
		XMLParser.DefineTag(TAG_CONTENT_SPRITE, "SPRITE");
		XMLParser.DefineTag(TAG_CONTENT_FONT, "FONT");
		XMLParser.DefineTag(TAG_CONTENT_TEXT, "TEXT");
		XMLParser.DefineTag(TAG_CONTENT_BUTTON, "BUTTON");
		XMLParser.DefineTag(TAG_CONTENT_EDITBOX, "EDITBOX");
		XMLParser.DefineAttb(TAG_GUI, ATTB_GUI_VERSION, "VERSION", "");
		XMLParser.DefineAttb(TAG_LAYOUT, ATTB_LAYOUT_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_LAYOUT_SPLIT, ATTB_LAYOUT_SPLIT_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_LAYOUT_SPLIT, ATTB_LAYOUT_SPLIT_POS, "POS", "left");
		XMLParser.DefineAttb(TAG_LAYOUT_SPLIT, ATTB_LAYOUT_SPLIT_SIZE, "SIZE", "100%");
		XMLParser.DefineAttb(TAG_LAYOUT_SPLIT, ATTB_LAYOUT_SPLIT_FILL, "FILL", "");
		XMLParser.DefineAttb(TAG_LAYOUT_OUTLINE, ATTB_LAYOUT_OUTLINE_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_LAYOUT_OUTLINE, ATTB_LAYOUT_OUTLINE_SIZE, "SIZE", "1");
		XMLParser.DefineAttb(TAG_LAYOUT_OUTLINE, ATTB_LAYOUT_OUTLINE_FILL, "FILL", "");
		XMLParser.DefineAttb(TAG_LAYOUT_EXTRA, ATTB_LAYOUT_EXTRA_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_LAYOUT_EXTRA, ATTB_LAYOUT_EXTRA_WIDTH, "WIDTH", "100%");
		XMLParser.DefineAttb(TAG_LAYOUT_EXTRA, ATTB_LAYOUT_EXTRA_HEIGHT, "HEIGHT", "100%");
		XMLParser.DefineAttb(TAG_LAYOUT_EXTRA, ATTB_LAYOUT_EXTRA_FILL, "FILL", "");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_POSX, "POSX", "left");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_POSY, "POSY", "top");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_WIDTH, "WIDTH", "");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_HEIGHT, "HEIGHT", "");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_SCROLL, "SCROLL", "hide");
		XMLParser.DefineAttb(TAG_LAYOUT_VIEW, ATTB_LAYOUT_VIEW_FILL, "FILL", "");
		XMLParser.DefineAttb(TAG_CONTENT, ATTB_CONTENT_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_COLOR, ATTB_CONTENT_COLOR_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_COLOR, ATTB_CONTENT_COLOR_ARGB, "ARGB", "");
		XMLParser.DefineAttb(TAG_CONTENT_EDGE, ATTB_CONTENT_EDGE_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_EDGE, ATTB_CONTENT_EDGE_COLORS, "COLORS", "");
		XMLParser.DefineAttb(TAG_CONTENT_IMAGE, ATTB_CONTENT_IMAGE_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_IMAGE, ATTB_CONTENT_IMAGE_URL, "URL", "");
		XMLParser.DefineAttb(TAG_CONTENT_SPRITE, ATTB_CONTENT_SPRITE_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_SPRITE, ATTB_CONTENT_SPRITE_IMAGES, "IMAGES", "");
		XMLParser.DefineAttb(TAG_CONTENT_SPRITE, ATTB_CONTENT_SPRITE_DELAY, "DELAY", "0");
		XMLParser.DefineAttb(TAG_CONTENT_FONT, ATTB_CONTENT_FONT_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_FONT, ATTB_CONTENT_FONT_SIZE, "SIZE", "");
		XMLParser.DefineAttb(TAG_CONTENT_FONT, ATTB_CONTENT_FONT_URL, "URL", "");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_FORMAT, "FORMAT", "");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_FONT, "FONT", "");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_COLOR, "COLOR", "");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_VECTOR, "VECTOR", "numpad1");
		XMLParser.DefineAttb(TAG_CONTENT_TEXT, ATTB_CONTENT_TEXT_LINE, "LINE", "single");
		XMLParser.DefineAttb(TAG_CONTENT_BUTTON, ATTB_CONTENT_BUTTON_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_BUTTON, ATTB_CONTENT_BUTTON_IMAGES, "IMAGES", "");
		XMLParser.DefineAttb(TAG_CONTENT_BUTTON, ATTB_CONTENT_BUTTON_EDGES, "EDGES", "");
		XMLParser.DefineAttb(TAG_CONTENT_BUTTON, ATTB_CONTENT_BUTTON_EVENT, "EVENT", "");
		XMLParser.DefineAttb(TAG_CONTENT_BUTTON, ATTB_CONTENT_BUTTON_LINK, "LINK", "");
		XMLParser.DefineAttb(TAG_CONTENT_EDITBOX, ATTB_CONTENT_EDITBOX_NAME, "NAME", "");
		XMLParser.DefineAttb(TAG_CONTENT_EDITBOX, ATTB_CONTENT_EDITBOX_TEXT, "TEXT", "");
		XMLParser.DefineAttb(TAG_CONTENT_EDITBOX, ATTB_CONTENT_EDITBOX_COLORS, "COLORS", "");
	}

	void LoadXML(string FileName)
	{
		IsValidDownload() = true;
		CurSection = NoSection;
		Layout.Reset();
		Content.Reset();
		Map.Reset();
		// 예약어 등록
		Content[0].AddNull(Map, "");
		Content[0].AddValue(Map, "pos.left", CodeLeft);
		Content[0].AddValue(Map, "pos.top", CodeTop);
		Content[0].AddValue(Map, "pos.right", CodeRight);
		Content[0].AddValue(Map, "pos.bottom", CodeBottom);
		Content[0].AddValue(Map, "posx.left", CodeLeft);
		Content[0].AddValue(Map, "posy.top", CodeTop);
		Content[0].AddValue(Map, "posx.right", CodeRight);
		Content[0].AddValue(Map, "posy.bottom", CodeBottom);
		Content[0].AddValue(Map, "scroll.hide", CodeHide);
		Content[0].AddValue(Map, "scroll.show", CodeShow);
		Content[0].AddValue(Map, "vector.numpad1", CodeNumpad1);
		Content[0].AddValue(Map, "vector.numpad2", CodeNumpad2);
		Content[0].AddValue(Map, "vector.numpad3", CodeNumpad3);
		Content[0].AddValue(Map, "vector.numpad4", CodeNumpad4);
		Content[0].AddValue(Map, "vector.numpad5", CodeNumpad5);
		Content[0].AddValue(Map, "vector.numpad6", CodeNumpad6);
		Content[0].AddValue(Map, "vector.numpad7", CodeNumpad7);
		Content[0].AddValue(Map, "vector.numpad8", CodeNumpad8);
		Content[0].AddValue(Map, "vector.numpad9", CodeNumpad9);
		Content[0].AddValue(Map, "line.single", CodeSingle);
		Content[0].AddValue(Map, "line.multi", CodeMulti);
		// XML파싱
		BxXml XMLParser;
		GetParser(XMLParser);
		if(XMLParser.ReadBegin(FileName))
		{
			BxXml::State State = BxXml::BLANK;
			while(State != BxXml::ENDOFFILE && State != BxXml::ERROR)
			{
				const int TAG = XMLParser.NextTag(&State);
				// 대상구간
				if(State == BxXml::OPEN)
				{
					switch(TAG)
					{
					case TAG_GUI:
						if(CurSection == NoSection)
							CurSection = InGUI;
						continue;
					case TAG_LAYOUT:
						if(CurSection == InGUI)
						{
							CurSection = InLayout;
							Layout.Insert(LAST);
						}
						continue;
					case TAG_CONTENT:
						if(CurSection == InGUI)
						{
							CurSection = InContent;
							Content.Insert(LAST);
						}
						continue;
					}
				}
				else if(State == BxXml::CLOSE)
				{
					switch(TAG)
					{
					case TAG_GUI:
						if(CurSection == InGUI)
							CurSection = NoSection;
						continue;
					case TAG_LAYOUT:
						if(CurSection == InLayout)
							CurSection = InGUI;
						continue;
					case TAG_CONTENT:
						if(CurSection == InContent)
							CurSection = InGUI;
						continue;
					}
				}
				// 레이아웃
				if(CurSection == InLayout)
				{
					if(State == BxXml::OPEN || State == BxXml::OPENCLOSE)
					{
						switch(TAG)
						{
						case TAG_LAYOUT_SPLIT:
							{
								BxString Name, Pos, Size, Fill;
								XMLParser.ReadAttb(ATTB_LAYOUT_SPLIT_NAME, Name);
								XMLParser.ReadAttb(ATTB_LAYOUT_SPLIT_POS, Pos);
								XMLParser.ReadAttb(ATTB_LAYOUT_SPLIT_SIZE, Size);
								XMLParser.ReadAttb(ATTB_LAYOUT_SPLIT_FILL, Fill);
								Layout[END].AddSplit(Map, Name, Pos, Size, Fill);
							}
							break;
						case TAG_LAYOUT_OUTLINE:
							{
								BxString Name, Size, Fill;
								XMLParser.ReadAttb(ATTB_LAYOUT_OUTLINE_NAME, Name);
								XMLParser.ReadAttb(ATTB_LAYOUT_OUTLINE_SIZE, Size);
								XMLParser.ReadAttb(ATTB_LAYOUT_OUTLINE_FILL, Fill);
								Layout[END].AddOutline(Map, Name, Size, Fill);
							}
							break;
						case TAG_LAYOUT_EXTRA:
							{
								BxString Name, Width, Height, Fill;
								XMLParser.ReadAttb(ATTB_LAYOUT_EXTRA_NAME, Name);
								XMLParser.ReadAttb(ATTB_LAYOUT_EXTRA_WIDTH, Width);
								XMLParser.ReadAttb(ATTB_LAYOUT_EXTRA_HEIGHT, Height);
								XMLParser.ReadAttb(ATTB_LAYOUT_EXTRA_FILL, Fill);
								Layout[END].AddExtra(Map, Name, Width, Height, Fill);
							}
							break;
						case TAG_LAYOUT_VIEW:
							{
								BxString Name, PosX, PosY, Width, Height, Scroll, Fill;
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_NAME, Name);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_POSX, PosX);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_POSY, PosY);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_WIDTH, Width);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_HEIGHT, Height);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_SCROLL, Scroll);
								XMLParser.ReadAttb(ATTB_LAYOUT_VIEW_FILL, Fill);
								Layout[END].AddView(Map, Name, PosX, PosY, Width, Height, Scroll, Fill);
							}
							break;
						}
						if(State == BxXml::OPEN)
							Layout[END].Push(END);
					}
					else if(State == BxXml::CLOSE)
					{
						Layout[END].Pull();
					}
				}
				// 컨텐트
				else if(CurSection == InContent)
				{
					if(State == BxXml::OPEN || State == BxXml::OPENCLOSE)
					{
						switch(TAG)
						{
						case TAG_CONTENT_COLOR:
							{
								BxString Name, Argb;
								XMLParser.ReadAttb(ATTB_CONTENT_COLOR_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_COLOR_ARGB, Argb);
								Content[END].AddColor(Map, Name, Argb);
							}
							break;
						case TAG_CONTENT_EDGE:
							{
								BxString Name, Colors;
								XMLParser.ReadAttb(ATTB_CONTENT_EDGE_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_EDGE_COLORS, Colors);
								Content[END].AddEdge(Map, Name, Colors);
							}
							break;
						case TAG_CONTENT_IMAGE:
							{
								BxString Name, Url;
								XMLParser.ReadAttb(ATTB_CONTENT_IMAGE_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_IMAGE_URL, Url);
								Content[END].AddImage(Map, Name, Url);
							}
							break;
						case TAG_CONTENT_SPRITE:
							{
								BxString Name, Images, Delay;
								XMLParser.ReadAttb(ATTB_CONTENT_SPRITE_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_SPRITE_IMAGES, Images);
								XMLParser.ReadAttb(ATTB_CONTENT_SPRITE_DELAY, Delay);
								Content[END].AddSprite(Map, Name, Images, Delay);
							}
							break;
						case TAG_CONTENT_FONT:
							{
								BxString Name, Size, Url;
								XMLParser.ReadAttb(ATTB_CONTENT_FONT_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_FONT_SIZE, Size);
								XMLParser.ReadAttb(ATTB_CONTENT_FONT_URL, Url);
								Content[END].AddFont(Map, Name, Size, Url);
							}
							break;
						case TAG_CONTENT_TEXT:
							{
								BxString Name, Format, Font, Color, Vector, Line;
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_FORMAT, Format);
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_FONT, Font);
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_COLOR, Color);
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_VECTOR, Vector);
								XMLParser.ReadAttb(ATTB_CONTENT_TEXT_LINE, Line);
								Content[END].AddText(Map, Name, Format, Font, Color, Vector, Line);
							}
							break;
						case TAG_CONTENT_BUTTON:
							{
								BxString Name, Images, Edges, Event, Link;
								XMLParser.ReadAttb(ATTB_CONTENT_BUTTON_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_BUTTON_IMAGES, Images);
								XMLParser.ReadAttb(ATTB_CONTENT_BUTTON_EDGES, Edges);
								XMLParser.ReadAttb(ATTB_CONTENT_BUTTON_EVENT, Event);
								XMLParser.ReadAttb(ATTB_CONTENT_BUTTON_LINK, Link);
								Content[END].AddButton(Map, Name, Images, Edges, Event, Link);
							}
							break;
						case TAG_CONTENT_EDITBOX:
							{
								BxString Name, Text, Colors;
								XMLParser.ReadAttb(ATTB_CONTENT_EDITBOX_NAME, Name);
								XMLParser.ReadAttb(ATTB_CONTENT_EDITBOX_TEXT, Text);
								XMLParser.ReadAttb(ATTB_CONTENT_EDITBOX_COLORS, Colors);
								Content[END].AddEditBox(Map, Name, Text, Colors);
							}
							break;
						}
					}
				}
			}
			XMLParser.ReadEnd();
			// 가공작업(1단계)
			for(int i = 0; i < Layout.Length(); ++i)
				Layout[i].PreProcessing();
			for(int i = 0; i < Content.Length(); ++i)
				Content[i].PreProcessing(Map);
			// 가공작업(2단계)
			for(int i = 0; i < Layout.Length(); ++i)
				Layout[i].Processing();
			for(int i = 0; i < Content.Length(); ++i)
				Content[i].Processing(Map);
			// 상호연결
			for(int i = 0; i < Layout.Length(); ++i)
				Layout[i].Linking(Map);
			for(int i = 0; i < Content.Length(); ++i)
				Content[i].Linking(Map);
		}
	}

	bool AdjustSplitSize(string SplitName, int SizeBy1024 = 0)
	{
		if(!Map.Access(SplitName)) return false;
		Layout::Split* SplitElement = (Layout::Split*) *Map.Access(SplitName);
		if(!SplitElement || SplitElement->GetType() != TypeSplit) return false;
		SplitElement->AdjustSize(SizeBy1024);
		return true;
	}

	bool AdjustViewScroll(string ViewName, point PosByPixel = point::zero())
	{
		if(!Map.Access(ViewName)) return false;
		Layout::View* ViewElement = (Layout::View*) *Map.Access(ViewName);
		if(!ViewElement || ViewElement->GetType() != TypeView) return false;
		ViewElement->AdjustScroll(PosByPixel.x, PosByPixel.y);
		return true;
	}

	bool GetViewSize(string ViewName, size& OutSize, size& InSize)
	{
		if(!Map.Access(ViewName)) return false;
		Layout::View* ViewElement = (Layout::View*) *Map.Access(ViewName);
		if(!ViewElement || ViewElement->GetType() != TypeView) return false;
		return ViewElement->GetSize(OutSize, InSize);
	}

	bool AdjustColorHLS(string ColorName, int AddHBy256 = 0, int AddLBy255 = 0, int AddSBy255 = 0)
	{
		if(!Map.Access(ColorName)) return false;
		Content::Color* ColorElement = (Content::Color*) *Map.Access(ColorName);
		if(!ColorElement || ColorElement->GetType() != TypeColor) return false;
		ColorElement->AdjustHLS(AddHBy256, AddLBy255, AddSBy255);
		return true;
	}

	bool AdjustTextFormat(string TextName, ...)
	{
		if(!Map.Access(TextName)) return false;
		Content::Text* TextElement = (Content::Text*) *Map.Access(TextName);
		if(!TextElement || TextElement->GetType() != TypeText) return false;
		va_list List;
		va_start(List, TextName);
		TextElement->AdjustFormat(true, List, &Map, nullptr);
		va_end(List);
		return true;
	}

	int GetImageWidth(string ImageName)
	{
		Element* ValueElement = *Map.Access(BxString("%s.width", nullptr, ImageName));
		return (ValueElement)? ValueElement->GetValue() : 0;
	}

	int GetImageHeight(string ImageName)
	{
		Element* ValueElement = *Map.Access(BxString("%s.height", nullptr, ImageName));
		return (ValueElement)? ValueElement->GetValue() : 0;
	}

	void Render(BxDraw& Draw)
	{
		if(IsValidDownload())
		for(int i = 0; i < Layout.Length(); ++i)
			Layout[i].Rendering(Map, Draw, BxDrawGlobal::XYWH(0, 0, Draw.Width(), Draw.Height()));
	}

	bool OnEvent(const sysevent& Event)
	{
		if(IsValidDownload())
		for(int i = Layout.Length() - 1; 0 <= i; --i)
			if(Layout[i].OnEvent(Event))
				return true;
		return false;
	}

protected:
	/// @cond SECTION_NAME
	enum Section {NoSection, InGUI, InLayout, InContent};
	enum ElementType {TypeNull, TypeValue, TypeColor, TypeEdge, TypeImage, TypeSprite, TypeFont, TypeText, TypeButton, TypeEditBox,
		TypeSplit, TypeOutline, TypeExtra, TypeView};
	enum ElementStep {StepAlloced, StepNamed, StepPreProcessed, StepProcessed, StepLinked};
	enum ReserveCode {CodeNull = -1, CodeLeft = 0, CodeTop, CodeRight, CodeBottom, CodeHide = 0, CodeShow,
		CodeNumpad1 = 0, CodeNumpad2, CodeNumpad3, CodeNumpad4, CodeNumpad5, CodeNumpad6, CodeNumpad7, CodeNumpad8, CodeNumpad9,
		CodeSingle = 0, CodeMulti};
	/// @endcond

	/// @cond SECTION_NAME
	class Element
	{
	public:
		typedef BxVar<Element*> VarPtr;
		typedef BxVarMap<Element*> VarMapPtr;
	protected:
		ElementType Type;
		ElementStep Step;
		int Value;
	public:
		Element() : Type(TypeNull), Step(StepAlloced), Value(0) {}
		Element(const int FirstValue) : Type(TypeValue), Step(StepAlloced), Value(FirstValue) {}
		virtual ~Element() {}
	public:
		ElementType GetType() {return Type;}
		const int GetValue()
		{
			BxAssert("BxGUI<값보관용 엘리먼트가 아닙니다>", Type == TypeValue);
			return Value;
		}
		void SetValue(const int SecondValue)
		{
			BxAssert("BxGUI<값보관용 엘리먼트가 아닙니다>", Type == TypeValue);
			Value = SecondValue;
		}
	public:
		void SetStep(ElementStep StepOrder)
		{
			switch(StepOrder)
			{
			case StepNamed: BxAssert("BxGUI<호출단계가 잘못되었습니다>", Step == StepAlloced); break;
			case StepPreProcessed: BxAssert("BxGUI<호출단계가 잘못되었습니다>", Step == StepNamed); break;
			case StepProcessed: BxAssert("BxGUI<호출단계가 잘못되었습니다>", Step == StepPreProcessed); break;
			case StepLinked: BxAssert("BxGUI<호출단계가 잘못되었습니다>", Step == StepProcessed); break;
			}
			Step = StepOrder;
		}
	public:
		virtual bool IsValidFill() {return (Type != TypeValue);}
		virtual void PreProcessing(VarMapPtr* Map, BxVar<Element>* Elements) {SetStep(StepPreProcessed);}
		virtual void Processing(VarMapPtr* Map, BxVar<Element>* Elements) {SetStep(StepProcessed);}
		virtual void Linking(const VarMapPtr& Map) {SetStep(StepLinked);}
		virtual void Filling(BxDraw& Draw, rect Rect) {}
		virtual rect Rendering(const VarMapPtr& Map, BxDraw& Draw, rect Rect, rect _out_ InRect) {return rect::zero();}
		virtual bool OnEvent(const sysevent& SysEvent) {return false;}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class ElementTree : public Element
	{
	protected:
		ElementTree* _ref_ Parent;
		BxVar<ElementTree> Child;
	public:
		ElementTree() : Parent(nullptr) {}
		virtual ~ElementTree()
		{
			for(int i = 0; i < Child.Length(); ++i)
			{
				ElementTree* OldData = nullptr;
				Child.RemoveData(i, &OldData);
				BxDelete(OldData);
			}
		}
	public:
		void AddChild(ElementTree* elementtree)
		{
			elementtree->Parent = this;
			Child.Insert(LAST, elementtree);
		}
		ElementTree* GetParent() {return Parent;}
		ElementTree* GetChild(int i) {return Child.Access(i);}
	public:
		void PreProcessingTree()
		{
			if(Parent) PreProcessing(nullptr, nullptr);
			for(int i = 0; i < Child.Length(); ++i)
				Child[i].PreProcessingTree();
		}
		void ProcessingTree()
		{
			if(Parent) Processing(nullptr, nullptr);
			for(int i = 0; i < Child.Length(); ++i)
				Child[i].ProcessingTree();
		}
		void LinkingTree(const Element::VarMapPtr& Map)
		{
			if(Parent) Linking(Map);
			for(int i = 0; i < Child.Length(); ++i)
				Child[i].LinkingTree(Map);
		}
		rect RenderingTree(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect)
		{
			rect OutRect = Rect, InRect = Rect;
			try(Draw, CLIP(Rect, -Rect.l, -Rect.t))
			{
				if(Parent) OutRect = Rendering(Map, Draw, Rect, InRect);
				for(int i = 0; i < Child.Length(); ++i)
					InRect = Child[i].RenderingTree(Map, Draw, InRect);
			}
			return OutRect;
		}
		bool OnEventTree(const sysevent& Event)
		{
			for(int i = Child.Length() - 1; 0 <= i; --i)
				if(Child[i].OnEventTree(Event)) return true;
			return OnEvent(Event);
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class Content
	{
	protected:
		BxVar<Element> Elements;
	public:
		class Color : public Element
		{
			string Name;
			uint Argb;
		protected:
			uint ArgbGen;
		public:
			Color() : Name(nullptr), Argb(0xFF000000), ArgbGen(0xFF000000) {Type = TypeColor;}
			virtual ~Color()
			{
				Name = BxUtilGlobal::StrFree(Name);
			}
		public:
			void SetAttb(string name, string argb)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				for(int i = 0; argb[i]; ++i)
				{
					if('0' <= argb[i] && argb[i] <= '9')
						Argb = (Argb << 4) | (argb[i] - '0');
					else if('A' <= argb[i] && argb[i] <= 'F')
						Argb = (Argb << 4) | (argb[i] - 'A' + 10);
					else if('a' <= argb[i] && argb[i] <= 'f')
						Argb = (Argb << 4) | (argb[i] - 'a' + 10);
				}
				ArgbGen = Argb;
			}
			void AdjustHLS(int AddHBy256, int AddLBy255, int AddSBy255)
			{
				const color_x888 HLS = BxDrawGlobal::ColorRGBToHLS((Argb >> 16) & 0xFF, (Argb >> 8) & 0xFF, Argb & 0xFF);
				const byte H = (byte) ((((HLS >> 16) & 0xFF) + AddHBy256) & 0xFF);
				const byte L = (byte) BxUtilGlobal::MinMax(0, ((HLS >> 8) & 0xFF) + AddLBy255, 0xFF);
				const byte S = (byte) BxUtilGlobal::MinMax(0, (HLS & 0xFF) + AddSBy255, 0xFF);
				ArgbGen = (Argb & 0xFF000000) | BxDrawGlobal::ColorHLSToRGB(H, L, S);
			}
			inline const uint GetArgb() {return ArgbGen;}
		public:
			virtual void Filling(BxDraw& Draw, rect Rect)
			{
				Draw.Rectangle(BxDrawGlobal::FILL, Rect,
					BxDrawGlobal::COLOR(ArgbGen & 0x00FFFFFF) >> BxDrawGlobal::OPACITY(ArgbGen >> 24));
			}
		};
		class Edge : public Element
		{
			string Name;
			string Colors;
		public:
			Element::VarPtr _ref_ ColorValues;
		public:
			Edge() : Name(nullptr), Colors(nullptr) {Type = TypeEdge;}
			virtual ~Edge()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Colors = BxUtilGlobal::StrFree(Colors);
			}
		public:
			void SetAttb(string name, string colors)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Colors = BxUtilGlobal::StrCpyWithAlloc(colors);
			}
		public:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxVar<BxString> ColorNames;
				BxString::ParseDivision(Colors, ColorNames);
				BxAssert("BxGUI<연결작업중 유효한 엘리먼트가 하나도 없습니다>", ColorNames.Length());
				for(int i = 0; i < ColorNames.Length(); ++i)
				{
					BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(ColorNames[i]) && *Map.Access(ColorNames[i]));
					BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(ColorNames[i]))->GetType() == TypeColor);
					ColorValues[i] = *Map.Access(ColorNames[i]);
				}
			}
		};
		class Image : public Element
		{
			string Name;
			string Url;
		public:
			int FileSize;
			string FilePath;
			BxImage CacheImage;
		public:
			Image() : Name(nullptr), Url(nullptr), FileSize(0), FilePath(nullptr) {Type = TypeImage;}
			virtual ~Image()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Url = BxUtilGlobal::StrFree(Url);
				FilePath = BxUtilGlobal::StrFree(FilePath);
			}
		public:
			void SetAttb(string name, string url)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Url = BxUtilGlobal::StrCpyWithAlloc(url);
			}
		public:
			virtual void Processing(Element::VarMapPtr* Map, BxVar<Element>* Elements)
			{
				SetStep(StepProcessed);
				BxWebContent Content;
				if(DownloadCache(Url, Content, FileSize, FilePath, true))
				{
					BxString FilePathString(FilePath);
					BxString FileTypeString = FilePathString.Right(4);
					BxImage::type FileType = BxImage::PNG;
					if(FileTypeString.CompareNoCase(".bmp") == same)
						FileType = BxImage::BMP;
					else if(FileTypeString.CompareNoCase(".b16") == same)
						FileType = BxImage::B16;
					else if(FileTypeString.CompareNoCase(".b32") == same)
						FileType = BxImage::B32;
					else if(FileTypeString.CompareNoCase(".png") == same)
						FileType = BxImage::PNG;
					else if(FileTypeString.CompareNoCase(".jpg") == same)
						FileType = BxImage::JPG;
					else if(FileTypeString.CompareNoCase(".gif") == same)
						FileType = BxImage::GIF;
					CacheImage.Load(Content.GetBytes(), Content.GetDataLength(), FileType);
					CacheImage.SetHotspot(0, 0);
				}
				else IsValidDownload() = false;
				// 가로길이 추가등록
				BxString WidthName("%s.width", nullptr, (string) Name);
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map->Access(WidthName));
				Element* NewData1 = BxNew_Param(Element, (CacheImage.IsExist())? CacheImage.Width() : 0);
				Elements->Insert(LAST, (*Map)[WidthName] = NewData1);
				NewData1->SetStep(StepNamed);
				NewData1->PreProcessing(nullptr, nullptr);
				// 세로길이 추가등록
				BxString HeightName("%s.height", nullptr, (string) Name);
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map->Access(HeightName));
				Element* NewData2 = BxNew_Param(Element, (CacheImage.IsExist())? CacheImage.Height() : 0);
				Elements->Insert(LAST, (*Map)[HeightName] = NewData2);
				NewData2->SetStep(StepNamed);
				NewData2->PreProcessing(nullptr, nullptr);
			}
			virtual void Filling(BxDraw& Draw, rect Rect)
			{
				if(CacheImage.IsExist())
				try(Draw, CLIP(Rect))
					Draw.Area(0, 0, BxDrawGlobal::FORM(&CacheImage));
			}
		};
		class Sprite : public Element
		{
			string Name;
			string Images;
			int Delay;
		public:
			Element::VarPtr _ref_ ImageValues;
		public:
			Sprite() : Name(nullptr), Images(nullptr), Delay(0) {Type = TypeSprite;}
			virtual ~Sprite()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Images = BxUtilGlobal::StrFree(Images);
			}
		public:
			void SetAttb(string name, string images, string delay)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Images = BxUtilGlobal::StrCpyWithAlloc(images);
				Delay = BxUtilGlobal::AtoI(delay);
			}
		public:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxVar<BxString> ImageNames;
				BxString::ParseDivision(Images, ImageNames);
				BxAssert("BxGUI<연결작업중 유효한 엘리먼트가 하나도 없습니다>", ImageNames.Length());
				for(int i = 0; i < ImageNames.Length(); ++i)
				{
					BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(ImageNames[i]) && *Map.Access(ImageNames[i]));
					BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(ImageNames[i]))->GetType() == TypeImage);
					ImageValues[i] = *Map.Access(ImageNames[i]);
				}
			}
		};
		class Font : public Element
		{
			string Name;
			int Size;
			string Url;
		public:
			int FileSize;
			string FilePath;
			id_font CacheFont;
		public:
			Font() : Name(nullptr), Size(0), Url(nullptr), FileSize(0), FilePath(nullptr), CacheFont(nullptr) {Type = TypeFont;}
			virtual ~Font()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Url = BxUtilGlobal::StrFree(Url);
				FilePath = BxUtilGlobal::StrFree(FilePath);
				BxCore::Font::Close(CacheFont);
			}
		public:
			void SetAttb(string name, string size, string url)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Size = BxUtilGlobal::AtoI(size);
				Url = BxUtilGlobal::StrCpyWithAlloc(url);
			}
			inline id_font GetCacheFont() {return CacheFont;}
		public:
			virtual bool IsValidFill() {return false;}
			virtual void PreProcessing(Element::VarMapPtr* Map, BxVar<Element>* Elements)
			{
				SetStep(StepPreProcessed);
				BxWebContent Content;
				if(DownloadCache(Url, Content, FileSize, FilePath, false))
					CacheFont = BxCore::Font::Open(BxString(BxWebContent::GetCachePath()) + "/" + FilePath, Size);
				else IsValidDownload() = false;
			}
		};
		class Text : public Element
		{
			string Name;
			string Format;
			string Font;
			string Color;
			string Vector;
			string Line;
		protected:
			string FormatGen;
		public:
			Content::Font* _ref_ FontValue;
			Content::Color* _ref_ ColorValue;
			ReserveCode VectorValue;
			ReserveCode LineValue;
		public:
			Text() : Name(nullptr), Format(nullptr), Font(nullptr), Color(nullptr), Vector(nullptr), Line(nullptr), FormatGen(nullptr),
				FontValue(nullptr), ColorValue(nullptr), VectorValue(CodeNull) {Type = TypeText;}
			virtual ~Text()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Format = BxUtilGlobal::StrFree(Format);
				Font = BxUtilGlobal::StrFree(Font);
				Color = BxUtilGlobal::StrFree(Color);
				Vector = BxUtilGlobal::StrFree(Vector);
				Line = BxUtilGlobal::StrFree(Line);
				FormatGen = BxUtilGlobal::StrFree(FormatGen);
			}
		public:
			void SetAttb(string name, string format, string font, string color, string vector, string line)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Format = BxUtilGlobal::StrCpyWithAlloc(format);
				Font = BxUtilGlobal::StrCpyWithAlloc(font);
				Color = BxUtilGlobal::StrCpyWithAlloc(color);
				Vector = BxUtilGlobal::StrCpyWithAlloc(BxString("vector.") + vector);
				Line = BxUtilGlobal::StrCpyWithAlloc(BxString("line.") + line);
			}
			void AdjustFormatNoList(Element::VarMapPtr* Map, BxVar<Element>* Elements, ...)
			{
				va_list List;
				va_start(List, Elements);
				AdjustFormat(false, List, Map, Elements);
				va_end(List);
			}
			void AdjustFormat(bool dosprint, va_list list, Element::VarMapPtr* Map, BxVar<Element>* Elements)
			{
				FormatGen = BxUtilGlobal::StrFree(FormatGen);
				if(dosprint)
				{
					const int FormatGenLen = BxCore::Util::VsnPrintf(nullptr, 0, Format, list);
					BxAssert("BxGUI<FormatGen의 예상길이를 얻는데 실패하였습니다>", 0 <= FormatGenLen);
					FormatGen = BxUtilGlobal::StrAlloc(FormatGenLen);
					BxCore::Util::VsnPrintf((string_rw) FormatGen, FormatGenLen + 1, Format, list);
				}
				else FormatGen = BxUtilGlobal::StrCpyWithAlloc(Format);
				if(Map)
				{
					const size TextSize = (FontValue->GetCacheFont())? BxCore::Font::GetSize(FontValue->GetCacheFont(), FormatGen) : WH(0, 0);
					// 가로길이 추가등록
					BxString WidthName("%s.width", nullptr, (string) Name);
					if(Map->Access(WidthName)) (*Map->Access(WidthName))->SetValue(TextSize.w);
					else if(Elements)
					{
						Element* NewData = BxNew_Param(Element, TextSize.w);
						Elements->Insert(LAST, (*Map)[WidthName] = NewData);
						NewData->SetStep(StepNamed);
						NewData->PreProcessing(nullptr, nullptr);
					}
					// 세로길이 추가등록
					BxString HeightName("%s.height", nullptr, (string) Name);
					if(Map->Access(HeightName)) (*Map->Access(HeightName))->SetValue(TextSize.h);
					else if(Elements)
					{
						Element* NewData = BxNew_Param(Element, TextSize.h);
						Elements->Insert(LAST, (*Map)[HeightName] = NewData);
						NewData->SetStep(StepNamed);
						NewData->PreProcessing(nullptr, nullptr);
					}
				}
			}
		public:
			virtual void Processing(Element::VarMapPtr* Map, BxVar<Element>* Elements)
			{
				SetStep(StepProcessed);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map->Access(Font) && *Map->Access(Font));
				BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map->Access(Font))->GetType() == TypeFont);
				FontValue = (Content::Font*) *Map->Access(Font);
				AdjustFormatNoList(Map, Elements);
			}
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Color) && *Map.Access(Color));
				BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(Color))->GetType() == TypeColor);
				ColorValue = (Content::Color*) *Map.Access(Color);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Vector) && *Map.Access(Vector));
				VectorValue = (ReserveCode) (*Map.Access(Vector))->GetValue();
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Line) && *Map.Access(Line));
				LineValue = (ReserveCode) (*Map.Access(Line))->GetValue();
			}
			virtual void Filling(BxDraw& Draw, rect Rect)
			{
				const int RectWidth = Rect.r - Rect.l;
				const int RectHeight = Rect.b - Rect.t;
				if(0 < RectWidth && 0 < RectHeight && FontValue->GetCacheFont())
				try(Draw, CLIP(Rect))
				{
					switch(VectorValue)
					{
					default:
					case CodeNumpad1: BxCore::Font::SetSort(fontsort_pad1); break;
					case CodeNumpad2: BxCore::Font::SetSort(fontsort_pad2); break;
					case CodeNumpad3: BxCore::Font::SetSort(fontsort_pad3); break;
					case CodeNumpad4: BxCore::Font::SetSort(fontsort_pad4); break;
					case CodeNumpad5: BxCore::Font::SetSort(fontsort_pad5); break;
					case CodeNumpad6: BxCore::Font::SetSort(fontsort_pad6); break;
					case CodeNumpad7: BxCore::Font::SetSort(fontsort_pad7); break;
					case CodeNumpad8: BxCore::Font::SetSort(fontsort_pad8); break;
					case CodeNumpad9: BxCore::Font::SetSort(fontsort_pad9); break;
					}
					if(LineValue == CodeSingle)
					{
						point xy = {0,};
						switch(VectorValue)
						{
						default:
						case CodeNumpad1: xy = BxDrawGlobal::XY(0, RectHeight - size::max); break;
						case CodeNumpad2: xy = BxDrawGlobal::XY(RectWidth / 2 - size::half, RectHeight - size::max); break;
						case CodeNumpad3: xy = BxDrawGlobal::XY(RectWidth - size::max, RectHeight - size::max); break;
						case CodeNumpad4: xy = BxDrawGlobal::XY(0, RectHeight / 2 - size::half); break;
						case CodeNumpad5: xy = BxDrawGlobal::XY(RectWidth / 2 - size::half, RectHeight / 2 - size::half); break;
						case CodeNumpad6: xy = BxDrawGlobal::XY(RectWidth - size::max, RectHeight / 2 - size::half); break;
						case CodeNumpad7: xy = BxDrawGlobal::XY(0, 0); break;
						case CodeNumpad8: xy = BxDrawGlobal::XY(RectWidth / 2 - size::half, 0); break;
						case CodeNumpad9: xy = BxDrawGlobal::XY(RectWidth - size::max, 0); break;
						}
						BxCore::Font::Draw(FontValue->GetCacheFont(), FormatGen,
							BxDrawGlobal::XY(Draw.TranslateX() + xy.x, Draw.TranslateY() + xy.y), size::full(),
							ColorValue->GetArgb() & 0x00FFFFFF, ColorValue->GetArgb() >> 24);
					}
					else if(LineValue == CodeMulti)
						BxCore::Font::Draw(FontValue->GetCacheFont(), FormatGen,
							BxDrawGlobal::XY(Draw.TranslateX(), Draw.TranslateY()), BxDrawGlobal::WH(RectWidth, RectHeight),
							ColorValue->GetArgb() & 0x00FFFFFF, ColorValue->GetArgb() >> 24);
				}
			}
		};
		class Button : public Element
		{
			string Name;
			string Images;
			string Edges;
			string Event;
			string Link;
		protected:
			rect EventAreaGen;
			bool IsClickedGen;
		public:
			Element::VarPtr _ref_ ImageValues;
			Element::VarPtr _ref_ EdgeValues;
		public:
			Button() : Name(nullptr), Images(nullptr), Edges(nullptr), Event(nullptr), Link(nullptr), EventAreaGen(rect::zero()), IsClickedGen(false)
			{Type = TypeButton;}
			virtual ~Button()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Images = BxUtilGlobal::StrFree(Images);
				Edges = BxUtilGlobal::StrFree(Edges);
				Event = BxUtilGlobal::StrFree(Event);
				Link = BxUtilGlobal::StrFree(Link);
			}
		public:
			void SetAttb(string name, string images, string edges, string event, string link)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Images = BxUtilGlobal::StrCpyWithAlloc(images);
				Edges = BxUtilGlobal::StrCpyWithAlloc(edges);
				Event = BxUtilGlobal::StrCpyWithAlloc(event);
				Link = BxUtilGlobal::StrCpyWithAlloc(link);
			}
		public:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxVar<BxString> ImageNames;
				BxString::ParseDivision(Images, ImageNames);
				BxAssert("BxGUI<연결작업중 유효한 엘리먼트가 하나도 없습니다>", ImageNames.Length());
				for(int i = 0; i < ImageNames.Length(); ++i)
				{
					BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(ImageNames[i]) && *Map.Access(ImageNames[i]));
					BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(ImageNames[i]))->GetType() == TypeImage);
					ImageValues[i] = *Map.Access(ImageNames[i]);
				}
				BxVar<BxString> EdgeNames;
				BxString::ParseDivision(Edges, EdgeNames);
				for(int i = 0; i < EdgeNames.Length(); ++i)
				{
					BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(EdgeNames[i]) && *Map.Access(EdgeNames[i]));
					BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(EdgeNames[i]))->GetType() == TypeEdge);
					EdgeValues[i] = *Map.Access(EdgeNames[i]);
				}
			}
			virtual void Filling(BxDraw& Draw, rect Rect)
			{
				if(2 <= ImageValues.Length())
				{
					int ImageID = (IsClickedGen)? 1 : 0;
					ImageValues[ImageID]->Filling(Draw, Rect);
					size OneSize = size::zero();
					if(ImageValues[ImageID]->GetType() == TypeImage)
					{
						Image* OneImage = (Image*) ImageValues[ImageID];
						OneSize.w = OneImage->CacheImage.Width();
						OneSize.h = OneImage->CacheImage.Height();
					}
					Rect.l = BxUtilGlobal::Max(Rect.l + Draw.TranslateX(), BxDrawGlobal::_DrawOption::CurClipRect().l);
					Rect.t = BxUtilGlobal::Max(Rect.t + Draw.TranslateY(), BxDrawGlobal::_DrawOption::CurClipRect().t);
					Rect.r = BxUtilGlobal::Min(Rect.r + Draw.TranslateX(), BxDrawGlobal::_DrawOption::CurClipRect().r);
					Rect.b = BxUtilGlobal::Min(Rect.b + Draw.TranslateY(), BxDrawGlobal::_DrawOption::CurClipRect().b);
					EventAreaGen.r = (EventAreaGen.l = Rect.l) + BxUtilGlobal::Min(Rect.r - Rect.l, OneSize.w);
					EventAreaGen.b = (EventAreaGen.t = Rect.t) + BxUtilGlobal::Min(Rect.b - Rect.t, OneSize.h);
				}
			}
			virtual bool OnEvent(const sysevent& SysEvent)
			{
				if(SysEvent.type != syseventtype_touch)
					return false;
				if(SysEvent.touch.type == systouchtype_down)
				{
					if(SysEvent.touch.x < EventAreaGen.l || SysEvent.touch.y < EventAreaGen.t ||
						EventAreaGen.r <= SysEvent.touch.x || EventAreaGen.b <= SysEvent.touch.y)
						IsClickedGen = false;
					else IsClickedGen = true;
				}
				else if(SysEvent.touch.type != systouchtype_move)
				{
					if(SysEvent.touch.type == systouchtype_up && IsClickedGen && BxGUI::Callback() &&
						!(SysEvent.touch.x < EventAreaGen.l || SysEvent.touch.y < EventAreaGen.t ||
						EventAreaGen.r <= SysEvent.touch.x || EventAreaGen.b <= SysEvent.touch.y))
						BxGUI::Callback()(true, Event, (void*) Link);
					IsClickedGen = false;
				}
				return IsClickedGen;
			}
		};
		class EditBox : public Element
		{
			string Name;
			string Text;
			string Colors;
		public:
			Element* _ref_ TextValue;
			Element::VarPtr _ref_ ColorValues;
		public:
			EditBox() : Name(nullptr), Text(nullptr), Colors(nullptr), TextValue(nullptr) {Type = TypeEditBox;}
			virtual ~EditBox()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Text = BxUtilGlobal::StrFree(Text);
				Colors = BxUtilGlobal::StrFree(Colors);
			}
		public:
			void SetAttb(string name, string text, string colors)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Text = BxUtilGlobal::StrCpyWithAlloc(text);
				Colors = BxUtilGlobal::StrCpyWithAlloc(colors);
			}
		public:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Text) && *Map.Access(Text));
				BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(Text))->GetType() == TypeText);
				TextValue = *Map.Access(Text);
				BxVar<BxString> ColorNames;
				BxString::ParseDivision(Colors, ColorNames);
				BxAssert("BxGUI<연결작업중 유효한 엘리먼트가 하나도 없습니다>", ColorNames.Length());
				for(int i = 0; i < ColorNames.Length(); ++i)
				{
					BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(ColorNames[i]) && *Map.Access(ColorNames[i]));
					BxAssert("BxGUI<연결작업중 타입이 일치하지 않습니다>", (*Map.Access(ColorNames[i]))->GetType() == TypeColor);
					ColorValues[i] = *Map.Access(ColorNames[i]);
				}
			}
		};

	public:
		Content() {}
		virtual ~Content()
		{
			for(int i = 0; i < Elements.Length(); ++i)
			{
				Element* OldData = nullptr;
				Elements.RemoveData(i, &OldData);
				BxDelete(OldData);
			}
		}
	public:
		BxVar<Element>* GetElements() {return &Elements;}
	public:
		void PreProcessing(Element::VarMapPtr& Map)
		{
			for(int i = 0; i < Elements.Length(); ++i)
				Elements[i].PreProcessing(&Map, &Elements);
		}
		void Processing(Element::VarMapPtr& Map)
		{
			for(int i = 0; i < Elements.Length(); ++i)
				Elements[i].Processing(&Map, &Elements);
		}
		void Linking(const Element::VarMapPtr& Map)
		{
			for(int i = 0; i < Elements.Length(); ++i)
				Elements[i].Linking(Map);
		}
		void AddNull(Element::VarMapPtr& Map, string Name)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Element* NewData = BxNew(Element);
			Elements.Insert(LAST, Map[Name] = NewData);
			NewData->SetStep(StepNamed);
		}
		void AddValue(Element::VarMapPtr& Map, string Name, int Value)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Element* NewData = BxNew_Param(Element, Value);
			Elements.Insert(LAST, Map[Name] = NewData);
			NewData->SetStep(StepNamed);
		}
		void AddColor(Element::VarMapPtr& Map, string Name, string Argb)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Color* NewData = BxNew(Color);
			NewData->SetAttb(Name, Argb);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddEdge(Element::VarMapPtr& Map, string Name, string Colors)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Edge* NewData = BxNew(Edge);
			NewData->SetAttb(Name, Colors);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddImage(Element::VarMapPtr& Map, string Name, string Url)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Image* NewData = BxNew(Image);
			NewData->SetAttb(Name, Url);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddSprite(Element::VarMapPtr& Map, string Name, string Images, string Delay)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Sprite* NewData = BxNew(Sprite);
			NewData->SetAttb(Name, Images, Delay);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddFont(Element::VarMapPtr& Map, string Name, string Size, string Url)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Font* NewData = BxNew(Font);
			NewData->SetAttb(Name, Size, Url);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddText(Element::VarMapPtr& Map, string Name, string Format, string Font, string Color, string Vector, string Line)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Text* NewData = BxNew(Text);
			NewData->SetAttb(Name, Format, Font, Color, Vector, Line);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddButton(Element::VarMapPtr& Map, string Name, string Images, string Edges, string Event, string Link)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			Button* NewData = BxNew(Button);
			NewData->SetAttb(Name, Images, Edges, Event, Link);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
		void AddEditBox(Element::VarMapPtr& Map, string Name, string Text, string Colors)
		{
			BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
			EditBox* NewData = BxNew(EditBox);
			NewData->SetAttb(Name, Text, Colors);
			Elements.Insert(LAST, Map[Name] = NewData);
		}
	};
	/// @endcond

	/// @cond SECTION_NAME
	class Layout
	{
	protected:
		ElementTree Top;
		ElementTree* CurFocus;
	public:
		class Split : public ElementTree
		{
			string Name;
			string Pos;
			string Size;
			string Fill;
		protected:
			int AddSize1024Gen;
		public:
			ReserveCode PosValue;
			Element* _ref_ FillValue;
		public:
			Split() : Name(nullptr), Pos(nullptr), Size(nullptr), Fill(nullptr), AddSize1024Gen(0), PosValue(CodeNull), FillValue(nullptr) {Type = TypeSplit;}
			virtual ~Split()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Pos = BxUtilGlobal::StrFree(Pos);
				Size = BxUtilGlobal::StrFree(Size);
				Fill = BxUtilGlobal::StrFree(Fill);
			}
		public:
			void SetAttb(string name, string pos, string size, string fill)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Pos = BxUtilGlobal::StrCpyWithAlloc(BxString("pos.") + pos);
				Size = BxUtilGlobal::StrCpyWithAlloc(size);
				Fill = BxUtilGlobal::StrCpyWithAlloc(fill);
			}
			void AdjustSize(int size1024)
			{
				AddSize1024Gen = size1024;
			}
		protected:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Pos) && *Map.Access(Pos));
				PosValue = (ReserveCode) (*Map.Access(Pos))->GetValue();
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Fill) && *Map.Access(Fill));
				BxAssert("BxGUI<FILL용 엘리먼트가 아닙니다>", (*Map.Access(Fill))->IsValidFill());
				FillValue = *Map.Access(Fill);
			}
			virtual rect Rendering(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect, rect _out_ InRect)
			{
				InRect = Rect;
				rect OutRect = Rect;
				const int RectWidth = Rect.r - Rect.l;
				const int RectHeight = Rect.b - Rect.t;
				if(0 < RectWidth && 0 < RectHeight)
				{
					// 동적데이터
					int WidthValue = 0, HeightValue = 0;
					if(Map.Access(Size)) WidthValue = (*Map.Access(Size))->GetValue();
					else WidthValue = BxUtilGlobal::AtoI(Size) * ((Size[BxUtilGlobal::StrLen(Size) - 1] == '%')? -1 : 1);
					if(Map.Access(Size)) HeightValue = (*Map.Access(Size))->GetValue();
					else HeightValue = BxUtilGlobal::AtoI(Size) * ((Size[BxUtilGlobal::StrLen(Size) - 1] == '%')? -1 : 1);
					if(WidthValue < 0) WidthValue = RectWidth * -WidthValue / 100;
					if(HeightValue < 0) HeightValue = RectHeight * -HeightValue / 100;
					const int AddWidthValue = RectWidth * AddSize1024Gen / 1024;
					const int AddHeightValue = RectHeight * AddSize1024Gen / 1024;
					// 구획작업
					switch(PosValue)
					{
					case CodeLeft: OutRect.l = InRect.r = InRect.l + BxUtilGlobal::MinMax(0, WidthValue + AddWidthValue, RectWidth); break;
					case CodeTop: OutRect.t = InRect.b = InRect.t + BxUtilGlobal::MinMax(0, HeightValue + AddHeightValue, RectHeight); break;
					case CodeRight: OutRect.r = InRect.l = InRect.r - BxUtilGlobal::MinMax(0, WidthValue + AddWidthValue, RectWidth); break;
					case CodeBottom: OutRect.b = InRect.t = InRect.b - BxUtilGlobal::MinMax(0, HeightValue + AddHeightValue, RectHeight); break;
					}
					if(FillValue && FillValue->GetType() != TypeNull)
						FillValue->Filling(Draw, InRect);
				}
				return OutRect;
			}
			virtual bool OnEvent(const sysevent& Event)
			{
				if(FillValue && FillValue->GetType() != TypeNull)
					return FillValue->OnEvent(Event);
				return false;
			}
		};
		class Outline : public ElementTree
		{
			string Name;
			string Size;
			string Fill;
		public:
			Element* _ref_ FillValue;
		public:
			Outline() : Name(nullptr), Size(nullptr), Fill(nullptr), FillValue(nullptr) {Type = TypeOutline;}
			virtual ~Outline()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Size = BxUtilGlobal::StrFree(Size);
				Fill = BxUtilGlobal::StrFree(Fill);
			}
		public:
			void SetAttb(string name, string size, string fill)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Size = BxUtilGlobal::StrCpyWithAlloc(size);
				Fill = BxUtilGlobal::StrCpyWithAlloc(fill);
			}
		protected:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<outline/size에서는 백분율이 사용될 수 없습니다>", Size[BxUtilGlobal::StrLen(Size) - 1] != '%');
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Fill) && *Map.Access(Fill));
				BxAssert("BxGUI<FILL용 엘리먼트가 아닙니다>", (*Map.Access(Fill))->IsValidFill());
				FillValue = *Map.Access(Fill);
			}
			virtual rect Rendering(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect, rect _out_ InRect)
			{
				InRect = Rect;
				rect OutRect = Rect;
				const int RectWidth = Rect.r - Rect.l;
				const int RectHeight = Rect.b - Rect.t;
				if(0 < RectWidth && 0 < RectHeight)
				{
					// 동적데이터
					int SizeValue = 0;
					if(Map.Access(Size)) SizeValue = (*Map.Access(Size))->GetValue();
					else SizeValue = BxUtilGlobal::AtoI(Size);
					// 구획작업
					const int OutRectWidth = BxUtilGlobal::Max(0, RectWidth - SizeValue * 2);
					const int OutRectHeight = BxUtilGlobal::Max(0, RectHeight - SizeValue * 2);
					OutRect.r = (OutRect.l += SizeValue) + OutRectWidth;
					OutRect.b = (OutRect.t += SizeValue) + OutRectHeight;
					rect ChildRect[4] = {Rect, Rect, Rect, Rect};
					ChildRect[0].b = OutRect.t; // 상측
					ChildRect[1].t = OutRect.b; // 하측
					ChildRect[2].r = OutRect.l; // 상하사이에 낀 좌측
					ChildRect[2].t = OutRect.t;
					ChildRect[2].b = OutRect.b;
					ChildRect[3].l = OutRect.r; // 상하사이에 낀 우측
					ChildRect[3].t = OutRect.t;
					ChildRect[3].b = OutRect.b;
					for(int i = 0; i < 4; ++i)
						if(FillValue && FillValue->GetType() != TypeNull)
							FillValue->Filling(Draw, ChildRect[i]);
				}
				return OutRect;
			}
			virtual bool OnEvent(const sysevent& Event)
			{
				if(FillValue && FillValue->GetType() != TypeNull)
					return FillValue->OnEvent(Event);
				return false;
			}
		};
		class Extra : public ElementTree
		{
			string Name;
			string Width;
			string Height;
			string Fill;
		public:
			Element* _ref_ FillValue;
		public:
			Extra() : Name(nullptr), Width(nullptr), Height(nullptr), Fill(nullptr), FillValue(nullptr) {Type = TypeExtra;}
			virtual ~Extra()
			{
				Name = BxUtilGlobal::StrFree(Name);
				Width = BxUtilGlobal::StrFree(Width);
				Height = BxUtilGlobal::StrFree(Height);
				Fill = BxUtilGlobal::StrFree(Fill);
			}
		public:
			void SetAttb(string name, string width, string height, string fill)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				Width = BxUtilGlobal::StrCpyWithAlloc(width);
				Height = BxUtilGlobal::StrCpyWithAlloc(height);
				Fill = BxUtilGlobal::StrCpyWithAlloc(fill);
			}
		protected:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Fill) && *Map.Access(Fill));
				BxAssert("BxGUI<FILL용 엘리먼트가 아닙니다>", (*Map.Access(Fill))->IsValidFill());
				FillValue = *Map.Access(Fill);
			}
			virtual rect Rendering(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect, rect _out_ InRect)
			{
				InRect = Rect;
				rect OutRect = Rect;
				const int RectWidth = Rect.r - Rect.l;
				const int RectHeight = Rect.b - Rect.t;
				if(0 < RectWidth && 0 < RectHeight)
				{
					// 동적데이터
					int WidthValue = 0, HeightValue = 0;
					if(Map.Access(Width)) WidthValue = (*Map.Access(Width))->GetValue();
					else WidthValue = BxUtilGlobal::AtoI(Width) * ((Width[BxUtilGlobal::StrLen(Width) - 1] == '%')? -1 : 1);
					if(Map.Access(Height)) HeightValue = (*Map.Access(Height))->GetValue();
					else HeightValue = BxUtilGlobal::AtoI(Height) * ((Height[BxUtilGlobal::StrLen(Height) - 1] == '%')? -1 : 1);
					if(WidthValue < 0) WidthValue = RectWidth * -WidthValue / 100;
					if(HeightValue < 0) HeightValue = RectHeight * -HeightValue / 100;
					// 구획작업
					const int OutRectWidth = BxUtilGlobal::MinMax(0, WidthValue, RectWidth);
					const int OutRectHeight = BxUtilGlobal::MinMax(0, HeightValue, RectHeight);
					OutRect.r = (OutRect.l += (RectWidth - OutRectWidth) / 2) + OutRectWidth;
					OutRect.b = (OutRect.t += (RectHeight - OutRectHeight) / 2) + OutRectHeight;
					rect ChildRect[4] = {Rect, Rect, Rect, Rect};
					ChildRect[0].b = OutRect.t; // 상측
					ChildRect[1].t = OutRect.b; // 하측
					ChildRect[2].r = OutRect.l; // 상하사이에 낀 좌측
					ChildRect[2].t = OutRect.t;
					ChildRect[2].b = OutRect.b;
					ChildRect[3].l = OutRect.r; // 상하사이에 낀 우측
					ChildRect[3].t = OutRect.t;
					ChildRect[3].b = OutRect.b;
					for(int i = 0; i < 4; ++i)
						if(FillValue && FillValue->GetType() != TypeNull)
							FillValue->Filling(Draw, ChildRect[i]);
				}
				return OutRect;
			}
			virtual bool OnEvent(const sysevent& Event)
			{
				if(FillValue && FillValue->GetType() != TypeNull)
					return FillValue->OnEvent(Event);
				return false;
			}
		};
		class View : public ElementTree
		{
			string Name;
			string PosX;
			string PosY;
			string Width;
			string Height;
			string Scroll;
			string Fill;
		protected:
			int AddPosXGen;
			int AddPosYGen;
			bool IsVisibleGen;
			size OutSizeGen;
			size InSizeGen;
		public:
			ReserveCode PosXValue;
			ReserveCode PosYValue;
			ReserveCode ScrollValue;
			Element* _ref_ FillValue;
		public:
			View() : Name(nullptr), PosX(nullptr), PosY(nullptr), Width(nullptr), Height(nullptr), Scroll(nullptr), Fill(nullptr),
				AddPosXGen(0), AddPosYGen(0), IsVisibleGen(false), OutSizeGen(size::zero()), InSizeGen(size::zero()),
				PosXValue(CodeNull), PosYValue(CodeNull), ScrollValue(CodeNull), FillValue(nullptr) {Type = TypeView;}
			virtual ~View()
			{
				Name = BxUtilGlobal::StrFree(Name);
				PosX = BxUtilGlobal::StrFree(PosX);
				PosY = BxUtilGlobal::StrFree(PosY);
				Width = BxUtilGlobal::StrFree(Width);
				Height = BxUtilGlobal::StrFree(Height);
				Scroll = BxUtilGlobal::StrFree(Scroll);
				Fill = BxUtilGlobal::StrFree(Fill);
			}
		public:
			void SetAttb(string name, string posx, string posy, string width, string height, string scroll, string fill)
			{
				SetStep(StepNamed);
				Name = BxUtilGlobal::StrCpyWithAlloc(name);
				PosX = BxUtilGlobal::StrCpyWithAlloc(BxString("posx.") + posx);
				PosY = BxUtilGlobal::StrCpyWithAlloc(BxString("posy.") + posy);
				Width = BxUtilGlobal::StrCpyWithAlloc(width);
				Height = BxUtilGlobal::StrCpyWithAlloc(height);
				Scroll = BxUtilGlobal::StrCpyWithAlloc(BxString("scroll.") + scroll);
				Fill = BxUtilGlobal::StrCpyWithAlloc(fill);
			}
			void AdjustScroll(int posx, int posy)
			{
				AddPosXGen = posx;
				AddPosYGen = posy;
			}
			bool GetSize(size& outsize, size& insize)
			{
				if(!IsVisibleGen) return false;
				outsize = OutSizeGen;
				insize = InSizeGen;
				return true;
			}
		protected:
			virtual void Linking(const Element::VarMapPtr& Map)
			{
				SetStep(StepLinked);
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(PosX) && *Map.Access(PosX));
				PosXValue = (ReserveCode) (*Map.Access(PosX))->GetValue();
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(PosY) && *Map.Access(PosY));
				PosYValue = (ReserveCode) (*Map.Access(PosY))->GetValue();
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Scroll) && *Map.Access(Scroll));
				ScrollValue = (ReserveCode) (*Map.Access(Scroll))->GetValue();
				BxAssert("BxGUI<연결작업에 실패했습니다>", Map.Access(Fill) && *Map.Access(Fill));
				BxAssert("BxGUI<FILL용 엘리먼트가 아닙니다>", (*Map.Access(Fill))->IsValidFill());
				FillValue = *Map.Access(Fill);
			}
			virtual rect Rendering(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect, rect _out_ InRect)
			{
				InRect = Rect;
				rect OutRect = Rect;
				const int RectWidth = Rect.r - Rect.l;
				const int RectHeight = Rect.b - Rect.t;
				if(0 < RectWidth && 0 < RectHeight)
				{
					// 동적데이터
					int WidthValue = 0, HeightValue = 0;
					if(Map.Access(Width)) WidthValue = (*Map.Access(Width))->GetValue();
					else WidthValue = BxUtilGlobal::AtoI(Width) * ((Width[BxUtilGlobal::StrLen(Width) - 1] == '%')? -1 : 1);
					if(Map.Access(Height)) HeightValue = (*Map.Access(Height))->GetValue();
					else HeightValue = BxUtilGlobal::AtoI(Height) * ((Height[BxUtilGlobal::StrLen(Height) - 1] == '%')? -1 : 1);
					if(WidthValue < 0) WidthValue = RectWidth * -WidthValue / 100;
					if(HeightValue < 0) HeightValue = RectHeight * -HeightValue / 100;
					InRect.l -= AddPosXGen;
					InRect.t -= AddPosYGen;
					// 구획작업
					switch(PosXValue)
					{
					case CodeLeft: InRect.r = InRect.l + BxUtilGlobal::Max(0, WidthValue); break;
					case CodeRight: InRect.l = InRect.r - BxUtilGlobal::Max(0, WidthValue); break;
					}
					switch(PosYValue)
					{
					case CodeTop: InRect.b = InRect.t + BxUtilGlobal::Max(0, HeightValue); break;
					case CodeBottom: InRect.t = InRect.b - BxUtilGlobal::Max(0, HeightValue); break;
					}
					// 상태기록
					IsVisibleGen = true;
					OutSizeGen = WH(OutRect.r - OutRect.l, OutRect.b - OutRect.t);
					InSizeGen = WH(InRect.r - InRect.l, InRect.b - InRect.t);
					// 칠하기
					if(FillValue && FillValue->GetType() != TypeNull)
						FillValue->Filling(Draw, InRect);
				}
				else IsVisibleGen = false;
				return OutRect;
			}
			virtual bool OnEvent(const sysevent& Event)
			{
				if(FillValue && FillValue->GetType() != TypeNull)
					return FillValue->OnEvent(Event);
				return false;
			}
		};
	public:
		Layout() {CurFocus = &Top;}
		~Layout() {}
	public:
		void PreProcessing()
		{
			Top.PreProcessingTree();
		}
		void Processing()
		{
			Top.ProcessingTree();
		}
		void Linking(const Element::VarMapPtr& Map)
		{
			Top.LinkingTree(Map);
		}
		void Push(int i)
		{
			CurFocus = CurFocus->GetChild(i);
		}
		void Pull()
		{
			CurFocus = CurFocus->GetParent();
		}
		void AddSplit(Element::VarMapPtr& Map, string Name, string Pos, string Size, string Fill)
		{
			Split* NewData = BxNew(Split);
			NewData->SetAttb(Name, Pos, Size, Fill);
			CurFocus->AddChild(NewData);
			if(*Name)
			{
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
				Map[Name] = NewData;
			}
		}
		void AddOutline(Element::VarMapPtr& Map, string Name, string Size, string Fill)
		{
			Outline* NewData = BxNew(Outline);
			NewData->SetAttb(Name, Size, Fill);
			CurFocus->AddChild(NewData);
			if(*Name)
			{
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
				Map[Name] = NewData;
			}
		}
		void AddExtra(Element::VarMapPtr& Map, string Name, string Width, string Height, string Fill)
		{
			Extra* NewData = BxNew(Extra);
			NewData->SetAttb(Name, Width, Height, Fill);
			CurFocus->AddChild(NewData);
			if(*Name)
			{
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
				Map[Name] = NewData;
			}
		}
		void AddView(Element::VarMapPtr& Map, string Name, string PosX, string PosY, string Width, string Height, string Scroll, string Fill)
		{
			View* NewData = BxNew(View);
			NewData->SetAttb(Name, PosX, PosY, Width, Height, Scroll, Fill);
			CurFocus->AddChild(NewData);
			if(*Name)
			{
				BxAssert("BxGUI<중복된 컨텐트명입니다>", !Map.Access(Name));
				Map[Name] = NewData;
			}
		}
		void Rendering(const Element::VarMapPtr& Map, BxDraw& Draw, rect Rect)
		{
			Top.RenderingTree(Map, Draw, Rect);
		}
		bool OnEvent(const sysevent& Event)
		{
			return Top.OnEventTree(Event);
		}
	};
	/// @endcond

	Section CurSection;
	BxVar<Layout> Layout;
	BxVar<Content> Content;
	Element::VarMapPtr Map;
};
