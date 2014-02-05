// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxCoreForQT5.hpp>
#include <BxCore.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#include <BxAutoBuffer.hpp>
#include <BxExpress.hpp>
#include <BxKeyword.hpp>
#include <BxMemory.hpp>
#include <BxScene.hpp>
#include <BxUtil.hpp>
#include <BxString.hpp>

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
namespace BxCore
{
	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Main
	{
		local_data BxDraw* Draw = nullptr;

		void EventAttach(void* drawclass)
		{
			Draw = (BxDraw*) drawclass;
		}

		void EventDetach()
		{
			Draw = nullptr;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace System
	{
		local_data bool QuitFlag = false;

		bool IsQuit()
		{
			return QuitFlag;
		}

		void DoQuit()
		{
			QuitFlag = true;
		}

		void Sleep(int ms)
		{
			uhuge StartTime = GetTimeMilliSecond();
			QEventLoop EventLoop(QThread::currentThread());
			EventLoop.processEvents();
			int SleepTime = ms - ((int) (GetTimeMilliSecond() - StartTime));
			if(0 < SleepTime) QThread::msleep(SleepTime);
		}

		uhuge GetTimeMilliSecond()
		{
			return (uhuge) QTime::currentTime().msecsSinceStartOfDay();
		}

		uhuge GetTimeNanoSecond()
		{
			return GetTimeMilliSecond() * 1000;
		}

		uhuge GetTimeSimilarMicroSecond()
		{
			return GetTimeNanoSecond() >> 10;
		}

		bool IsMultiTouchAvailable()
		{
			////////////////////////////////////////
			return false;
		}

		bool IsTouchDown()
		{
			////////////////////////////////////////
			return false;
		}

		void MakeTouchEvent(systouchtype type, int x, int y)
		{
			////////////////////////////////////////
		}

		bool IsKeyDown(keykind key)
		{
			////////////////////////////////////////
			return false;
		}

		int GetPlatformConfigNumber(string name)
		{
			int Result = -1;
			////////////////////////////////////////
			if(BxUtilGlobal::StrICmp(name, "MemSizeBx") == same)
				Result = 20000000;
			////////////////////////////////////////
			return Result;
		}

		string _tmp_ GetPlatformConfigString(string name)
		{
			global_data char Result[256] = {'\0',};
			////////////////////////////////////////
			return Result;
		}

		void SetSimulatorWindowPos(int x, int y)
		{
			////////////////////////////////////////
		}

		point GetSimulatorWindowPos()
		{
			point SimulatorPos = point::zero();
			////////////////////////////////////////
			return SimulatorPos;
		}

		point GetSimulatorCursorPos()
		{
			point SimulatorCursorPos = point::zero();
			////////////////////////////////////////
			return SimulatorCursorPos;
		}

		void DoSimulatorMinimize()
		{
			////////////////////////////////////////
		}

		bool Break(string name, string query __DEBUG_PRM__)
		{
			printf("#####-[%s]\r\n", name);
			fflush(stdout);
			return true;
			#ifdef __BX_DEBUG
				QMessageBox AssertBox(QMessageBox::Warning, "ASSERT BREAK", name,
					QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll);
				char AssertMessage[1024];
				sprintf(AssertMessage,
						"[QUERY] %s\t\t\r\n"
						"[METHOD] %s()\t\t\r\n"
						"[FILE/LINE] %s(%dLn)\t\t\r\n"
						"[THREAD_ID] %d\t\t\r\n"
						"\r\n"
						"(YES is Break, NO is Ignore)\t\t",
						query, __DEBUG_FUNC__, __DEBUG_FILE__, __DEBUG_LINE__,
						QThread::currentThreadId());
				AssertBox.setInformativeText(AssertMessage);
				AssertBox.setDefaultButton(QMessageBox::Yes);
				switch(AssertBox.exec())
				{
				case QMessageBox::Yes:
					Q_ASSERT(0);//////////////////////////////////
					break;
				case QMessageBox::NoToAll:
					return false;
				}
				return true;
			#else
				return false;
			#endif
		}

		void Trace(string map, const BxThrow& args)
		{
			printf(BxCore::Util::Print(map, args));
			fflush(stdout);
		}

		string _tmp_ GetOSName()
		{
			////////////////////////////////////////
			return "UNSUPPORTED";
		}

		string _tmp_ GetArchName()
		{
			////////////////////////////////////////
			return "UNSUPPORTED";
		}

		string _tmp_ GetDeviceID(int* integerid)
		{
			////////////////////////////////////////
			return "UNSUPPORTED";
		}

		string _tmp_ GetPhoneNumber()
		{
			////////////////////////////////////////
			return "UNSUPPORTED";
		}

		string _tmp_ GetAppPackage()
		{
			return GetConfigString("Bx.Application.Package", "com.bx.game");
		}

		string _tmp_ GetAppPackageExt()
		{
			////////////////////////////////////////
			return ".apk";
		}

		string _tmp_ GetAppVersion()
		{
			////////////////////////////////////////
			return GetConfigString("Bx.Application.Version", "1.x.x");
		}

		bool IsExistApp(string packagename)
		{
			////////////////////////////////////////
			return false;
		}

		void PopupOSExecute(string url, bool exitme)
		{
			////////////////////////////////////////
		}

		bool PopupOSKeyboard(string_rw result_str256, string default_str, string keyboard_name, oskeyboardtype type)
		{
			////////////////////////////////////////
			return false;
		}

		bool IsExistClassJNI(string classpath)
		{
			////////////////////////////////////////
			return false;
		}

		int CallStaticMethodJNI(string classpath, string methodname, string param1_str1024, int param2)
		{
			////////////////////////////////////////
			return -90001;
		}

		int AddCallbackJNI(string classpath, string methodname, callback_jni methodCB)
		{
			////////////////////////////////////////
			return -90001;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Surface
	{
		void Init()
		{
			////////////////////////////////////////
		}

		void Flush(void* ptr, const int row, const int height, const int pitch)
		{
			////////////////////////////////////////
		}

		void MiddleFlush()
		{
			////////////////////////////////////////
		}

		int GetWidthHW(bool usemargin)
		{
			int WidthHW = 0;
			////////////////////////////////////////
			return WidthHW;
		}

		int GetHeightHW(bool usemargin)
		{
			int HeightHW = 0;
			////////////////////////////////////////
			return HeightHW;
		}

		bool SetMarginMode(bool enable)
		{
			////////////////////////////////////////
			return enable;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Sound
	{
		id_sound Create(string filename, bool autoload)
		{
			////////////////////////////////////////
			return (id_sound) nullptr;
		}

		void Release(id_sound sound)
		{
			////////////////////////////////////////
		}

		void PlayMusic(id_sound sound, bool interrupt, const BxThrow& panorama)
		{
			////////////////////////////////////////
		}

		void StopMusic()
		{
			////////////////////////////////////////
		}

		void SetMusicVolume(uint percent)
		{
			////////////////////////////////////////
		}

		void PlayEffect(id_sound sound, bool interrupt)
		{
			////////////////////////////////////////
		}

		void StopEffect()
		{
			////////////////////////////////////////
		}

		void SetEffectVolume(uint percent)
		{
			////////////////////////////////////////
		}

		void PauseAll()
		{
			////////////////////////////////////////
		}

		void ResumeAll()
		{
			////////////////////////////////////////
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Util
	{
		void* Alloc(int size)
		{
			return (void*) new byte[size];
		}

		void Free(void* buf)
		{
			delete[] (byte*) buf;
		}

		void MemSet(void* dst, byte value, int length)
		{
			if(dst) memset(dst, value, length);
		}

		void MemCpy(void* dst, const void* src, int length)
		{
			if(dst) memmove(dst, src, length);
		}

		int MemCmp(const void* dst, const void* src, int length)
		{
			return memcmp(dst, src, length);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace File
	{
		bool IsExist(string filename)
		{
			return QFile::exists(filename);
		}

		id_file Open(string filename, string mode)
		{
			QFile* File = BxNew_Param(QFile, filename);
			if(File->open((BxUtilGlobal::StrCmp(mode, "wb") == same)? QFile::WriteOnly : QFile::ReadOnly))
				return (id_file) File;
			BxDelete(File);
			return nullptr;
		}

		void Close(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			BxDelete_ByType(QFile, file);
		}

		uint Read(id_file file, void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
				return (uint) ((QFile*) file)->read((char*) buffer, length);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
				Result += (uint) ((QFile*) file)->read(((char*) buffer) + i, BxUtilGlobal::Min(length - i, 4096));
				if(progress && !progress(-1, 0))
					return Result;
			}
			return Result;
		}

		uint ReadVariable(id_file file, uhuge* variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			byte CurBits = 0x80;
			while((CurBits & 0x80) && Read(file, &CurBits, 1, progress))
			{
				CurBits += additionkey;
				*variable |= (CurBits & 0x7F) << (7 * Result);
				++Result;
			}
			return Result;
		}

		uint Write(id_file file, const void* buffer, uint length, callback_progress progress)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			if(!progress)
				return (uint) ((QFile*) file)->write((const char*) buffer, length);
			uint Result = 0;
			for(uint i = 0; i < length; i += 4096)
			{
				Result += (uint) ((QFile*) file)->write(((const char*) buffer) + i, BxUtilGlobal::Min(length - i, 4096));
				if(progress && !progress(-1, 0))
					return Result;
			}
			return Result;
		}

		uint WriteVariable(id_file file, const uhuge variable, const byte additionkey, callback_progress progress)
		{
			uint Result = 0;
			uhuge AllBits = variable;
			do
			{
				byte CurBits = (AllBits & 0x7F);
				if(AllBits >>= 7) CurBits |= 0x80;
				CurBits += 0x100 - additionkey;
				Write(file, &CurBits, 1, progress);
				++Result;
			}
			while(AllBits);
			return Result;
		}

		uint WriteString(id_file file, string buffer, callback_progress progress)
		{
			return Write(file, buffer, BxUtilGlobal::StrLen(buffer), progress);
		}

		int Skip(id_file file, uint length)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			((QFile*) file)->seek(length);
			return (int) ((QFile*) file)->pos();
		}

		int GetSize(id_file file)
		{
			BxASSERT("BxCore::File<파일핸들이 nullptr입니다>", file);
			return (int) ((QFile*) file)->size();
		}

		int GetSize(string filename)
		{
			return (int) QFileInfo(filename).size();
		}

		uhuge GetDate(string filename)
		{
			return (uhuge) QFileInfo(filename).lastModified().toMSecsSinceEpoch();
		}

		bool Attach(string archivename)
		{
			return false;
		}

		void Detach()
		{
		}

		void MakeDirectory(string dirname, bool isFile)
		{
			char PathName[1024];
			PathName[0] = '\0';
			for(int i = 0; dirname[i]; ++i)
			{
				if(dirname[i] == '/' || dirname[i] == '\\')
				{
					if(*PathName) QDir().mkdir(PathName);
					PathName[i] = '/';
				}
				else PathName[i] = dirname[i];
				PathName[i + 1] = '\0';
			}
			if(*PathName && !isFile)
				QDir().mkdir(PathName);
		}

		uint SearchFiles(string dirname, callback_filesearch resultCB, void* data)
		{
			QStringList Files;
			QDir(dirname).entryList(Files, QDir::Files | QDir::Dirs | QDir::NoSymLinks);
			const int DirLength = BxUtilGlobal::StrLen(dirname);
			uint SearchCount = 0;
			char FileName[1024];
			BxUtilGlobal::StrCpy(FileName, dirname, DirLength);
			FileName[DirLength] = '/';
			for(int i = 0, iend = Files.length(); i < iend; ++i)
			{
				++SearchCount;
				BxUtilGlobal::StrCpy(&FileName[DirLength + 1], (string) Files[i].data());
				if(resultCB) resultCB(!IsExist(FileName), FileName, data);
			}
			return SearchCount;
		}

		bool RemoveFile(string filename, bool doRemoveBlankedDirectory)
		{
			if(!QFile::remove(filename))
				return false;
			else if(doRemoveBlankedDirectory)
			{
				BxString::Parse DirName(filename);
				do
				{
					DirName.DeleteLast();
					while(DirName.GetLast() != '/' && DirName.GetLast() != '\\')
						DirName.DeleteLast();
				}
				while(QDir().rmdir((string) DirName));
			}
			return true;
		}

		bool RenameFile(string srcname, string dstname)
		{
			return QFile::rename(srcname, dstname);
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Socket
	{
		id_socket Create()
		{
			TCPData* TCP = BxNew(TCPData);
			if(TCP->GetState() == socketstate_created)
				return (id_socket) TCP;
			BxDelete(TCP);
			return nullptr;
		}

		void Release(id_socket sock)
		{
			if(!sock) return;
			BxDelete_ByType(TCPData, sock);
		}

		socketstate GetState(id_socket sock)
		{
			if(!sock) return socketstate_null;
			return ((TCPData*) sock)->GetState();
		}

		connectresult Connect(id_socket sock, string addr, ushort port, uint timeout, callback_progress progress)
		{
			if(!sock) return connect_error_param;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() == socketstate_null) return connect_error_param;
			else if(TCP->GetState() == socketstate_connecting) return connect_connecting;
			else if(TCP->GetState() == socketstate_connected) return connect_connected;
			// IP조사
			QList<QHostAddress> Address = QHostInfo::fromName(QString(addr)).addresses();
			if(Address.length() == 0) return connect_error_wrong_address;
			TCP->SetState(socketstate_connecting);
			TCP->connectToHost(Address[0], port);
			if(timeout == 0) return connect_connecting;
			// 결과확인
			if(!TCP->waitForConnected(timeout))
			{
				TCP->Disconnect(true);
				return connect_disconnected;
			}
			return connect_connected;
		}

		void Disconnect(id_socket sock)
		{
			if(!sock) return;
			TCPData* TCP = (TCPData*) sock;
			TCP->Disconnect(false);
		}

		int Send(id_socket sock, const void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected)
				return -2;
			const int Result = TCP->write((const char*) buffer, len);
			TCP->flush();
			if(0 <= Result) return Result;
			return -100 - ((int) TCP->error());
		}

		int Recv(id_socket sock, void* buffer, int len)
		{
			if(!sock) return -1;
			TCPData* TCP = (TCPData*) sock;
			if(TCP->GetState() != socketstate_connected)
				return -2;
			const int Result = TCP->read((char*) buffer, len);
			if(0 <= Result) return Result;
			return -100 - ((int) TCP->error());
		}

		int Ping(string addr, uint timeout)
		{
			return -1;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Font
	{
		id_font Open(string filename, const int pointsize, const int numCacheGlyphs)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_font Open(const byte* buffer, const int buffersize, const int pointsize, const int numCacheGlyphs)
		{
			////////////////////////////////////////
			return nullptr;
		}

		void Close(id_font font)
		{
			////////////////////////////////////////
		}

		void SetEncoding(fontencoding encoding)
		{
			global_data fontencoding oldvalue = (fontencoding) -1;
			if(oldvalue == encoding) return;
			oldvalue = encoding;
			////////////////////////////////////////
		}

		fontsort SetSort(fontsort sort)
		{
			global_data fontsort oldvalue = (fontsort) -1;
			fontsort result = oldvalue;
			if(oldvalue != sort)
			{
				oldvalue = sort;
				////////////////////////////////////////
			}
			return result;
		}

		void SetStyle(bool underline, bool italic)
		{
			global_data bool oldvalue1 = false;
			global_data bool oldvalue2 = false;
			if(oldvalue1 == underline && oldvalue2 == italic) return;
			oldvalue1 = underline;
			oldvalue2 = italic;
			////////////////////////////////////////
		}

		size GetSize(id_font font, string str, const size s)
		{
			size Size = {0, 0};
			////////////////////////////////////////
			return Size;
		}

		void Draw(id_font font, string str, const point p, const size s, const color_x888 color, const byte opacity)
		{
			////////////////////////////////////////
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Thread
	{
		id_thread Create(callback_thread threadCB, void* data)
		{
			ThreadClass* Class = new ThreadClass(threadCB, data);
			return (id_thread) Class;
		}

		void Release(id_thread thread, bool dokill, bool dowait)
		{
			if(!thread) return;
			ThreadClass* Class = (ThreadClass*) thread;
			if(dokill) Class->exit();
			if(dowait) Class->wait();
			delete Class;
		}

		id_mutex OpenMutex()
		{
			QMutex* Mutex = new QMutex();
			return (id_mutex) Mutex;
		}

		void CloseMutex(id_mutex mutex)
		{
			if(!mutex) return;
			delete (QMutex*) mutex;
		}

		void Lock(id_mutex mutex)
		{
			if(!mutex) return;
			((QMutex*) mutex)->lock();
		}

		void Unlock(id_mutex mutex)
		{
			if(!mutex) return;
			((QMutex*) mutex)->unlock();
		}

		void* BindStorage(int* storagekey)
		{
			// 스레드별 저장소그룹 접근
			Lock(StorageClass::Mutex());
			BxVarMap<StorageClass>& StorageByThread = StorageClass::Map()[StorageClass::CurrentThreadID()];
			Unlock(StorageClass::Mutex());
			// 스레드내 개별저장소 접근
			StorageClass* Result = StorageByThread.Access((uint) storagekey);
			if(Result) return Result->Data;
			return StorageByThread[(uint) storagekey].Init(*storagekey);
		}

		void UnbindStorageAll()
		{
			Lock(StorageClass::Mutex());
			StorageClass::Map()[StorageClass::CurrentThreadID()].Reset();
			Unlock(StorageClass::Mutex());
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace Library
	{
		id_library Open(string filename)
		{
			////////////////////////////////////////
			return nullptr;
		}

		void Close(id_library handle)
		{
			////////////////////////////////////////
		}

		void* Link(id_library handle, string name)
		{
			////////////////////////////////////////
			return nullptr;
		}
	}

	// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	namespace OpenGL2D
	{
		void Init()
		{
			////////////////////////////////////////
		}

		void Quit()
		{
			////////////////////////////////////////
		}

		void Flush()
		{
			////////////////////////////////////////
		}

		void SetPerspective(const int mul, const int far, const int near)
		{
			////////////////////////////////////////
		}

		void SetOrthogonal(const int width, const int height, const int far, const int near)
		{
			////////////////////////////////////////
		}

		void AddViewTrans(int x, int y, int z)
		{
			////////////////////////////////////////
		}

		void AddViewRotX(fint angle)
		{
			////////////////////////////////////////
		}

		void AddViewRotY(fint angle)
		{
			////////////////////////////////////////
		}

		void AddViewRotZ(fint angle)
		{
			////////////////////////////////////////
		}

		id_opengl_form MakeForm_TextureMaterial(string filename)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_opengl_form MakeForm_TextureMaterial(color_a888* image, const int width, const int height, const int pitch)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_opengl_form MakeForm_ColorAmbient(const color_x888 color)
		{
			////////////////////////////////////////
			return nullptr;
		}

		void FreeForm(id_opengl_form form)
		{
			////////////////////////////////////////
		}

		id_opengl_outline MakeOutline_VertexUV(const rect& r, const rect& uvclip)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_opengl_outline MakeOutline_PolyVertexUV(const points& p, const rect& uvclip)
		{
			////////////////////////////////////////
			return nullptr;
		}

		id_opengl_outline MakeOutline_PolyVertexUV3D(const vertexs& v, const points& uv)
		{
			////////////////////////////////////////
			return nullptr;
		}

		void FreeOutline(id_opengl_outline outline)
		{
			////////////////////////////////////////
		}

		color_x888 Form_ColorAmbient_GetColor(id_opengl_form form)
		{
			////////////////////////////////////////
			return (color_x888) 0;
		}

		void Outline_VertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			////////////////////////////////////////
		}

		void Outline_VertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
			////////////////////////////////////////
		}

		void Outline_PolyVertexUV_SetScale(id_opengl_outline outline, fint hoz, fint ver)
		{
			////////////////////////////////////////
		}

		void Outline_PolyVertexUV_SetRotate(id_opengl_outline outline, int angle1024, bool doFlip)
		{
			////////////////////////////////////////
		}

		void Render(id_opengl_form form, id_opengl_outline outline, int x, int y, const byte opacity, const color_x888 color)
		{
			////////////////////////////////////////
		}

		void Render3D(id_opengl_form form, id_opengl_outline outline, int x, int y, int z, const byte opacity, const color_x888 color)
		{
			////////////////////////////////////////
		}

		void Clip(rect r)
		{
			////////////////////////////////////////
		}
	}
}
