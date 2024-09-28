#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

CUIResizeEvent::CUIResizeEvent(  UIID uiID
							   , UIEventType type
							   , const UISIZE<float>& size
							   , const UISIZE<float>& oldSize
							   , double timestamp )
							   : CUIEvent( uiID, type, timestamp )
							   , m_size( size )
							   , m_oldSize( oldSize ) { }

CUIResizeEvent::~CUIResizeEvent() { }

}