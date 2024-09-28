#ifndef _APEX_MSG_H_
#define _APEX_MSG_H_

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../Network/NetLogicDefine.h"

// Apex 利侩( 全尼 )
struct NET_APEX_RETURN : public NET_MSG_GENERIC
{
    int nReturn;

    NET_APEX_RETURN()
        : nReturn ( 0 )
    {
        nType  = NET_MSG_APEX_RETURN;
        dwSize = sizeof(NET_APEX_RETURN);		
    }
};

//! Apex 皋技瘤
struct NET_APEX_DATA : public NET_MSG_GENERIC
{
    enum {
        MAX_APEX_PACKET_LENGTH = NET_DATA_BUFSIZE - sizeof(NET_MSG_GENERIC),
    };

    char szData[MAX_APEX_PACKET_LENGTH];

    NET_APEX_DATA()
    {
        nType  = NET_MSG_APEX_DATA;
        dwSize = sizeof(NET_APEX_DATA);
        memset(szData, 0, sizeof(CHAR) * MAX_APEX_PACKET_LENGTH);
        MIN_STATIC_ASSERT(sizeof(NET_APEX_DATA) <= NET_DATA_BUFSIZE);
    }

    bool SetData(const char* pBuffer, int Length)
    {
        if (Length >= MAX_APEX_PACKET_LENGTH)
        {
            return false;
        }
        else
        {
            memcpy(szData, pBuffer, Length);
            dwSize = sizeof(NET_MSG_GENERIC) + Length;
            return true;
        }
    }
};
typedef NET_APEX_DATA* LPNET_APEX_DATA;

//! Apex 皋技瘤
struct NET_APEX_ANSWER : public NET_MSG_GENERIC
{    
    WORD			wAction;
    WORD			wHour;

    NET_APEX_ANSWER()
    {
        nType  = NET_MSG_APEX_ANSWER;
        dwSize = sizeof(NET_APEX_ANSWER);
        wAction = 0;
        wHour = 0;
    }
};
typedef NET_APEX_ANSWER* LPNET_APEX_ANSWER;

#endif // _APEX_MSG_H_