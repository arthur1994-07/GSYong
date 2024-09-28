#include "stdafx.h"
#include "./InnerInterface.h"

#include "./OldUI/PrivateMarket/ItemSearchResultWindow.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::ItemSearchResultUpdate(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum)
{
    m_pItemSearchResultWindow->UpdateSearchResult(pSearchItemResult, dwUpdateNum, dwPageNum);
}