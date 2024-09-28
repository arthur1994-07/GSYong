#include "../pch.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

PER_IO_OPERATION_DATA* CAgentServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void CAgentServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}