#include "stdafx.h"
#include "./InnerInterface.h"

#include "./OldUI/Display/SummonNameDisplayMan.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::SummonSetCount(DWORD Count)
{
    if (m_pSummonNameDisplayMan)
        m_pSummonNameDisplayMan->SET_COUNT(Count);
}

void CInnerInterface::SummonAddDisp(DWORD GlobalId)
{
    if (m_pSummonNameDisplayMan)
        m_pSummonNameDisplayMan->ADD_DISP(GlobalId);
}