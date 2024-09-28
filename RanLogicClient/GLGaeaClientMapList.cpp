#include "pch.h"
#include "./Land/GLMapListClient.h"
#include "./GLGaeaClient.h"


#include "../SigmaCore/DebugInclude.h"


SMAPNODE* GLGaeaClient::FindMapNode(const SNATIVEID& sNativeID)
{
    return m_pMapList->FindMapNode(sNativeID);
}

const char* GLGaeaClient::GetMapName(const SNATIVEID& sNativeID)
{
    return m_pMapList->GetMapName(sNativeID);
}

HRESULT GLGaeaClient::LoadMapList(
    BOOL bTool,
    F_SERVER_INFO* pServerInfo,
    int nServerNum)
{
    return m_pMapList->LoadMapList(bTool, pServerInfo, nServerNum);
}

GLMapList::FIELDMAP& GLGaeaClient::GetMapList()
{
    return m_pMapList->GetMapList();
}

std::tr1::shared_ptr<GLMapAxisInfo> GLGaeaClient::GetAxisInfo(const std::string& FileName)
{
    return m_pMapList->GetAxisInfo(FileName);
}

std::string GLGaeaClient::GetMapNameStr(const SNATIVEID& MapID) const
{
    return m_pMapList->GetMapNameStr(MapID);
}

void GLGaeaClient::ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY)
{
    m_pMapList->ConvertWorldToMap(mapID.getBaseMapID(), vPos, nPosX, nPosY);
}

void GLGaeaClient::ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos)
{
    m_pMapList->ConvertMapToWorld(mapID.getBaseMapID(), PosX, PosY, vPos);
}