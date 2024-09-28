#include "StdAfx.h"
#include "./OldUI/Interface/BasicGameMenu.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::GameMenuFlipRunButton(BOOL bRun)
{
    if (m_pGameMenu)
        m_pGameMenu->SetFlipRunButton(bRun);
}

const UIRECT& CInnerInterface::GameMenuGetGlobalPos() const
{
    return m_pGameMenu->GetGlobalPos();
}