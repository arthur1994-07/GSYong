#pragma once

#include <vector>

#include <boost/exception/exception.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include "GLTournamentTreeDefine.h"

namespace MatchingSystem
{

	// 프로그래머에게만 사용되는 에러 메시지
	enum GROUP_ERROR
	{
		GROUP_HEAD_IS_INVALITED, // 유요하지 않은 Node를 Head로 사용했습니다.

	};

	enum GROUP_STATUS
	{
		GROUP_LEFT_WIN ,					// 왼쪽이 이김
		GROUP_RIGHT_LOSE = GROUP_LEFT_WIN,	// 왼쪽이 이김 ( = 오른쪽이 짐)

		GROUP_RIGHT_WIN,					// 오른쪽이 이김
		GROUP_LEFT_LOSE = GROUP_RIGHT_WIN,	// 오른쪽이 이김 ( = 왼쪽이 짐)

		GROUP_UNKWON, // 상태 불명 ( 진행할수 없는상태거나, 하부리그 기다리는중 )
		GROUP_WAIT_INITALIZE, // 초기화 기다리는중
		GROUP_IN_PROGRESS, // 진행중
		// 이건 노드 매우 마지막이라는 의미 즉. 플레이어들이다.
		GROUP_TAIL,
	};

	// 토너먼트 트리 구조체 클레스
	// Client Server 공통 클레스
	// 복잡도가 높아 코드해석에 어려움이 있으니 
	// 주석 참조 요망!
	// thkwon 2013 10 01
	class TournamentTree
	{

		
		typedef boost::shared_ptr<TournamentTree> SPTR_TTREE;
		// 너무길어서 위크 포인터를 축약했다.
		typedef boost::weak_ptr<TournamentTree> WPTR_TTREE;
		
		/*

		***************
		* LEFT  GROUP *----*
		***************    |    ************** 
                           *--->*  MY GROUP  *--*
		***************    |    **************  |    ************
		* RIGHT GROUP *----*                    *--->*  PARENT  *
		***************                         |    ************

		*/
	public:
		// 위 주석 참조
		SPTR_TTREE pLeftGroup;

		SPTR_TTREE pRightGroup;

		WPTR_TTREE pParentGroup;

	private:


		//TTID(Tournament Tree IDentify) 토너먼트 트리 번호
		//TOP(머리) 만 가지고있다. 아니면 NULL
		MatchingID TTID;

		GROUP_STATUS m_GroupStatus;

		//TGID(Tournament Group IDentify) 그룹 식별 번호
		//TAIL(꼬리) 만 가지고 있다. 아니면 NULL
		MatchingGroupID m_tgid;

		//토너먼트 중복 방지 및 정확한 토너먼트 위치를 지정하기 위하여 
		//깊이 값을 줌
		DWORD m_tournament_num;
		

	public:

		// 최상단 노드 만들때만 사용
		// Paraent : 상부 노드
		TournamentTree( MatchingID _TournamentTreeID) 
			: TTID(_TournamentTreeID), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			pLeftGroup.reset();
			pRightGroup.reset();
		}

		// 최 하단 노드 만들때만 사용
		// Paraent : 상부 노드
		// TGID    : 그룹 고유값
		TournamentTree( WPTR_TTREE parent, MatchingGroupID _TGID ) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_TAIL)
		{
			if( parent.lock()->getTournamentTreeID() == NULL)
			{
				// 최상단 노드가 없다.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = parent;
			m_tgid = _TGID;
		}

		// 최 하단 노드 만들때만 사용
		// Paraent : 상부 노드
		// TGID    : 그룹 고유값
		TournamentTree( TournamentTree* parent, MatchingGroupID _TGID ) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_TAIL)
		{
			if( parent->getTournamentTreeID() == NULL)
			{
				// 최상단 노드가 없다.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = SPTR_TTREE(parent);
			m_tgid = _TGID;
		}



		// 노드 만들때만 사용
		// Paraent : 상부 노드
		TournamentTree( WPTR_TTREE parent) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			if(  parent.lock()->getTournamentTreeID() == NULL)
			{
				// 최상단 노드가 없다.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = parent;
		}

		// 노드 만들때만 사용
		// Paraent : 상부 노드
		TournamentTree( TournamentTree* parent) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			if( parent->getTournamentTreeID() == NULL)
			{
				// 최상단 노드가 없다.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = SPTR_TTREE(parent);
		}



	public:

		void SetChildNode()
		{
			pLeftGroup.reset(new TournamentTree(this));
			pRightGroup.reset(new TournamentTree(this));
		}

		void SetChildNode(MatchingGroupID Ltgid,MatchingGroupID Rtgid )
		{
			pLeftGroup.reset(new TournamentTree(this,Ltgid));
			pRightGroup.reset(new TournamentTree(this,Rtgid));
		}

		void SetGroupStatus(GROUP_STATUS _status)
		{
			// 추가 작업 하지 않는다.
			m_GroupStatus = _status;
		}

		void SetWinner(MatchingGroupID _tgid)
		{
			MatchingGroupID ltgid =pParentGroup.lock()->getLeft().lock()->getTGID();
			MatchingGroupID rtgid =pParentGroup.lock()->getRight().lock()->getTGID();

			if(ltgid == _tgid)
				pParentGroup.lock()->SetGroupStatus(GROUP_LEFT_WIN);
			if(rtgid == _tgid)
				pParentGroup.lock()->SetGroupStatus(GROUP_RIGHT_WIN);
		}

		DWORD getWinTGID()
		{
			if(m_GroupStatus == GROUP_LEFT_WIN)
				return pLeftGroup->getWinTGID();
			else if(m_GroupStatus == GROUP_RIGHT_WIN)
				return pRightGroup->getWinTGID();
			return m_tgid;
		}
		DWORD getTGID()
		{
			return m_tgid;
		}		

		WPTR_TTREE getRight()
		{return WPTR_TTREE(pRightGroup); }
		
		WPTR_TTREE getLeft()
		{return WPTR_TTREE(pLeftGroup); }

		WPTR_TTREE GetParent()
		{return WPTR_TTREE(pParentGroup); }

		DWORD getTournamentTreeID()
		{
			if(TTID == NULL)
				return pParentGroup.lock()->getTournamentTreeID();
			else
				return TTID;
		}

		bool isTail(){ return m_GroupStatus == GROUP_TAIL; }

		// 전체 하부까지 모두 리셋해버림
		void Reset()
		{
			m_GroupStatus = GROUP_UNKWON;
			
			if(isTail())
			{
				return;
			}

			m_tgid = NULL;
			if(pLeftGroup.get())
				pLeftGroup->Reset();
			if (pRightGroup.get())
				pRightGroup->Reset();
		}

		
	};



}