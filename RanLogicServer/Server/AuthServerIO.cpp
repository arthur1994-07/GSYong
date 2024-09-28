#include "pch.h"
#include "../Client/NetAuthClientMan.h"
#include "./AuthServer.h"
#include "../../SigmaCore/DebugInclude.h"

PER_IO_OPERATION_DATA* CAuthServer::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pClientManager->GetFreeOverIO(nType);
}

void CAuthServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
    m_pClientManager->ReleaseOperationData(pData);
}
