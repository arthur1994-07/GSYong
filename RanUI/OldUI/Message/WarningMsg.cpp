#include "StdAfx.h"
#include "WarningMsg.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWarningMsg::CWarningMsg(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pControl(NULL)
	, m_TotElapsedTime(0.0f)
{
}

CWarningMsg::~CWarningMsg ()
{
	m_TotElapsedTime = 0.0f;
}

void CWarningMsg::CreateSubControl()
{
	CUIControl * pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, "WARNING_MSG" );
	RegisterControl( pControl );
	m_pControl = pControl;
}

void CWarningMsg::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_TotElapsedTime += fElapsedTime;

	if( m_pControl->IsVisible() )
	{
		if( m_TotElapsedTime > 1.5f )
		{
			m_pControl->SetVisibleSingle( FALSE );
			m_TotElapsedTime = 0.0f;
		}
	}
	else
	{
		if( m_TotElapsedTime > 0.5f )
		{
			m_pControl->SetVisibleSingle( TRUE );
			m_TotElapsedTime = 0.0f;
		}
	}
}