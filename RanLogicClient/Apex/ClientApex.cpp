#include "pch.h"
#include "../../=Include/ApexClient.h"
#include "../../RanLogic/Msg/ApexMsg.h"
#include "../Stage/DxGlobalStage.h"
#include "./ClientApex.h"

//#ifdef	CH_PARAM 
//#pragma comment(lib, "../=Library_CH/AHClientInterface.lib")
//#else	
//#pragma comment(lib, "../=Library/AHClientInterface.lib")
//#endif


_FUNC_C_REC pfRec = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static DxGlobalStage* gpGlobalStage = NULL;

// Apex Àû¿ë
namespace APEX_CLIENT
{
	long NetSendToGameServer( const char * pBuffer,int nLen )
	{
        if ( (nLen <= 0) || (nLen >= NET_APEX_DATA::MAX_APEX_PACKET_LENGTH) )
			return -1;

		NET_APEX_DATA ApexMsg;
        bool bSet = ApexMsg.SetData(pBuffer, nLen);
		if (gpGlobalStage && bSet)
		{
            gpGlobalStage->NetSend(&ApexMsg);
		}
		return 0;
	}

	int StartApexClient(DxGlobalStage* pGlobalStage)
	{
        gpGlobalStage = pGlobalStage;
#ifndef _DEBUG
		return CHCStart( NetSendToGameServer, pfRec );
#else
		return 0;
#endif
	}

	int StopApexClient()
	{
#ifndef _DEBUG
		CHCEnd();
#endif
		return 0;
	}

	int NoticeApex_UserData(const char * pBuf,int nBufLen)
	{
		if(pfRec)
		{
			pfRec(pBuf,nBufLen);			
		}
		return 0;
	}
};
