#include "../pch.h"
#include "../Client/NetSessionClientMan.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

PER_IO_OPERATION_DATA* CSessionServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void CSessionServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}
