#pragma once
#include <QEventLoop>
#include <QTcpSocket>
#include <QHostInfo>
#include <QThread>
#include <QMutex>
#include <QTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#ifdef __BX_DEBUG
	#include <QMessageBox>
#endif

#include <BxSingleton.hpp>
#include <BxPool.hpp>
#include <BxCore.hpp>
#include <BxVarMap.hpp>

namespace BxCore
{
	namespace Socket
	{
		/// @cond SECTION_NAME
		class TCPData : public QTcpSocket
		{
			Q_OBJECT
		private:
			socketstate State;
		private slots:
			void OnConnected()
			{
				State = socketstate_connected;
			}
			void OnDisconnected()
			{
				State = socketstate_disconnected;
			}
			void OnReadyRead()
			{
			}
			void OnError()
			{
				State = socketstate_null;
			}
		public:
			TCPData() : State(socketstate_null)
			{
				connect(this, SIGNAL(connected()), this, SLOT(OnConnected()));
				connect(this, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
				connect(this, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
				connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError()));
				ResetClient();
			}
			virtual ~TCPData()
			{
				Disconnect(true);
			}
			inline socketstate GetState()
			{
				return State;
			}
			inline void SetState(socketstate state)
			{
				State = state;
			}
			inline void ResetClient()
			{
				Disconnect(true);
				State = socketstate_created;
			}
			inline void Disconnect(bool dowait)
			{
				disconnectFromHost();
				if(dowait && !waitForDisconnected())
					State = socketstate_null;
			}
		};
		/// @endcond
	}

	namespace Thread
	{
		/// @cond SECTION_NAME
		class ThreadClass : public QThread
		{
			Q_OBJECT
		private:
			callback_thread CB;
			void* Data;
		protected:
			void run()
			{
				if(CB) CB(Data);
				BxSingleton::UnbindAll(true);
				BxPoolGlobal::UnbindAll();
				BxCore::Thread::UnbindStorageAll();
			}
		public:
			ThreadClass(callback_thread cb, void* data) : CB(cb), Data(data)
			{
				start();
			}
			virtual ~ThreadClass()
			{
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class StorageClass
		{
		public:
			void* Data;
		public:
			StorageClass() : Data(nullptr) {}
			~StorageClass() {BxCore::Util::Free(Data);}
			void* Init(int size)
			{
				Data = BxCore::Util::Alloc(size);
				BxCore::Util::MemSet(Data, 0, size);
				return Data;
			}
		public:
			global_func uint CurrentThreadID() {return (uint) QThread::currentThreadId();}
			global_func id_mutex& Mutex() {global_data id_mutex _ = OpenMutex(); return _;}
			global_func BxVarMap< BxVarMap<StorageClass> >& Map() {global_data BxVarMap< BxVarMap<StorageClass> > _; return _;}
		};
		/// @endcond
	}

	namespace OpenGL2D
	{
		// OpenGLForm
		/// @cond SECTION_NAME
		class OpenGLForm
		{
		};
		/// @endcond

		// OpenGLOutline
		/// @cond SECTION_NAME
		class OpenGLOutline
		{
		};
		class VertexUV : public OpenGLOutline
		{
		};
		class PolyVertexUV : public OpenGLOutline
		{
		};
		class PolyVertexUV3D : public OpenGLOutline
		{
		};
		/// @endcond
	}
}
