#include "pch.h"
#include "UIMessageQueue.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIMessageQueue::CUIMessageQueue ()
{
}

CUIMessageQueue::~CUIMessageQueue ()
{
}

void CUIMessageQueue::PostUIMessage ( UIGUID cID, DWORD dwMsg )
{
	SUIMSG uiMsg;
	uiMsg.cID = cID;
	uiMsg.dwMsg = dwMsg;

	m_MessageQueue.push( uiMsg );
}

bool CUIMessageQueue::PeekUIMessage ( UIGUID* pcID, DWORD* pdwMsg )
{
	if ( !pcID || !pdwMsg ) return false;
	if ( m_MessageQueue.empty () ) return false;

	const SUIMSG& uiMsg = m_MessageQueue.front ();
	*pcID = uiMsg.cID;
	*pdwMsg = uiMsg.dwMsg;

	m_MessageQueue.pop ();

	return true;
}