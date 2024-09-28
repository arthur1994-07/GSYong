#include "StdAfx.h"
#include "IWindow.h"
#include "IWebView.h"
#include "UIEventManager.h"
#include "UIWindowManager.h"

namespace GUIBase
{

void IWindow::EventProc()
{
	// Event 존재 여부 확인;
	while( !m_pEventQueue.empty() )
	{
		// 발생된 Event와 Event를 발생시킨 Component를 파악;
		std::tr1::shared_ptr< CUIEvent > curEvent;
		curEvent = m_pEventQueue.front();
		m_pEventQueue.pop();

		// ID 검사;
		if( curEvent->GetUIID() != m_uiID )
			continue;

		// 각 Event에 따른 처리;
		switch( curEvent->GetEventType() )
		{
		case CUIEvent::UI_SetFocus:
		case CUIEvent::UI_KillFocus:
			OnFocus( std::tr1::static_pointer_cast<CUIFocusEvent>( curEvent ) );
			break;
		case CUIEvent::UI_Size:
			OnSize( std::tr1::static_pointer_cast<CUIResizeEvent>( curEvent ) );
			break;
		case CUIEvent::UI_KeyDown:
		case CUIEvent::UI_KeyUp:
		case CUIEvent::UI_Char:
			OnKeyEvent( std::tr1::static_pointer_cast<CUIKeyEvent>( curEvent ) );
			break;
		case CUIEvent::UI_MouseDown:
		case CUIEvent::UI_MouseMove:
		case CUIEvent::UI_MouseUp:
			OnMouseEvent( std::tr1::static_pointer_cast<CUIMouseEvent>( curEvent ) );
			break;
		case CUIEvent::UI_Wheel:
			OnWheelEvent( std::tr1::static_pointer_cast<CUIWheelEvent>( curEvent ) );
			break;
		case CUIEvent::UI_ShowWindow:
			OnShow( std::tr1::static_pointer_cast<CUIShowEvent>( curEvent ) );
			break;
		case CUIEvent::UI_HideWindow:
			OnHide( std::tr1::static_pointer_cast<CUIShowEvent>( curEvent ) );
			break;
		case CUIEvent::UI_Invoke:
			OnInvokeEvent( std::tr1::static_pointer_cast<CUIInvokeEvent>( curEvent ) );
			break;
		}	
	}

	FireArrayInvokeEvent();
}

}