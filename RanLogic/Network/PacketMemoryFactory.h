#ifndef _PACKET_MEMORY_FACTORY_H_
#define _PACKET_MEMORY_FACTORY_H_

#pragma once

#include "../../SigmaCore/Memory/SharedMemoryPool.h"
#include "../../SigmaCore/Net/NetDefine.h"

//! 2011-08-12 jgkim
class SharedPacket
{
public:
    SharedPacket();
    ~SharedPacket();    
    bool SetData(BYTE* pData, size_t Size);

private:
    BYTE m_Data[NET_DATA_BUFSIZE];
};

#endif