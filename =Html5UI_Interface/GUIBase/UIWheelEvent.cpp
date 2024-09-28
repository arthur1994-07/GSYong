#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIWheelEvent::CUIWheelEvent(  UIID uiID
							 , UIEventType type
							 , unsigned keyModifiers
							 , const UIPOINT<int>& position
							 , const UIPOINT<int>& globalPosition
							 , float delta
							 , double timestamp )
							 : CUIInputEvent( uiID, type, keyModifiers, timestamp )
							 , m_position( position )
							 , m_globalPosition( globalPosition )
							 , m_delta( delta ) { }

CUIWheelEvent::~CUIWheelEvent() { }

}