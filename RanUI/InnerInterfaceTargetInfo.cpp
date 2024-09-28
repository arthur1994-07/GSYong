#include "stdafx.h"
#include "./OldUI/Display/TargetInfoDisplay.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

const CString CInnerInterface::TargetInfoGetMouseOverName() const
{
    const CString strTargetName = UI::ToString( m_pTargetInfoDisplay->GetMouseOverTargetName() );
    return strTargetName;
}