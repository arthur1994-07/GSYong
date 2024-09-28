#pragma once

#include <vector>

#include <boost/exception/exception.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include "GLTournamentTreeDefine.h"

namespace MatchingSystem
{

	// ���α׷��ӿ��Ը� ���Ǵ� ���� �޽���
	enum GROUP_ERROR
	{
		GROUP_HEAD_IS_INVALITED, // �������� ���� Node�� Head�� ����߽��ϴ�.

	};

	enum GROUP_STATUS
	{
		GROUP_LEFT_WIN ,					// ������ �̱�
		GROUP_RIGHT_LOSE = GROUP_LEFT_WIN,	// ������ �̱� ( = �������� ��)

		GROUP_RIGHT_WIN,					// �������� �̱�
		GROUP_LEFT_LOSE = GROUP_RIGHT_WIN,	// �������� �̱� ( = ������ ��)

		GROUP_UNKWON, // ���� �Ҹ� ( �����Ҽ� ���»��°ų�, �Ϻθ��� ��ٸ����� )
		GROUP_WAIT_INITALIZE, // �ʱ�ȭ ��ٸ�����
		GROUP_IN_PROGRESS, // ������
		// �̰� ��� �ſ� �������̶�� �ǹ� ��. �÷��̾���̴�.
		GROUP_TAIL,
	};

	// ��ʸ�Ʈ Ʈ�� ����ü Ŭ����
	// Client Server ���� Ŭ����
	// ���⵵�� ���� �ڵ��ؼ��� ������� ������ 
	// �ּ� ���� ���!
	// thkwon 2013 10 01
	class TournamentTree
	{

		
		typedef boost::shared_ptr<TournamentTree> SPTR_TTREE;
		// �ʹ��� ��ũ �����͸� ����ߴ�.
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
		// �� �ּ� ����
		SPTR_TTREE pLeftGroup;

		SPTR_TTREE pRightGroup;

		WPTR_TTREE pParentGroup;

	private:


		//TTID(Tournament Tree IDentify) ��ʸ�Ʈ Ʈ�� ��ȣ
		//TOP(�Ӹ�) �� �������ִ�. �ƴϸ� NULL
		MatchingID TTID;

		GROUP_STATUS m_GroupStatus;

		//TGID(Tournament Group IDentify) �׷� �ĺ� ��ȣ
		//TAIL(����) �� ������ �ִ�. �ƴϸ� NULL
		MatchingGroupID m_tgid;

		//��ʸ�Ʈ �ߺ� ���� �� ��Ȯ�� ��ʸ�Ʈ ��ġ�� �����ϱ� ���Ͽ� 
		//���� ���� ��
		DWORD m_tournament_num;
		

	public:

		// �ֻ�� ��� ���鶧�� ���
		// Paraent : ��� ���
		TournamentTree( MatchingID _TournamentTreeID) 
			: TTID(_TournamentTreeID), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			pLeftGroup.reset();
			pRightGroup.reset();
		}

		// �� �ϴ� ��� ���鶧�� ���
		// Paraent : ��� ���
		// TGID    : �׷� ������
		TournamentTree( WPTR_TTREE parent, MatchingGroupID _TGID ) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_TAIL)
		{
			if( parent.lock()->getTournamentTreeID() == NULL)
			{
				// �ֻ�� ��尡 ����.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = parent;
			m_tgid = _TGID;
		}

		// �� �ϴ� ��� ���鶧�� ���
		// Paraent : ��� ���
		// TGID    : �׷� ������
		TournamentTree( TournamentTree* parent, MatchingGroupID _TGID ) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_TAIL)
		{
			if( parent->getTournamentTreeID() == NULL)
			{
				// �ֻ�� ��尡 ����.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = SPTR_TTREE(parent);
			m_tgid = _TGID;
		}



		// ��� ���鶧�� ���
		// Paraent : ��� ���
		TournamentTree( WPTR_TTREE parent) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			if(  parent.lock()->getTournamentTreeID() == NULL)
			{
				// �ֻ�� ��尡 ����.
				throw GROUP_HEAD_IS_INVALITED;
			}

			pLeftGroup.reset();
			pRightGroup.reset();
			pParentGroup = parent;
		}

		// ��� ���鶧�� ���
		// Paraent : ��� ���
		TournamentTree( TournamentTree* parent) 
			: TTID(NULL), m_tgid(NULL),m_GroupStatus(GROUP_UNKWON)
		{
			if( parent->getTournamentTreeID() == NULL)
			{
				// �ֻ�� ��尡 ����.
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
			// �߰� �۾� ���� �ʴ´�.
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

		// ��ü �Ϻα��� ��� �����ع���
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