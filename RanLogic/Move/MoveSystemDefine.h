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
		EMMOVE_TYPE_DEFAULT,			// �Ϲ�;
		EMMOVE_TYPE_RECALL,				// ��ȯ��;
		EMMOVE_TYPE_TELEPORT,			// ��ȯ��;
		EMMOVE_TYPE_CHECK,				// ĳ���� ��ȿ�� �˻� �ؾ��ϴ� �̵�;
		EMMOVE_TYPE_RESURRECT,			// ��Ȱ;
		EMMOVE_TYPE_TRIGGERSYSTEM,		// Ʈ���� �ý���;
		EMMOVE_TYPE_GATE,				// ����Ʈ �̵�;
	};

	// �ִ� ��õ� Ƚ���� 3ȸ�� �����ϰ� 3ȸ�̻� ������ ��� �̵��� ���н�Ų��;
	const DWORD				MAX_RETRY_COUNT = 3;

	// Move Map ���� ����;
	struct SMOVE_INFO
	{
		EMMOVE_TYPE			emMoveType;		// �̵� Ÿ��;
		DWORD				dwChaDBNum;		// �̵� ĳ����;
		MapID				sMapID;			// �̵��� ��;
		DWORD				dwGateID;		// �̵��� ���� ����Ʈ;
		D3DXVECTOR3			vPosition;		// �̵��� ���� ��ġ;

		// �̵��� ������ ����� ��õ� Ƚ��;
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


	// Move Map �� Queue �� ����Ͽ� ���ʷ� ó���Ѵ�;
	// tbb::concurrent_queue -> (empty, push, try_pop) thread safe queue;
	// �� �ܿ��� thread unsafe;
#ifdef _USE_TBB
	typedef tbb::concurrent_queue< SMOVE_INFO >		QUEUE_MOVE_MAP;
	typedef tbb::concurrent_vector< SMOVE_INFO >	VEC_MOVE_MAP;
#else
	typedef std::queue< SMOVE_INFO >				QUEUE_MOVE_MAP;
	typedef std::vector< SMOVE_INFO >				VEC_MOVE_MAP;
#endif

}