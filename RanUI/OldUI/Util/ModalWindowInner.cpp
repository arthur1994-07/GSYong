#include "StdAfx.h"

#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "ModalWindowInner.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CModalWindowInner::CModalWindowInner( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CModalWindow(pEngineDevice)
    , m_pInterface( pInterface )
{

}

CModalWindowInner::~CModalWindowInner()
{

}

void CModalWindowInner::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIModal::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MODAL_YES:
	case MODAL_OK:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if (IsEditBoxType()) // MEMO : 에디터 박스 타입의 모달이면...
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

				m_pEditBox->EndEdit();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx( dwMsg | UIMSG_MODAL_OK );

				if ( m_bTEST_MULTI )
				{
					m_pInterface->CLOSE_MODAL( GetWndID(), true );
					return ;
				}

				m_pInterface->UiHideGroup( GetWndID(), true );		
			}
		}
		break;

	case MODAL_NO:
	case MODAL_CANCEL:
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				switch( m_nModalType )
				{
				case UI::EDITBOX_NUMBER:	m_pEditBox->DoUSE_NUMBER( false );	break;
				case UI::EDITBOX_MONEY:		m_pEditBox->SET_MONEY_FORMAT( 0 );	break;
				}

				m_pEditBox->EndEdit();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx( dwMsg | UIMSG_MODAL_CANCEL );
				// AddMessageEx( UIMSG_MODAL_CANCEL );			

				if ( m_bTEST_MULTI )
				{
					m_pInterface->CLOSE_MODAL( GetWndID(), true );
					return ;
				}

				m_pInterface->UiHideGroup( GetWndID(), true );
			}
		}
		break;
	}
}

void	CModalWindowInner::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() ) return ;

	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pEditBox->IsVisible() && m_pEditBox->IsBegin() )
	{
		static bool bVALID_INPUT_BACK = true;

		if ( bVALID_INPUT_BACK && !m_pEditBox->IsVALID_INPUT() )
		{
			m_pInterface->GetGaeaClient()->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVALID_KEYBOARD_INPUT") );
		}

		bVALID_INPUT_BACK = m_pEditBox->IsVALID_INPUT();
	}
}

MyModalWindow::MyModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CModalWindowInner( pInterface, pEngineDevice )
{
}

void MyModalWindow::CreateUIWindowAndRegisterOwnership()
{
    CModalWindowInner::Create( MODAL_WINDOW, "BASIC_MODAL_WINDOW", UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
    CModalWindowInner::CreateSubControl();
    CModalWindowInner::m_pInterface->UiRegisterControl( this );
    CModalWindowInner::m_pInterface->UiShowGroupFocus( MODAL_WINDOW );
}

void MyModalWindow::SetHide( BOOL bHide )
{
    CModalWindowInner::SetHide( bHide );
}

void MyModalWindow::DoModal( const std::string& strText, int nModalTitle, int nModalType, UIGUID CallerID )
{
    CModalWindowInner::DoModal( strText, nModalTitle, nModalType, CallerID );
}

void MyModalWindow::DoModalTipBox( const std::string& strText, const std::string& strTip,
                                  int nModalTitle, int nModalType, UIGUID CallerID )
{
    CModalWindowInner::DoModalTipBox( strText, strTip, nModalTitle, nModalType, CallerID );
}

void MyModalWindow::SetModalData( const D3DXVECTOR3& vData )
{
    CModalWindowInner::SetModalData( vData );
}

void MyModalWindow::SetModalData( int nData1, int nData2 )
{
    CModalWindowInner::SetModalData( nData1, nData2 );
}

void MyModalWindow::GetModalData( int* pnData1, int* pnData2 )
{
    CModalWindowInner::GetModalData( pnData1, pnData2 );
}

void MyModalWindow::GetModalData( D3DXVECTOR3* vData )
{
    CModalWindowInner::GetModalData( vData );
}

bool MyModalWindow::IsUseModaless()
{
    return CModalWindowInner::IsUseModaless();
}

UI::String MyModalWindow::GetEditString()
{
    return UI::ToString( CModalWindowInner::GetEditString() );
}

UIGUID MyModalWindow::GetCallerID()
{
    return CModalWindowInner::GetCallerID();
}