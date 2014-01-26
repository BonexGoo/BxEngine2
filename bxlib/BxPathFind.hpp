#pragma once
#include <BxVarVector.hpp>
#include <BxUtil.hpp>
#include <BxDraw.hpp>

//! \brief 다각형맵 길찾기 제공
namespace BxPathFind
{
	enum linetype {linetype_bound, linetype_space, linetype_wall};
	struct Line
	{
		linetype Type;
		int DotA;
		int DotB;
		void Set(linetype type, int dotA, int dotB)
		{
			Type = type;
			DotA = dotA;
			DotB = dotB;
		}
	};
	typedef BxVarVector<point, 4> DotList;
	typedef BxVarVector<Line, 8> LineList;
	typedef BxVarVector<points, 8> PolygonList;

	class Path
	{
		friend class Map;
		public: DotList Dots;

		private: Path()
		{
		}
		public: ~Path()
		{
		}

		public: global_func void Release(Path*& path)
		{
			BxDelete(path);
		}
	};

	class Map
	{
		friend class Hurdle;
		public: DotList Dots;
		public: LineList Lines;
		private: class Triangle
		{
			friend class Map;
			private: linetype TypeAB;
			private: linetype TypeAC;
			private: linetype TypeBC;
			private: int DotA;
			private: int DotB;
			private: int DotC;
			private: Triangle* LinkAB;
			private: Triangle* LinkAC;
			private: Triangle* LinkBC;
			private: point WayDot;
			private: Triangle* WayBack;
			private: int DistanceSum;
			private: Triangle* Next;

			private: Triangle()
			{
				TypeAB = linetype_bound;
				TypeAC = linetype_bound;
				TypeBC = linetype_bound;
				DotA = -1;
				DotB = -1;
				DotC = -1;
				LinkAB = nullptr;
				LinkAC = nullptr;
				LinkBC = nullptr;
				WayDot = point::zero();
				WayBack = nullptr;
				DistanceSum = 0;
				Next = nullptr;
			}
			public: ~Triangle()
			{
				Triangle* DeleteNode = Next;
				while(DeleteNode)
				{
					Triangle* DeleteNodeNext = DeleteNode->Next;
					DeleteNode->Next = nullptr;
					BxDelete(DeleteNode);
					DeleteNode = DeleteNodeNext;
				}
			}

			private: Triangle* INSERT_FIRST()
			{
				Triangle* Result = BxNew(Triangle);
				Result->Next = Next;
				return Next = Result;
			}
		} Top;

		private: Map()
		{
		}
		public: ~Map()
		{
		}

		public: global_func void Release(Map*& map)
		{
			BxDelete(map);
		}

		public: Path* BuildPath(const point& beginPos, const point& endPos, const int step = 0)
		{
			Triangle* ClearNode = &Top;
			while(ClearNode = ClearNode->Next)
			{
				ClearNode->WayDot = point::zero();
				ClearNode->WayBack = nullptr;
				ClearNode->DistanceSum = 0;
			}
			Triangle* Begin = FIND_PICK_TRIANGLE(beginPos);
			Triangle* End = FIND_PICK_TRIANGLE(endPos);
			if(Begin)
			{
				Begin->WayDot = beginPos;
				Begin->WayBack = (Triangle*) 0xFFFFFFFF;
				bool IsSuccess = PATH_FIND(Begin, End, endPos);
				Begin->WayBack = nullptr;
				if(IsSuccess)
				{
					Path* Result = BxNew(Path);
					Result->Dots[LAST] = endPos;
					Triangle* CurTriangle = End;
					while(CurTriangle)
					{
						// 중간정점 삽입
						if(0 < step)
						{
							const point DotA = Result->Dots[END];
							const point DotB = CurTriangle->WayDot;
							const int Distance = BxUtilGlobal::Sqrt(BxUtilGlobal::Pow(DotB.x - DotA.x) + BxUtilGlobal::Pow(DotB.y - DotA.y));
							for(int f = step; f < Distance; f += step)
								Result->Dots[LAST] = BxDrawGlobal::XY((DotA.x * (Distance - f) + DotB.x * f) / Distance, (DotA.y * (Distance - f) + DotB.y * f) / Distance);
						}
						Result->Dots[LAST] = CurTriangle->WayDot;
						CurTriangle = CurTriangle->WayBack;
					}
					return Result;
				}
			}
			return nullptr;
		}

		private: void CREATE_TRIANGLES(const rect& boundBox, PolygonList& list)
		{
			for(int i = 0; i < list.Length(); ++i)
			for(int j = 0, jend = list[i].count; j < jend; ++j)
			{
				const int CurIndex = Dots.Length();
				Dots[LAST] = list[i].pt[j];
				Lines[LAST].Set((i == list.Length() - 1)? linetype_bound : linetype_wall, CurIndex, CurIndex + ((j + 1) % jend) - j);
			}
			// 맵구성
			MAPPING(Top.INSERT_FIRST(), nullptr, linetype_bound, 0, 1);
		}

		private: void MAPPING(Triangle* focus, Triangle* parent, linetype type, int dotA, int dotB)
		{
			for(int dotC = 0; dotC < Dots.Length(); dotC++)
			{
				// 점C의 조건
				if(!parent) {if(dotC == dotA || dotC == dotB) continue;}
				else if(dotC == parent->DotA || dotC == parent->DotB || dotC == parent->DotC)
					continue;
				// 삼각형구성의 조건
				if(IS_INCLUDE_ANI_DOT_BY(dotA, dotB, dotC)) continue;
				int LineAC = FIND_LINE_ID(dotA, dotC);
				int LineBC = FIND_LINE_ID(dotB, dotC);
				if((LineAC != -1 || !IS_CROSSING_ANI_LINE_BY(dotA, dotC)) && (LineBC != -1 || !IS_CROSSING_ANI_LINE_BY(dotB, dotC)))
				{
					// 삼각형추가
					focus->TypeAB = type;
					focus->TypeAC = (LineAC == -1)? linetype_space : Lines[LineAC].Type;
					focus->TypeBC = (LineBC == -1)? linetype_space : Lines[LineBC].Type;
					focus->DotA = dotA;
					focus->DotB = dotB;
					focus->DotC = dotC;
					// 선추가
					if(LineAC == -1) Lines[LAST].Set(linetype_space, focus->DotA, focus->DotC);
					if(LineBC == -1) Lines[LAST].Set(linetype_space, focus->DotB, focus->DotC);
					// 연결되는 삼각형정보
					focus->LinkAB = parent;
					if(focus->TypeAC != linetype_bound && !(focus->LinkAC = FIND_SAME_TRIANGLE(focus->DotA, focus->DotC, focus)))
						MAPPING(focus->LinkAC = Top.INSERT_FIRST(), focus, focus->TypeAC, focus->DotA, focus->DotC);
					if(focus->TypeBC != linetype_bound && !(focus->LinkBC = FIND_SAME_TRIANGLE(focus->DotB, focus->DotC, focus)))
						MAPPING(focus->LinkBC = Top.INSERT_FIRST(), focus, focus->TypeBC, focus->DotB, focus->DotC);
					return;
				}
			}
		}

		private: bool IS_INCLUDE_ANI_DOT_BY(int dotA, int dotB, int dotC)
		{
			if(R_SIDE1(dotA, dotB, dotC) < 0)
			{
				int Swap = dotB;
				dotB = dotC;
				dotC = Swap;
			}
			for(int i = 0; i < Dots.Length(); i++)
			{
				if(i == dotA || i == dotB || i == dotC) continue;
				if(R_SIDE1(dotA, dotB, i) < 0 || R_SIDE1(dotB, dotC, i) < 0 || R_SIDE1(dotC, dotA, i) < 0)
					continue;
				return true;
			}
			return false;
		}

		private: bool IS_CROSSING_ANI_LINE_BY(int dotA, int dotB)
		{
			for(int i = 0; i < Lines.Length(); ++i)
			{
				Line* Node = &Lines[i];
				if(dotA == Node->DotA || dotA == Node->DotB
					|| dotB == Node->DotA || dotB == Node->DotB)
					continue;
				if(BxUtilGlobal::Max(Dots[Node->DotA].x, Dots[Node->DotB].x) < BxUtilGlobal::Min(Dots[dotA].x, Dots[dotB].x)
					|| BxUtilGlobal::Max(Dots[dotA].x, Dots[dotB].x) < BxUtilGlobal::Min(Dots[Node->DotA].x, Dots[Node->DotB].x)
					|| BxUtilGlobal::Max(Dots[Node->DotA].y, Dots[Node->DotB].y) < BxUtilGlobal::Min(Dots[dotA].y, Dots[dotB].y)
					|| BxUtilGlobal::Max(Dots[dotA].y, Dots[dotB].y) < BxUtilGlobal::Min(Dots[Node->DotA].y, Dots[Node->DotB].y))
					continue;
				int ResultA1 = R_SIDE1(dotA, dotB, Node->DotA);
				int ResultB1 = R_SIDE1(dotA, dotB, Node->DotB);
				int ResultA2 = R_SIDE1(Node->DotA, Node->DotB, dotA);
				int ResultB2 = R_SIDE1(Node->DotA, Node->DotB, dotB);
				if((ResultA1 == 0 || ResultB1 == 0 || (ResultA1 < 0 && 0 < ResultB1) || (0 < ResultA1 && ResultB1 < 0))
					&& (ResultA2 == 0 || ResultB2 == 0 || (ResultA2 < 0 && 0 < ResultB2) || (0 < ResultA2 && ResultB2 < 0)))
					return true;
			}
			return false;
		}

		private: int FIND_LINE_ID(int dotA, int dotB)
		{
			int Count = 0;
			for(int i = 0; i < Lines.Length(); ++i)
			{
				Line* Node = &Lines[i];
				if((dotA == Node->DotA && dotB == Node->DotB)
					|| (dotA == Node->DotB && dotB == Node->DotA))
					return Count;
				Count++;
			}
			return -1;
		}

		private: Triangle* FIND_PICK_TRIANGLE(const point& pos)
		{
			Triangle* Node = &Top;
			while(Node = Node->Next)
			{
				if(R_SIDE1(Node->DotA, Node->DotB, Node->DotC) < 0)
				{
					if(0 < R_SIDE2(Node->DotA, Node->DotB, pos)
						|| 0 < R_SIDE2(Node->DotB, Node->DotC, pos)
						|| 0 < R_SIDE2(Node->DotC, Node->DotA, pos))
						continue;
				}
				else if(R_SIDE2(Node->DotA, Node->DotB, pos) < 0
					|| R_SIDE2(Node->DotB, Node->DotC, pos) < 0
					|| R_SIDE2(Node->DotC, Node->DotA, pos) < 0)
					continue;
				return Node;
			}
			return nullptr;
		}

		private: Triangle* FIND_SAME_TRIANGLE(int dotA, int dotB, Triangle* parent)
		{
			Triangle* Node = &Top;
			while(Node = Node->Next)
			{
				if(Node == parent) continue;
				if((dotA == Node->DotA || dotA == Node->DotB || dotA == Node->DotC)
					&& (dotB == Node->DotA || dotB == Node->DotB || dotB == Node->DotC))
					return Node;
			}
			return nullptr;
		}

		private: bool PATH_FIND(Triangle* focus, const Triangle* target, const point& endPos)
		{
			if(target == focus) return true;
			bool Result = false;
			const point DotAB = (DISTANCE(focus->WayDot, Dots[focus->DotA]) < DISTANCE(focus->WayDot, Dots[focus->DotB]))? DOT_AB_SIDE_A(*focus) : DOT_AB_SIDE_B(*focus);
			const point DotAC = (DISTANCE(focus->WayDot, Dots[focus->DotA]) < DISTANCE(focus->WayDot, Dots[focus->DotC]))? DOT_AC_SIDE_A(*focus) : DOT_AC_SIDE_C(*focus);
			const point DotBC = (DISTANCE(focus->WayDot, Dots[focus->DotB]) < DISTANCE(focus->WayDot, Dots[focus->DotC]))? DOT_BC_SIDE_B(*focus) : DOT_BC_SIDE_C(*focus);
			const int DistanceAB = DISTANCE(focus->WayDot, DotAB) + ((target == focus->LinkAB)? DISTANCE(DotAB, endPos) : 0);
			const int DistanceAC = DISTANCE(focus->WayDot, DotAC) + ((target == focus->LinkAC)? DISTANCE(DotAC, endPos) : 0);
			const int DistanceBC = DISTANCE(focus->WayDot, DotBC) + ((target == focus->LinkBC)? DISTANCE(DotBC, endPos) : 0);
			if(focus->TypeAB == linetype_space && (!focus->LinkAB->WayBack || focus->DistanceSum + DistanceAB < focus->LinkAB->DistanceSum))
			{
				focus->LinkAB->WayDot = DotAB;
				focus->LinkAB->WayBack = focus;
				focus->LinkAB->DistanceSum = focus->DistanceSum + DistanceAB;
				Result |= PATH_FIND(focus->LinkAB, target, endPos);
			}
			if(focus->TypeAC == linetype_space && (!focus->LinkAC->WayBack || focus->DistanceSum + DistanceAC < focus->LinkAC->DistanceSum))
			{
				focus->LinkAC->WayDot = DotAC;
				focus->LinkAC->WayBack = focus;
				focus->LinkAC->DistanceSum = focus->DistanceSum + DistanceAC;
				Result |= PATH_FIND(focus->LinkAC, target, endPos);
			}
			if(focus->TypeBC == linetype_space && (!focus->LinkBC->WayBack || focus->DistanceSum + DistanceBC < focus->LinkBC->DistanceSum))
			{
				focus->LinkBC->WayDot = DotBC;
				focus->LinkBC->WayBack = focus;
				focus->LinkBC->DistanceSum = focus->DistanceSum + DistanceBC;
				Result |= PATH_FIND(focus->LinkBC, target, endPos);
			}
			return Result;
		}

		private: inline const int R_SIDE1(const int dotA, const int dotB, const int dotC)
		{
			return ((Dots[dotA].x - Dots[dotB].x) * (Dots[dotC].y - Dots[dotB].y)
				- (Dots[dotA].y - Dots[dotB].y) * (Dots[dotC].x - Dots[dotB].x));
		}

		private: inline const int R_SIDE2(const int dotA, const int dotB, const point& C)
		{
			return ((Dots[dotA].x - Dots[dotB].x) * (C.y - Dots[dotB].y)
				- (Dots[dotA].y - Dots[dotB].y) * (C.x - Dots[dotB].x));
		}

		private: inline const int DISTANCE(const point& A, const point& B) {return BxUtilGlobal::Sqrt(BxUtilGlobal::Pow(A.x - B.x) + BxUtilGlobal::Pow(A.y - B.y));}
		private: inline const point DOT_AB_SIDE_A(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotA].x * 2 + Dots[t.DotB].x) / 3, (Dots[t.DotA].y * 2 + Dots[t.DotB].y) / 3);}
		private: inline const point DOT_AC_SIDE_A(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotA].x * 2 + Dots[t.DotC].x) / 3, (Dots[t.DotA].y * 2 + Dots[t.DotC].y) / 3);}
		private: inline const point DOT_BC_SIDE_B(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotB].x * 2 + Dots[t.DotC].x) / 3, (Dots[t.DotB].y * 2 + Dots[t.DotC].y) / 3);}
		private: inline const point DOT_AB_SIDE_B(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotA].x + Dots[t.DotB].x * 2) / 3, (Dots[t.DotA].y + Dots[t.DotB].y * 2) / 3);}
		private: inline const point DOT_AC_SIDE_C(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotA].x + Dots[t.DotC].x * 2) / 3, (Dots[t.DotA].y + Dots[t.DotC].y * 2) / 3);}
		private: inline const point DOT_BC_SIDE_C(const Triangle& t) {return BxDrawGlobal::XY((Dots[t.DotB].x + Dots[t.DotC].x * 2) / 3, (Dots[t.DotB].y + Dots[t.DotC].y * 2) / 3);}
	};

	class Hurdle
	{
		friend class GetPosition;
		private: PolygonList List;
		private: int ObjectBeginID;

		private: Hurdle() : ObjectBeginID(-1)
		{
		}
		public: ~Hurdle()
		{
			for(int i = List.Length() - 1; 0 <= i; --i)
			{
				points* DeleteNode = nullptr;
				List.Delete(i, &DeleteNode);
				BxFree(DeleteNode);
			}
		}

		public: global_func Hurdle* Create(Hurdle* hurdle = nullptr)
		{
			Hurdle* Result = BxNew(Hurdle);
			if(hurdle)
			for(int i = 0; i < hurdle->List.Length(); ++i)
			{
				Result->List[i].count = hurdle->List[i].count;
				for(int j = 0; j < hurdle->List[i].count; ++j)
					Result->List[i].pt[j] = hurdle->List[i].pt[j];
			}
			return Result;
		}

		public: global_func void Release(Hurdle*& hurdle)
		{
			BxDelete(hurdle);
		}

		public: void Add(const points& polygon, const bool isBoundLine = false)
		{
			BxASSERT("Object가 추가된 Hurdle은 Add를 지원하지 않습니다>", ObjectBeginID == -1);
			rect Bound = {polygon.pt[0].x, polygon.pt[0].y, polygon.pt[0].x, polygon.pt[0].y};
			for(int i = 1; i < polygon.count; ++i)
			{
				Bound.l = BxUtilGlobal::Min(Bound.l, polygon.pt[i].x);
				Bound.t = BxUtilGlobal::Min(Bound.t, polygon.pt[i].y);
				Bound.r = BxUtilGlobal::Max(Bound.r, polygon.pt[i].x);
				Bound.b = BxUtilGlobal::Max(Bound.b, polygon.pt[i].y);
			}
			for(int i = List.Length() - 1; 0 <= i; --i)
			{
				const points* Result = MERGE_POLYGON(List[i], polygon, Bound, isBoundLine);
				if(Result)
				{
					BxCore::Util::MemCpy(&polygon, Result, sizeof(points));
					points* DeleteNode = nullptr;
					List.Delete(i, &DeleteNode);
					BxFree(DeleteNode);
				}
			}
			points* NewPoints = (points*) BxAlloc(sizeof(int) + sizeof(point) * polygon.count);
			points& NewList = List.Insert(LAST, NewPoints);
			NewList.count = polygon.count;
			for(int i = 0; i < polygon.count; ++i)
				NewList.pt[i] = polygon.pt[i];
		}

		public: void AddWithoutMerging(const points& polygon)
		{
			if(ObjectBeginID == -1) ObjectBeginID = List.Length();
			points* NewPoints = (points*) BxAlloc(sizeof(int) + sizeof(point) * polygon.count);
			points& NewList = List.Insert(LAST, NewPoints);
			NewList.count = polygon.count;
			for(int i = 0; i < polygon.count; ++i)
				NewList.pt[i] = polygon.pt[i];
		}

		public: Map* BuildMap(const rect& boundBox)
		{
			BxASSERT("Object가 추가된 Hurdle은 BuildMap을 지원하지 않습니다>", ObjectBeginID == -1);
			points BoundPolygon;
			BoundPolygon.count = 4;
			BoundPolygon.pt[0] = BxDrawGlobal::XY(boundBox.l, boundBox.t);
			BoundPolygon.pt[1] = BxDrawGlobal::XY(boundBox.l, boundBox.b);
			BoundPolygon.pt[2] = BxDrawGlobal::XY(boundBox.r, boundBox.b);
			BoundPolygon.pt[3] = BxDrawGlobal::XY(boundBox.r, boundBox.t);
			Add(BoundPolygon, true);
			Map* Result = BxNew(Map);
			Result->CREATE_TRIANGLES(boundBox, List);
			return Result;
		}

		private: const points* MERGE_POLYGON(const points& Dst, const points& Src, const rect SrcBound, const bool IsBoundLine)
		{
			global_data points Result = {0,};
			// 한계검사
			bool IsL = true, IsT = true, IsR = true, IsB = true;
			int BestDstI = -1;
			for(int i = 0; i < Dst.count; ++i)
			{
				if(IsL) IsL &= (Dst.pt[i].x < SrcBound.l);
				if(IsT) IsT &= (Dst.pt[i].y < SrcBound.t);
				if(IsR) IsR &= (Dst.pt[i].x > SrcBound.r);
				if(IsB) IsB &= (Dst.pt[i].y > SrcBound.b);
				if(!IsBoundLine && BestDstI == -1 && (Dst.pt[i].x < SrcBound.l || Dst.pt[i].y < SrcBound.t))
					BestDstI = i;
			}
			BxASSERT("BxPathFind<Src는 Dst의 우측하단에 위치하여야 안전합니다>", IsBoundLine || BestDstI != -1);
			if(IsL || IsT || IsR || IsB) return nullptr;
			// 교차검사 수집데이터(최소한 Dst의 0번 정점은 Src에 포함되지 않아야 함)
			point CollectDstB[1024];
			point CollectDstE[1024];
			int CollectDstLength = Dst.count;
			BxCore::Util::MemCpy(CollectDstB, &Dst.pt[0], sizeof(point) * CollectDstLength);
			BxCore::Util::MemCpy(CollectDstE, &Dst.pt[1], sizeof(point) * (CollectDstLength - 1));
			CollectDstE[CollectDstLength - 1] = Dst.pt[0];
			point CollectSrcB[1024];
			point CollectSrcE[1024];
			int CollectSrcLength = Src.count;
			BxCore::Util::MemCpy(CollectSrcB, &Src.pt[0], sizeof(point) * CollectSrcLength);
			BxCore::Util::MemCpy(CollectSrcE, &Src.pt[1], sizeof(point) * (CollectSrcLength - 1));
			CollectSrcE[CollectSrcLength - 1] = Src.pt[0];
			for(int d = 0; d < CollectDstLength; ++d)
			for(int s = 0; s < CollectSrcLength; ++s)
			{
				const point* CrossDot = nullptr;
				if(CrossDot = BxUtil::GetDotByLineCross(CollectDstB[d], CollectDstE[d], CollectSrcB[s], CollectSrcE[s]))
				{
					if((CrossDot->x != CollectDstB[d].x || CrossDot->y != CollectDstB[d].y)
						&& (CrossDot->x != CollectDstE[d].x || CrossDot->y != CollectDstE[d].y))
					{
						CollectDstE[CollectDstLength] = CollectDstE[d];
						CollectDstB[CollectDstLength++] = (CollectDstE[d] = *CrossDot);
					}
					if((CrossDot->x != CollectSrcB[s].x || CrossDot->y != CollectSrcB[s].y)
						&& (CrossDot->x != CollectSrcE[s].x || CrossDot->y != CollectSrcE[s].y))
					{
						CollectSrcE[CollectSrcLength] = CollectSrcE[s];
						CollectSrcB[CollectSrcLength++] = (CollectSrcE[s] = *CrossDot);
					}
					BxASSERT("BxPathFind<계산상 오류입니다>", CollectDstLength < 1024 && CollectSrcLength < 1024);
				}
			}
			if(CollectDstLength == Dst.count)
				return nullptr;
			Result.count = 0;
			if(IsBoundLine)
			{
				Result.pt[0] = point::zero();
				for(int d = 0; d < CollectDstLength; ++d)
				{
					if(CollectDstB[d].x <= SrcBound.l) continue;
					if(CollectDstB[d].y <= SrcBound.t) continue;
					if(CollectDstB[d].x >= SrcBound.r) continue;
					if(CollectDstB[d].y >= SrcBound.b) continue;
					Result.pt[0] = CollectDstB[d];
					break;
				}
				BxASSERT("BxPathFind<계산상 오류입니다>", Result.pt[0].x != 0 || Result.pt[0].y != 0);
			}
			else Result.pt[0] = Dst.pt[BestDstI];
			do
			{
				int DstIndex = -1;
				for(int d = 0; d < CollectDstLength; ++d)
					if(Result.pt[Result.count].x == CollectDstB[d].x && Result.pt[Result.count].y == CollectDstB[d].y
						&& (CollectDstB[d].x != CollectDstE[d].x || CollectDstB[d].y != CollectDstE[d].y))
					{
						DstIndex = d;
						break;
					}
				int SrcIndex = -1;
				for(int s = 0; s < CollectSrcLength; ++s)
					if(Result.pt[Result.count].x == CollectSrcB[s].x && Result.pt[Result.count].y == CollectSrcB[s].y
						&& (CollectSrcB[s].x != CollectSrcE[s].x || CollectSrcB[s].y != CollectSrcE[s].y))
					{
						SrcIndex = s;
						break;
					}
				BxASSERT("BxPathFind<계산상 오류입니다>", DstIndex != -1 || SrcIndex != -1);
				point SelectedDot;
				if(DstIndex != -1 && SrcIndex != -1 && 0 < Result.count)
				{
					const int PrevAngle = BxUtil::GetAngle1024(Result.pt[Result.count - 1].x - Result.pt[Result.count].x,
						Result.pt[Result.count - 1].y - Result.pt[Result.count].y);
					const int NextAngleDst = BxUtil::GetAngle1024(CollectDstE[DstIndex].x - Result.pt[Result.count].x,
						CollectDstE[DstIndex].y - Result.pt[Result.count].y);
					const int NextAngleSrc = BxUtil::GetAngle1024(CollectSrcE[SrcIndex].x - Result.pt[Result.count].x,
						CollectSrcE[SrcIndex].y - Result.pt[Result.count].y);
					if(NextAngleDst < NextAngleSrc)
					{
						if(NextAngleDst < PrevAngle && PrevAngle < NextAngleSrc)
							SelectedDot = CollectSrcE[SrcIndex];
						else SelectedDot = CollectDstE[DstIndex];
					}
					else
					{
						if(NextAngleSrc < PrevAngle && PrevAngle < NextAngleDst)
							SelectedDot = CollectDstE[DstIndex];
						else SelectedDot = CollectSrcE[SrcIndex];
					}
				}
				else if(DstIndex != -1)
					SelectedDot = CollectDstE[DstIndex];
				else SelectedDot = CollectSrcE[SrcIndex];
				if(0 < Result.count && ((Result.pt[Result.count - 1].x == SelectedDot.x && Result.pt[Result.count].x == SelectedDot.x)
					|| (Result.pt[Result.count - 1].y == SelectedDot.y && Result.pt[Result.count].y == SelectedDot.y)))
					Result.pt[Result.count] = SelectedDot;
				else Result.pt[++Result.count] = SelectedDot;
			}
			while(Result.pt[Result.count].x != Result.pt[0].x || Result.pt[Result.count].y != Result.pt[0].y);
			return &Result;
		}
	};

	class GetPosition
	{
		private: GetPosition()
		{
		}
		private: ~GetPosition()
		{
		}

		public: global_func const point SubTarget(Hurdle* hurdle, Path* path, const point& curPos)
		{
			if(hurdle && path)
				for(int p = 0; p < path->Dots.Length(); ++p)
				{
					bool IsFind = false;
					const point& SubTarget = path->Dots[p];
					for(int h = 0; !IsFind && h < hurdle->List.Length(); ++h)
					{
						// curPos를 둘러싼 오브젝트는 검사에서 제외
						if(0 <= hurdle->ObjectBeginID && hurdle->ObjectBeginID <= h)
						{
							bool IsInHurdle = true;
							for(int l = 0, lend = hurdle->List[h].count; IsInHurdle && l < lend; ++l)
								if(0 < BxUtil::GetClockwiseValue(hurdle->List[h].pt[l], hurdle->List[h].pt[(l + 1) % lend], curPos))
									IsInHurdle = false;
							if(IsInHurdle) continue;
						}
						for(int l = 0, lend = hurdle->List[h].count; !IsFind && l < lend; ++l)
						{
							IsFind = (nullptr != BxUtil::GetDotByLineCross(hurdle->List[h].pt[l], hurdle->List[h].pt[(l + 1) % lend], curPos, SubTarget));
							if(IsFind && !BxUtil::GetClockwiseValue(hurdle->List[h].pt[l], hurdle->List[h].pt[(l + 1) % lend], curPos))
								IsFind = false;
						}
					}
					if(!IsFind) return SubTarget;
				}
				return curPos;
		}

		public: global_func const point ValidNext(Hurdle* hurdle, const point& curPos, const point& nextPos)
		{
			for(int h = 0; h < hurdle->List.Length(); ++h)
			for(int l = 0, lend = hurdle->List[h].count; l < lend; ++l)
			{
				const point* Result = BxUtil::GetDotByLineCross(hurdle->List[h].pt[l], hurdle->List[h].pt[(l + 1) % lend], curPos, nextPos);
				if(Result && BxUtil::GetClockwiseValue(hurdle->List[h].pt[l], hurdle->List[h].pt[(l + 1) % lend], curPos))
					return *Result;
			}
			return nextPos;
		}
	};
}
