#include "StdAfx.h"
#include "SubMasterSet.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tool/DxParamSet.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSubMasterSet::CSubMasterSet(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindow(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pJoinButton(NULL)
	, m_pStrikeButton(NULL)
	, m_pNoticeButton(NULL)
	, m_pMarkButton(NULL)
	, m_pCertifyButton(NULL)
	, m_pTextBox(NULL)
	, m_pTextCDM(NULL)
	, m_dwCharID(CLUB_NULL)
	, m_pMyClub(NULL)
	, m_dwClubFlag(0)
{
}

CSubMasterSet::~CSubMasterSet ()
{
}

void CSubMasterSet::CreateSubControl ()
{
	CD3DFontPar* m_pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// 뒷 배경
	{
		CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "SUBMASTER_LINE_BOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

		pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "SUBMASTER_LINETEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
	}

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub( this, "SUBMASTER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont( m_pFont9 );
	RegisterControl( pTextBox );
	m_pTextBox = pTextBox;

	{
		CBasicTextBox* pTextBox = NULL;

		pTextBox = CreateStaticControl( "SUBMASTER_JOIN_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 1 ) );

		pTextBox = CreateStaticControl( "SUBMASTER_STRIKE_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 2 ) );

		pTextBox = CreateStaticControl( "SUBMASTER_NOTICE_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 3 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_MARK_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 4 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_CERTIFY_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 5 ) );

		pTextBox = CreateStaticControl ( "SUBMASTER_CDM_STATIC", m_pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText( ID2GAMEWORD( "SUBMASTER_OPTION", 6 ) );
		m_pTextCDM = pTextBox;
	}

	{
		m_pJoinButton = CreateFlipButton( "SUBMASTER_JOIN_BUTTON", "SUBMASTER_JOIN_BUTTON_F", SUBMASTER_JOIN_BUTTON );
		m_pStrikeButton = CreateFlipButton( "SUBMASTER_STRIKE_BUTTON", "SUBMASTER_STRIKE_BUTTON_F", SUBMASTER_STRIKE_BUTTON );
		m_pNoticeButton = CreateFlipButton( "SUBMASTER_NOTICE_BUTTON", "SUBMASTER_NOTICE_BUTTON_F", SUBMASTER_NOTICE_BUTTON );
		m_pMarkButton = CreateFlipButton( "SUBMASTER_MARK_BUTTON", "SUBMASTER_MARK_BUTTON_F", SUBMASTER_MARK_BUTTON );
		m_pCertifyButton = CreateFlipButton( "SUBMASTER_CERTIFY_BUTTON", "SUBMASTER_CERTIFY_BUTTON_F", SUBMASTER_CERTIFY_BUTTON );
		m_pCDMButton = CreateFlipButton( "SUBMASTER_CDM_BUTTON", "SUBMASTER_CDM_BUTTON_F", SUBMASTER_CDM_BUTTON );
	}

	{
		// 확인, 취소
		CBasicTextButton* pButton = NULL;
		pButton = CreateTextButton( "SUBMASTER_OK", SUBMASTER_OK, (char*)ID2GAMEWORD( "PRIVATE_MARKET_MAKE_BUTTON", 0 ) ); // 개인상점의 확인,취소를 그대로 사용
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		pButton = CreateTextButton( "SUBMASTER_CANCEL", SUBMASTER_CANCEL, (char*)ID2GAMEWORD( "PRIVATE_MARKET_MAKE_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
	}
}

CBasicButton* CSubMasterSet::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CSubMasterSet::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

CBasicTextButton* CSubMasterSet::CreateTextButton( char* szButton, UIGUID ControlID , char* szText )
{
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );
	return pButton;
}

void CSubMasterSet::LoadJoin()
{	
	bool bJoin = m_pMyClub->HaveMemberJoinAuthority(m_dwCharID);
	m_pJoinButton->SetFlip( bJoin );
	SetCheck_Flags(bJoin, m_dwClubFlag, club::AUTHORITY_MEMBER_JOIN);
}

void CSubMasterSet::LoadStrike()
{
	bool bStrike = m_pMyClub->HaveMemberKickAuthority( m_dwCharID );
	m_pStrikeButton->SetFlip( bStrike );
	SetCheck_Flags( bStrike, m_dwClubFlag, club::AUTHORITY_MEMBER_KICK );
}

void CSubMasterSet::LoadNotice()
{
	bool bNotice = m_pMyClub->IsMemberFlgNotice( m_dwCharID );
	m_pNoticeButton->SetFlip( bNotice );
	SetCheck_Flags( bNotice, m_dwClubFlag, club::AUTHORITY_NOTICE_CHANGE );
}

void CSubMasterSet::LoadMark()
{
	bool bMark = m_pMyClub->IsMemberFlgMarkChange( m_dwCharID );
	m_pMarkButton->SetFlip( bMark );
	SetCheck_Flags( bMark, m_dwClubFlag, club::AUTHORITY_MARK_CHANGE );
}

void CSubMasterSet::LoadCertify()
{
	bool bCertify = m_pMyClub->IsMemberFlgCDCertify( m_dwCharID );
	m_pCertifyButton->SetFlip( bCertify );
	SetCheck_Flags( bCertify, m_dwClubFlag, club::AUTHORITY_CD );
}

void CSubMasterSet::LoadClubDeathMatch()
{
	if ( !GLCONST_CHAR::bCLUB_DEATHMATCH ) return;
	
	bool bCDM = m_pMyClub->IsMemberFlgCDM( m_dwCharID );
	m_pCDMButton->SetFlip( bCDM );
	SetCheck_Flags( bCDM, m_dwClubFlag, club::AUTHORITY_DEATH_MATCH );
}

void CSubMasterSet::LoadCurrentOption( DWORD dwCharID ) // 보이기 전에 반드시 호출되어야 한다.
{
	m_pMyClub = m_pGaeaClient->GetMyClub();
	m_dwCharID = dwCharID;

	LoadJoin();
	LoadStrike();
	LoadNotice();
	LoadMark();
	LoadCertify();
	LoadClubDeathMatch();
}

void CSubMasterSet::SetSubMasterName( const CString& strName )
{
	CString strMessage;
	strMessage.Format( "%s%s", strName, ID2GAMEWORD( "SUBMASTER_OPTION", 0 ) );

	m_pTextBox->SetText( strMessage );
}

void CSubMasterSet::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		if ( !GLCONST_CHAR::bCLUB_DEATHMATCH )
		{
			m_pCDMButton->SetVisibleSingle( FALSE );		
			m_pTextCDM->SetVisibleSingle( FALSE );
		}
	}
}

void CSubMasterSet::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SUBMASTER_JOIN_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bJoin = GetCheck_Flags(m_dwClubFlag, club::AUTHORITY_MEMBER_JOIN);
					SetCheck_Flags(!bJoin, m_dwClubFlag, club::AUTHORITY_MEMBER_JOIN);
					m_pJoinButton->SetFlip( !bJoin );
				}
			}
		}
		break;

	case SUBMASTER_STRIKE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bStrike = GetCheck_Flags( m_dwClubFlag, club::AUTHORITY_MEMBER_KICK );
					SetCheck_Flags( !bStrike, m_dwClubFlag, club::AUTHORITY_MEMBER_KICK );
					m_pStrikeButton->SetFlip( !bStrike );
				}
			}
		}
		break;

	case SUBMASTER_NOTICE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bNotice = GetCheck_Flags( m_dwClubFlag, club::AUTHORITY_NOTICE_CHANGE );
					SetCheck_Flags( !bNotice, m_dwClubFlag, club::AUTHORITY_NOTICE_CHANGE );
					m_pNoticeButton->SetFlip( !bNotice );
				}
			}
		}
		break;

	case SUBMASTER_MARK_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bMark = GetCheck_Flags( m_dwClubFlag, club::AUTHORITY_MARK_CHANGE );
					SetCheck_Flags( !bMark, m_dwClubFlag, club::AUTHORITY_MARK_CHANGE );
					m_pMarkButton->SetFlip( !bMark );
				}
			}
		}
		break;

	case SUBMASTER_CERTIFY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{	
					bool bCertify = GetCheck_Flags( m_dwClubFlag, club::AUTHORITY_CD );
					SetCheck_Flags( !bCertify, m_dwClubFlag, club::AUTHORITY_CD );
					m_pCertifyButton->SetFlip( !bCertify );
				}
			}
		}
		break;

	case SUBMASTER_CDM_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					bool bCDM = GetCheck_Flags( m_dwClubFlag, club::AUTHORITY_DEATH_MATCH );
					SetCheck_Flags( !bCDM, m_dwClubFlag, club::AUTHORITY_DEATH_MATCH );
					m_pCDMButton->SetFlip( !bCDM );
				}
			}
		}
		break;

	case SUBMASTER_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	Note : 클럽 부마 설정.
				//GLCharacter *pCharacter = m_pGaeaClient->GetCharacter();
				//pCharacter->ReqClubSubMaster( m_dwCharID, m_dwClubFlag );

				m_pInterface->UiHideGroup( GetWndID (), true );
			}
		}
		break;

	case SUBMASTER_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID(), true );				
			}
		}
		break;
	}
}
