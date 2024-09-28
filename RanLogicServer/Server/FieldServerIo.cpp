#include "../pch.h"
#include "../Client/NetFieldClientMan.h"
#include "s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

PER_IO_OPERATION_DATA* CFieldServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void CFieldServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}