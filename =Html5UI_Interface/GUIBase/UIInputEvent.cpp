#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIInputEvent::CUIInputEvent(  UIID uiID
							 , UIEventType type
							 , unsigned keyModifiers
							 , double timestamp )
							 : CUIEvent( uiID, type, timestamp )
							 , m_keyModifiers( keyModifiers ) { }

CUIInputEvent::~CUIInputEvent() { }

}