#include "pch.h"
#include "./GLTournamentTree.h"
#include "./GLTournamentTreeManager.h"
#include "./GLTournamentTreeDefine.h"
#include <math.h>


namespace MatchingSystem
{

	//�ڵ����� Ʈ���� ����� �ְڴ�!
	//��ʸ�Ʈ ���� �ο�, Ʈ�� ��ȣ
	TournamentTreeManager::TournamentTreeManager(UINT TournamentGuropSize,MatchingID _ttid)
	{

		m_sqid = 0;

		m_ttid= _ttid;

		m_TournamentGuropSize = TournamentGuropSize;

		//��ʸ�Ʈ ���� ����. 2^x = N

		// n= log2(x)

		double _tosize = TournamentGuropSize;
		double _log2 = 2;
		double m_log = (log(_tosize) / log(_log2));
		

		//��ʸ�Ʈ ������ �ϴ� ���� 
		double resualt =  (1-pow(2,m_log-1))/(1- m_log-1);

		m_Count = (UINT)resualt;

		// �Ӹ� �����
		m_tornamentreeHead.reset(new TournamentTree(_ttid));

		MakeTree(m_Depth);

	}

	void TournamentTreeManager::TournamentWin(MatchingID _ttid)
	{
		WPTR_TTREE winner = FindWinNode(m_tornamentreeHead,_ttid);
		winner.lock().get()->SetWinner(_ttid);
	}

	void TournamentTreeManager::MakeTree(UINT _Depth)
	{
		if (!m_tornamentreeHead.get())
		{
			return;
		}

		m_tornamentreeHead.get()->SetChildNode();
		_Depth--;

		WPTR_TTREE Lptr = m_tornamentreeHead.get()->getLeft();
		MakeTreeNode(Lptr,_Depth);

		WPTR_TTREE Rptr = m_tornamentreeHead.get()->getRight();
		MakeTreeNode(Rptr,_Depth);
	}

	void TournamentTreeManager::MakeTreeNode(WPTR_TTREE _node,UINT _Depth)
	{
		if(_Depth < 0)
		{
			return;
		}
		else if(_Depth == 0)
		{
			_node.lock().get()->SetChildNode(m_sqid,m_sqid+1);
			m_sqid+=2;
		}
		else
		{
			_node.lock().get()->SetChildNode();
			_Depth--;

			WPTR_TTREE Lptr = _node.lock().get()->getLeft();
			MakeTreeNode(Lptr,_Depth);

			WPTR_TTREE Rptr = _node.lock().get()->getRight();
			MakeTreeNode(Rptr,_Depth);
		}
	}

	// �� ã��.
	WPTR_TTREE TournamentTreeManager::FindNode(WPTR_TTREE _node,MatchingGroupID _id)
	{
		if(_node.lock().get() == NULL)
			return _node;

		WPTR_TTREE Lptr = _node.lock().get()->getLeft();

		if(Lptr.lock().get())
		{
			if(Lptr.lock().get()->getTGID() == _id)
				return Lptr;
			else
				return FindNode(Lptr,_id);
		}

		WPTR_TTREE Rptr = _node.lock().get()->getRight();

		if(Rptr.lock().get())
		{
			if(Rptr.lock().get()->getTGID() == _id)
				return Rptr;
			else
				return FindNode(Rptr,_id);
		}

		return _node;
	}

	// ���׿��� ����� �༮�� ã��
	WPTR_TTREE TournamentTreeManager::FindWinNode(WPTR_TTREE _node,MatchingGroupID _id)
	{
		SPTR_TTREE _sPnode = _node.lock();

		if(!_sPnode)
			return _node;

		

		WPTR_TTREE Lptr = _sPnode->getLeft();

		SPTR_TTREE LSptr = Lptr.lock();

		if(LSptr.get())
		{
			if(LSptr->getWinTGID() == _id)
				return Lptr;
			else
				return FindNode(Lptr,_id);
		}

		WPTR_TTREE Rptr = _sPnode->getRight();
		SPTR_TTREE RSptr = Rptr.lock();

		if(RSptr.get())
		{
			if(RSptr->getWinTGID() == _id)
				return Rptr;
			else
				return FindNode(Rptr,_id);
		}

		return _node;
	}
}