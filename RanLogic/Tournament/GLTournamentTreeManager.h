#pragma once

#include "./GLTournamentTree.h"
#include "./GLTournamentTreeDefine.h"



#include <math.h>


namespace MatchingSystem
{
	//��ʸ�Ʈ Ʈ���� ���� ���� �Ǵ� ����ϱ����Ͽ� ���� Ŭ����
	class TournamentTreeManager
	{

	private:
		boost::shared_ptr<TournamentTree> m_tornamentreeHead;

		std::vector<DWORD> GroupIDs;
		

		UINT m_Depth;

		UINT m_Count;

		UINT m_TournamentGuropSize;

		MatchingID m_ttid;

		UINT m_sqid;


	public:

		//�ڵ����� Ʈ���� ����� �ְڴ�!
		//��ʸ�Ʈ ���� �ο�, Ʈ�� ��ȣ
		TournamentTreeManager(UINT TournamentGuropSize,MatchingID _ttid);

		// ��ʸ�Ʈ ��� ���� (�ſ� ������ �����ؾ���.)
		void TournamentWin(MatchingID _ttid);

	private:
		// ���̿� ���� Ʈ�� ����
		void MakeTree(UINT _Depth);

		// Ʈ�� ��� �����
		void MakeTreeNode(WPTR_TTREE _node,UINT _Depth);

		// �� ã��.
		WPTR_TTREE FindNode(WPTR_TTREE _node,MatchingGroupID _id);
		// ���׿��� ����� �༮�� ã��
		WPTR_TTREE FindWinNode(WPTR_TTREE _node,MatchingGroupID _id);



	};
}