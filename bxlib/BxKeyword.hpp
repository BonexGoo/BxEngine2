#pragma once
#include <BxString.hpp>

#define BxKeyEvent(NAME) (BxKeyword::Access<BxKeyword::Event>::ByTemplate<__COUNTER__>("" NAME).LetString())
#define BxKeyCheck(NAME) (BxKeyword::Access<BxKeyword::Check>::ByTemplate<__COUNTER__>("" NAME).LetBool())
#define BxKeyRadio_Get(NAME) (BxKeyword::Access<BxKeyword::Radio>::ByTemplate<__COUNTER__>("" NAME).GetConst())
#define BxKeyRadio_On(NAME) (BxKeyword::Access<BxKeyword::Radio>::ByTemplate<__COUNTER__>("" NAME).SetOn())
#define BxKeyRadio_Off(NAME) (BxKeyword::Access<BxKeyword::Radio>::ByTemplate<__COUNTER__>("" NAME).SetOff())
#define BxKeyNumber(NAME) (BxKeyword::Access<BxKeyword::Number>::ByTemplate<__COUNTER__>("" NAME).LetInt())
#define BxKeyString(NAME) (BxKeyword::Access<BxKeyword::String>::ByTemplate<__COUNTER__>("" NAME).LetString())
#define BxKeyEventBySearch(NAME) (BxKeyword::Access<BxKeyword::Event>::BySearch(NAME, true)->LetString())
#define BxKeyCheckBySearch(NAME) (BxKeyword::Access<BxKeyword::Check>::BySearch(NAME, true)->LetBool())
#define BxKeyRadioBySearch_Get(NAME) (BxKeyword::Access<BxKeyword::Radio>::BySearch(NAME, true)->GetConst())
#define BxKeyRadioBySearch_On(NAME) (BxKeyword::Access<BxKeyword::Radio>::BySearch(NAME, true)->SetOn())
#define BxKeyRadioBySearch_Off(NAME) (BxKeyword::Access<BxKeyword::Radio>::BySearch(NAME, true)->SetOff())
#define BxKeyNumberBySearch(NAME) (BxKeyword::Access<BxKeyword::Number>::BySearch(NAME, true)->LetInt())
#define BxKeyStringBySearch(NAME) (BxKeyword::Access<BxKeyword::String>::BySearch(NAME, true)->LetString())
#define IsExistBxKeyEvent(NAME) (BxKeyword::Access<BxKeyword::Event>::BySearch(NAME, false) != nullptr)
#define IsExistBxKeyCheck(NAME) (BxKeyword::Access<BxKeyword::Check>::BySearch(NAME, false) != nullptr)
#define IsExistBxKeyRadio(NAME) (BxKeyword::Access<BxKeyword::Radio>::BySearch(NAME, false) != nullptr)
#define IsExistBxKeyNumber(NAME) (BxKeyword::Access<BxKeyword::Number>::BySearch(NAME, false) != nullptr)
#define IsExistBxKeyString(NAME) (BxKeyword::Access<BxKeyword::String>::BySearch(NAME, false) != nullptr)

//! \brief 직렬자료 제공
class BxKeyword
{
	BxString FileName;

	global_func inline BxVar<BxKeyword>& LetPool()
	{BxVar<BxKeyword>* _; BxSINGLETON(_, BxVar<BxKeyword>, 1); return *_;}

	global_func inline uint MakeParentID(bool IsAdd = true)
	{
		global_data uint ParentID = 1;
		return (IsAdd)? ParentID++ : ParentID;
	}

public:
	BxKeyword() : FileName("") {}
	~BxKeyword() {}

	global_func bool IsLoaded(string filename)
	{
		BxVar<BxKeyword>& Pool = LetPool();
		for(int i = 0; i < Pool.Length(); ++i)
			if(Pool[i].FileName.Compare(filename) == same)
				return true;
		return false;
	}

	global_func bool Load(string filename, callback_event method = nullptr)
	{
		BxVar<BxKeyword>& Pool = LetPool();
		Pool[LAST].FileName = filename;
		for(int i = 0; i < Pool.Length() - 1; ++i)
		{
			if(Pool[i].FileName.Compare(filename) != same)
				continue;
			Pool.Delete(i);
			break;
		}
		id_file KeywordFile = BxCore::File::Open(filename, "rb");
		if(KeywordFile)
		{
			const int KeywordBufferSize = BxCore::File::GetSize(KeywordFile);
			string_rw KeywordBuffer = (string_rw) BxUtilGlobal::StrAlloc(KeywordBufferSize);
			BxCore::File::Read(KeywordFile, KeywordBuffer, KeywordBufferSize);
			KeywordBuffer[KeywordBufferSize] = '\0';
			BxCore::File::Close(KeywordFile);
			LoadScript(KeywordBuffer, method);
			KeywordBuffer = (string_rw) BxUtilGlobal::StrFree(KeywordBuffer);
			return true;
		}
		return false;
	}

	global_func void LoadScript(string buffer, callback_event method)
	{
		enum bekon_type:int {bekon_begin, bekon_finish, bekon_end,
			bekon_group, bekon_event, bekon_check, bekon_radio, bekon_number, bekon_string, bekon_max};
		global_data string Command[bekon_max] = {
			"bekon_script_area<begin>", "bekon_script_area<finish>", "bekon_script_area<end>", // 분기명령
			"group(", "event(", "check(", "radio(", "number(", "string("}; // 키워드명령
		global_data const int CommandLength[bekon_max] = {
			BxUtilGlobal::StrLen(Command[0]), BxUtilGlobal::StrLen(Command[1]), BxUtilGlobal::StrLen(Command[2]),
			BxUtilGlobal::StrLen(Command[3]), BxUtilGlobal::StrLen(Command[4]), BxUtilGlobal::StrLen(Command[5]),
			BxUtilGlobal::StrLen(Command[6]), BxUtilGlobal::StrLen(Command[7]), BxUtilGlobal::StrLen(Command[8])};
		// Flag
		bool IsScriptArea = false;
		bool IsWaitReturn = false;
		bool IsInputMode = false;
		// Group
		BxVar<int> GroupArrayIndex;
		BxVar<int> GroupNameLength;
		BxString GroupName("");
		GroupArrayIndex[0] = 0;
		// Reserved
		BxVar<int> ReservedKeywordType;
		BxString ReservedKeywordName("");
		ReservedKeywordType[0] = bekon_group;
		// Temp
		BxVar<int> TempType;
		BxVar<BxString> TempName;
		for(int i = 0; buffer[i] != '\0'; i += (buffer[i] != '\0'))
		{
			if(IsWaitReturn) IsWaitReturn = (buffer[i] != '\n');
			else if(buffer[i] == '/' && buffer[i + 1] == '/') IsWaitReturn = true;
			else if(buffer[i] == '@')
			{
				int TempIndex = 0;
				while(buffer[i + 1] != '\0' && '0' <= buffer[i + 1] && buffer[i + 1] <= '9')
					TempIndex = (TempIndex * 10) + (buffer[++i] - '0');
				ReservedKeywordType[END] = TempType[TempIndex];
				ReservedKeywordName = TempName[TempIndex];
			}
			else if(buffer[i] == '(' || buffer[i] == '<' || buffer[i] == '>') // <는 if때 필요
			{
				for(int j = 0, jend = (IsScriptArea)? bekon_max : bekon_end; j < jend; ++j)
				{
					// 명령어선정
					if(i < CommandLength[j] - 1) continue;
					bool IsMatch = true;
					for(int k = 0; IsMatch && k < CommandLength[j]; ++k)
						if(buffer[i - k] != Command[j][CommandLength[j] - k - 1])
							IsMatch = false;
					if(!IsMatch) continue;
					// 기능실행
					if(j == bekon_begin) IsScriptArea = true;
					else if(j == bekon_finish) return;
					else if(j == bekon_end) IsScriptArea = false;
					else if(buffer[i] == '(')
					{
						BxString Name("");
						while(buffer[++i] != '\0' && buffer[i] != ')' && buffer[i] != '@')
							Name += buffer[i];
						if(buffer[i] != '\0')
						{
							ReservedKeywordType[END] = (bekon_type) j;
							ReservedKeywordName = Name;
						}
						if(buffer[i] == '@')
						{
							int TempIndex = 0;
							while(buffer[++i] != '\0' && buffer[i] != ')')
								TempIndex = (TempIndex * 10) + (buffer[i] - '0');
							TempType[TempIndex] = (bekon_type) j;
							TempName[TempIndex] = Name;
						}
					}
					IsInputMode = false;
					break;
				}
			}
			else if(buffer[i] == '{' || buffer[i] == '[')
			{
				if(0 < ReservedKeywordName.GetLength())
				{
					const bool IsMiddleName = (0 < GroupName.GetLength());
					GroupNameLength[LAST] = ReservedKeywordName.GetLength() + IsMiddleName;
					if(IsMiddleName) GroupName += ".";
					GroupName += ReservedKeywordName;
				}
				if(buffer[i] == '[')
				{
					BxString Index(".%d", nullptr, GroupArrayIndex[END]++);
					GroupArrayIndex[LAST] = 0;
					GroupNameLength[LAST] = Index.GetLength();
					GroupName += Index;
					if(bekon_group < ReservedKeywordType[END])
						IsInputMode = true;
				}
				ReservedKeywordType[LAST] = ReservedKeywordType[END];
				ReservedKeywordName = "";
			}
			else if(buffer[i] == '}' || buffer[i] == ']' || buffer[i] == '.')
			{
				if(0 < GroupNameLength.Length())
				{
					GroupName = GroupName.Left(GroupName.GetLength() - GroupNameLength[END]);
					GroupNameLength.Delete(END);
				}
				if(buffer[i] == '.') GroupArrayIndex[END] = 0;
				else if(buffer[i] == ']' && 1 < GroupArrayIndex.Length())
					GroupArrayIndex.Delete(END);
				if(buffer[i] != '.' && 1 < ReservedKeywordType.Length())
					ReservedKeywordType.Delete(END);
				ReservedKeywordName = "";
			}
			else if(buffer[i] == '=' && bekon_group < ReservedKeywordType[END])
				IsInputMode = true;
			else if(IsInputMode)
			{
				if(ReservedKeywordType[END] == bekon_event || ReservedKeywordType[END] == bekon_string)
				{
					if(buffer[i] == '\"' || buffer[i] == '\'')
					{
						BxString Value("");
						const char EndCode = buffer[i];
						while(buffer[++i] != '\0' && buffer[i] != EndCode)
							Value += buffer[i];
						BxString FullName((0 < GroupName.GetLength() && 0 < ReservedKeywordName.GetLength())?
							GroupName + "." + ReservedKeywordName : GroupName + ReservedKeywordName);
						switch(ReservedKeywordType[END])
						{
						case bekon_event: MakeEvent(FullName, Value, method); break;
						case bekon_string: Access<String>::BySearch(FullName, true)->LetString() = Value; break;
						}
						IsInputMode = false;
					}
				}
				else if(('0' <= buffer[i] && buffer[i] <= '9') || ('a' <= buffer[i] && buffer[i] <= 'z') || buffer[i] == '-' || buffer[i] == '+')
				{
					BxString Value(buffer[i]);
					while(buffer[++i] != '\0' && buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\r' && buffer[i] != '\n'
						&& buffer[i] != '(' && buffer[i] != '}' && buffer[i] != ']' && !(buffer[i] == '/' && buffer[i + 1] == '/'))
						Value += buffer[i];
					if(buffer[i] != '(')
					{
						BxString FullName((0 < GroupName.GetLength() && 0 < ReservedKeywordName.GetLength())?
							GroupName + "." + ReservedKeywordName : GroupName + ReservedKeywordName);
						switch(ReservedKeywordType[END])
						{
						case bekon_check: Access<Check>::BySearch(FullName, true)->LetBool() = (Value.GetLength() && Value[0] == 't')? true : false; break;
						case bekon_radio: MakeRadio(FullName, (Value.GetLength())? BxUtilGlobal::AtoI(Value) : -1); break;
						case bekon_number: Access<Number>::BySearch(FullName, true)->LetInt() = (Value.GetLength())? BxUtilGlobal::AtoI(Value) : 0; break;
						}
					}
					if(buffer[i] == '(' || buffer[i] == '}' || buffer[i] == ']' || buffer[i] == '/') --i;
					IsInputMode = false;
				}
			}
		}
	}

	global_func void Save(string filename)
	{
		id_file KeywordFile = BxCore::File::Open(filename, "wb");
		if(KeywordFile)
		{
			BxCore::File::WriteString(KeywordFile, "bekon_script_area<begin>\r\n");
			// event
			{
				BxVar< Access<Event> >& EventPool = Access<Event>::LetPool();
				for(int i = 0; i < EventPool.Length(); ++i)
				{
					if(i == 0) BxCore::File::WriteString(KeywordFile, "\r\n// event\r\n");
					BxCore::File::WriteString(KeywordFile, "event(");
					BxCore::File::WriteString(KeywordFile, EventPool[i].GetKeyword());
					BxCore::File::WriteString(KeywordFile, ") = \"");
					BxCore::File::WriteString(KeywordFile, EventPool[i].LetString());
					BxCore::File::WriteString(KeywordFile, "\"\r\n");
				}
			}
			// check
			{
				BxVar< Access<Check> >& CheckPool = Access<Check>::LetPool();
				for(int i = 0; i < CheckPool.Length(); ++i)
				{
					if(i == 0) BxCore::File::WriteString(KeywordFile, "\r\n// check\r\n");
					BxCore::File::WriteString(KeywordFile, "check(");
					BxCore::File::WriteString(KeywordFile, CheckPool[i].GetKeyword());
					BxCore::File::WriteString(KeywordFile, ") = ");
					if(CheckPool[i].LetBool()) BxCore::File::WriteString(KeywordFile, "true\r\n");
					else BxCore::File::WriteString(KeywordFile, "false\r\n");
				}
			}
			// radio
			{
				BxVar< Access<Radio> >& RadioPool = Access<Radio>::LetPool();
				const uint ParentIDLength = MakeParentID(false);
				bool IsFirstFind = true;
				for(uint p = 0; p < ParentIDLength; ++p)
				{
					bool IsFind = false;
					for(int i = 0; !IsFind && i < RadioPool.Length(); ++i)
						if(RadioPool[i].LetParentID() == p)
							IsFind = true;
					if(IsFind)
					{
						if(IsFirstFind)
						{
							IsFirstFind = false;
							BxCore::File::WriteString(KeywordFile, "\r\n// radio\r\n");
						}
						BxCore::File::WriteString(KeywordFile, "radio(");
						int CurrentRadioIndex = 0;
						int SelectRadioIndex = -1;
						for(int i = 0; i < RadioPool.Length(); ++i)
						{
							if(RadioPool[i].LetParentID() != p) continue;
							if(0 < CurrentRadioIndex) BxCore::File::WriteString(KeywordFile, "/");
							BxCore::File::WriteString(KeywordFile, RadioPool[i].GetKeyword());
							if(RadioPool[i].GetConst())
								SelectRadioIndex = CurrentRadioIndex;
							++CurrentRadioIndex;
						}
						BxCore::File::WriteString(KeywordFile, ") = ");
						BxString SelectNumber;
						SelectNumber.Format("%d\r\n", SelectRadioIndex);
						BxCore::File::WriteString(KeywordFile, SelectNumber);
					}
				}
			}
			// number
			{
				BxVar< Access<Number> >& NumberPool = Access<Number>::LetPool();
				for(int i = 0; i < NumberPool.Length(); ++i)
				{
					if(i == 0) BxCore::File::WriteString(KeywordFile, "\r\n// number\r\n");
					BxCore::File::WriteString(KeywordFile, "number(");
					BxCore::File::WriteString(KeywordFile, NumberPool[i].GetKeyword());
					BxString NumberValue;
					NumberValue.Format(") = %d\r\n", NumberPool[i].LetInt());
					BxCore::File::WriteString(KeywordFile, NumberValue);
				}
			}
			// string
			{
				BxVar< Access<String> >& StringPool = Access<String>::LetPool();
				for(int i = 0; i < StringPool.Length(); ++i)
				{
					if(i == 0) BxCore::File::WriteString(KeywordFile, "\r\n// string\r\n");
					BxCore::File::WriteString(KeywordFile, "string(");
					BxCore::File::WriteString(KeywordFile, StringPool[i].GetKeyword());
					BxCore::File::WriteString(KeywordFile, ") = \"");
					BxCore::File::WriteString(KeywordFile, StringPool[i].LetString());
					BxCore::File::WriteString(KeywordFile, "\"\r\n");
				}
			}
			BxCore::File::WriteString(KeywordFile, "\r\nbekon_script_area<end>\r\n");
			BxCore::File::Close(KeywordFile);
		}
	}

	global_func bool IsExist(keyword type, string name)
	{
		switch(type)
		{
		case keyword_event: return (Access<Event>::BySearch(name, false) != nullptr);
		case keyword_check: return (Access<Check>::BySearch(name, false) != nullptr);
		case keyword_radio: return (Access<Radio>::BySearch(name, false) != nullptr);
		case keyword_number: return (Access<Number>::BySearch(name, false) != nullptr);
		case keyword_string: return (Access<String>::BySearch(name, false) != nullptr);
		}
		return false;
	}

	global_func bool CallEvent(string name)
	{
		Access<Event>* Result = Access<Event>::BySearch(name, false);
		if(!Result || !Result->LetMethodCB()) return false;
		Result->LetMethodCB()(false, name, nullptr);
		return true;
	}

	global_func void MakeEvent(string name, string value, callback_event method)
	{
		Access<Event>* Result = Access<Event>::BySearch(name, true);
		Result->LetString() = value;
		Result->LetMethodCB() = method;
	}

	global_func void MakeRadio(string names, int value)
	{
		uint NewParentID = MakeParentID();
		int RadioCount = 0;
		Access<Radio>* ResultForSet = nullptr;
		do
		{
			BxString Name("");
			Name = *names;
			while(*names != '\0' && *(++names) != '/')
				Name += *names;
			Access<Radio>* Result = Access<Radio>::BySearch(Name, true);
			Result->LetParentID() = NewParentID;
			if(value == RadioCount++) ResultForSet = Result;
		}
		while(*names != '\0' && *(names++) == '/');
		if(ResultForSet) ResultForSet->SetOn();
	}

	class Clone
	{
	public:
		global_func void Link(uint id)
		{
		}

		global_func void Unlink()
		{
		}

		global_func Clone& Get(uint id)
		{
			global_data Clone a;
			return a;
		}

		int Reload(string searchword)
		{
			return 0;
		}

		void Remove()
		{
		}

		void SwapTo(uint id)
		{
		}
	};

	class Event {public: BxString Data; callback_event MethodCB; Event() : Data(""), MethodCB(nullptr) {}};
	class Check {public: bool Data; Check() : Data(false) {}};
	class Radio {public: bool Data; uint ParentID; Radio() : Data(false), ParentID(0) {}};
	class Number {public: int Data; Number() : Data(0) {}};
	class String {public: BxString Data; String() : Data("") {}};

	template<typename TYPE>
	class Access
	{
		BxString Keyword;
		TYPE Element;

	public:
		Access() : Keyword("") {}
		~Access() {}

		global_func inline BxVar<Access>& LetPool()
		{BxVar<Access>* _; BxSINGLETON(_, BxVar<Access>, 1); return *_;}

		template<int COUNT>
		global_func inline Access& ByTemplate(string name)
		{
			global_data Access* Result = BySearch(name, true);
			return *Result;
		}

		global_func Access* BySearch(string name, bool doCreate)
		{
			BxVar<Access>& Pool = LetPool();
			if(!Pool.Length())
			{
				if(doCreate)
				{
					Pool[0].Keyword = name;
					return &Pool[0];
				}
				else return nullptr;
			}
			int Middle = 0, Lower = 0, Upper = Pool.Length() - 1;
			compare Result = same;
			while(Lower <= Upper)
			{
				Middle = (Lower + Upper) / 2;
				Result = Pool[Middle].Keyword.Compare(name);
				if(Result == same)
					return &Pool[Middle];
				if(Result == param_is_more)
					Lower = Middle + 1;
				else Upper = Middle - 1;
			}
			if(doCreate)
			{
				if(Result == param_is_more) ++Middle;
				Pool.Insert(Middle).Keyword = name;
				return &Pool[Middle];
			}
			return nullptr;
		}

		inline string GetKeyword() {return Keyword;}
		inline int& LetInt() {return Element.Data;}
		inline bool& LetBool() {return Element.Data;}
		inline BxString& LetString() {return Element.Data;}
		inline bool GetConst() {return Element.Data;}
		void SetOn()
		{
			BxVar<Access>& Pool = LetPool();
			for(int i = 0; i < Pool.Length(); ++i)
				if(Pool[i].Element.ParentID == Element.ParentID)
					Pool[i].Element.Data = false;
			Element.Data = true;
		}
		inline void SetOff() {Element.Data = false;}
		inline callback_event& LetMethodCB() {return Element.MethodCB;}
		inline uint& LetParentID() {return Element.ParentID;}
	};
};
