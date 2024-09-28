#pragma once

#include "./GLTournamentTree.h"
#include "./GLTournamentTreeDefine.h"



#include <math.h>


namespace MatchingSystem
{
	//토너먼트 트리를 쉽게 관리 또는 사용하기위하여 만든 클레스
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

		//자동으로 트리를 만들어 주겠다!
		//토너먼트 참가 인원, 트리 번호
		TournamentTreeManager(UINT TournamentGuropSize,MatchingID _ttid);

		// 토너먼트 우승 설정 (매우 신중히 설정해야함.)
		void TournamentWin(MatchingID _ttid);

	private:
		// 깊이에 따라 트리 만듦
		void MakeTree(UINT _Depth);

		// 트리 노드 만들기
		void MakeTreeNode(WPTR_TTREE _node,UINT _Depth);

		// 다 찾기.
		WPTR_TTREE FindNode(WPTR_TTREE _node,MatchingGroupID _id);
		// 리그에서 우승한 녀석만 찾기
		WPTR_TTREE FindWinNode(WPTR_TTREE _node,MatchingGroupID _id);



	};
}