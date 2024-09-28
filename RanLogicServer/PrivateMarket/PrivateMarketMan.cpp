#include "pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "PrivateMarketMan.h"

#include "../../SigmaCore/DebugInclude.h"

PrivateMarketMan::PrivateMarketMan(GLGaeaServer* pServer)
    : m_pGaeaServer(pServer)
{
}

PrivateMarketMan::~PrivateMarketMan()
{
}

void PrivateMarketMan::CloseMarket(DWORD ChaDbNum)
{
    DelItem(ChaDbNum);
}

void PrivateMarketMan::OpenMarket(DWORD ChaDbNum)
{
}

void PrivateMarketMan::AddItem(DWORD ChaDbNum, const SSEARCHITEMDATA& Data)
{
}

void PrivateMarketMan::DelItem(DWORD ChaDbNum)
{
}

