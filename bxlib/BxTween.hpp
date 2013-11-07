#pragma once
#include <BxUtil.hpp>

//! \brief 티폰툴 파티클 운영
class BxTween
{
	// 콜백
	typedef void (*OnPart)(const int ID, const int X, const int Y, const int Opacity, const int Motion, const id_memory Message, void* Data);

public:
	BxTween() : _PartLength(0) {}
	~BxTween() {Close();}

	bool Open(string filename)
	{
		id_file File = BxCore::File::Open(filename, "rb");
		int Length = BxCore::File::GetSize(File);
		byte* Resource = (byte*) BxAlloc(Length);
		BxCore::File::Read(File, Resource, Length);
		BxCore::File::Close(File);
		bool Result = Open(Resource);
		BxFree(Resource);
		return Result;
	}

	bool Open(byte* resource)
	{
		// 포맷인증
		byte* Resource = resource;
		if(BxUtilGlobal::LoadUint32(Resource) != *((uint*) "ptb2"))
			return false;
		// 초기화
		Close();
		const int PartLength = BxUtilGlobal::LoadUint8(Resource);
		BxCore::Util::MemSet(this, 0, sizeof(BxTween) - sizeof(BxVar<_PartTarget>));
		for(int i = 0; i < PartLength - 1; ++i)
			BxCore::Util::MemSet(&_Target[i], 0, sizeof(_PartTarget));
		_IsAnimationPlay = false;
		_IsAnimationFinished = true;
		_MsPerFrame = 50;
		_AnimationRandKey = 1234;
		_PartLength = PartLength;
		// 로드과정
		for(int i = 0; i < _PartLength; ++i)
		{
			_PartTarget* Target = (i == 0)? (_PartTarget*) &_Begin : &_Target[i - 1];
			Target->IsBegin = (i == 0);
			Target->TargetX1 = BxUtilGlobal::LoadInt16(Resource);
			Target->TargetY1 = BxUtilGlobal::LoadInt16(Resource);
			Target->TargetX2 = BxUtilGlobal::LoadInt16(Resource);
			Target->TargetY2 = BxUtilGlobal::LoadInt16(Resource);
			Target->PartRateMin = BxUtilGlobal::LoadInt16(Resource);
			Target->PartRateMax = BxUtilGlobal::LoadInt16(Resource);
			Target->Position = BxUtilGlobal::LoadInt8(Resource);
			Target->OpacityMin = BxUtilGlobal::LoadInt8(Resource);
			Target->OpacityMax = BxUtilGlobal::LoadInt8(Resource);
			const int MessageLength = BxUtilGlobal::LoadInt8(Resource);
			if(0 < MessageLength)
			{
				const id_memory Message = BxCore::Util::Malloc(MessageLength);
				BxCore::Util::MemMove(BxCore::Util::GetPtr(Message), Resource, MessageLength);
				Resource += MessageLength;
				Target->Message = Message;
			}
			Target->AnimationLength = 0;
			if(0 < i)
			{
				Target->Waiting = BxUtilGlobal::LoadInt8(Resource);
				Target->DelayMin = BxUtilGlobal::LoadInt16(Resource);
				Target->DelayMax = BxUtilGlobal::LoadInt16(Resource);
				Target->WorkTimeMin = BxUtilGlobal::LoadInt16(Resource);
				Target->WorkTimeMax = BxUtilGlobal::LoadInt16(Resource);
				Target->Movement = BxUtilGlobal::LoadInt8(Resource);
				Target->GravityMin = BxUtilGlobal::LoadInt16(Resource);
				Target->GravityMax = BxUtilGlobal::LoadInt16(Resource);
				Target->WindMin = BxUtilGlobal::LoadInt16(Resource);
				Target->WindMax = BxUtilGlobal::LoadInt16(Resource);
				if(Target->WindMin || Target->WindMax)
				{
					Target->WindAngleMin = BxUtilGlobal::LoadInt16(Resource);
					Target->WindAngleMax = BxUtilGlobal::LoadInt16(Resource);
				}
				Target->Pattern = BxUtilGlobal::LoadInt8(Resource);
				if(Target->Pattern)
				{
					Target->PatternRXMin = BxUtilGlobal::LoadInt16(Resource);
					Target->PatternRXMax = BxUtilGlobal::LoadInt16(Resource);
					Target->PatternRYMin = BxUtilGlobal::LoadInt16(Resource);
					Target->PatternRYMax = BxUtilGlobal::LoadInt16(Resource);
				}
				Target->Motion = BxUtilGlobal::LoadUint8(Resource);
				Target->CurrentlyDelay = 0;
			}
		}
		return true;
	}

	void Close()
	{
		for(int i = 0; i < _PartLength; ++i)
		{
			_PartBegin* Begin = (i == 0)? &_Begin : (_PartBegin*) &_Target[i - 1];
			const id_memory Message = Begin->Message;
			const id_memory Animation = Begin->Animation;
			if(Message) BxCore::Util::Free(Message);
			if(Animation) BxCore::Util::Free(Animation);
		}
		_Target.Reset();
	}

	void NextAnimate()
	{
		if(!_IsAnimationPlay)
		{
			_IsAnimationPlay = true;
			BxUtil::SetRandom(_AnimationRandKey);
			const int NumPart = BxUtil::GetValue(_Begin.PartRateMin, _Begin.PartRateMax, 0);
			for(int i = 0; i < _PartLength; ++i)
			{
				_PartTarget* Target = (i == 0)? (_PartTarget*) &_Begin : &_Target[i - 1];
				Target->Targeting(NumPart, _AnimationRandKey);
				if(i == 0) continue;
				Target->StopAnimation();
				for(int j = 0; j < Target->AnimationLength; ++j)
					((_PartAnimation*) BxCore::Util::GetPtr(Target->Animation))[j].ClearAnimation(false);
			}
		}
		for(int i = 1; i < _PartLength; ++i)
		{
			_PartTarget* Target = &_Target[i - 1];
			_PartBegin* PrevBegin = (i == 1)? &_Begin : (_PartBegin*) &_Target[i - 2];
			Target->PlayAnimation(PrevBegin, _MsPerFrame);
		}
	}

	void DrawAnimate(OnPart PartCB, void* Data = null)
	{
		if(!_IsAnimationPlay)
			NextAnimate();
		_IsAnimationFinished = true;
		for(int i = 1; i < _PartLength; ++i)
		{
			_PartTarget* Target = &_Target[i - 1];
			_PartAnimation* Animation = (_PartAnimation*) BxCore::Util::GetPtr(Target->Animation);
			for(int j = Target->AnimationLength - 1; 0 <= j; --j)
			{
				if(!Animation[j].GetRunAnimation()) continue;
				if(0 <= Animation[j].AnimationFrame || j == 0
					|| Animation[j].GetMoverX() != Animation[j - 1].GetMoverX() || Animation[j].GetMoverY() != Animation[j - 1].GetMoverY())
				{
					const bool IsFirstAnimation = (i == 1 && Animation[j].AnimationFrame <= 0);
					PartCB(j, (Target->TargetX1 + Target->TargetX2) / 2 + Animation[j].GetMoverX(),
						(Target->TargetY1 + Target->TargetY2) / 2 + Animation[j].GetMoverY(),
						Animation[j].AnimationOpacity, Target->Motion,
						(IsFirstAnimation)? _Begin.Message : Target->Message, Data);
				}
				_IsAnimationFinished = false;
			}
		}
	}

	inline void ResetAnimate(int MsPerFrame = 50, int AnimationRandKey = 1234)
	{
		_IsAnimationPlay = false;
		_MsPerFrame = MsPerFrame;
		_AnimationRandKey = AnimationRandKey;
	}

	inline bool IsAnimateFinished()
	{
		return _IsAnimationFinished;
	}

private:
	class _PartAnimation
	{
	public:
		bool IsEnable;
		byte IsAnimation;
		byte FirstOpacity;
		byte AnimationOpacity;
		short TargetX;
		short TargetY;
		short MoverX;
		short MoverY;
		short AnimationTargetX;
		short AnimationTargetY;
		int AnimationTargetAddX;
		int AnimationTargetAddY;
		short AnimationMoverX;
		short AnimationMoverY;
		short AnimationMoverAddX;
		short AnimationMoverAddY;
		int AnimationGravityVector;
		ushort AnimationWindAngle;
		ushort AnimationCalcValue;
		short AnimationFrame;
		short AnimationFrameBegin;

	public:
		inline bool IsShow() {return (0 <= AnimationFrame);}
		inline int GetMoverX() {return AnimationMoverX + AnimationMoverAddX;}
		inline int GetMoverY() {return AnimationMoverY + AnimationMoverAddY;}
		inline bool GetRunAnimation() {return (IsAnimation >> 0) & 1;}
		inline void SetRunAnimation(bool Value) {IsAnimation = (IsAnimation & ~1) | ((Value << 0) & 1);}
		inline bool GetEndAnimation() {return (IsAnimation >> 1) & 1;}
		inline void SetEndAnimation(bool Value) {IsAnimation = (IsAnimation & ~2) | ((Value << 1) & 2);}

		inline void SetOpacity(const int Opacity, const int Step, const int End)
		{
			BxAssert("BxTween", End != 0);
			AnimationOpacity = (FirstOpacity * (End - Step) + Opacity * Step) / End;
		}

		inline void SetTarget(const int AddX, const int AddY)
		{
			AnimationTargetX = TargetX + AddX;
			AnimationTargetY = TargetX + AddY;
		}

		inline void PullTarget(const int Step, int End)
		{
			BxAssert("BxTween", End != 0);
			AnimationTargetX = (AnimationTargetX * (End - Step) + TargetX * Step) / End;
			AnimationTargetY = (AnimationTargetY * (End - Step) + TargetY * Step) / End;
		}

		inline void ResetTarget(const bool Enable, const int PosX, const int PosY)
		{
			IsEnable = Enable;
			AnimationTargetX = TargetX = PosX;
			AnimationTargetY = TargetY = PosY;
			AnimationTargetAddX = 0;
			AnimationTargetAddY = 0;
			AnimationGravityVector = 0;
			AnimationWindAngle = 0;
		}

		inline void ResetAnimation(const bool IsBeginByPrev, const int Opacity, const int BeginX, const int BeginY,
			const int CalcValue, const int WindAngle, const int BeginFrame)
		{
			FirstOpacity = Opacity;
			AnimationOpacity = (IsBeginByPrev)? 0 : FirstOpacity;
			SetRunAnimation(true);
			SetEndAnimation(false);
			AnimationCalcValue = CalcValue;
			MoverX = BeginX;
			MoverY = BeginY;
			AnimationMoverX = MoverX;
			AnimationMoverY = MoverY;
			AnimationMoverAddX = 0;
			AnimationMoverAddY = 0;
			AnimationTargetX = TargetX;
			AnimationTargetY = TargetY;
			AnimationTargetAddX = 0;
			AnimationTargetAddY = 0;
			AnimationGravityVector = 0;
			AnimationWindAngle = WindAngle;
			AnimationFrame = -BeginFrame;
			AnimationFrameBegin = -BeginFrame;
		}

		inline void ClearAnimation(bool AniFinished)
		{
			SetRunAnimation(false);
			SetEndAnimation(AniFinished);
		}
	};

	class _PartBegin
	{
	public:
		bool IsBegin;
		int TargetX1;
		int TargetY1;
		int TargetX2;
		int TargetY2;
		int PartRateMin;
		int PartRateMax;
		int Position;
		int OpacityMin;
		int OpacityMax;
		id_memory Message;
		int AnimationLength;
		id_memory Animation;

	public:
		void Targeting(const int NumPart, const int RandKey)
		{
			BxUtil::SetRandom(RandKey);
			const int Detail = 256;
			const bool IsCircle = (Position < 18 && Position / 9 == 1);
			const bool IsRandom = (Position < 18 && (Position % 9) / 3 == 1);
			const bool IsPull = (Position == 19 || (Position % 9) / 3 == 2);
			const bool IsMiddle = (18 <= Position);
			int RateLeft = 0, RateTop = 0, RateRight = 0, RateBottom = 0;
			// 메모리할당
			if(Animation) BxCore::Util::Free(Animation);
			AnimationLength = NumPart;
			Animation = BxCore::Util::Malloc(sizeof(_PartAnimation) * AnimationLength);
			BxCore::Util::MemSet(BxCore::Util::GetPtr(Animation), 0, sizeof(_PartAnimation) * AnimationLength);
			id_memory Point = BxCore::Util::Malloc(sizeof(int) * AnimationLength * 2);
			BxCore::Util::MemSet(BxCore::Util::GetPtr(Point), 0, sizeof(int) * AnimationLength * 2);
			// 포인터화
			_PartAnimation* AnimationPtr = (_PartAnimation*) BxCore::Util::GetPtr(Animation);
			int* PosX = &((int*) BxCore::Util::GetPtr(Point))[0];
			int* PosY = &((int*) BxCore::Util::GetPtr(Point))[AnimationLength];

			if(!IsMiddle)
			{
				// 수집과정
				int PrevPosX = PosX[0];
				int PrevPosY = PosY[0];
				int Dist = Detail * 2, Angle = 0, Count = 1, Step = 0, Vector = 0;
				for(int i = 1; i < AnimationLength; ++i)
				{
					if(IsCircle)
					{
						PosX[i] = FtoI(Dist * BxUtil::Cos(Angle));
						PosY[i] = FtoI(Dist * BxUtil::Sin(Angle));
					}
					else
					{
						PosX[i] = PrevPosX;
						PosY[i] = PrevPosY;
						switch(Vector)
						{
						case 0: PosX[i] += Detail * 2; break;
						case 1: PosY[i] += Detail * 2; break;
						case 2: PosX[i] -= Detail * 2; break;
						case 3: PosY[i] -= Detail * 2; break;
						}
						PrevPosX = PosX[i];
						PrevPosY = PosY[i];
					}
					if(IsRandom)
					{
						PosX[i] = (PosX[i] - Detail * 4 + (BxUtil::GetRandom() % (Detail * 8))) * ((BxUtil::GetRandom() % 2)? 1 : -1);
						PosY[i] = (PosY[i] - Detail * 4 + (BxUtil::GetRandom() % (Detail * 8))) * ((BxUtil::GetRandom() % 2)? 1 : -1);
					}
					else if(IsPull)
					{
						PosX[i] -= Detail / 2 - (BxUtil::GetRandom() % Detail);
						PosY[i] -= Detail / 2 - (BxUtil::GetRandom() % Detail);
					}
					RateLeft = BxUtilGlobal::Min(RateLeft, PosX[i]);
					RateTop = BxUtilGlobal::Min(RateTop, PosY[i]);
					RateRight = BxUtilGlobal::Max(RateRight, PosX[i]);
					RateBottom = BxUtilGlobal::Max(RateBottom, PosY[i]);
					if(IsCircle)
					{
						BxAssert("BxTween", (Dist * 2 * BxUtilGlobal::Pi()) != 0);
						const int AddAngle = (int)((((huge)(Detail * 2 * 1024)) << 16) / (Dist * 2 * BxUtilGlobal::Pi()));
						Angle = (Angle + AddAngle) % 1024;
						Dist += Detail * 2 * AddAngle / 1024;
					}
					else
					{
						if(++Step == Count)
						{
							Step = 0;
							if(Vector % 2) ++Count;
							Vector = (Vector + 1) % 4;
						}
					}
				}
				// 가공과정
				const int CenterX = (RateLeft + RateRight) / 2, CenterY = (RateTop + RateBottom) / 2;
				const int SumDistPow = (RateRight - RateLeft) * (RateRight - RateLeft)
					+ (RateBottom - RateTop) * (RateBottom - RateTop);
				const int SumDist = (0 < SumDistPow)? BxUtilGlobal::Sqrt(SumDistPow) : 0;
				for(int i = 0; i < AnimationLength; ++i)
				{
					PosX[i] -= CenterX;
					PosY[i] -= CenterY;
					if(0 < Position % 3)
					{
						const int DistPow = PosX[i] * PosX[i] + PosY[i] * PosY[i];
						const int Dist = (0 < DistPow)? BxUtilGlobal::Sqrt(DistPow) : 0;
						const huge CalcDist = (Position % 3 == 1)? Dist : SumDist - Dist;
						BxAssert("BxTween", (AnimationLength * Detail) != 0);
						PosX[i] = (int)(PosX[i] * CalcDist / (AnimationLength * Detail));
						PosY[i] = (int)(PosY[i] * CalcDist / (AnimationLength * Detail));
					}
					RateLeft = (i == 0)? PosX[i] : BxUtilGlobal::Min(RateLeft, PosX[i]);
					RateTop = (i == 0)? PosY[i] : BxUtilGlobal::Min(RateTop, PosY[i]);
					RateRight = (i == 0)? PosX[i] : BxUtilGlobal::Max(RateRight, PosX[i]);
					RateBottom = (i == 0)? PosY[i] : BxUtilGlobal::Max(RateBottom, PosY[i]);
				}
			}
			// 비례과정
			int EnableRate = (IsBegin)? 100 : BxUtil::GetValue(PartRateMin, PartRateMax);
			int EnableRateSum = 100;
			for(int i = 0; i < AnimationLength; ++i)
			{
				BxAssert("BxTween", (RateRight - RateLeft + 2) != 0);
				BxAssert("BxTween", (RateBottom - RateTop + 2) != 0);
				AnimationPtr[i].ResetTarget(100 <= EnableRateSum,
					(PosX[i] - RateLeft + 1) * (TargetX2 - TargetX1 + 1) / (RateRight - RateLeft + 2) - (TargetX2 - TargetX1) / 2,
					(PosY[i] - RateTop + 1) * (TargetY2 - TargetY1 + 1) / (RateBottom - RateTop + 2) - (TargetY2 - TargetY1) / 2);
				EnableRateSum = (EnableRateSum % 100) + EnableRate;
			}
			BxCore::Util::Free(Point);
		}
	};

	class _PartTarget : public _PartBegin
	{
	public:
		int Waiting;
		int DelayMin;
		int DelayMax;
		int WorkTimeMin;
		int WorkTimeMax;
		int Movement;
		int GravityMin;
		int GravityMax;
		int WindMin;
		int WindMax;
		int WindAngleMin;
		int WindAngleMax;
		int Pattern;
		int PatternRXMin;
		int PatternRXMax;
		int PatternRYMin;
		int PatternRYMax;
		int Motion;
		int CurrentlyDelay;

	public:
		void PlayAnimation(_PartBegin* PrevTarget, int MsPerFrame)
		{
			#define MS_TO_FRAME(A) (((A) + MsPerFrame / 2) / MsPerFrame)
			_PartAnimation* AnimationPtr = (_PartAnimation*) BxCore::Util::GetPtr(Animation);
			for(int i = 0, j = 0; i < AnimationLength; ++i)
			{
				if(!AnimationPtr[i].IsEnable)
					continue;
				// 진행
				if(AnimationPtr[i].GetRunAnimation())
				{
					const int EndFrame = BxUtilGlobal::Max(1, MS_TO_FRAME(BxUtil::GetValue(WorkTimeMin, WorkTimeMax, i)));
					const int AniFrame = ++AnimationPtr[i].AnimationFrame;
					if(AniFrame <= 0)
					{
						// 불투명도적용
						const int Step = AnimationPtr[i].AnimationFrame - AnimationPtr[i].AnimationFrameBegin;
						const int End = -AnimationPtr[i].AnimationFrameBegin;
						switch(Waiting)
						{
						case 0: // 급진소멸
							AnimationPtr[i].SetOpacity(0, 1, 1);
							break;
						case 1: // 완만소멸
							if(PrevTarget->IsBegin)
								AnimationPtr[i].SetOpacity(0, End - Step, End);
							else AnimationPtr[i].SetOpacity(0, (Step <= End / 2)? Step : End - Step, End / 2);
							break;
						case 2: // 급진점화
							AnimationPtr[i].SetOpacity(100, 1, 1);
							break;
						case 3: // 완만점화
							if(PrevTarget->IsBegin)
								AnimationPtr[i].SetOpacity(0, End - Step, End);
							else AnimationPtr[i].SetOpacity(100, (Step <= End / 2)? Step : End - Step, End / 2);
							break;
						case 4: // 규칙점멸
							AnimationPtr[i].SetOpacity(50 * BxUtilGlobal::Abs(((i + Step) % 4) - 2), 1, 1);
							break;
						case 5: // 랜덤점멸
							AnimationPtr[i].SetOpacity(50 * BxUtilGlobal::Abs((BxUtil::GetRandom(i + Step) % 4) - 2), 1, 1);
							break;
						}
					}
					else if(AniFrame <= EndFrame)
					{
						// 이동계산
						int EndValue = 0, AniValue = 0;
						switch(Movement)
						{
						case 0: // 등속
							EndValue = EndFrame;
							AniValue = AniFrame;
							break;
						case 1: // 가속
							EndValue = EndFrame * EndFrame;
							AniValue = AniFrame * AniFrame;
							break;
						case 2: // 감속
							EndValue = EndFrame * EndFrame;
							AniValue = EndValue - (EndFrame - AniFrame) * (EndFrame - AniFrame);
							break;
						case 3: // 가감변속
							if(AniFrame <= EndFrame / 2)
							{
								const int End = EndFrame / 2;
								const int EndHalf = End * End;
								EndValue = EndHalf * 2;
								AniValue = AniFrame * AniFrame;
							}
							else
							{
								const int Ani = AniFrame - EndFrame / 2;
								const int End = EndFrame - EndFrame / 2;
								const int EndHalf = End * End;
								EndValue = EndHalf * 2;
								AniValue = EndHalf + EndHalf - (End - Ani) * (End - Ani);
							}
							break;
						case 4: // 감가변속
							if(AniFrame <= EndFrame / 2)
							{
								const int End = EndFrame / 2;
								const int EndHalf = End * End;
								EndValue = EndHalf * 2;
								AniValue = EndHalf - (End - AniFrame) * (End - AniFrame);
							}
							else
							{
								const int Ani = AniFrame - EndFrame / 2;
								const int End = EndFrame - EndFrame / 2;
								const int EndHalf = End * End;
								EndValue = EndHalf * 2;
								AniValue = EndHalf + Ani * Ani;
							}
							break;
						}
						// 이동적용
						BxAssert("BxTween", EndValue != 0);
						AnimationPtr[i].AnimationMoverX = (AnimationPtr[i].AnimationTargetX - AnimationPtr[i].MoverX)
							* AniValue / EndValue + AnimationPtr[i].MoverX;
						AnimationPtr[i].AnimationMoverY = (AnimationPtr[i].AnimationTargetY - AnimationPtr[i].MoverY)
							* AniValue / EndValue + AnimationPtr[i].MoverY;
						// 중력상황
						if(BxUtil::GetValue(GravityMin, GravityMax, i))
						{
							AnimationPtr[i].AnimationGravityVector += (int)((((huge) BxUtil::GetValue(GravityMin, GravityMax, i)) << 16) * MsPerFrame / 1000);
							AnimationPtr[i].AnimationTargetAddY += AnimationPtr[i].AnimationGravityVector;
						}
						// 바람상황
						if(BxUtil::GetValue(WindMin, WindMax, i))
						{
							AnimationPtr[i].AnimationTargetAddX += (int)(((huge)(BxUtil::Sin(AnimationPtr[i].AnimationWindAngle * 1024 / 360)))
								* BxUtil::GetValue(WindMin, WindMax, i) * MsPerFrame / 1000);
							AnimationPtr[i].AnimationTargetAddY -= (int)(((huge)(BxUtil::Cos(AnimationPtr[i].AnimationWindAngle * 1024 / 360)))
								* BxUtil::GetValue(WindMin, WindMax, i) * MsPerFrame / 1000);
						}
						// 타겟적용
						if(BxUtil::GetValue(GravityMin, GravityMax, i) || BxUtil::GetValue(WindMin, WindMax, i))
							AnimationPtr[i].SetTarget((AnimationPtr[i].AnimationTargetAddX + 0x7FFF) >> 16, (AnimationPtr[i].AnimationTargetAddY + 0x7FFF) >> 16);
						// 인력적용
						if(Position == 19 || (Position % 9) / 3 == 2)
							AnimationPtr[i].PullTarget(AniFrame, EndFrame);
						// 불투명도적용
						AnimationPtr[i].SetOpacity(BxUtil::GetValue(OpacityMin, OpacityMax, i), AniFrame, EndFrame);
						// 진행패턴계산
						if(0 < Pattern)
						{
							BxAssert("BxTween", EndFrame != 0);
							int MoverAddX = 0, MoverAddY = 0, CalcValue1 = 0, CalcValue2 = 0;
							switch(Pattern)
							{
							case 1: // 시계
								CalcValue1 = (1024 / 2) * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddX = FtoI(BxUtil::Sin((EndFrame - AniFrame) * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRXMin, PatternRXMax, i) + 0x7FFF);
								MoverAddY = FtoI(BxUtil::Cos((EndFrame - AniFrame) * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRYMin, PatternRYMax, i) + 0x7FFF);
								break;
							case 2: // 반시계
								CalcValue1 = (1024 / 2) * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddX = FtoI(BxUtil::Sin(AniFrame * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRXMin, PatternRXMax, i) + 0x7FFF);
								MoverAddY = FtoI(BxUtil::Cos(AniFrame * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRYMin, PatternRYMax, i) + 0x7FFF);
								break;
							case 3: // 지그재그
								CalcValue1 = BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * (4 / 2) * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddX = BxUtilGlobal::Abs(((AniFrame * CalcValue1 / EndFrame) % (BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * 4))
									- BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * 2) - BxUtil::GetValue(PatternRXMin, PatternRXMax, i);
								CalcValue2 = BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * (4 / 4) * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddY = BxUtilGlobal::Abs(((AniFrame * CalcValue2 / EndFrame) % (BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * 4))
									- BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * 2) - BxUtil::GetValue(PatternRYMin, PatternRYMax, i);
								break;
							case 4: // 나뭇잎
								CalcValue1 = (1024 / 2) * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								CalcValue2 = AniFrame + (AnimationPtr[i].AnimationCalcValue >> 8);
								MoverAddX = FtoI(BxUtil::Sin(CalcValue2 * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRXMin, PatternRXMax, i) + 0x7FFF);
								MoverAddY = BxUtilGlobal::Abs(FtoI(BxUtil::Cos(CalcValue2 * CalcValue1 / EndFrame) * BxUtil::GetValue(PatternRYMin, PatternRYMax, i) + 0x7FFF));
								break;
							case 5: // 워프
								CalcValue1 = ((AnimationPtr[i].AnimationCalcValue >> 8) & 0xF) + 0x7;
								BxAssert("BxTween", CalcValue1 != 0);
								CalcValue2 = AniFrame + AniFrame / CalcValue1 * CalcValue1;
								CalcValue1 = BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * 4 * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddX = BxUtilGlobal::Abs(((CalcValue2 * CalcValue1 / (EndFrame * 2)) % (BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * 4))
									- BxUtil::GetValue(PatternRXMin, PatternRXMax, i) * 2) - BxUtil::GetValue(PatternRXMin, PatternRXMax, i);
								CalcValue1 = ((AnimationPtr[i].AnimationCalcValue >> 12) & 0xF) + 0x7;
								BxAssert("BxTween", CalcValue1 != 0);
								CalcValue2 = AniFrame + AniFrame / CalcValue1 * CalcValue1;
								CalcValue1 = BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * 4 * (AnimationPtr[i].AnimationCalcValue & 0xFF);
								MoverAddY = BxUtilGlobal::Abs(((CalcValue2 * CalcValue1 / (EndFrame * 2)) % (BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * 4))
									- BxUtil::GetValue(PatternRYMin, PatternRYMax, i) * 2) - BxUtil::GetValue(PatternRYMin, PatternRYMax, i);
								break;
							}
							// 회전
							if(Pattern == 4)
							{
								AnimationPtr[i].AnimationMoverAddX = MoverAddX;
								AnimationPtr[i].AnimationMoverAddY = MoverAddY;
							}
							else
							{
								const int Angle = (BxUtil::GetAngle1024(AnimationPtr[i].AnimationTargetX - AnimationPtr[i].AnimationMoverX,
									AnimationPtr[i].AnimationTargetY - AnimationPtr[i].AnimationMoverY) + 256) % 1024;
								AnimationPtr[i].AnimationMoverAddX = FtoI(MoverAddX * BxUtil::Cos(Angle) - MoverAddY * BxUtil::Sin(Angle) + 0x7FFF);
								AnimationPtr[i].AnimationMoverAddY = FtoI(MoverAddY * BxUtil::Cos(Angle) + MoverAddX * BxUtil::Sin(Angle) + 0x7FFF);
							}
							// 축소
							CalcValue1 = EndFrame * 4;
							CalcValue2 = EndFrame - BxUtilGlobal::Abs(AniFrame * 2 - EndFrame);
							CalcValue2 = BxUtilGlobal::Min(CalcValue2 * CalcValue2, CalcValue1);
							BxAssert("BxTween", CalcValue1 != 0);
							AnimationPtr[i].AnimationMoverAddX = AnimationPtr[i].AnimationMoverAddX * CalcValue2 / CalcValue1;
							AnimationPtr[i].AnimationMoverAddY = AnimationPtr[i].AnimationMoverAddY * CalcValue2 / CalcValue1;
						}
					}
					else AnimationPtr[i].ClearAnimation(true);
				}
				// 신규
				else if(PrevTarget && !AnimationPtr[i].GetEndAnimation())
				{
					_PartAnimation* PrevAnimationPtr = (_PartAnimation*) BxCore::Util::GetPtr(PrevTarget->Animation);
					for(j = i; 0 <= j; --j)
						if(PrevAnimationPtr[j].IsEnable)
							break;
					if(PrevTarget->IsBegin || PrevAnimationPtr[j].GetEndAnimation())
					{
						// 패턴횟수
						const int TargetDistX = (PrevTarget->TargetX1 + PrevTarget->TargetX2) / 2 - (TargetX1 + TargetX2) / 2;
						const int TargetDistY = (PrevTarget->TargetY1 + PrevTarget->TargetY2) / 2 - (TargetY1 + TargetY2) / 2;
						int CalcValue = 0;
						if(0 < Pattern)
						{
							const int Dist16 = BxUtilGlobal::Sqrt64(((huge)(TargetDistX * TargetDistX + TargetDistY * TargetDistY)) << 32);
							BxAssert("BxTween", ((BxUtil::GetValue(PatternRXMin, PatternRXMax, i) + BxUtil::GetValue(PatternRYMin, PatternRYMax, i)) * BxUtilGlobal::Pi()) != 0);
							CalcValue = ((BxUtil::GetRandom() & 0xFF) << 8)
								| BxUtilGlobal::Min(int(Dist16 / ((BxUtil::GetValue(PatternRXMin, PatternRXMax, i)
								+ BxUtil::GetValue(PatternRYMin, PatternRYMax, i)) * BxUtilGlobal::Pi())) + 2, 0xFF);
						}
						// 신규초기화
						CurrentlyDelay += BxUtil::GetValue(DelayMin, DelayMax, i);
						AnimationPtr[i].ResetAnimation(PrevTarget->IsBegin, BxUtil::GetValue(PrevTarget->OpacityMin, PrevTarget->OpacityMax, j),
							PrevAnimationPtr[j].AnimationTargetX + TargetDistX,
							PrevAnimationPtr[j].AnimationTargetY + TargetDistY,
							CalcValue, (BxUtil::GetValue(WindAngleMin, WindAngleMax, i) == -1)?
							BxUtil::GetRandom() % 360 : BxUtil::GetValueAngle(WindAngleMin, WindAngleMax, i), MS_TO_FRAME(CurrentlyDelay));
					}
				}
			}
			CurrentlyDelay = BxUtilGlobal::Max(0, CurrentlyDelay - MsPerFrame);
			#undef MS_TO_FRAME
		}

		void StopAnimation()
		{
			CurrentlyDelay = 0;
		}
	};

private:
	bool _IsAnimationPlay;
	bool _IsAnimationFinished;
	int _MsPerFrame;
	int _AnimationRandKey;
	int _PartLength;
	_PartBegin _Begin;
	BxVar<_PartTarget> _Target;
};
