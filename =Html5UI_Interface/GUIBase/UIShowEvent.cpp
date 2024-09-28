#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIShowEvent::CUIShowEvent(  UIID uiID
						   , UIEventType type
						   , bool bVisible
						   , double timestamp )
						   : CUIEvent( uiID, type, timestamp )
						   , m_bVisible( bVisible ) { }

CUIShowEvent::~CUIShowEvent() { }

}