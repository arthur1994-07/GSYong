#include "StdAfx.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../OuterInterface.h"

#include "ModalWindowOuter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CModalWindowOuter::CModalWindowOuter ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CModalWindow(pEngineDevice)
    , m_pInterface ( pInterface )
{

}

CModalWindowOuter::~CModalWindowOuter()
{

}

void CModalWindowOuter::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CModalWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MODAL_YES:
	case MODAL_OK:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( IsEditBoxType() ) // MEMO : 에디터 박스 타입의 모달이면...
				{
                    DXInputString* pInputString = m_pEngineDevice->GetInputString();
					if (CHECK_KEYFOCUSED(dwMsg) && !pInputString->CheckEnterKeyDown())
						return;
				}

				switch( m_nModalType )
				{
				case UI::MODAL_EDITBOX_LOTTERY:
					{
						if( m_pEditBox->GetEditLength() != 10 )
							return ;
					}
					break;

				case UI::EDITBOX_NUMBER:	m_pEditBox->DoUSE_NUMBER( false );	break;
				case UI::EDITBOX_MONEY:		m_pEditBox->SET_MONEY_FORMAT( 0 );	break;
				}

				m_pEditBox->EndEdit ();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx ( dwMsg | UIMSG_MODAL_OK );

				if ( m_bTEST_MULTI )
				{
					m_pInterface->CLOSE_MODAL ( GetWndID (), true );
					return ;
				}

				m_pInterface->UiHideGroup ( GetWndID (), true );				
			}
		}
		break;

	case MODAL_NO:
	case MODAL_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				switch( m_nModalType )
				{
				case UI::EDITBOX_NUMBER:	m_pEditBox->DoUSE_NUMBER( false );	break;
				case UI::EDITBOX_MONEY:		m_pEditBox->SET_MONEY_FORMAT( 0 );	break;
				}

				m_pEditBox->EndEdit ();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx ( dwMsg | UIMSG_MODAL_CANCEL );
				// AddMessageEx ( UIMSG_MODAL_CANCEL );			

				if ( m_bTEST_MULTI )
				{
					m_pInterface->CLOSE_MODAL ( GetWndID (), true );
					return ;
				}

				m_pInterface->UiHideGroup ( GetWndID (), true );
			}
		}
		break;
	}
}

void CModalWindowOuter::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CModalWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pEditBox->IsVisible() && m_pEditBox->IsBegin() )
	{
		static bool bVALID_INPUT_BACK = true;

		if ( bVALID_INPUT_BACK && !m_pEditBox->IsVALID_INPUT () )
		{
			// OutInterface 출력이 없음
			//m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVALID_KEYBOARD_INPUT") );
		}

		bVALID_INPUT_BACK = m_pEditBox->IsVALID_INPUT ();
	}
}
