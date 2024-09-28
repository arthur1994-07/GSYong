#pragma once

#include "../../RanLogic/Move/MoveSystemDefine.h"

namespace MoveSystem
{

	// Move Map 관련 정보 ( Agent );
	struct SMOVE_INFO_AGENT : public SMOVE_INFO
	{
		DWORD				dwFieldNum;		// 이동할 필드의 번호;
		DWORD				dwChannelNum;	// 이동할 채널의 번호;

		SMOVE_INFO_AGENT ()
			: SMOVE_INFO ()
			, dwFieldNum ( GAEAID_NULL )
			, dwChannelNum ( 0 )
		{

		}
	};

}