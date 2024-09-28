#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

	CUIFocusEvent::CUIFocusEvent(  UIID uiID
		, UIEventType type
		, bool bFocusable
		, double timestamp )
		: CUIEvent( uiID, type, timestamp )
		, m_bFocusable( bFocusable ) { }

	CUIFocusEvent::~CUIFocusEvent() { }

}