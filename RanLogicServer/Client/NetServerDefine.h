#ifndef _NET_SERVER_DEFINE_H_
#define _NET_SERVER_DEFINE_H_

#pragma once

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"

struct CNT_USR_INFO
{		
    INT nSvrGrp; //! ���� �׷�
    INT nSvrNum; //! ���� ��ȣ
    INT nUsrNum; //! ����� ��ȣ	(DB)
    INT nChaNum; //! ĳ���͹�ȣ(DB)	
    DWORD dwGaeaID; //! ���̾� ID

    DWORD dwSvrClient; //! ���Ǽ��� Ŭ���̾�Ʈ ��ȣ
    DWORD dwGameClient; //! ���Ӽ��� Ŭ���̾�Ʈ ��ȣ
    TCHAR szUserID[GSP_USERID]; //! ����� ID
    TCHAR szChaName[CHR_ID_LENGTH+1]; //! �÷������� ĳ���͸�

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