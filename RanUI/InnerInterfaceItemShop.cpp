#include "stdafx.h"
#include "./OldUI/Display/ItemShopIconMan.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::ItemShopAddShopIcon(DWORD GaeaId)
{
    if (m_pItemShopIconMan)
        m_pItemShopIconMan->ADD_SHOP_ICON(GaeaId);
}

void CInnerInterface::ItemShopDelShopIcon(DWORD GaeaId)
{
    if (m_pItemShopIconMan)
        m_pItemShopIconMan->ADD_SHOP_ICON(GaeaId);
}

void CInnerInterface::ItemShopSetRender(DWORD dwGaeaID)
{
    if (m_pItemShopIconMan) 
        m_pItemShopIconMan->SetRender(dwGaeaID);
}