#pragma once

#include "GLContrlBaseMsg.h"
#include <string>
#include <vector>
//#include "../../SigmaCore/MsgPack/MinMsgPack.h"

namespace GLMSG
{
	struct SNETPC_RNATTENDANCE_QUEST_LIST_FC
	{
		std::vector<DWORD> vecQuestIDList;
		MSGPACK_DEFINE(vecQuestIDList);
		//NET_MSG_RNATTENDANCE_QUEST_LIST_FC
		SNETPC_RNATTENDANCE_QUEST_LIST_FC()
		{}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_RNATTENDANCE_QUEST_START : public NET_MSG_GENERIC
	{
		DWORD				dwQuestID;

		SNETPC_REQ_RNATTENDANCE_QUEST_START (DWORD _dwQuestID) 
			: dwQuestID(_dwQuestID)
		{
			dwSize = sizeof(SNETPC_REQ_RNATTENDANCE_QUEST_START);
			nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_REQ_RNATTENDANCE_QUEST_START;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_RNATTENDANCE_QUEST_START)<=NET_DATA_BUFSIZE);
		}
	};

} 

