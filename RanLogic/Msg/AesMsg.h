#ifndef _GLOGIC_AES_MSG_H_
#define _GLOGIC_AES_MSG_H_

#pragma once

#include "../s_NetGlobal.h"

//! Client->Server ����Ű�� ��û�Ѵ�
struct NET_CS_AES_REQ_SERVER_PUB_KEY : public NET_MSG_GENERIC
{
    NET_CS_AES_REQ_SERVER_PUB_KEY()
    {
        wSize = sizeof(NET_CS_AES_REQ_SERVER_PUB_KEY);
        wType = NET_MSG_CS_AES_REQ_SERVER_PUB_KEY;
    }
};

#endif // _GLOGIC_AES_MSG_H_