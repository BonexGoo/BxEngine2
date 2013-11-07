#pragma once
#include <BxType.hpp>
#include <BxThrow.hpp>

//! \brief BxEngine Core-API
namespace BxCore
{
	//! \brief Main파트
	namespace Main
	{
		void EventAttach(void* drawclass);
		void EventDetach();
		int GetCurrentUpdateCount();
		int GetCurrentRenderCount();
		void SetCurrentFrameTime(int ms);
		int GetCurrentFrameTime(bool onlyFrame = false);
		rect GetCurrentGUIMargin();
		void PushFrameDelay();
		void PullFrameDelay();
	}
	//! \brief System파트
	namespace System
	{
		/*!
		\brief 어플종료 요청여부
		\return true-종료가 요청됨, false-종료요청 없음
		\see DoQuit
		*/
		bool IsQuit();

		/*!
		\brief 어플종료 요청
		\see IsQuit
		*/
		void DoQuit();

		/*!
		\brief 프로세스 슬립(밀린 소켓/키이벤트등 처리)
		\param ms : 쉴 시간(1/1000초)
		*/
		void Sleep(int ms);

		/*!
		\brief 현재시간을 밀리초로 구하기(천분의 1초)
		\return 1/1,000초 단위로 계산된 현재시간(UTC기준)
		*/
		uhuge GetTimerMilliSecond();

		/*!
		\brief 현재시간을 나노초로 구하기(10억분의 1초)
		\return 1/1,000,000,000초 단위로 계산된 현재시간
		*/
		uhuge GetTimerNanoSecond();

		/*!
		\brief 현재시간을 유사 마이크로초(나노초>>10)로 구하기
		\return 1/976,562초 단위로 계산된 현재시간
		*/
		uhuge GetTimerSimilarMicroSecond();

		/*!
		\brief 멀티터치 가능여부
		\return true-가능, false-불가능
		*/
		bool IsMultiTouchAvailable();

		/*!
		\brief 터치의 현재 누름여부
		\return true-눌러짐, false-안눌러짐
		*/
		bool IsTouchDown();

		/*!
		\brief 터치이벤트 제작
		\param type : 이벤트타입
		\param x : X좌표값
		\param y : Y좌표값
		*/
		void MakeTouchEvent(systouchtype type, int x, int y);

		/*!
		\brief 특정 키의 현재 누름여부
		\param key : 확인할 키값
		\return true-눌러짐, false-안눌러짐
		*/
		bool IsKeyDown(keykind key);

		/*!
		\brief 환경설정값 존재여부
		\param type : 변수타입
		\param name : 변수명
		\return true-있음, false-없음
		*/
		bool IsExistConfig(keyword type, string name);

		/*!
		\brief Check형 환경설정값 불러오기
		\param name : 변수명
		\param defaultCheck : 디폴트값
		\return 해당 설정값
		*/
		bool GetConfigCheck(string name, bool defaultCheck);

		/*!
		\brief Number형 환경설정값 불러오기
		\param name : 변수명
		\param defaultNumber : 디폴트값
		\return 해당 설정값
		*/
		int GetConfigNumber(string name, int defaultNumber);

		/*!
		\brief String형 환경설정값 불러오기
		\param name : 변수명
		\param defaultString : 디폴트값
		\return 해당 설정값
		*/
		string _tmp_ GetConfigString(string name, string defaultString);

		/*!
		\brief 마멀레이드의 Number형 환경설정값(icf/s3e) 불러오기
		\param name : 변수명
		\return 해당 설정값
		*/
		int GetPlatformConfigNumber(string name);

		/*!
		\brief 마멀레이드의 String형 환경설정값(icf/s3e) 불러오기
		\param name : 변수명
		\return 해당 설정값
		*/
		string _tmp_ GetPlatformConfigString(string name);

		/*!
		\brief 시뮬레이터 윈도우위치설정(데스크탑 빌드전용)
		\param x : 위치값X
		\param y : 위치값Y
		*/
		void SetSimulatorWindowPos(int x, int y);

		/*!
		\brief 시뮬레이터 윈도우위치얻기(데스크탑 빌드전용)
		\return 위치값
		*/
		point GetSimulatorWindowPos();

		/*!
		\brief 시뮬레이터 커서위치얻기(데스크탑 빌드전용)
		\return 위치값
		*/
		point GetSimulatorCursorPos();

		/*!
		\brief 시뮬레이터 최소화하기(데스크탑 빌드전용)
		*/
		void DoSimulatorMinimize();

		/*!
		\brief 디버깅코드(Macro함수인 BxAssert로 사용권장)
		\param name : 에러내용
		\param flag : 정상동작여부(true-정상동작, false-오류동작)
		\param filename : 해당파일명(__FILE__을 사용)
		\param linenumber : 해당줄번호(__LINE__을 사용)
		*/
		void Assert(string name, bool& IsIgnore, bool flag, string filename, int linenumber);

		/*!
		\brief OS이름 구하기
		\return 자기 플랫폼의 OS이름(예: SYMBIAN, BREW, WINDOWS, WINMOBILE, LINUX, WIPI, IPHONE, ANDROID)
		*/
		string GetOSName();

		/*!
		\brief 아키텍쳐이름 구하기
		\return 컴파일된 아키텍쳐이름(예: ARM4T, ARM4, ARM5T, ARM5TE, ARM5TEJ, ARM6, ARM6K, ARM6T2, ARM6Z, ARM7, X86, PPC, AMD64, MIPS)
		*/
		string GetArchitectureName();

		/*!
		\brief 휴대폰번호 구하기
		\return 자신의 휴대폰번호(예: 01022223333)
		*/
		string GetPhoneNumber();

		/*!
		\brief 웹페이지연결
		\param url : 연결할 웹주소(예: market://search?q=pname:com.popcap.pvz, tstore://PRODUCT_VIEW/0000252470/1)
		\param exitme : 현재 프로그램의 종료여부
		*/
		void PopupOSExecute(string url, bool exitme);

		/*!
		\brief 가상키보드로 스트링얻기
		\param result_str256 : 얻을 스트링(256짜리 배열권장)
		\param default_str : 처음 에디터에 보일 스트링
		\param keyboard_name : 가상키보드의 이름
		\param type : 키보드종류(기본, 이메일, 숫자, 비번, 웹주소)
		\return 성공여부(미지원 OS를 고려)
		*/
		bool PopupOSKeyboard(string_rw result_str256, string default_str = "", string keyboard_name = "", oskeyboardtype type = oskeyboardtype_base);

		/*!
		\brief 프레임단위 정기적 콜백등록(참조식)
		\param method : 콜백함수(호출된 콜백함수에서 false리턴시 해제)
		\param data : 데이터원본
		\param datasize : -1은 참조식, 0보다 같거나 크면 복사식(false리턴시 함께 해제)
		*/
		void AddCallback(callback_frame method, int value, void* data, int datasize = -1);

		/*!
		\brief 자식프로세스의 실행
		\param process : 프로세스를 진행시킬 콜백함수
		\return 실행여부(여러개의 자식프로세스가 실행될 순 없으므로 false이면 실행거절)
		*/
		bool BeginChildProcess(callback_process process);

		/*!
		\brief JAVA클래스 존재확인
		\param classpath : 클래스주소(예: com/abc/ABC)
		\return 존재시 true, 없을시 false
		*/
		bool IsExistClassJNI(string classpath);

		/*!
		\brief C→JAVA 정적함수호출
		\param classpath : 클래스주소(예: com/abc/ABC)
		\param methodname : 함수이름
		\param param1_str1024 : 전달할 스트링
		\param param2 : 전달할 상수
		\return JAVA함수리턴값, -90001에서 -90002는 JNI연결실패, -90003은 classpath없음
		*/
		int CallStaticMethodJNI(string classpath, string methodname, string param1_str1024, int param2);

		/*!
		\brief JAVA→C 콜백등록(최대 8개까지 등록)
		\param classpath : 클래스주소(예: com/abc/ABC)
		\param methodname : 함수이름
		\param methodCB : 콜백함수
		\return 0-성공, -90001에서 -90002는 JNI연결실패, -90003은 classpath없음, -90004는 methodname없음, -90005는 콜백등록수량초과
		*/
		int AddCallbackJNI(string classpath, string methodname, callback_jni methodCB);

		/*!
		\brief EDK수신용 콜백등록(어떤 스레드에서 호출당하던지 엔진스레드로 비동기수신)
		\param methodCB : 콜백함수
		\return EDK수신용 내부함수 전달
		*/
		callback_edk SetCallbackEDK(callback_edk methodCB);
	}
	//! \brief Surface파트
	namespace Surface
	{
		/*!
		\brief 화면생성
		*/
		void Init();

		/*!
		\brief 업데이트 및 백버퍼를 화면에 출력
		\param ptr : 화면에 출력할 이미지의 포인터
		\param row : 이미지의 너비의 바이트길이
		\param height : 이미지의 픽셀높이
		\param pitch : 이미지의 더미를 포함한 너비의 바이트길이
		*/
		void Flush(void* ptr, const int row, const int height, const int pitch);

		/*!
		\brief 중간 업데이트
		*/
		void MiddleFlush();

		/*!
		\brief 화면너비 구하기
		\param isfixedvalue : icf의 SurfaceWidthBx를 적용한 고정사이즈 여부조사(null이면 생략)
		\param usemargin : Margin값 적용여부
		\return 너비의 픽셀길이
		*/
		int GetWidth(bool* isfixedvalue = null, bool usemargin = true);

		/*!
		\brief 화면높이 구하기
		\param isfixedvalue : icf의 SurfaceHeightBx를 적용한 고정사이즈 여부조사(null이면 생략)
		\param usemargin : Margin값 적용여부
		\return 높이의 픽셀길이
		*/
		int GetHeight(bool* isfixedvalue = null, bool usemargin = true);

		/*!
		\brief 하드웨어적 화면너비 구하기
		\param usemargin : Margin값 적용여부
		\return 너비의 픽셀길이
		*/
		int GetWidthHW(bool usemargin = true);

		/*!
		\brief 하드웨어적 화면높이 구하기
		\param usemargin : Margin값 적용여부
		\return 높이의 픽셀길이
		*/
		int GetHeightHW(bool usemargin = true);

		/*!
		\brief 마진고려한 Width/Height값의 작동여부
		\param enable : 작동여부
		\return 작동여부
		*/
		bool SetMarginMode(bool enable);
	}
	//! \brief Sound파트
	namespace Sound
	{
		/*!
		\brief 사운드ID 할당(icf상의 [game]SoundCacheSize내에서 할당)
		\param filename : 파일명(mid, mp3, aac, qcp, wav)
		\param autoload : 자동로딩해제시스템
		\return 사운드ID(null은 실패)
		\see Release
		*/
		id_sound Create(string filename, bool autoload = true);

		/*!
		\brief 사운드ID 반환
		\param sound : 사운드ID
		\see Create
		*/
		void Release(id_sound sound);

		/*!
		\brief 배경음 출력
		\param sound : 사운드ID(파노라마시 첫번째 사운드ID)
		\param interrupt : 출력중인 사운드ID를 즉시 중지시키고 새로 출력
		\param panorama : 파노라마로 추가될 사운드ID들
		\see StopMusic
		*/
		void PlayMusic(id_sound sound, bool interrupt = false, const BxThrow& panorama = BxThrow::zero());

		/*!
		\brief 배경음 중지
		\see PlayMusic
		*/
		void StopMusic();

		/*!
		\brief 배경음 볼륨
		\param percent : 백분율(디폴트값 100)
		*/
		void SetMusicVolume(uint percent);

		/*!
		\brief 효과음 출력
		\param sound : 사운드ID
		\param interrupt : 출력중인 효과음중 동일한 사운드ID가 있을때 즉시 중지시키고 새로 출력
		\see StopEffect
		*/
		void PlayEffect(id_sound sound, bool interrupt = false);

		/*!
		\brief 모든 효과음 중지
		\see PlayEffect
		*/
		void StopEffect();

		/*!
		\brief 효과음 볼륨
		\param percent : 백분율(디폴트값 100)
		*/
		void SetEffectVolume(uint percent);

		/*!
		\brief 전체 배경음/효과음 일시정지
		*/
		void PauseAll();

		/*!
		\brief 전체 배경음/효과음 일시정지 해제
		*/
		void ResumeAll();
	}
	//! \brief Util파트
	namespace Util
	{
		/*!
		\brief 메모리할당
		\param size : 요청할 바이트길이
		\return 할당된 메모리ID, 실패시 0
		\see Free, GetPtr
		*/
		id_memory Malloc(int size);

		/*!
		\brief 메모리반환
		\param item : 반환할 메모리ID
		\see Malloc
		*/
		void Free(id_memory item);

		/*!
		\brief 메모리ID의 실제 메모리번지 구하기
		\param item : 요청할 메모리ID
		\return 해당 메모리ID의 메모리번지
		\see Malloc
		*/
		void* GetPtr(id_memory item);

		/*!
		\brief 메모리 복사
		\param dst : 붙여넣을 시작 메모리번지
		\param src : 복사할 원본의 시작 메모리번지
		\param length : 복사할 바이트길이
		*/
		void MemMove(const void* dst, const void* src, int length);

		/*!
		\brief 메모리 초기화
		\param dst : 초기화할 시작 메모리번지
		\param value : 초기화시킬 값
		\param length : 초기화할 바이트길이
		*/
		void MemSet(const void* dst, byte value, int length);

		/*!
		\brief 메모리 비교
		\param dst : 비교대상 시작 메모리번지
		\param src : 비교주체 시작 메모리번지
		\param length : 비교할 바이트길이
		\return 동일하면 0, 다르면 0이 아님
		*/
		int MemCmp(const void* dst, const void* src, int length);

		/*!
		\brief 스트링구성
		\param src : "<A>"형식의 스크립트
		\param args : src에 쓰일 가변인자
		\param length : 구성된 스트링의 길이
		\return 구성된 스트링
		*/
		string _tmp_ Print(string src, const BxThrow& args = BxThrow::zero(), int* length = null);

		/*!
		\brief 출력창에 스트링기록(Release도 사용하려면 mkb의 defines{}에 IW_USE_TRACING추가)
		\param src : printf양식의 맵(%d/%s/...)
		\param ... : src에 쓰일 가변인자
		\return 기록된 바이트길이
		*/
		int Printf(string src, ...);

		/*!
		\brief 스트링에 스트링기록
		\param dst : 기록할 시작 메모리번지
		\param length : dst의 길이
		\param src : sprintf양식의 맵(%d/%s/...)
		\param ... : src에 쓰일 가변인자
		\return 기록된 바이트길이
		*/
		int SnPrintf(string_rw dst, int length, string src, ...);

		/*!
		\brief 스트링에 va_list식 스트링기록
		\param dst : 기록할 시작 메모리번지
		\param length : dst의 길이
		\param src : sprintf양식의 맵(%d/%s/...)
		\param list : src에 쓰일 va_list형 가변인자
		\return 기록된 바이트길이
		*/
		int VsnPrintf(string_rw dst, int length, string src, va_list list);

		/*!
		\brief 현재 함수의 콜깊이조사
		\return 콜깊이
		*/
		uint GetCurrentCallDepth();

		/*!
		\brief 현재 함수의 시작주소조사
		\return 시작주소
		*/
		void* GetCurrentCallAddress();

		/*!
		\brief 현재 함수의 콜순번조사
		\return 콜순번
		*/
		uint GetCurrentCallCount();

		/*!
		\brief 콜순번 초기화
		\param value : 콜순번
		*/
		void SetCallCount(uint value);

		/*!
		\brief 콜깊이 더하기
		\see PopCallDepthAndLog
		*/
		void PushCallDepthAndLog(string file, const int line, string func);

		/*!
		\brief 콜깊이 빼기
		\see PushCallDepthAndLog
		*/
		void PopCallDepthAndLog(string file, const int line);
	}
	//! \brief AddOn파트
	namespace AddOn
	{
		/*!
		\brief JPG이미지로 BMP를 제작
		\param jpg : JPG리소스
		\return BMP리소스(ReleaseBMP로 해제)
		\see ReleaseBMP
		*/
		const byte* JPGToBMP(const byte* jpg);

		/*!
		\brief GIF이미지로 BMP를 제작
		\param gif : GIF리소스
		\param length : GIF리소스의 바이트길이
		\param numpage : 장면수
		\return 모든 장면이 세로로 연결된 BMP리소스(ReleaseBMP로 해제)
		\see ReleaseBMP
		*/
		const byte* GIFToBMP(const byte* gif, const int length, int _out_ numpage);

		/*!
		\brief HQX방식으로 재가공된 BMP를 제작
		\param bmp : BMP리소스
		\param scale : HQX옵션(2, 3, 4중 선택)
		\return 재가공된 BMP리소스(ReleaseBMP로 해제)
		\see ReleaseBMP
		*/
		const byte* HQXToBMP(const byte* bmp, int scale);

		/*!
		\brief BMP리소스 해제
		\param bmp : JPGToBMP로 제작되었던 BMP리소스
		\see JPGToBMP
		*/
		void ReleaseBMP(const byte* bmp);
	}
	//! \brief File파트
	namespace File
	{
		/*!
		\brief 파일의 존재여부
		\param filename : 파일명
		\return 존재시 true, 없을시 false
		\see Open
		*/
		bool IsExist(string filename);

		/*!
		\brief 파일열기
		\param filename : 파일명
		\param mode : 열기옵션(전:"r"/"w"/"a"/"r+"/"w+"/"a+", 후:"t"/"b"/"U")
		\return 열려진 파일ID, 실패시 0
		\see IsExist, Close, Attach
		*/
		id_file Open(string filename, string mode);

		/*!
		\brief 파일닫기
		\param file : 열린 파일ID
		\see Open
		*/
		void Close(id_file file);

		/*!
		\brief 파일에서 읽기
		\param file : 파일ID
		\param buffer : 읽은 내용을 저장할 메모리
		\param length : 읽기요청할 바이트길이
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 읽은 바이트길이
		*/
		uint Read(id_file file, void* buffer, uint length, callback_progress progress = null);

		/*!
		\brief 파일로 쓰기
		\param file : 파일ID
		\param buffer : 쓸 내용을 담고 있는 메모리
		\param length : 쓰기요청할 바이트길이
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 쓴 바이트길이
		*/
		uint Write(id_file file, const void* buffer, uint length, callback_progress progress = null);

		/*!
		\brief 파일로 스트링쓰기
		\param file : 파일ID
		\param buffer : 쓸 내용을 담고 있는 스트링
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 쓴 바이트길이
		*/
		uint WriteString(id_file file, string buffer, callback_progress progress = null);

		/*!
		\brief 파일위치 스킵하기
		\param file : 파일ID
		\param length : 스킵요청할 바이트길이
		\return 결과 파일위치
		*/
		int Skip(id_file file, uint length);

		/*!
		\brief 파일의 전체길이 얻기
		\param file : 파일ID
		\return 전체 바이트길이
		*/
		int GetSize(id_file file);

		/*!
		\brief 파일의 전체길이 얻기
		\param filename : 파일명
		\return 전체 바이트길이
		*/
		int GetSize(string filename);

		/*!
		\brief 파일의 마지막 수정날짜 얻기
		\param filename : 파일명
		\return 마지막 수정날짜(1970년1월1일기준 밀리초환산값)
		*/
		uhuge GetDate(string filename);

		/*!
		\brief DZ압축파일의 연동(여러번 Attach하면 마지막 Attach가 최우선순위)
		\param archivename : 압축파일명
		\return 연동의 성공여부
		\see Detach
		*/
		bool Attach(string archivename);

		/*!
		\brief DZ압축파일의 연동해제(Attach에 성공한 횟수만큼 해제)
		\see Attach
		*/
		void Detach();

		/*!
		\brief 폴더만들기
		\param dirname : 폴더명
		\param isFile : dirname이 파일인지의 여부
		*/
		void MakeDirectory(string dirname, bool isFile = false);

		/*!
		\brief 파일검색
		\param dirname : 폴더명
		\param resultCB : 결과받을 콜백함수
		\param data : 콜백함수에 전달할 데이터
		\return 검색된 수량
		*/
		uint SearchFiles(string dirname, callback_filesearch resultCB, void* data);

		/*!
		\brief 파일삭제
		\param filename : 파일명
		\param doRemoveBlankedDirectory : 빈 폴더가 생기는 경우 폴더까지 삭제할지 여부
		*/
		void RemoveFile(string filename, bool doRemoveBlankedDirectory = false);
	}
	//! \brief Socket파트
	namespace Socket
	{
		/*!
		\brief 소켓ID 할당
		\return 소켓ID(null은 실패)
		\see Release, Connect
		*/
		id_socket Create();

		/*!
		\brief 소켓ID 반환, 접속종료
		\param sock : 소켓ID
		\see Create
		*/
		void Release(id_socket sock);

		/*!
		\brief 소켓 상태
		\param sock : 소켓ID
		\return 해당 소켓의 상태
		*/
		socketstate GetState(id_socket sock);

		/*!
		\brief 서버로 접속
		\param sock : 소켓ID
		\param addr : 도메인 또는 IP
		\param port : 포트번호
		\param timeout : 결과를 기다릴 시간
		\param progress : 프로그레스의 전달(업데이트전용, progress(-1, 0)로 호출)
		\return 결과를 기다릴 경우 dowait_connected/dowait_disconnected, 안 기다릴 경우 connecting
		\see Disconnect, GetState
		*/
		connectresult Connect(id_socket sock, string addr, ushort port, uint timeout = 0, callback_progress progress = null);

		/*!
		\brief 서버와의 접속종료
		\param sock : 소켓ID
		\see Connect, GetState
		*/
		void Disconnect(id_socket sock);

		/*!
		\brief 데이터 송신
		\param sock : 소켓ID
		\param buffer : 송신할 버퍼
		\param len : 송신할 바이트길이
		\return 송신된 바이트길이(음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int Send(id_socket sock, const void* buffer, int len);

		/*!
		\brief 데이터 수신
		\param sock : 소켓ID
		\param buffer : 수신할 버퍼
		\param len : 버퍼의 바이트길이
		\return 수신된 바이트길이(음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int Recv(id_socket sock, void* buffer, int len);

		/*!
		\brief 데이터 강제수신
		\param sock : 소켓ID
		\param buffer : 수신할 버퍼
		\param len : 수신할 바이트길이
		\param timeout : 결과를 기다릴 시간(-1은 디폴트값)
		\param cancelmode : 탈출가능여부(현재 수신된 데이터가 0일 경우만 작동)
		\return 수신된 바이트길이(음수는 수신에러코드, 이때 GetState로 Create/Connect를 판단)
		\see GetState
		*/
		int RecvFully(id_socket sock, byte* buffer, int len, int timeout, bool cancelmode);

		/*!
		\brief 핑시간 측정(ICMP프로토콜)
		\param addr : 도메인 또는 IP
		\param timeout : 결과를 기다릴 시간
		\return 핑시간(밀리초, 음수면 실패)
		*/
		int Ping(string addr, uint timeout);
	}
	//! \brief Font파트
	namespace Font
	{
		/*!
		\brief 폰트열기(파일)
		\param filename : TTF파일명
		\param pointsize : 글자크기
		\param numCacheGlyphs : 빠른출력을 위한 캐시수(icf의 [gxfont] CacheTextureMaxSize보다 작아야 함, -1은 자동처리)
		\return 폰트ID(null은 실패)
		\see Close
		*/
		id_font Open(string filename, const int pointsize, const int numCacheGlyphs = -1);

		/*!
		\brief 폰트열기(버퍼)
		\param buffer : TTF버퍼
		\param buffersize : TTF버퍼사이즈
		\param pointsize : 글자크기
		\param numCacheGlyphs : 빠른출력을 위한 캐시수(icf의 [gxfont] CacheTextureMaxSize보다 작아야 함, -1은 자동처리)
		\return 폰트ID(null은 실패)
		\see Close
		*/
		id_font Open(const byte* buffer, const int buffersize, const int pointsize, const int numCacheGlyphs = -1);

		/*!
		\brief 폰트닫기
		\param font : 폰트ID
		\see Open
		*/
		void Close(id_font font);

		/*!
		\brief 인코딩방식 설정
		\param encoding : 인코딩 코드
		\see SetSort, SetStyle, GetSize
		*/
		void SetEncoding(fontencoding encoding);

		/*!
		\brief 정렬방식 설정
		\param fontsort : 숫자키패드 방식
		\return 정렬방식 이전값
		\see SetEncoding, SetStyle, GetSize
		*/
		fontsort SetSort(fontsort sort);

		/*!
		\brief 스타일 설정
		\param underline : 밑줄 여부
		\param italic : 이탤릭체 여부
		\see SetEncoding, SetSort, GetSize
		*/
		void SetStyle(bool underline, bool italic);

		/*!
		\brief 사이즈 얻기
		\param font : 폰트ID
		\param str : 스트링
		\param s : 지정공간
		\return 해당 스트링의 출력사이즈
		\see SetEncoding, SetSort, SetStyle
		*/
		size GetSize(id_font font, string str, const size s = size::full());

		/*!
		\brief 스트링 출력
		\param font : 폰트ID
		\param str : 스트링
		\param p : 출력위치
		\param s : 지정공간
		\param color : 색상
		\param opacity : 불투명도
		*/
		void Draw(id_font font, string str, const point p, const size s = size::full(), const color_x888 color = 0, const byte opacity = 0xFF);
	}
	//! \brief Library파트
	namespace Library
	{
		/*!
		\brief 라이브러리 열기
		\param filename : 파일명
		\return 라이브러리ID(null은 실패)
		\see Close
		*/
		id_library Open(string filename);

		/*!
		\brief 라이브러리 닫기
		\param handle : 라이브러리ID
		\see Open
		*/
		void Close(id_library handle);

		/*!
		\brief 함수연결
		\param handle : 라이브러리ID
		\param name : 함수명
		\see 함수포인터
		*/
		void* Link(id_library handle, string name);
	}
	/// @cond SECTION_NAME
	#ifdef __BX_OPENGL
	namespace OpenGL2D
	{
		void Init();
		void Quit();
		void Flush();
		// View
		void SetPerspective(const int mul, const int far, const int near);
		void SetOrthogonal(const int width, const int height, const int far, const int near);
		void AddViewTrans(int x, int y, int z);
		void AddViewRotX(fint angle);
		void AddViewRotY(fint angle);
		void AddViewRotZ(fint angle);
		// Make/Free
		id_opengl_form MakeForm_TextureMaterial(string filename);
		id_opengl_form MakeForm_TextureMaterial(color_a888* image, const int width, const int height, const int pitch);
		id_opengl_form MakeForm_ColorAmbient(const color_x888 color);
		void FreeForm(id_opengl_form form);
		id_opengl_outline MakeOutline_VertexUV(const rect& r, const rect& uvclip);
		id_opengl_outline MakeOutline_PolyVertexUV(const points& p, const rect& uvclip);
		id_opengl_outline MakeOutline_PolyVertexUV3D(const vertexs& v, const points& uv);
		void FreeOutline(id_opengl_outline outline);
		// Get
		color_x888 Form_ColorAmbient_GetColor(id_opengl_form form);
		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver);
		void Outline_VertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip);
		void Outline_PolyVertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver);
		void Outline_PolyVertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip);
		// Render
		void Render(id_opengl_form form, id_opengl_outline outline, int x, int y, const byte opacity, const color_x888 color);
		void Render3D(id_opengl_form form, id_opengl_outline outline, int x, int y, int z, const byte opacity, const color_x888 color);
		// Option
		void Clip(rect r);
		// Property
		const int GetTextureMargin();
		const bool DoTextureInterpolation();
	}
	#endif
	/// @endcond
}
