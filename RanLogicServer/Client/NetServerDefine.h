#ifndef _NET_SERVER_DEFINE_H_
#define _NET_SERVER_DEFINE_H_

#pragma once

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"

struct CNT_USR_INFO
{		
    INT nSvrGrp; //! 서버 그룹
    INT nSvrNum; //! 서버 번호
    INT nUsrNum; //! 사용자 번호	(DB)
    INT nChaNum; //! 캐릭터번호(DB)	
    DWORD dwGaeaID; //! 가이아 ID

    DWORD dwSvrClient; //! 세션서버 클라이언트 번호
    DWORD dwGameClient; //! 게임서버 클라이언트 번호
    TCHAR szUserID[GSP_USERID]; //! 사용자 ID
    TCHAR szChaName[CHR_ID_LENGTH+1]; //! 플레이중인 캐릭터명

    CNT_USR_INFO()
        : nSvrGrp(-1)
        , nSvrNum(-1)
        , nUsrNum(-1)
        , nChaNum(-1)
        , dwGaeaID(-1)
        , dwSvrClient(-1)
        , dwGameClient(-1)
    {        
        memset(szUserID, 0,  sizeof(TCHAR) * (GSP_USERID));
        memset(szChaName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1));
    }
};
typedef CNT_USR_INFO* LPCNT_USR_INFO;

#endif // _NET_SERVER_DEFINE_H_