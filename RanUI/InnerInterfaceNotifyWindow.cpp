#include "stdafx.h"
#include "./InnerInterface.h"
#include "./OldUI/NotifyWindow/NotifyButton.h"
#include "./OldUI/NotifyWindow/NotifyWindow.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::NotifyWindowSetNewAlarm()
{
    if( m_pNotifyButton != NULL )
	    m_pNotifyButton->SetNotifyAlarm();

    if( m_pNotifyWindow != NULL)
        m_pNotifyWindow->NewMessage();
}