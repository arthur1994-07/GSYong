#include "stdafx.h"
#include "./OldUI/Util/UIColorSelectorWindow.h"
#include "./OldUI/Interface/QBoxButton.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


bool CInnerInterface::QBoxButtonGetEnable()
{
	if ( m_pQBoxButton )
		return m_pQBoxButton->GetQBoxEnable();

	return false;
}

void CInnerInterface::QBoxButtonSetEnable(bool bQBoxEnable)
{
	if ( m_pQBoxButton )
		m_pQBoxButton->SetQBoxEnable(bQBoxEnable);
}

const UIRECT& CInnerInterface::QBoxButtonGetGlobalPos() const
{
	static UIRECT sTempRect;
	if ( m_pQBoxButton )
		return m_pQBoxButton->GetGlobalPos();

	return sTempRect;
}

void CInnerInterface::CostumColorChangeWindowSetRestrict(bool bRestrict)
{
    m_pCostumColorChangeWindow->SetRestrict(bRestrict);
}
