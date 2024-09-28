#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

	CUIKeyEvent::CUIKeyEvent(  UIID uiID
		, UIEventType type
		, unsigned keyModifiers
		, const std::wstring& text
		, int key
		, int nativeVirtualKeyCode
		, double timestamp )
		: CUIInputEvent( uiID, type, keyModifiers, timestamp )
		, m_text( text )
		, m_key( key )
		, m_nativeVirtualKeyCode( nativeVirtualKeyCode ) { }

	CUIKeyEvent::~CUIKeyEvent() { }

}