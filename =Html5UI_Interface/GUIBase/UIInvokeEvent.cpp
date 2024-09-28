#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{
	// Invoke
	CUIInvokeEvent::CUIInvokeEvent( UIID uiID
		, UIEventType type
		, const std::wstring& message
		, double timestamp )
		: CUIEvent( uiID, type, timestamp )
		, m_message( message ) { }

	CUIInvokeEvent::~CUIInvokeEvent() { }

	// ArrayInvoke
	CUIArrayInvokeEvent::CUIArrayInvokeEvent( UIID uiID
		, UIEventType type
		, unsigned int length
		, double timestamp )
		: CUIEvent( uiID, type, timestamp )
		, m_length( length )
	{
		m_messages = new std::wstring[ length ];
	}

	CUIArrayInvokeEvent::~CUIArrayInvokeEvent()
	{
		SAFE_DELETE_ARRAY( m_messages );
	}

	void CUIArrayInvokeEvent::AddInvokeMessage(
		const std::wstring& message, unsigned int index )
	{
		if( index >= 0 && index < m_length )
			m_messages[ index ] = message;
	}

}