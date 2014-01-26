#pragma once
#include <BxString.hpp>
#include <BxVarVector.hpp>

//! \brief HTTP기반 쿼리송수신, 파일다운로드
class BxWebContent
{
	byte* ResponseBytes;
	int ResponseLength;

private:
	class CachePool
	{
	public:
		string_rw Path;
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
		BxString Name(name); // 안전을 위해 재귀처리는 새로운 공간할당이 필요
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
	\brief 캐시폴더이름 받기
	\return 캐시폴더이름
	*/
	global_func inline string GetCachePath() {return ".webcontent_cache";}

	/*!
	\brief 쿼리결과 받기
	\return 쿼리결과 바이트배열
	*/
	inline const byte* _tmp_ GetBytes() {return ResponseBytes;}

	/*!
	\brief 쿼리결과의 사이즈얻기(데이터)
	\return 바이트길이
	*/
	inline int GetDataLength() {return ResponseLength;}

	/*!
	\brief 쿼리결과의 사이즈얻기(스트링)
	\return 바이트길이
	*/
	inline int GetTextLength() {return BxUtilGlobal::StrLenEx((string) ResponseBytes);}

	/*!
	\brief HTTP쿼리실행
	\param Domain : 접속할 서버(예: "www.naver.com")
	\param Port : 포트번호(HTTP는 보통 80번포트)
	\param Path : 다운받을 파일 또는 실행할 서블릿의 경로(예: "Update/Run.aspx")
	\param Arguments : 서블릿의 경우 전달할 인자들(예: "Data0=aaa&Data1=bbb&Data2=ccc")
	\param Timeout : 각 단계별 지연허용시간
	\param Progress : 프로그레스의 전달
	\return 송수신 성공여부
	*/
	bool Query(string Domain, ushort Port, string Path, string Arguments = nullptr, int Timeout = -1, callback_progress Progress = nullptr)
	{
		if(Timeout < 0) Timeout = 5000;
		// 소켓연결, 쿼리제작 및 전송
		id_socket Socket = BxCore::Socket::Create();
		if(connect_connected == BxCore::Socket::Connect(Socket, Domain, Port, Timeout, Progress))
		{
			int RequestCharsLength = 0;
			string _tmp_ RequestChars = BxCore::Util::Print(
				"<>:GET /<A><A><A> HTTP/1.1<R><N>"
				"Accept: text/html, application/xhtml+xml, */*<R><N>"
				"Accept-Language: ko-KR<R><N>"
				"User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)<R><N>"
				"Accept-Encoding: deflate<R><N>"//"Accept-Encoding: gzip, deflate<R><N>"
				"Host: <A><R><N>"
				"Connection: Keep-Alive<R><N>"
				"<R><N>", BxTHROW(Path, (!Arguments || !*Arguments)? "" : "?", (!Arguments)? "" : Arguments, Domain),
				&RequestCharsLength);
			int SendResult = 0, SendedLength = 0;
			do
			{
				if(Progress && !Progress(-1, 0)) return false; // 업데이트
				SendResult = BxCore::Socket::Send(Socket, RequestChars + SendedLength, RequestCharsLength - SendedLength);
				if(SendResult < 0)
				{
					BxCore::Socket::Release(Socket);
					return false; // 송신실패
				}
			}
			while((SendedLength += SendResult) < RequestCharsLength);
			BxCore::System::Sleep(0);
		}
		else
		{
			BxCore::Socket::Release(Socket);
			return false; // 연결실패
		}

		// HTTP헤더 수신
		char ResponseChars[4096];
		char* EndCodeSearchFocus = ResponseChars;
		int RecvResult = 0, RecvedLength = 0;
		int ErrorTime = 0, SleepTime = 0;
		const int HTMLEndCodeLength = BxUtilGlobal::StrLen("\r\n\r\n");
		while(0 <= (RecvResult = BxCore::Socket::Recv(Socket, ResponseChars + RecvedLength, (4096 - 1) - RecvedLength)))
		{
			if(Progress && !Progress(-1, 0)) return false; // 업데이트
			RecvedLength += RecvResult;
			// 받은 내용이 있을 경우
			if(0 < RecvResult)
			{
				ErrorTime = 0;
				SleepTime >>= 1;
				// HTML헤더 종료코드 검색
				if(HTMLEndCodeLength < RecvedLength)
				{
					ResponseChars[RecvedLength] = '\0';
					while(*(++EndCodeSearchFocus) && BxUtilGlobal::StrSameCount(EndCodeSearchFocus, "\r\n\r\n") != HTMLEndCodeLength);
					if(*EndCodeSearchFocus)
					{
						EndCodeSearchFocus += HTMLEndCodeLength;
						// 데이터사이즈 조사
						char* ContentLengthSearchFocus = ResponseChars;
						while(++ContentLengthSearchFocus < EndCodeSearchFocus && BxUtilGlobal::StrSameCount(ContentLengthSearchFocus, "Content-Length:") != 15);
						if(ContentLengthSearchFocus == EndCodeSearchFocus)
						{
							// Content-Length가 없을 경우 5MByte로 잡는다
							ResponseLength = 1024 * 1024 * 5;
						}
						else
						{
							ContentLengthSearchFocus += 15;
							ResponseLength = BxUtilGlobal::AtoI(ContentLengthSearchFocus, EndCodeSearchFocus - ContentLengthSearchFocus);
						}
						// 버퍼확보
						BxFree(ResponseBytes);
						ResponseBytes = (byte*) BxAlloc(ResponseLength + 1);
						break;
					}
                    else if(ResponseChars < EndCodeSearchFocus - HTMLEndCodeLength)
                        EndCodeSearchFocus = EndCodeSearchFocus - HTMLEndCodeLength;
                    else EndCodeSearchFocus = ResponseChars;
				}
			}
			// 받은 내용이 없을 경우
			else if(Timeout < (ErrorTime += SleepTime))
			{
				BxCore::Socket::Release(Socket);
				return false; // 대기시간초과
			}
			else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
		}
		if(RecvResult < 0)
		{
			BxCore::Socket::Release(Socket);
			return false; // 수신실패
		}

		// 데이터 이관
		RecvedLength = ResponseChars + RecvedLength - EndCodeSearchFocus;
		BxCore::Util::MemCpy(ResponseBytes, EndCodeSearchFocus, RecvedLength);
		if(Progress && !Progress(RecvedLength, ResponseLength)) // 프로그레스
			return false;

		// 데이터 수신
		RecvResult = 0;
		if(RecvedLength < ResponseLength)
		{
			// 추가수신
			ErrorTime = 0, SleepTime = 0;
			while(0 <= (RecvResult = BxCore::Socket::Recv(Socket, ResponseBytes + RecvedLength, ResponseLength - RecvedLength)))
			{
				RecvedLength += RecvResult;
				if(Progress && !Progress(RecvedLength, ResponseLength)) // 프로그레스
					return false;
				// 받은 내용이 있을 경우
				if(0 < RecvResult)
				{
					ErrorTime = 0;
					SleepTime >>= 1;
					// 추가수신완료1 : 정상적인 파일
					if(RecvedLength == ResponseLength)
						break;
					// 추가수신완료2 : Content-Length가 없는 HTML파일
					else if(BxUtilGlobal::StrSameCount((string) &ResponseBytes[RecvedLength - 5], "0\r\n\r\n") == 5)
					{
						ResponseLength = RecvedLength;
						if(Progress && !Progress(RecvedLength, ResponseLength)) // 프로그레스
							return false;
						break;
					}
				}
				// 받은 내용이 없을 경우
				else if(Timeout < (ErrorTime += SleepTime))
				{
					BxCore::Socket::Release(Socket);
					return false; // 대기시간초과
				}
				else BxCore::System::Sleep(BxUtilGlobal::Min(SleepTime++, Timeout >> 6));
			}
		}
		BxCore::Socket::Release(Socket);
		if(RecvResult < 0)
		{
			BxFree(ResponseBytes);
			ResponseLength = 0;
			return false; // 수신실패
		}
		ResponseBytes[ResponseLength] = '\0';
		return true;
	}

	/*!
	\brief 캐시파일 로드(없으면 생성)
	\param Domain : 접속할 서버(예: "www.naver.com")
	\param Port : 포트번호(HTTP는 보통 80번포트)
	\param FilePath : 다운받을 파일경로(예: "Update/Abc.bmp")
	\param DoLoad : 메모리에 로딩할지의 여부, false면 다운로드여부 확인만 하겠다는 의미
	\param Progress : 프로그레스의 전달
	\return 캐시로드 성공여부
	*/
	bool Cache(string Domain, ushort Port, string FilePath, bool DoLoad = true, callback_progress Progress = nullptr)
	{
		global_data const int CacheCountSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheCountSumMax", -1);
		global_data const int CacheSizeSumMax = BxCore::System::GetConfigNumber("Bx.WebContent.CacheSizeSumMax", -1);

		// 캐시로드
		BxString CacheFileName("<>:<A><FS><A>", BxTHROW(GetCachePath(), FilePath));
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

		// 쿼리실행
		if(!Query(Domain, Port, FilePath, nullptr, -1, Progress))
			return false;

		// 캐시풀 초기수집
		if(!IsInitPool() && (CacheCountSumMax != -1 || CacheSizeSumMax != -1))
		{
			IsInitPool() = true;
			BxCore::File::SearchFiles(GetCachePath(), OnFileSearch, nullptr);
		}

		// 캐시저장
		BxCore::File::MakeDirectory(CacheFileName.GetFilePath());
		// 다운로드중 생성
		id_file DownloadingFile = BxCore::File::Open(CacheFileName + ".downloading", "wb");
		BxCore::File::Write(DownloadingFile, &ResponseLength, 4);
		BxCore::File::Close(DownloadingFile);
		// 실제파일 생성
		id_file CacheFile = BxCore::File::Open(CacheFileName, "wb");
		BxCore::File::Write(CacheFile, ResponseBytes, ResponseLength);
		BxCore::File::Close(CacheFile);
		// 다운로드중 삭제
		BxCore::File::RemoveFile(CacheFileName + ".downloading");
		if(!DoLoad)
		{
			ResponseLength = 0;
			BxFree(ResponseBytes);
			ResponseBytes = nullptr;
		}

		// 캐시풀 운영
		if(IsInitPool())
		{
			// 제한처리
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
				// RemoveFile로 지워지지 않는 파일이라고 해도 캐시관리 특성상 지워졌다고 판단함
				BxCore::File::RemoveFile(Pool()[iBest].Path, true);
				PoolSumSize() -= Pool()[iBest].Size;
				Pool().Delete(iBest);
			}
			// 항목추가
			Pool()[LAST].Path = BxUtilGlobal::StrCpyWithAlloc(CacheFileName);
			Pool()[END].Size = NewFileSize;
			Pool()[END].Date = BxCore::File::GetDate(CacheFileName);
			PoolSumSize() += NewFileSize;
		}
		return true;
	}
};
