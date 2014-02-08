#pragma once
#include <BxString.hpp>

//! \brief XML 관리
class BxXml
{
public:
	enum State {ERROR = -1, BLANK = 0, OPEN, CLOSE, OPENCLOSE, ENDOFFILE};

	// DEFINE
	void DefineTag(const int tagID, string tagName)
	{
		BxASSERT("BxXml", Mode == DEFINE);
		Tag[tagID].TagName = tagName;
	}
	void DefineAttb(const int tagID, const int attbID, string attbName, int intDefault)
	{
		BxASSERT("BxXml", Mode == DEFINE);
		Tag[tagID].AttbName[attbID] = attbName;
		Tag[tagID].AttbValue[attbID].Format("<>:<A>", BxARG(intDefault));
	}
	void DefineAttb(const int tagID, const int attbID, string attbName, string strDefault)
	{
		BxASSERT("BxXml", Mode == DEFINE);
		Tag[tagID].AttbName[attbID] = attbName;
		Tag[tagID].AttbValue[attbID] = strDefault;
	}

	// READ
	bool ReadBegin(string fileName)
	{
		BxASSERT("BxXml", Mode == DEFINE);
		File = BxCore::File::Open(fileName, "rb");
		if(File)
		{
			const int FileLen = BxCore::File::GetSize(File);
			Xml = BxNew_Array(char, FileLen + 1);
			BxCore::File::Read(File, (void*) Xml, FileLen);
			((char*) Xml)[FileLen] = '\0';
			BxCore::File::Close(File);
			File = nullptr;
		}
		else return false;
		Mode = READ;
		return true;
	}
	void ReadEnd()
	{
		BxASSERT("BxXml", Mode == READ);
		TagStack.Reset();
		BxDelete_Array(Xml);
		Xml = nullptr;
		Focus = 0;
		TagID = -1;
		Mode = DEFINE;
	}
	const int NextTag(State* state = nullptr)
	{
		BxASSERT("BxXml", Mode == READ);
		TagStack.Reset();
		Focus = TagStack[0].Read(Xml, Focus, state);
		string CompareName = TagStack[0].TagName;
		for(int i = 0; i < Tag.Length(); ++i)
			if(Tag[i].TagName.CompareNoCase(CompareName) == same)
				return (TagID = i);
		return (TagID = -1);
	}
	bool ReadAttb(const int attbID, int& intValue)
	{
		BxASSERT("BxXml", Mode == READ);
		if(TagID == -1) return false;
		Unit& OneTag = Tag[TagID];
		if(OneTag.AttbName.Length() <= attbID) return false;
		string CompareName = OneTag.AttbName[attbID];
		for(int i = 0; i < TagStack[0].AttbName.Length(); ++i)
			if(TagStack[0].AttbName[i].CompareNoCase(CompareName) == same)
			{
				intValue = BxUtilGlobal::AtoI(TagStack[0].AttbValue[i]);
				return true;
			}
		intValue = BxUtilGlobal::AtoI(OneTag.AttbValue[attbID]);
		return true;
	}
	bool ReadAttb(const int attbID, BxString& strValue)
	{
		BxASSERT("BxXml", Mode == READ);
		if(TagID == -1) return false;
		Unit& OneTag = Tag[TagID];
		if(OneTag.AttbName.Length() <= attbID) return false;
		string CompareName = OneTag.AttbName[attbID];
		for(int i = 0; i < TagStack[0].AttbName.Length(); ++i)
			if(TagStack[0].AttbName[i].CompareNoCase(CompareName) == same)
			{
				strValue = TagStack[0].AttbValue[i];
				return true;
			}
		strValue = OneTag.AttbValue[attbID];
		return true;
	}
	bool ReadAttb(const int attbID, BxVar<byte>& hexValue)
	{
		BxASSERT("BxXml", Mode == READ);
		if(TagID == -1) return false;
		Unit& OneTag = Tag[TagID];
		if(OneTag.AttbName.Length() <= attbID) return false;
		string CompareName = OneTag.AttbName[attbID];
		for(int i = 0; i < TagStack[0].AttbName.Length(); ++i)
			if(TagStack[0].AttbName[i].CompareNoCase(CompareName) == same)
			{
				Unit::AtoH(TagStack[0].AttbValue[i], hexValue);
				return true;
			}
		Unit::AtoH(OneTag.AttbValue[attbID], hexValue);
		return false;
	}
	bool ReadContent(BxString& content)
	{
		BxASSERT("BxXml", Mode == READ);
		if(TagID == -1) return false;
		Unit& OneTag = Tag[TagID];
		content = TagStack[0].Content;
		return true;
	}

	// WRITE
	bool WriteBegin(string fileName)
	{
		BxASSERT("BxXml", Mode == DEFINE);
		File = BxCore::File::Open(fileName, "wb");
		if(!File) return false;
		Mode = WRITE;
		return true;
	}
	void WriteEnd()
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(File)
		{
			BxCore::File::Close(File);
			File = nullptr;
		}
		TagStack.Reset();
		Mode = DEFINE;
	}
	void PushTag(const int tagID)
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			if(!OneTag.IsWritten)
			{
				OneTag.IsWritten = true;
				IsWriting = OneTag.Write(File, OPEN, (IsWriting)? 0 : TagStack.Length() - 1);
			}
		}
		TagStack[LAST] = Tag[TagID];
	}
	void PopTag()
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			IsWriting = OneTag.Write(File, (OneTag.IsWritten)? CLOSE : OPENCLOSE,
				(IsWriting)? 0 : TagStack.Length() - 1);
			TagStack.Delete(END);
		}
	}
	void WriteAttb(const int attbID, int intValue)
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			OneTag.AttbValue[attbID].Format("<>:<A>", BxARG(intValue));
		}
	}
	void WriteAttb(const int attbID, string strValue)
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			OneTag.AttbValue[attbID] = strValue;
		}
	}
	void WriteAttb(const int attbID, BxVar<byte>& hexValue)
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			OneTag.AttbValue[attbID] = "";
			for(int i = 0; i < hexValue.Length(); ++i)
			{
				const byte UpCode = (hexValue[i] & 0xF0) >> 4;
				const byte DownCode = (hexValue[i] & 0x0F);
				OneTag.AttbValue[attbID] += (UpCode < 10)? '0' + UpCode : 'A' + UpCode - 10;
				OneTag.AttbValue[attbID] += (DownCode < 10)? '0' + DownCode : 'A' + DownCode - 10;
			}
		}
	}
	void WriteContent(string content)
	{
		BxASSERT("BxXml", Mode == WRITE);
		if(TagStack.Length())
		{
			Unit& OneTag = TagStack[END];
			OneTag.Content = content;
		}
	}

	BxXml() {Init(-1);}
	virtual ~BxXml() {Quit(-1);}
	void Init(int index)
	{
		Mode = DEFINE;
		Xml = nullptr;
		Focus = 0;
		TagID = -1;
		File = nullptr;
		IsWriting = false;
	}
	void Quit(int index)
	{
		BxDelete_Array(Xml);
		if(File)
		{
			BxCore::File::Close(File);
			File = nullptr;
		}
	}

private:
	class Unit
	{
	public:
		bool IsWritten;
		BxString TagName;
		BxString Content;
		BxVar<BxString> AttbName;
		BxVar<BxString> AttbValue;
		Unit() : IsWritten(false) {}

		Unit& operator=(Unit& rhs)
		{
			TagName = rhs.TagName;
			Content = rhs.Content;
			AttbName = rhs.AttbName;
			AttbValue = rhs.AttbValue;
			return *this;
		}
		int Read(string xml, int focus, State* state = nullptr)
		{
			TagName = "";
			Content = "";
			AttbName.Reset();
			AttbValue.Reset();
			bool IsFinded = false;
			bool IsNameTurn = true;
			int AttbStringFlag = 0;
			int AttbIndex = 0;
			if(state) *state = BLANK;
			int i = 0;
			for(i = focus; xml[i] != '\0'; ++i)
			{
				if(IsFinded)
				{
					if(0 < AttbStringFlag)
					{
						if(AttbStringFlag == 1 && xml[i] != '\"')
							AttbValue[AttbIndex] += xml[i];
						else if(AttbStringFlag == 2 && xml[i] != ' ')
							AttbValue[AttbIndex] += xml[i];
						else
						{
							++AttbIndex;
							AttbStringFlag = 0;
						}
					}
					else if(xml[i] == '>')
					{
						if(state && *state == BLANK)
						{
							if(xml[i - 1] == '/')
								*state = OPENCLOSE;
							else *state = OPEN;
						}
						if(state && *state == OPEN)
						{
							bool IsContentEnable = false;
							int j = 0;
							for(j = i + 1; xml[j] != '\0'; ++j)
							{
								if(xml[j] == '<')
								{
									if(!IsContentEnable)
										Content = "";
									return j - 1;
								}
								else
								{
									Content += xml[j];
									if(!IsContentEnable && xml[j] != ' ' && xml[j] != '\t'
										 && xml[j] != '\r' && xml[j] != '\n')
										 IsContentEnable = true;
								}
							}
							return j;
						}
						return i + 1;
					}
					else if(IsNameTurn)
					{
						if(xml[i] != ' ' && xml[i] != '/'  && xml[i] != '\t' && xml[i] != '\r' && xml[i] != '\n')
							TagName += xml[i];
						else if(0 < TagName.GetLength())
							IsNameTurn = false;
					}
					else
					{
						if(xml[i] != ' ' && xml[i] != '=' && xml[i] != '\"' && xml[i] != '/' && xml[i] != '\t' && xml[i] != '\r' && xml[i] != '\n')
						{
							if(AttbStringFlag == -2)
							{
								AttbStringFlag = 2;
								AttbValue[AttbIndex] += xml[i];
							}
							else AttbName[AttbIndex] += xml[i];
						}
						else if(xml[i] == '\"')
							AttbStringFlag = 1;
						else if(xml[i] == '=')
							AttbStringFlag = -2;
					}
				}
				else if(xml[i] == '<' && xml[i + 1] != '?')
				{
					IsFinded = true;
					if(xml[i + 1] == '/')
					{
						if(state) *state = CLOSE;
						++i;
					}
				}
			}
			if(state)
			{
				if(IsFinded) *state = ERROR;
				else *state = ENDOFFILE;
			}
			return i;
		}
		bool Write(id_file file, State state, int level)
		{
			if(!file) return false;
			BxString Sum, Temp;
			while(0 < level--) Sum += "    ";
			switch(state)
			{
			case OPEN:
			case OPENCLOSE:
				{
					Temp.Format("<>:<<<A>", BxARG(TagName));
					Sum += Temp;
					for(int i = 0; i < AttbName.Length(); ++i)
					{
						Temp.Format("<>: <A>=<DQ><A><DQ>", BxARG(AttbName[i], AttbValue[i]));
						Sum += Temp;
					}
					Temp.Format("<>:<A><A>", BxARG((state == OPEN)? ">" : "/>", (Content.GetLength())? (string) Content : "\r\n"));
					Sum += Temp;
				}
				break;
			case CLOSE:
				{
					Temp.Format("<>:<<<FS><A>>><R><N>", BxARG(TagName));
					Sum += Temp;
				}
				break;
			default:
				return false;
			}
			BxCore::File::Write(file, (string) Sum, Sum.GetLength());
			return (0 < Content.GetLength());
		}
		global_func void AtoH(BxString& strValue, BxVar<byte>& hexValue)
		{
			hexValue.Reset();
			string String = strValue;
			const int Length = strValue.GetLength();
			for(int i = 1; i < Length; i += 2)
			{
				hexValue[LAST] = 0;
				for(int j = i - 1; j <= i; ++j)
				{
					int OneValue = 0;
					if('0' <= String[j] && String[j] <= '9')
						OneValue = String[j] - '0';
					else if('A' <= String[j] && String[j] <= 'F')
						OneValue = String[j] - 'A' + 10;
					else if('a' <= String[j] && String[j] <= 'f')
						OneValue = String[j] - 'a' + 10;
					hexValue[END] = (hexValue[END] << 4) | OneValue;
				}
			}
		}
	};

	enum {DEFINE, READ, WRITE} Mode;
	BxVar<Unit> Tag;
	BxVar<Unit> TagStack;
	string Xml;
	int Focus;
	int TagID;
	id_file File;
	bool IsWriting;
};
