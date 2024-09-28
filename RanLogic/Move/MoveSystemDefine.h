#pragma once

#ifdef _USE_TBB
#include "../../=TBB/include/tbb/concurrent_queue.h"
#include "../../=TBB/include/tbb/concurrent_vector.h"
#else
#include <queue>
#include <vector>
#endif

#include "../../SigmaCore/Util/GLSingletone.h"

namespace MoveSystem
{

	enum EMMOVE_TYPE
	{
		EMMOVE_TYPE_DEFAULT,			// 일반;
		EMMOVE_TYPE_RECALL,				// 소환서;
		EMMOVE_TYPE_TELEPORT,			// 귀환서;
		EMMOVE_TYPE_CHECK,				// 캐릭터 유효성 검사 해야하는 이동;
		EMMOVE_TYPE_RESURRECT,			// 부활;
		EMMOVE_TYPE_TRIGGERSYSTEM,		// 트리거 시스템;
		EMMOVE_TYPE_GATE,				// 게이트 이동;
	};

	// 최대 재시도 횟수를 3회로 설정하고 3회이상 실패할 경우 이동을 실패시킨다;
	const DWORD				MAX_RETRY_COUNT = 3;

	// Move Map 관련 정보;
	struct SMOVE_INFO
	{
		EMMOVE_TYPE			emMoveType;		// 이동 타입;
		DWORD				dwChaDBNum;		// 이동 캐릭터;
		MapID				sMapID;			// 이동할 맵;
		DWORD				dwGateID;		// 이동할 맵의 게이트;
		D3DXVECTOR3			vPosition;		// 이동할 맵의 위치;

		// 이동에 실패할 경우의 재시도 횟수;
		DWORD				dwRetryCount;

		bool				bCompleteProc_InstanceSystem;

		SMOVE_INFO ()
			: emMoveType ( EMMOVE_TYPE_DEFAULT )
			, dwChaDBNum ( 0 )
			, dwGateID ( UINT_MAX )
			, dwRetryCount ( 0 )
			, bCompleteProc_InstanceSystem ( false )
		{
			vPosition = D3DXVECTOR3( 0.f, 0.f, 0.f );
		}
	};


	// Move Map 을 Queue 에 등록하여 차례로 처리한다;
	// tbb::concurrent_queue -> (empty, push, try_pop) thread safe queue;
	// 그 외에는 thread unsafe;
#ifdef _USE_TBB
	typedef tbb::concurrent_queue< SMOVE_INFO >		QUEUE_MOVE_MAP;
	typedef tbb::concurrent_vector< SMOVE_INFO >	VEC_MOVE_MAP;
#else
	typedef std::queue< SMOVE_INFO >				QUEUE_MOVE_MAP;
	typedef std::vector< SMOVE_INFO >				VEC_MOVE_MAP;
#endif

}