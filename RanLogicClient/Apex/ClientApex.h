#ifndef CLIENTAPEX_H_
#define CLIENTAPEX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DxGlobalStage;

namespace APEX_CLIENT
{    
	long NetSendToGameServer( const char * pBuffer,int nLen );

	int StartApexClient(DxGlobalStage* pGlobalStage);
	int StopApexClient();

	int NoticeApex_UserData(const char * pBuf,int nBufLen);
};

#endif // CLIENTAPEX_H_