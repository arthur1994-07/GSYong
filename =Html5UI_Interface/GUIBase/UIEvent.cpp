#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIEvent::CUIEvent( UIID uiID, UIEventType type, double timestamp )
: m_uiID( uiID )
, m_type( type )
, m_timestamp( timestamp ) { }

CUIEvent::~CUIEvent() { }

}