#include "pch.h"
#include "./PacketMemoryFactory.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SharedPacket::SharedPacket()
{
}

SharedPacket::~SharedPacket()
{
}

bool SharedPacket::SetData(BYTE* pData, size_t Size)
{
    if (Size < NET_DATA_BUFSIZE)
    {
        memcpy(m_Data, pData, Size);
        return true;
    }
    else
    {
        return false;
    }
}