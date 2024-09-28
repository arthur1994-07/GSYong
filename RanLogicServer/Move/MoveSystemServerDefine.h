#pragma once

#include "../../RanLogic/Move/MoveSystemDefine.h"

namespace MoveSystem
{

	// Move Map ���� ���� ( Agent );
	struct SMOVE_INFO_AGENT : public SMOVE_INFO
	{
		DWORD				dwFieldNum;		// �̵��� �ʵ��� ��ȣ;
		DWORD				dwChannelNum;	// �̵��� ä���� ��ȣ;

		SMOVE_INFO_AGENT ()
			: SMOVE_INFO ()
			, dwFieldNum ( GAEAID_NULL )
			, dwChannelNum ( 0 )
		{

		}
	};

}