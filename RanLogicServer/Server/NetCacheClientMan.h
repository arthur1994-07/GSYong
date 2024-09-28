#ifndef _NET_CACHE_CLIENT_MAN_H_
#define _NET_CACHE_CLIENT_MAN_H_

#pragma once

#include "../Client/NetClientManager.h"

class NetCacheClientMan : public NetClientManager
{
public:
    NetCacheClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax);
    virtual ~NetCacheClientMan();
};

#endif // _NET_CACHE_CLIENT_MAN_H_