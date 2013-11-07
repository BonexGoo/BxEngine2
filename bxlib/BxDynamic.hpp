#pragma once
#include <BxCore.hpp>
#include <BxUtil.hpp>
#include <BxVarMap.hpp>

//! \brief ��������
namespace BxDynamic
{
	//! \brief ���ҽ� ����
	class Resource
	{
		private: class Pool;
		public: class BindPool;
		public: class LinkPool;

		// ����ȭ ���� ///////////////////////////////////////////////////////////////////////////////
		// ���
		protected: bool Bind(rsid ID)
		{
			if(mBinder) return false;
			mBinder = BinderBegin(GetCachePoolName())->FindBinder(ID);
			mBinder->ResetData(this);
			((LinkPool*) mBinder->TopLinker())->CallSerialize(bindstate_bind);
			return true;
		}
		// �̰�
		protected: bool Entrust(Resource* Res)
		{
			if(!mBinder) return false;
			if(Res && Res->mBinder) return false;
			mBinder->ResetData(Res);
			((LinkPool*) mBinder->TopLinker())->CallSerialize((Res)? bindstate_entrust : bindstate_unbind);
			if(Res)
			{
				Res->mBinder = mBinder;
				mBinder = null;
			}
			return true;
		}
		// ID��ȯ
		public: rsid GetID()
		{
			if(!mBinder) return rsid_null;
			return mBinder->GetID();
		}
		// ID����
		protected: global_func rsid MakeID()
		{
			global_data rsid LastID = rsid_temp;
			return LastID = (rsid)(LastID + (rsid) 1);
		}
		// �������̽�
		protected: virtual void OnLoad(id_file File) = 0;
		protected: virtual void OnSave(id_file File) = 0;
		// �����Լ�
		public: global_func BindPool* BinderBegin(string CachePoolName)
		{
			global_data LinkPool LPool(null);
			global_data BxVarMap<BindPool> BPoolMap;
			if(!BPoolMap.Access(CachePoolName))
				BPoolMap[CachePoolName].Init(LPool.New(null));
			return &BPoolMap[CachePoolName];
		}

		// ĳ�� ���� /////////////////////////////////////////////////////////////////////////////////
		public: global_func void SortCachePool(string CachePoolName)
		{
			// ������ ����(��������:1024)
			BindPool* HeadBinder = BinderBegin(CachePoolName);
			BindPool* CurBinder = HeadBinder->GetNext();
			for(int i = 0; i < 1024 && CurBinder != HeadBinder && CurBinder->GetNext() != HeadBinder; ++i)
			{
				BindPool* NextBinder = CurBinder->GetNext();
				if(NextBinder->GetData()->mCacheScore < CurBinder->GetData()->mCacheScore)
					CurBinder->GetPrev()->Add(NextBinder->Sub());
				CurBinder = CurBinder->GetNext();
			}
		}
		protected: void AllocCache(int BufferSize, int UsingCacheSize)
		{
			BxAssert("BxDynamic::Resource<�̹� �Ҵ�� ���۰� �����մϴ�>", mBuffer == null);
			const int CachePoolSize = GetCachePoolSize(GetCachePoolName());
			int CachePoolUsed = GetCachePoolUsed(GetCachePoolName());
			// Ȯ��
			BindPool* HeadBinder = BinderBegin(GetCachePoolName());
			BindPool* CurBinder = HeadBinder->GetNext();
			while(CachePoolSize < CachePoolUsed + UsingCacheSize || !(mBuffer = (byte*) BxAlloc(BufferSize)))
			{
				BxAssert("BxDynamic::Resource<���õ� ��� ĳ�ø� ������� ���� �Ҵ翡 �����Ͽ����ϴ�>", CurBinder != HeadBinder);
				Resource* CurResource = CurBinder->GetData();
				if(CurResource->mBuffer && CurResource->mLockCount == 0)
				{
					CurResource->FreeCacheManually();
					CachePoolUsed = GetCachePoolUsed(GetCachePoolName());
				}
				CurBinder = CurBinder->GetNext();
			}
			AdjustCachePoolUsed(GetCachePoolName(), mUsedCacheSize = UsingCacheSize);
			UpdateCache();
		}
		protected: void FreeCacheManually()
		{
			if(!mBuffer) return;
			OnPreRemoveCache();
			BxFree(mBuffer);
			AdjustCachePoolUsed(GetCachePoolName(), -mUsedCacheSize);
			mUsedCacheSize = 0;
		}
		protected: byte* LockCache()
		{
			++mLockCount;
			return mBuffer;
		}
		protected: int UnlockCache()
		{
			BxAssert("BxDynamic::Resource<LockCount�� �ʰ��� Unlock�Դϴ�>", 0 < mLockCount);
			return --mLockCount;
		}
		protected: void UpdateCache()
		{
			BxAssert("BxDynamic::Resource<��ϵ��� ���� ĳ���Դϴ�>", mBinder != null);
			// ���۰� �������� ������ �۾����
			if(!mBuffer) return;
			// ���ھ��
			mCacheScore = GetCacheScore(BxCore::Main::GetCurrentRenderCount(), mUsedCacheSize);
			BindPool* HeadBinder = BinderBegin(GetCachePoolName());
			// �̹� �ö� �������ھ ������ ����
			if(HeadBinder->GetPrev() == mBinder) return;
			// ������ ������
			HeadBinder->GetPrev()->Add(mBinder->Sub());
		}
		// �������̽�
		public: virtual string _tmp_ GetCachePoolName() = 0;
		protected: virtual int GetCacheScore(int FrameTime, int CacheSize) = 0;
		protected: virtual void OnPreRemoveCache() = 0;
		// �����Լ�
		protected: global_func int GetCachePoolSize(string CachePoolName)
		{
			global_data BxVarMap<int> CachePoolSizeMap;
			if(!CachePoolSizeMap.Access(CachePoolName))
			{
				const int CachePoolSize = BxCore::System::GetConfigNumber(CachePoolName, 1024 * 1024 * 1);
				CachePoolSizeMap[CachePoolName] = CachePoolSize;
			}
			return CachePoolSizeMap[CachePoolName];
		}
		private: global_func int AdjustCachePoolUsed(string CachePoolName, int CacheSize)
		{
			global_data BxVarMap<int> CachePoolUsedMap;
			if(!CachePoolUsedMap.Access(CachePoolName))
				CachePoolUsedMap[CachePoolName] = 0;
			return CachePoolUsedMap[CachePoolName] += CacheSize;
		}
		protected: global_func int GetCachePoolUsed(string CachePoolName)
		{
			return AdjustCachePoolUsed(CachePoolName, 0);
		}

		// ������ & �Ҹ��� ///////////////////////////////////////////////////////////////////////////
		protected: Resource() : mBinder(null), mBuffer(null), mCacheScore(0), mUsedCacheSize(0), mLockCount(0) {}
		public: virtual ~Resource() {Entrust(null); FreeCacheManually();}
		// �������
		private: BindPool* mBinder;
		private: byte* mBuffer;
		private: int mCacheScore;
		private: int mUsedCacheSize;
		private: int mLockCount;

		// Ǯ���� ////////////////////////////////////////////////////////////////////////////////////
		private: class Pool
		{
			protected: Pool* Prev; // MemberData
			protected: Pool* Next; // MemberData
			protected: Pool() {Prev = this; Next = this;}
			public: virtual ~Pool()
			{
				Pool* CurNode = Next;
				while(CurNode != this)
				{
					Pool* NextNode = CurNode->Next;
					CurNode->Next = CurNode;
					BxDelete(CurNode);
					CurNode = NextNode;
				}
			}
			public: virtual Pool* New(Pool*) = 0;
			public: virtual void Delete() = 0;
			public: virtual bool IsSame(void*) = 0;
			public: Pool* Add(Pool* Data)
			{
				Data->Prev = this;
				Data->Next = Next;
				Next->Prev = Data;
				Next = Data;
				return Data;
			}
			public: Pool* Sub()
			{
				Prev->Next = Next;
				Next->Prev = Prev;
				Prev = this;
				Next = this;
				return this;
			}
			public: Pool* Find(void* Key)
			{
				Pool* CurNode = Next;
				while(CurNode != this)
				{
					if(CurNode->IsSame(Key))
						return CurNode;
					CurNode = CurNode->Next;
				}
				return null;
			}
		};

		// ���ε�Ǯ���� //////////////////////////////////////////////////////////////////////////////
		public: class BindPool : public Pool
		{
			friend class LinkPool;
			friend BindPool* Resource::BinderBegin(string CachePoolName);
			private: rsid ParamID; // MemberData
			private: Resource* ParamData; // MemberData
			private: Pool* TopChildLinker; // MemberData
			public: BindPool(Pool* TopChild = null) : ParamData(null) {Init(TopChild);}
			public: virtual ~BindPool() {ResetData(); TopChildLinker->Delete();}
			public: void Init(Pool* TopChild) {TopChildLinker = TopChild;}
			private: virtual Pool* New(Pool* TopChild) {return BxNew_Param(BindPool, TopChild);}
			private: virtual void Delete() {Pool* Temp = this; BxDelete(Temp);}
			private: virtual bool IsSame(void* Key) {return (Key == (void*) ParamID);}
			// �����Լ�
			public: void ResetData(Resource* Data = null) {ParamData = Data;}
			public: BindPool* FindBinder(rsid ID)
			{
				BindPool* OneBinder = (BindPool*) Find((void*) ID);
				if(!OneBinder)
				{
					OneBinder = (BindPool*) New(TopChildLinker->New(null));
					OneBinder->ParamID = ID;
					Add(OneBinder);
				}
				return OneBinder;
			}
			public: rsid GetID() {return ParamID;}
			public: Resource* GetData() {return ParamData;}
			public: Pool* TopLinker() {return TopChildLinker;}
			public: Pool* AddLinker() {return TopChildLinker->Add(TopChildLinker->New(this));}
			public: BindPool* GetPrev() {return (BindPool*) Prev;}
			public: BindPool* GetNext() {return (BindPool*) Next;}
		};

		// ��ũǮ���� ////////////////////////////////////////////////////////////////////////////////
		public: class LinkPool : public Pool
		{
			friend BindPool* Resource::BinderBegin(string CachePoolName);
			private: BindPool* ParentBinder; // MemberData
			private: callback_serialize ParamCB; // MemberData
			private: string ParamComment; // MemberData
			private: LinkPool(BindPool* Parent) : ParentBinder(Parent), ParamCB(null), ParamComment(null) {}
			public: virtual ~LinkPool() {ResetData();}
			private: virtual Pool* New(Pool* Parent) {return BxNew_Param(LinkPool, (BindPool*) Parent);}
			private: virtual void Delete() {Pool* Temp = this; BxDelete(Temp);}
			private: virtual bool IsSame(void* Key) {return (this == (LinkPool*) Key);}
			// �����Լ�
			public: void ResetData(callback_serialize CB = null, string Comment = null)
			{
				ParamCB = CB;
				ParamComment = BxUtilGlobal::StrFree(ParamComment);
				if(Comment) ParamComment = BxUtilGlobal::StrCpyWithAlloc(Comment);
				if(ParamCB && ParentBinder && ParentBinder->ParamData)
					ParamCB(bindstate_binded, ParamComment);
			}
			public: void Remove() {Sub()->Delete();}
			public: rsid GetID()
			{
				if(!ParentBinder) return rsid_null;
				return ParentBinder->ParamID;
			}
			public: Resource* GetResource()
			{
				if(!ParentBinder) return null;
				return ParentBinder->ParamData;
			}
			public: void CallSerialize(bindstate State)
			{
				LinkPool* CurNode = (LinkPool*) Next;
				while(CurNode != this)
				{
					if(CurNode->ParamCB)
						CurNode->ParamCB(State, CurNode->ParamComment);
					CurNode = (LinkPool*) CurNode->Next;
				}
			}
		};
	};

	//! \brief ����ȭ ����
	template<typename TYPE>
	class Serialize
	{
		// ����
		public: bool Link(rsid ID, callback_serialize CB = null, string Comment = null)
		{
			if(Linker) return false;
			Linker = (Resource::LinkPool*) Resource::BinderBegin(GetClass()->GetCachePoolName())->FindBinder(ID)->AddLinker();
			Linker->ResetData(CB, Comment);
			return true;
		}
		// ����
		public: TYPE* Access()
		{
			if(!Linker) return null;
			return (TYPE*) Linker->GetResource();
		}
		// ���Ϸε�
		public: rsid Load(id_file File)
		{
			uhuge ID = (uhuge) rsid_null;
			BxCore::File::Read(File, &ID, 8);
			return (rsid) ID;
		}
		// ���ϼ��̺�
		public: void Save(id_file File)
		{
			uhuge ID = (uhuge) rsid_null;
			if(Linker) ID = (uhuge) Linker->GetID();
			BxCore::File::Write(File, &ID, 8);
		}
		// ������ & �Ҹ���
		public: Serialize() : Linker(null) {}
		public: ~Serialize() {if(Linker) Linker->Remove();}
		// �������
		private: Resource::LinkPool* Linker;
		// �����Լ�
		private: global_func Resource* GetClass() {global_data TYPE Class; return &Class;}
	};
}
