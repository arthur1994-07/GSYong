#pragma once

#include "GLContrlBaseMsg.h"

namespace GLMSG
{
	//! 추적할 id 추가
	struct NET_UPDATE_TRACINGCHAR_ALL : public NET_MSG_GENERIC
	{    
		STracingData tracingData[MAX_TRACING_NUM];

		NET_UPDATE_TRACINGCHAR_ALL()
		{
			nType = NET_MSG_UPDATE_TRACING_ALL;
			dwSize = sizeof(NET_UPDATE_TRACINGCHAR_ALL);
			MIN_STATIC_ASSERT(sizeof(NET_UPDATE_TRACINGCHAR_ALL)<=NET_DATA_BUFSIZE);

			for (int i=0; i<MAX_TRACING_NUM; ++i)
			{
				tracingData[i].dwUserNum      = 0;
				tracingData[i].ResetAccount();
				tracingData[i].dwOnLine		  = 255;
				tracingData[i].nConnectServer = -1;
			}
		}
	};

	//! 추적할 id 추가
	struct NET_UPDATE_TRACINGCHAR : public NET_MSG_GENERIC
	{    
		BYTE		 updateNum;
		STracingData tracingData;

		NET_UPDATE_TRACINGCHAR () :
		updateNum(0)
		{
			nType = NET_MSG_UPDATE_TRACING_CHARACTER;
			dwSize = sizeof(NET_UPDATE_TRACINGCHAR);
			MIN_STATIC_ASSERT(sizeof(NET_UPDATE_TRACINGCHAR)<=NET_DATA_BUFSIZE);

			tracingData.dwUserNum      = 0;
			tracingData.ResetAccount();
			tracingData.dwOnLine		  = 255;
			tracingData.nConnectServer = -1;
		}
	};


	//! 추적중인 유저의 로그를 agent서버에 보낸다.
	struct NET_LOG_UPDATE_TRACINGCHAR : public NET_MSG_GENERIC
	{
		int  nUserNum;
		CHAR szAccount[USR_ID_LENGTH+1];    
		CHAR szLogMsg[TRACING_LOG_SIZE];

		NET_LOG_UPDATE_TRACINGCHAR()
			: nUserNum(-1)
		{
			nType = NET_MSG_LOG_UPDATE_TRACING_CHARACTER;
			dwSize = sizeof(NET_LOG_UPDATE_TRACINGCHAR);
			MIN_STATIC_ASSERT(sizeof(NET_LOG_UPDATE_TRACINGCHAR)<=NET_DATA_BUFSIZE);

			memset(szAccount, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
			memset(szLogMsg, 0, sizeof(CHAR) * (TRACING_LOG_SIZE));
		}

		void SetAccount(const char* _Account)
		{
			StringCchCopy(szAccount, USR_ID_LENGTH+1, _Account);
		}

		void SetLog(const char* _Msg)
		{
			StringCchCopy(szLogMsg, TRACING_LOG_SIZE, _Msg);
		}
	};

	//! 필드->클라이언트 추적 유저인지 아닌지를 보낸다.
	struct NET_TRACING_CHARACTER : public NET_MSG_GENERIC
	{    
		bool			bTracingChar;

		NET_TRACING_CHARACTER () :
		bTracingChar(FALSE)
		{
			nType = NET_MSG_TRACING_CHARACTER;
			dwSize = sizeof(NET_TRACING_CHARACTER);
			MIN_STATIC_ASSERT(sizeof(NET_TRACING_CHARACTER)<=NET_DATA_BUFSIZE);
		}
	};

}
