#include "pch.h"
#include <string.h>
#include "../../SigmaCore/Util/CBit.h"

#include "GLContrlCharJoinMsg.h"
#include "GLContrlPcMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLMSG
{
	void SNETPC_UPDATE_STATE::ENCODE ( DWORD dwKEY )
	{
		char *pBUFFER = (char*) ( (char*)(this) + sizeof(NET_MSG_GENERIC) );
		int nSize = dwSize - sizeof(NET_MSG_GENERIC);

		sc::CBit cBIT;
		cBIT._buf_encode ( pBUFFER, nSize, ROT_RIGHT, int(dwKEY+1) );
	}

	void SNETPC_UPDATE_STATE::DECODE ( DWORD dwKEY )
	{
		char *pBUFFER = (char*) ( (char*)(this) + sizeof(NET_MSG_GENERIC) );
		int nSize = dwSize - sizeof(NET_MSG_GENERIC);

		sc::CBit cBIT;
		cBIT._buf_decode ( pBUFFER, nSize, ROT_RIGHT, int(dwKEY+1) );
	}
};
