#include "stdafx.h"
#include "./OldUI/Feature/StatsResetWindow.h"
#include "InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::StatsResetWindowRecalculate()
{
    m_pStatsResetWindow->Recalculate();
}

void CInnerInterface::StatsResetWindowResetProperty(
    const WORD wReturnStats, const UI::EMSTATSRESETWINDOW_TYPE emType, const EMITEM_TYPE emItemType)
{
    m_pStatsResetWindow->ResetProperty(wReturnStats, emType, emItemType);
}