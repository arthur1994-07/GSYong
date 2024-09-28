#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIMouseEvent::CUIMouseEvent(  UIID uiID
							 , UIEventType type
							 , unsigned keyModifiers
							 , const UIPOINT<int>& position
							 , const UIPOINT<int>& globalPosition
							 , Button& button
							 , float deltaX
							 , float deltaY
							 , float deltaZ
							 , double timestamp )
							 : CUIInputEvent( uiID, type, keyModifiers, timestamp )
							 , m_position( position )
							 , m_globalPosition( globalPosition )
							 , m_button( button )
							 , m_deltaX( deltaX )
							 , m_deltaY( deltaY )
							 , m_deltaZ( deltaZ ) { }

CUIMouseEvent::~CUIMouseEvent() { }

}