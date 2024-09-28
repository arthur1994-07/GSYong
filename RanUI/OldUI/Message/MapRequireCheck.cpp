#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../InnerInterface.h"

#include "MapRequireCheck.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CMapRequireCheck::fDEFAULT_TIME_LEFT = 5.0f;

CMapRequireCheck::CMapRequireCheck(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindow(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTextBox(NULL)
    , m_pButton(NULL)
    , m_fLeftTime(0.0f)
    , m_emMapCheckType(UI::EMMAPCHECK_NONE)
    , m_bActive(false)
{
}

CMapRequireCheck::~CMapRequireCheck(void)
{
}

void CMapRequireCheck::CreateSubControl()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer( "MAP_REQUIRE_CHECK_LINE_BOX" );
	RegisterControl( pLineBox );	

	m_pButton = CreateTextButton( "MAP_REQUIRE_CHECK_BUTTON", MAP_REQUIRE_CHECK_BUTTON, (char*)ID2GAMEWORD( "GAMBLE_OKCANCEL" ) );
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub( this, "MAP_REQUIRE_CHECK_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont( pFont9 );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
	pTextBox->SetPartInterval( 5.0f );
	RegisterControl( pTextBox );
	m_pTextBox = pTextBox;

}


CBasicTextButton* CMapRequireCheck::CreateTextButton( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );
	return pButton;
}


void CMapRequireCheck::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MAP_REQUIRE_CHECK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				if( pCharacter == NULL )
					return;

				GLMSG::SNETPC_REQ_MUST_LEAVE_MAP NetMsg;
				switch( m_emMapCheckType )
				{
				case UI::EMMAPCHECK_LEVELUP:
				case UI::EMMAPCHECK_ENTRYFAIL:
				case UI::EMMAPCHECK_EXPEDITION:
				    {
						NetMsg.sMAPID   = GLCONST_CHAR::nidSTARTMAP[pCharacter->m_wSchool];
						NetMsg.dwGATEID = GLCONST_CHAR::dwSTARTGATE[pCharacter->m_wSchool];
						m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
					}
				case UI::EMMAPCHECK_LIMITTIME:
					{
						if( m_pGaeaClient->IsInstantMap() )						
						{
							GLMSG::SNETREQ_INSTANTMAP_MOVEOUT_REQ NetMsgMoveOut;
							m_pGaeaClient->NETSENDTOFIELD(&NetMsgMoveOut);
						}						
						else
						{
							GLMSG::SNETREQ_GATEOUT_REQ NetMsgGateOut;
							NetMsgGateOut.dwGateID = 0;
							m_pGaeaClient->NETSENDTOFIELD(&NetMsgGateOut);
						}

						pCharacter->SetActState(EM_REQ_GATEOUT);						
					}						
					break;
				}

                m_bActive = false;
                m_pInterface->UiHideGroup( GetWndID() );
				
			}
		}
		break;
	}
}

void CMapRequireCheck::SetVisibleSingle( BOOL bVisible )
{
    //  동작중일땐 절대 닫기지 않게 한다.
    if ( !bVisible )
    {
        if ( m_bActive ) return;
    }

	CUIGroup::SetVisibleSingle( bVisible );
}

void CMapRequireCheck::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible() )
		return ;
    if ( m_emMapCheckType == UI::EMMAPCHECK_NONE )
		return;

	m_fLeftTime -= fElapsedTime;

	switch( m_emMapCheckType )
	{
	case UI::EMMAPCHECK_LEVELUP:
		m_pTextBox->SetText( ID2GAMEINTEXT("LEVELUP_MAP_REQUIRE_MSG") );
		break;
	case UI::EMMAPCHECK_ENTRYFAIL:	
		m_pTextBox->SetText( ID2GAMEINTEXT("REGEN_ENTRY_FAIL") );	
		break;
	case UI::EMMAPCHECK_LIMITTIME:
		m_pTextBox->SetText( ID2GAMEINTEXT("LIMITTIME_OVER") );	
		break;
	case UI::EMMAPCHECK_EXPEDITION:
		m_pTextBox->SetText( ID2GAMEINTEXT("NOT_IN_EXPEDITION") );	
		break;
	}

	CString strLeftTime;
	strLeftTime.Format( "%1.0f%s", floor(m_fLeftTime) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));		
	m_pTextBox->AddText( strLeftTime );

	if( m_fLeftTime < 0.0f )
	{		
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if( pCharacter == NULL )
			return;

		switch( m_emMapCheckType )
		{
		case UI::EMMAPCHECK_LEVELUP:		
		case UI::EMMAPCHECK_ENTRYFAIL:		
			{
				GLMSG::SNETPC_REQ_MUST_LEAVE_MAP NetMsg;
				NetMsg.sMAPID   = GLCONST_CHAR::nidSTARTMAP[pCharacter->m_wSchool];
				NetMsg.dwGATEID = GLCONST_CHAR::dwSTARTGATE[pCharacter->m_wSchool];	
				NetMsg.emReason = EMREQ_MUST_LEAVE_REASON_LEVEL;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}
			break;
		case UI::EMMAPCHECK_EXPEDITION:
			{
				GLMSG::SNETPC_REQ_MUST_LEAVE_MAP NetMsg;
				NetMsg.sMAPID   = GLCONST_CHAR::nidSTARTMAP[pCharacter->m_wSchool];
				NetMsg.dwGATEID = GLCONST_CHAR::dwSTARTGATE[pCharacter->m_wSchool];	
				NetMsg.emReason = EMREQ_MUST_LEAVE_REASON_EXPEDITION;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				break;
			}
		case UI::EMMAPCHECK_LIMITTIME:
			if( m_pGaeaClient->IsInstantMap() )
			{
				GLMSG::SNETREQ_INSTANTMAP_MOVEOUT_REQ NetMsgMoveOut;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsgMoveOut);
			}			
			else
			{
				GLMSG::SNETREQ_GATEOUT_REQ NetMsgGateOut;
				NetMsgGateOut.dwGateID = 0;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsgGateOut);
			}
			
			pCharacter->SetActState(EM_REQ_GATEOUT);
			break;
		}

        m_bActive = false;
        m_pInterface->UiHideGroup( GetWndID() );
	}

	CUIWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}

void CMapRequireCheck::SetMapCheckType( UI::EMMAPCHECK_TYPE emMapCheckType )
{ 
	//  [6/1/2014 gbgim];
	// 해당 UI는 UIHideGroup, EMMAPCHECK_NONE으로 설정을 해도;
	// m_bActive가 TRUE라면 위의 조건을 다무시해버린다;
	if ( emMapCheckType == UI::EMMAPCHECK_NONE )
	{
		m_bActive = false;
		m_pInterface->UiHideGroup( GetWndID() );
	}
	else
	{
		m_fLeftTime = fDEFAULT_TIME_LEFT;
		m_emMapCheckType = emMapCheckType;
		m_bActive = true;
	}
}

MyMapRequireCheck::MyMapRequireCheck( GLGaeaClient* pGaeaClient, CInnerInterface* pInerface, EngineDeviceMan* pEngineDevice )
    : CMapRequireCheck( pGaeaClient, pInerface, pEngineDevice )
{
}

void MyMapRequireCheck::CreateUIWindowAndRegisterOwnership()
{
	CMapRequireCheck::Create( MAP_REQUIRE_CHECK, "MAP_REQUIRE_CHECK_DIALOGUE", UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
    CMapRequireCheck::CreateSubControl();
    CMapRequireCheck::m_pInterface->UiRegisterControl( this );
    CMapRequireCheck::m_pInterface->UiShowGroupFocus( MAP_REQUIRE_CHECK );
}

void MyMapRequireCheck::SetMapCheckType( UI::EMMAPCHECK_TYPE emMapCheckType )
{
    CMapRequireCheck::SetMapCheckType( emMapCheckType );
}