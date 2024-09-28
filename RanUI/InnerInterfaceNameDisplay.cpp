#include "stdafx.h"
#include "./InnerInterface.h"
#include "./OldUI/Display/NameDisplayMan.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------
void CInnerInterface::NameDisplaySetCount( DWORD Count )
{
    if ( m_pNameDisplayMan )
        m_pNameDisplayMan->SET_COUNT( Count );
}

void CInnerInterface::NameDisplayAddName( CROWREN &sDISP_NAME, const TCHAR * szOwnerName )
{
    if ( m_pNameDisplayMan )
        m_pNameDisplayMan->ADD_DISP_NAME( sDISP_NAME, szOwnerName );
}

void CInnerInterface::NameDisplayShowCountryMark ( bool bShowCountry )
{
	if ( m_pNameDisplayMan )
		m_pNameDisplayMan->SetShowCountryMark( bShowCountry );
}