#include "stdafx.h"
#include "./OldUI/Tutorial/MouseTip.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


void CInnerInterface::MouseTipResizeControl(const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos)
{
    m_pMouseTip->ReSizeControl(rcParentOldPos, rcParentNewPos);
}

const UIRECT& CInnerInterface::MouseTipGetGlobalPos() const
{
    return m_pMouseTip->GetGlobalPos();
}