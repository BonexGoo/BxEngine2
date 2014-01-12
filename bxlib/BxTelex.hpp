#pragma once
#include <BxSingleton.hpp>
#include <BxThrow.hpp>
#include <BxMemory.hpp>
#include <BxVar.hpp>
#include <BxVarMap.hpp>
#include <BxVarVector.hpp>

//! \brief HQ서버연동 전보시스템
class BxTelex
{
	public: class HQ;
	public: class Report;
	public: class Command;
	public: typedef BxVar<HQ> BxVarHQ;
	public: typedef BxVar<Report> BxVarReport;
	public: typedef BxVar<Command> BxVarCommand;
	public: enum Type {Field, Check, Combo, Timer};
	public: typedef bool (*UpdateCommandCB)(BxTelex* telex, void* cbdata);

	public: void UpdateReportValue(Report* report, string value)
	{
		BxAssert("BxTelex<해당 Report가 존재하지 않습니다>", ReportValues.Access((uint) report));
		ReportValues[(uint) report].SetValue(value, true);
	}

	public: string _tmp_ GetReportValue(Report* report, bool doFlush)
	{
		BxAssert("BxTelex<해당 Report가 존재하지 않습니다>", ReportValues.Access((uint) report));
		return ReportValues[(uint) report].GetValue(doFlush);
	}

	public: string _tmp_ GetDifferentedValue(Report* report)
	{
		BxAssert("BxTelex<해당 Report가 존재하지 않습니다>", ReportValues.Access((uint) report));
		return ReportValues[(uint) report].GetDifferentedValue(report->GetDefault());
	}

	public: string _tmp_ GetCommandValue(Command* command)
	{
		BxAssert("BxTelex<해당 Command가 존재하지 않습니다>", CommandValues.Access((uint) command));
		return CommandValues[(uint) command].GetValue(false);
	}

	public: Command* FindCommand(bool updatedOnly)
	{
		return nullptr; ///////////////////////////////////////////////////////////////////
	}

	public: Command* NextCommand()
	{
		return nullptr; ///////////////////////////////////////////////////////////////////
	}

	private: class UpdatedValue
	{
		friend class BxVarMap<UpdatedValue>;
		private: bool IsUpdated;
		private: string Value;
		private: UpdatedValue() : IsUpdated(false), Value(nullptr) {}
		private: ~UpdatedValue() {Value = BxUtilGlobal::StrFree(Value);}
		public: inline void SetValue(string _tmp_ value, bool doUpdate)
		{
			if(doUpdate) IsUpdated = true;
			Value = BxUtilGlobal::StrFree(Value);
			Value = BxUtilGlobal::StrCpyWithAlloc(value);
		}
		public: inline string _tmp_ GetValue(bool doFlush)
		{
			if(doFlush)
			{
				if(IsUpdated)
				{
					IsUpdated = false;
					return Value;
				}
				return nullptr;
			}
			return Value;
		}
		public: inline string _tmp_ GetDifferentedValue(string defaultValue)
		{
			IsUpdated = false;
			if(BxUtilGlobal::StrCmp(Value, defaultValue) == same)
				return nullptr;
			return Value;
		}
	};

	friend class BxMemory;
	private: int TelexID;
	private: string ProjectName;
	private: string PeerName;
	private: BxVarMap<UpdatedValue> ReportValues;
	private: BxVarVector<uint, 4> ReportIDs;
	private: BxVarMap<UpdatedValue> CommandValues;
	private: BxVarVector<uint, 4> CommandIDs;
	private: UpdateCommandCB CB;
	private: void* CBData;
	// 생성자, 소멸자
	private: BxTelex() {HQ::Bind(this);}
	private: ~BxTelex()
	{
		HQ::Unbind(this);
		ProjectName = BxUtilGlobal::StrFree(ProjectName);
		PeerName = BxUtilGlobal::StrFree(PeerName);
	}
	// 인스턴스관리
	public: global_func BxTelex* Create(string projectname, string peername, const BxThrow& reports, const BxThrow& commands, UpdateCommandCB cb, void* cbdata)
	{
		global_data int LastTelexID = 0;
		BxTelex* Result = BxNew(BxTelex);
		Result->TelexID = --LastTelexID;
		Result->ProjectName = BxUtilGlobal::StrCpyWithAlloc(projectname);
		Result->PeerName = BxUtilGlobal::StrCpyWithAlloc(peername);
		for(int i = 0; i < reports.Length(); ++i)
		{
			Report** OneReport = reports.Access<Report*>(i);
			BxAssert("BxTelex<reports파라미터 타입매칭 실패>", OneReport);
			if(OneReport)
			{
				Result->ReportValues[(uint) *OneReport].SetValue((*OneReport)->GetDefault(), false);
				Result->ReportIDs[LAST] = (uint) *OneReport;
			}
		}
		for(int i = 0; i < commands.Length(); ++i)
		{
			Command** OneCommand = commands.Access<Command*>(i);
			BxAssert("BxTelex<commands파라미터 타입매칭 실패>", OneCommand);
			if(OneCommand)
			{
				Result->CommandValues[(uint) *OneCommand].SetValue((*OneCommand)->GetDefault(), false);
				Result->CommandIDs[LAST] = (uint) *OneCommand;
			}
		}
		Result->CB = cb;
		Result->CBData = cbdata;
		return Result;
	}
	public: void DestroyMe(BxTelex*& me)
	{
		BxAssert("BxTelex<파라미터 오류>", this == me);
		BxDelete(me);
	}

	public: class HQ
	{
		friend class BxPool<HQ>;
		private: string Domain;
		private: int Port;
		private: int Freq;
		private: id_socket Sock;
		private: uhuge TransTime;
		private: BxVarVector<BxTelex*, 16> TransPeers;
		private: BxVarVector<int, 4> DestroyedTelexIDs;
		// 생성자, 소멸자, 풀
		private: HQ() {}
		private: ~HQ()
		{
			Domain = BxUtilGlobal::StrFree(Domain);
			BxCore::Socket::Release(Sock);
			BxAssert("BxTelex::HQ<아직 연결된 Peer가 있습니다>", TransPeers.Length() == 0);
		}
		private: global_func BxVarHQ& Pool() {global_data BxVarHQ _; return _;}
		// 인스턴스관리
		public: global_func void Attach(string domain, int port, int freq)
		{
			Pool()[LAST].Domain = BxUtilGlobal::StrCpyWithAlloc(domain);
			Pool()[END].Port = port;
			Pool()[END].Freq = freq;
			Pool()[END].Sock = nullptr;
			Pool()[END].TransTime = BxCore::System::GetTimeMilliSecond();
		}
		public: global_func bool Detach()
		{
			if(Pool().Length()) Pool().Delete(END);
			return (0 < Pool().Length());
		}
		// 바인딩관련
		public: global_func void Bind(BxTelex* Peer)
		{
			global_data int Turn = 0;
			if(!Pool().Length()) return;
			Pool()[Turn++ % Pool().Length()].TransPeers[LAST] = Peer;
		}
		public: global_func void Unbind(BxTelex* Peer)
		{
			for(int i = 0; i < Pool().Length(); ++i)
			{
				HQ& CurHQ = Pool()[i];
				for(int j = 0; j < CurHQ.TransPeers.Length(); ++j)
					if(CurHQ.TransPeers[j] == Peer)
					{
						if(0 <= Peer->TelexID)
							CurHQ.DestroyedTelexIDs[LAST] = Peer->TelexID;
						CurHQ.TransPeers.Delete(j);
						return;
					}
			}
			BxAssert("BxTelex::HQ<Peer를 찾지 못하였습니다>", Pool().Length() == 0);
		}

		// 트랜잭션시도
		// ■ 업데이트(생성, 종료, 변경)
		// 피어생성(클→HQ) : [4:INIT][4:사이즈] - [4:텔렉스ID][S:프로젝트명][S:피어명]{[4:로그PTR]...}
		// 피어종료(클→HQ) : [4:EXIT][4:사이즈] - {[4:텔렉스ID]...}
		// 로그변경(클→HQ, HQ→클) : [4:LOGU][4:사이즈] - [4:텔렉스ID]{[4:로그PTR][S:값]...}
		// 모든업데이트완료(클→HQ, HQ→클) : [4:DONE][4:0]
		// ■ 로그정보
		// 로그정보요청(HQ→클) : [4:LOGQ][4:사이즈] - {[4:로그PTR]...}
		// 로그정보제공(클→HQ) : [4:LOGA][4:사이즈] - [4:로그PTR][4:타입ID][S:이름][S:디폴트값]
		// ★ 사이즈필드의 값은 1024이하만 가능
		public: global_func void TryTransaction()
		{
			#define SEND_PACKET(HQ, NAME) for(bool _try_ = HQ._SendBegin(NAME); _try_; _try_ = HQ._SendEnd())
			const int iTurn = BxCore::Main::GetCurrentUpdateCount();
			for(int i = 0; i < Pool().Length(); ++i)
			{
				HQ& CurHQ = Pool()[(i + iTurn) % Pool().Length()];
				if(CurHQ.TransTime < BxCore::System::GetTimeMilliSecond())
				{
					CurHQ.TransTime = BxCore::System::GetTimeMilliSecond() + CurHQ.Freq;
					// 연결상태 체크
					bool IsReconnected = false;
					if(BxCore::Socket::GetState(CurHQ.Sock) != socketstate_connected)
					{
						if(CurHQ.Sock) IsReconnected = true;
						BxCore::Socket::Release(CurHQ.Sock);
						CurHQ.Sock = BxCore::Socket::Create();
						BxCore::Socket::Connect(CurHQ.Sock, CurHQ.Domain, CurHQ.Port, 3000);
					}
					// 업데이트
					if(BxCore::Socket::GetState(CurHQ.Sock) == socketstate_connected)
					{
						// 예약된 패킷
						if(0 < CurHQ.DestroyedTelexIDs.Length() && !IsReconnected)
						{
							// 피어종료
							SEND_PACKET(CurHQ, 'EXIT')
							{
								for(int j = 0; j < CurHQ.DestroyedTelexIDs.Length(); ++j)
									CurHQ._SendInt4(CurHQ.DestroyedTelexIDs[j]); // [4:텔렉스ID]
							}
						}
						CurHQ.DestroyedTelexIDs.Reset();
						// 피어별 패킷
						for(int j = 0; j < CurHQ.TransPeers.Length(); ++j)
						{
							BxTelex* CurPeer = CurHQ.TransPeers[j];
							// 피어생성
							if(CurPeer->TelexID < 0 || IsReconnected)
							{
								CurPeer->TelexID = BxUtilGlobal::Abs(CurPeer->TelexID);
								SEND_PACKET(CurHQ, 'INIT')
								{
									CurHQ._SendInt4(CurPeer->TelexID); // [4:텔렉스ID]
									CurHQ._SendString4(CurPeer->ProjectName); // [S:프로젝트명]
									CurHQ._SendString4(CurPeer->PeerName); // [S:피어명]
									for(int p = 0; p < CurPeer->ReportIDs.Length(); ++p)
										CurHQ._SendInt4(CurPeer->ReportIDs[p]); // [4:로그PTR]
									for(int p = 0; p < CurPeer->CommandIDs.Length(); ++p)
										CurHQ._SendInt4(CurPeer->CommandIDs[p]); // [4:로그PTR]
								}
							}
							// 리포트업데이트
							bool IsSomethingUpdated = false;
							for(int r = 0; r < CurPeer->ReportIDs.Length(); ++r)
							{
								string _tmp_ ReportValue = (IsReconnected)? CurPeer->GetDifferentedValue((Report*) CurPeer->ReportIDs[r])
									: CurPeer->GetReportValue((Report*) CurPeer->ReportIDs[r], true);
								if(ReportValue)
								{
									if(!IsSomethingUpdated)
									{
										IsSomethingUpdated = true;
										CurHQ._SendBegin('LOGU');
										CurHQ._SendInt4(CurPeer->TelexID); // [4:텔렉스ID]
									}
									CurHQ._SendInt4((int) CurPeer->ReportIDs[r]); // [4:로그PTR]
									CurHQ._SendString4(ReportValue); // [S:값]
								}
							}
							if(IsSomethingUpdated) CurHQ._SendEnd();
						}
						// 업데이트완료
						SEND_PACKET(CurHQ, 'DONE');
						// 리시브
						uint RecvHeader[2] = {0, 0};
						int RecvHeaderResult = 0, TempID = 0;
						while((RecvHeaderResult = BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) RecvHeader, 8, 3000, false)) == 8)
						{
							if(RecvHeader[0] == 'DONE')
								break;
							switch(RecvHeader[0])
							{
							case 'LOGQ':
								SEND_PACKET(CurHQ, 'LOGA')
								{
									for(int k = 0, kend = RecvHeader[1] / 4; k < kend; ++k)
									{
										Log* LogPtr = nullptr;
										if(BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) &LogPtr, 4, 3000, false) == 4)
										{
											CurHQ._SendInt4((int) LogPtr);
											CurHQ._SendInt4(LogPtr->GetType());
											CurHQ._SendString4(LogPtr->GetName());
											CurHQ._SendString4(LogPtr->GetDefault());
										}
									}
								}
								break;
							case 'LOGU':
								if(BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) &TempID, 4, 3000, false) == 4)
								{
									RecvHeader[1] -= 4;
									BxTelex* CurPeer = nullptr;
									for(int k = 0; k < CurHQ.TransPeers.Length() && !CurPeer; ++k)
										if(CurHQ.TransPeers[k]->TelexID == TempID)
											CurPeer = CurHQ.TransPeers[k];
									while(0 < RecvHeader[1])
									{
										if(BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) &TempID, 4, 3000, false) == 4)
										{
											RecvHeader[1] -= 4;
											short ValueLength;
											if(BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) &ValueLength, 2, 3000, false) == 2)
											{
												RecvHeader[1] -= 2;
												string Value = BxUtilGlobal::StrAlloc(ValueLength);
												if(BxCore::Socket::RecvFully(CurHQ.Sock, (byte*) Value, ValueLength, 3000, false) == ValueLength)
												{
													RecvHeader[1] -= ValueLength;
													if(CurPeer && CurPeer->CommandValues.Access((uint) TempID))
														CurPeer->CommandValues[(uint) TempID].SetValue(Value, false);
												}
												BxUtilGlobal::StrFree(Value);
											}
										}
									}
									if(CurPeer) CurPeer->CB(CurPeer, CurPeer->CBData);
								}
								break;
							}
						}
						if(RecvHeaderResult != 8)
							BxCore::Socket::Disconnect(CurHQ.Sock);
					}
					break; // 프레임당 처리제한
				}
			}
			#undef SEND_PACKET
		}
		private: void _PushByteStream(const void* data, int length)
		{
			global_data const int Length = 32;
			global_data int Offset = 4;
			byte* Bytes = nullptr;
			BxSINGLETON(Bytes, byte, Length);
			if(0 < length)
			{
				if(Length < Offset + length)
					BxSingleton::Rebind(Bytes, (Offset + length + 0xF) & ~0xF);
				BxCore::Util::MemMove(Bytes + Offset, data, length);
				Offset += length;
			}
			else
			{
				((uint*) Bytes)[0] = Offset - 4;
				*((void**) data) = Bytes;
				Offset = 4;
			}
		}
		private: bool _SendBegin(uint value)
		{
			if(Sock)
				BxCore::Socket::Send(Sock, &value, 4);
			return true;
		}
		private: bool _SendEnd()
		{
			// 스트림마감
			if(Sock)
			{
				byte* Bytes = nullptr;
				_PushByteStream(&Bytes, 0);
				const uint Length = 4 + *((uint*) Bytes);
				BxCore::Socket::Send(Sock, Bytes, Length);
			}
			return false;
		}
		private: void _SendInt4(int value)
		{
			if(Sock)
				_PushByteStream(&value, 4);
		}
		private: void _SendString4(string value)
		{
			if(Sock)
			{
				const short Length = (short) BxUtilGlobal::StrLen(value);
				_PushByteStream(&Length, 2);
				if(0 < Length) _PushByteStream(value, Length);
			}
		}
	};

	private: class Log
	{
		friend class BxPool<Log>;
		protected: BxTelex::Type Type;
		protected: string Name;
		protected: string Default;
		// 생성자, 소멸자
		protected: Log() {}
		protected: Log(BxTelex::Type type, string name, string defaultValue) : Type(type)
		{
			Name = BxUtilGlobal::StrCpyWithAlloc(name);
			Default = BxUtilGlobal::StrCpyWithAlloc(defaultValue);
		}
		protected: virtual ~Log()
		{
			Name = BxUtilGlobal::StrFree(Name);
			Default = BxUtilGlobal::StrFree(Default);
		}
		protected: template<typename TYPE> bool RemoveByPool(BxVar<TYPE>& Pool)
		{
			for(int i = 0; i < Pool.Length(); ++i)
				if(Pool.Access(i) == this)
				{
					TYPE* OneData = nullptr;
					Pool.RemoveData(i, &OneData);
					return true;
				}
			BxAssert("BxTelex::Log<Pool에서 This를 찾을 수 없습니다>", false);
			return false;
		}
		public: virtual uint GetType() = 0;
		public: string GetName() {return Name;}
		public: string GetDefault() {return Default;}
	};

	public: class Report : public Log
	{
		friend class BxPool<Report>;
		friend class BxMemory;
		// 생성자, 소멸자, 풀
		private: Report() {}
		private: Report(BxTelex::Type type, string name, string defaultValue) : Log(type, name, defaultValue) {}
		private: ~Report() {}
		private: global_func BxVarReport& Pool() {global_data BxVarReport _; return _;}
		// 인스턴스관리
		public: global_func Report* Create(BxTelex::Type type, string name, string defaultValue)
		{
			Report* Result = BxNew_Param(Report, type, name, defaultValue);
			Pool().Insert(LAST, Result);
			return Result;
		}
		public: void DestroyMe(Report*& me)
		{
			BxAssert("BxTelex::Report<파라미터 오류>", this == me);
			if(RemoveByPool(Pool())) BxDelete(me);
		}
		public: virtual uint GetType() {return 0 + ((uint) Type);}
	};

	public: class Command : public Log
	{
		friend class BxPool<Command>;
		friend class BxMemory;
		// 생성자, 소멸자, 풀
		private: Command() {}
		private: Command(BxTelex::Type type, string name, string defaultValue) : Log(type, name, defaultValue) {}
		private: ~Command() {}
		private: global_func BxVarCommand& Pool() {global_data BxVarCommand _; return _;}
		// 인스턴스관리
		public: global_func Command* Create(BxTelex::Type type, string name, string defaultValue)
		{
			Command* Result = BxNew_Param(Command, type, name, defaultValue);
			Pool().Insert(LAST, Result);
			return Result;
		}
		public: void DestroyMe(Command*& me)
		{
			BxAssert("BxTelex::Command<파라미터 오류>", this == me);
			if(RemoveByPool(Pool())) BxDelete(me);
		}
		public: virtual uint GetType() {return 4 + ((uint) Type);}
	};
};
