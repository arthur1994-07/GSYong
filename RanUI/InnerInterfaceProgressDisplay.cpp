#include "stdafx.h"

#include "./OldUI/Display/ProgressDisplay.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::ProgressDisplayShow ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText )
{
    m_pProgressDisplay->Show( emType, strText );
}


void CInnerInterface::ProgressDisplayShowPulblic( const std::string& strText )
{
	m_pProgressDisplay->ShowPublic( strText );
}