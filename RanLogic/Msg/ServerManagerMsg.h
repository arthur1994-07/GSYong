#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"

namespace GLMSG
{
	struct NET_INSPECTION_REWARD_SET_REQ : public NET_MSG_GENERIC
    {
        int nServerGroup;
		__time64_t tStart;
		__time64_t tEnd;
		int nRewardTime;
        NET_INSPECTION_REWARD_SET_REQ( int nServerGroup_, __time64_t tStart_, __time64_t tEnd_, int nRewardTime_ )
            : nServerGroup(nServerGroup_)
			, tStart(tStart_)
			, tEnd(tEnd_)
			, nRewardTime(nRewardTime_)
        {
            dwSize = sizeof( NET_INSPECTION_REWARD_SET_REQ );
            nType  = NET_MSG_INSPECTION_REWARD_SET_REQ;
        }
    };

	struct NET_INSPECTION_REWARD_SET_ACK : public NET_MSG_GENERIC
	{
		enum
		{
			EM_ACK_NOT_USE = 0,
			EM_ACK_OK,
			EM_ACK_ERROR,
			EM_ACK_DB_ERROR,

		};


		int	nResult;
		NET_INSPECTION_REWARD_SET_ACK()
			: nResult(EM_ACK_NOT_USE)
		{
			dwSize = sizeof( NET_INSPECTION_REWARD_SET_ACK );
			nType  = NET_MSG_INSPECTION_REWARD_SET_ACK;
		}
	};

} // namespace GLMSG

