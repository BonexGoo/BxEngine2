#pragma once
#include <BxString.hpp>
#include <BxVarVector.hpp>

//! \brief HTTP��� �����ۼ���, ���ϴٿ�ε�
class BxWebContent
{
	byte* ResponseBytes;
	int ResponseLength;

private:
	class CachePool
	{
	public:
		string Path;
		uint Size;
		uhuge Date;
		CachePool() : Path(nullptr), Size(0), Date(0) {}
		~CachePool() {Path = BxUtilGlobal::StrFree(Path);}
	};
	typedef BxVarVector<CachePool, 64> CachePoolList;
	global_func macro bool& IsInitPool() {global_data bool _IsInitPool = false; return _IsInitPool;}
	global_func macro CachePoolList& Pool() {global_data CachePoolList _Pool; return _Pool;}
	global_func macro int& PoolSumSize() {global_data int _PoolSumSize = 0; return _PoolSumSize;}
	global_func void OnFileSearch(bool isdirname, string _tmp_ name, void* data)
	{
		BxString Name(name); // ������ ���� ���ó���� ���ο� �����Ҵ��� �ʿ�
		if(!isdirname)
		{
			if(Name.GetFileExt().CompareNoCase("downloading") != same)
			{
				Pool()[LAST].Path = Name.CloneWithAlloc();
				PoolSumSize() += (Pool()[END].Size = BxCore::File::GetSize(Name));
				Pool()[END].Date = BxCore::File::GetDate(Name);
			}
		}
		else BxCore::File::SearchFiles(Name, OnFileSearch, data);
	}

public:
	BxWebContent() : ResponseBytes(nullptr), ResponseLength(0) {}
	~BxWebContent() {BxFree(ResponseBytes);}

	/*!
	\brief ĳ�������̸� �ޱ�
	\return ĳ�������̸�
	*/
	global_func inline string GetCachePath() {return ".webcontent_cache";}

	/*!
	\brief ������� �ޱ�
	\return ������� ����Ʈ�迭
	*/
	inline const byte* _tmp_ GetBytes() {return ResponseBytes;}

	/*!
	\brief ��������� ��������(������)
	\return ����Ʈ����
	*/
	inline int GetDataLength() {return ResponseLength;}

	/*!
	\brief ��������� ��������(��Ʈ��)
	\return ����Ʈ����
	*/
	inline int GetTextLength() {return BxUtilGlobal::StrLenEx((string) ResponseBytes);}

	/*!
	\brief HTTP��������
	\param Domain : ������ ����(��: "www.naver.com")
	\param Port : ��Ʈ��ȣ(HTTP�� ���� 80����Ʈ)
	\param Path : �ٿ���� ���� �Ǵ� ������ ������ ���(��: "Update/Run.aspx")
	\param Arguments : ������ ��� ������ ���ڵ�(��: "Data0=aaa&Data1=bbb&Data2=ccc")
	\param Timeout : �� �ܰ躰 �������ð�
	\param Progress : ���α׷����� ����
	\return �ۼ��� ��������
	*/
	bool Query(string Domain, ushort Port, string Path, string Arguments = nullptr, int Timeout = -1, callback_progress Progress = nullptr)
	{
		if(Timeout < 0) Timeout = 5000;
		// ���Ͽ���, �������� �� ����
		id_socket Socket = BxCore::Socket::Create();
		if(connect_connected == BxCore::Socket::Connect(Socket, Domain, Port, Timeout, Progress))
		{
			char RequestChars[4096];
			BxCore::Util::SnPrintf(RequestChars, 4096,
				"GET /%s%s%s HTTP/1.1\r\n"
				"Accept: text/html, application/xhtml+xml, */*\r\n"
				"Accept-Language: ko-KR\r\n"
				"User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n"
				"Accept-Encoding: deflate\r\n"//"Accept-Encoding: gzip, deflate\r\n"
				"Host: %s\r\n"
				"Connection: Keep-Alive\r\n"
				"\r\n", Path, (!Arguments || !*Arguments)? "" : "?", (!Arguments)? "" : Arguments, Domain);
			int SendResult = 0, SendedLength = 0;
			const int RequestCharsLength = BxUtilGlobal::StrLen(RequestChars);
			do
			{
				if(Progress && !Progress(-1, 0)) return false; // ������Ʈ
				SendResult = BxCore::Socket::Send(Socket, RequestChars + SendedLength, RequestCharsLength - SendedLength);
				if(SendResult < 0)
				{
					BxCore::Socket::Release(Socket);
					return false; // �۽Ž���
				}
			}
			while((SendedLength += SendResult) < RequestCharsLength);
			BxCore::System::Sleep(0);
		}
		else
		{
			BxCore::Socket::Release(Socket);
			return false; // �������
		}

		// HTTP��� ����
		char ResponseChars[4096];
		char* EndCodeSearchFocus = ResponseChars;
		int RecvResult = 0, RecvedLength = 0;
		int ErrorTime = 0, SleepTime = 0;
		const int HTMLEndCodeLength = BxUtilGlobal::StrLen("\r\n\r\n");
		while(0 <= (RecvResult = BxCore::Socket::Recv(Socket, ResponseChars + RecvedLength, (4096 - 1) - RecvedLength)))
		{
			if(Progress && !Progress(-1, 0)) return false; // ������Ʈ
			RecvedLength += RecvResult;
			// ���� ������ ���� ���
			if(0 < RecvResult)
			{
				ErrorTime = 0;
				SleepTime >>= 1;
				// HTML��� �����ڵ� �˻�
				if(HTMLEndCodeLength < RecvedLength)
				{
					ResponseChars[RecvedLength] = '\0';
					while(*(++EndCodeSearchFocus) && BxUtilGlobal::StrSameCount(EndCodeSearchFocus, "\r\n\r\n") != HTMLEndCodeLength);
					if(*EndCodeSearchFocus)
					{
						EndCodeSearchFocus += HTMLEndCodeLength;
						// �����ͻ����� ����
						char* ContentLengthSearchFocus = ResponseChars;
						while(++ContentLengthSearchFocus < EndCodeSearchFocus && BxUtilGlobal::StrSameCount(ContentLengthSearchFocus, "Content-Length:") != 15);
						if(ContentLengthSearchFocus == EndCodeSearchFocus)
						{
							// Content-Length�� ���� ��� 5MByte�� ��´�
							ResponseLength = 1024 * 1024 * 5;
						}
						else
						{
							ContentLengthSearchFocus += 15;
							ResponseLength = BxUtilGlobal::AtoI(ContentLengthSearchFocus, EndCodeSearchFocus - ContentLengthSearchFocus);
						}
						// ����Ȯ��
						BxFree(ResponseBytes);
						ResponseBytes = (byte*) BxAlloc(ResponseLength + 1);
						break;
					}
					else EndCodeSearchFocus = (char*) BxUtilGlobal::Max((int) ResponseChars, (int)(EndCodeSearchFocus - HTMLEndCodeLength));
				}
			}
			// ���� ������ ���� ���
			else if(Timeout < (ErrorTime += SleepTime))
			{
				BxCore::Socket::Release(Socket);
				return false; // ���ð��ʰ�
			}
			else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
		}
		if(RecvResult < 0)
		{
			BxCore::Socket::Release(Socket);
			return false; // ���Ž���
		}

		// ������ �̰�
		RecvedLength = ResponseChars + RecvedLength - EndCodeSearchFocus;
		BxCore::Util::MemMove(ResponseBytes, EndCodeSearchFocus, RecvedLength);
		if(Progress && !Progress(RecvedLength, ResponseLength)) // ���α׷���
			return false;

		// ������ ����
		RecvResult = 0;
		if(RecvedLength < ResponseLength)
		{
			// �߰�����
			ErrorTime = 0, SleepTime = 0;
			while(0 <= (RecvResult = BxCore::Socket::Recv(Socket, ResponseBytes + RecvedLength, ResponseLength - RecvedLength)))
			{
				RecvedLength += RecvResult;
				if(Progress && !Progress(RecvedLength, ResponseLength)) // ���α׷���
					return false;
				// ���� ������ ���� ���
				if(0 < RecvResult)
				{
					ErrorTime = 0;
					SleepTime >>= 1;
					// �߰����ſϷ�1 : �������� ����
					if(RecvedLength == ResponseLength)
						break;
					// �߰����ſϷ�2 : Content-Length�� ���� HTML����
					else if(BxUtilGlobal::StrSameCount((string) &ResponseBytes[RecvedLength - 5], "0\r\n\r\n") == 5)
					{
						ResponseLength = RecvedLength;
						if(Progress && !Progress(RecvedLength, ResponseLength)) // ���α׷���
							return false;
						break;
					}
				}
				// ���� ������ ���� ���
				else if(Timeout < (ErrorTime += SleepTime))
				{
					BxCore::Socket::Release(Socket);
					return false; // ���ð��ʰ�
				}
				else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
			}
		}
		BxCore::Socket::Release(Socket);
		if(RecvResult < 0)
		{
			BxFree(ResponseBytes);
			ResponseLength = 0;
			return false; // ���Ž���
		}
		ResponseBytes[ResponseLength] = '\0';
		return true;
	}

	/*!
	\brief ĳ������ �ε�(������ ����)
	\param Domain : ������ ����(��: "www.naver.com")
	\param Port : ��Ʈ��ȣ(HTTP�� ���� 80����Ʈ)
	\param FilePath : �ٿ���� ���ϰ��(��: "Update/Abc.bmp")
	\param DoLoad : �޸𸮿� �ε������� ����, false�� �ٿ�ε忩�� Ȯ�θ� �ϰڴٴ� �ǹ�
	\param Progress : ���α׷����� ����
	\return ĳ�÷ε� ��������
	*/
	bool Cache(string Domain, ushort Port, string FilePath, bool DoLoad = true, callback_progress Progress = nullptr)
	{
		global_data const int CacheCountSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheCountSumMax", -1);
		global_data const int CacheSizeSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheSizeSumMax", -1);

		// ĳ�÷ε�
		BxString CacheFileName("%s/%s", nullptr, GetCachePath(), FilePath);
		int FindedPos = -1;
		while((FindedPos = CacheFileName.Find('?')) != -1)
			CacheFileName[FindedPos] = '_';
		if(BxCore::File::IsExist(CacheFileName))
		{
			id_file CacheFile = BxCore::File::Open(CacheFileName, "rb");
			const int FileSize = BxCore::File::GetSize(CacheFile);
			if(!BxCore::File::IsExist(CacheFileName + ".downloading"))
			{
				if(DoLoad)
				{
					ResponseLength = FileSize;
					BxFree(ResponseBytes);
					ResponseBytes = (byte*) BxAlloc(ResponseLength + 1);
					BxCore::File::Read(CacheFile, ResponseBytes, ResponseLength);
					BxCore::File::Close(CacheFile);
					ResponseBytes[ResponseLength] = '\0';
				}
				else 
				{
					ResponseLength = 0;
					BxFree(ResponseBytes);
					ResponseBytes = nullptr;
				}
				return true;
			}
			BxCore::File::Close(CacheFile);
		}

		// ��������
		if(!Query(Domain, Port, FilePath, nullptr, -1, Progress))
			return false;

		// ĳ��Ǯ �ʱ����
		if(!IsInitPool() && (CacheCountSumMax != -1 || CacheSizeSumMax != -1))
		{
			IsInitPool() = true;
			BxCore::File::SearchFiles(GetCachePath(), OnFileSearch, nullptr);
		}

		// ĳ������
		BxCore::File::MakeDirectory(CacheFileName.GetFilePath());
		// �ٿ�ε��� ����
		id_file DownloadingFile = BxCore::File::Open(CacheFileName + ".downloading", "wb");
		BxCore::File::Write(DownloadingFile, &ResponseLength, 4);
		BxCore::File::Close(DownloadingFile);
		// �������� ����
		id_file CacheFile = BxCore::File::Open(CacheFileName, "wb");
		BxCore::File::Write(CacheFile, ResponseBytes, ResponseLength);
		BxCore::File::Close(CacheFile);
		// �ٿ�ε��� ����
		BxCore::File::RemoveFile(CacheFileName + ".downloading");
		if(!DoLoad)
		{
			ResponseLength = 0;
			BxFree(ResponseBytes);
			ResponseBytes = nullptr;
		}

		// ĳ��Ǯ �
		if(IsInitPool())
		{
			// ����ó��
			const int NewFileSize = BxCore::File::GetSize(CacheFileName);
			while((CacheCountSumMax != -1 && 0 < Pool().Length() && CacheCountSumMax < Pool().Length() + 1) ||
				(CacheSizeSumMax != -1 && 0 < PoolSumSize() && CacheSizeSumMax < PoolSumSize() + NewFileSize))
			{
				int iBest = 0;
				uhuge BestDate = Pool()[0].Date;
				for(int i = 1; i < Pool().Length(); ++i)
					if(Pool()[i].Date < BestDate)
					{
						iBest = i;
						BestDate = Pool()[i].Date;
					}
				// RemoveFile�� �������� �ʴ� �����̶�� �ص� ĳ�ð��� Ư���� �������ٰ� �Ǵ���
				BxCore::File::RemoveFile(Pool()[iBest].Path, true);
				PoolSumSize() -= Pool()[iBest].Size;
				Pool().Delete(iBest);
			}
			// �׸��߰�
			Pool()[LAST].Path = BxUtilGlobal::StrCpyWithAlloc(CacheFileName);
			Pool()[END].Size = NewFileSize;
			Pool()[END].Date = BxCore::File::GetDate(CacheFileName);
			PoolSumSize() += NewFileSize;
		}
		return true;
	}
};
