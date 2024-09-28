#include "StdAfx.h"
#include "PartyLinkWindow.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPartyLinkWindow::CPartyLinkWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx ( pInterface, pEngineDevice )
	, m_pGaeaClient ( pGaeaClient )
	, m_nLinkIndex ( -1 )
{
	memset( m_pSectionTextBox, NULL, sizeof( CBasicTextBox* ) * MAXPARTY );
	memset( m_pClassTextBox, NULL, sizeof( CBasicTextBox* ) * MAXPARTY );
	memset( m_pNameTextBox, NULL, sizeof( CBasicTextBox* ) * MAXPARTY );
}


CPartyLinkWindow::~CPartyLinkWindow ()
{
}

void CPartyLinkWindow::SetVisibleSingle(BOOL visible)
{

	if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
	{
		if ( visible == TRUE)
		{
			SetVisibleSingle(FALSE);
			return;
		}
	}

	CUIGroup::SetVisibleSingle(visible);
}

void CPartyLinkWindow::CreateSubControl ()
{
	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "PARTY_LINK_WINDOW_BACK" );
		pBasicLineBox->ResetAlignFlag ();
		RegisterControl ( pBasicLineBox );		
	}

	CreateTextBox( "PARTY_LINK_SECTION_TEXT", ID2GAMEWORD( "PARTY_LINK_WINDOW", 1 ), NO_ID );
	CreateTextBox( "PARTY_LINK_CLASS_TEXT", ID2GAMEWORD( "PARTY_LINK_WINDOW", 2 ), NO_ID );
	CreateTextBox( "PARTY_LINK_NAME_TEXT", ID2GAMEWORD( "PARTY_LINK_WINDOW", 3 ), NO_ID );

	for ( int i = 0; i < MAXPARTY; ++i ) 
	{
		CString strID;
		strID.Format( "PARTY_LINK_SECTION_TEXT%d", i );
		m_pSectionTextBox[i] = CreateTextBox ( strID, "", NO_ID );
	}

	for ( int i = 0; i < MAXPARTY; ++i ) 
	{
		CString strID;
		strID.Format( "PARTY_LINK_CLASS_TEXT%d", i );
		m_pClassTextBox[i] = CreateTextBox ( strID, "", NO_ID );
	}

	for ( int i = 0; i < MAXPARTY; ++i ) 
	{
		CString strID;
		strID.Format( "PARTY_LINK_NAME_TEXT%d", i );
		m_pNameTextBox[i] = CreateTextBox ( strID, "", NO_ID );
	}

	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, PARTY_LINK_JOIN_BUTTON );
	pButton->CreateBaseButton( "PARTY_LINK_JOIN_BUTTON", CBasicTextButton::SIZE14, 
								CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "PARTY_LINK_WINDOW", 4 ) );
	RegisterControl( pButton );
	m_pJoinButton = pButton;
}

CBasicTextBox*	CPartyLinkWindow::CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID )
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox*	pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pTextBox->SetFont ( pFont );
	pTextBox->AddText ( strText );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pTextBox );
	return pTextBox;
}

void CPartyLinkWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case PARTY_LINK_JOIN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_sPartyLink.sPartyMem[0].VALID() ) break;

				// 어차피 인덱스를  검색해서 없으면 처리 안하기때문에 큰 의미가 없고 index가 필요 없는 처리가 필요하므로
				//				if ( m_nLinkIndex < 0 ) break;
				
				if ( m_sPartyLink.bOnce ) 
				{
					m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_ONCE") );
				}
				else
				{
					m_pGaeaClient->GetCharacter()->ReqPartyLinkJoin( m_sPartyLink.sPartyMem[0].dwGaeaID, m_nLinkIndex );				
				}

				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}

void CPartyLinkWindow::SetPartyLink( int nLinkIndex, const GLPARTY_LINK& sPartyLink )
{
	for ( int i = 0; i < MAXPARTY; ++i ) 
	{
		m_pSectionTextBox[i]->ClearText();
		m_pClassTextBox[i]->ClearText();
		m_pNameTextBox[i]->ClearText();
	}

	for ( int i = 0; i < MAXPARTY; ++i ) 
	{
		if ( !sPartyLink.sPartyMem[i].VALID() ) break;

		CString strText;
		if ( i == 0 )	strText = ID2GAMEWORD( "PARTY_LINK_SECTION",0 );
		else strText = ID2GAMEWORD( "PARTY_LINK_SECTION",1 );

		m_pSectionTextBox[i]->SetText( strText );
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			m_pClassTextBox[i]->SetText(CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(static_cast<EMCHARCLASS> (sPartyLink.sPartyMem[i].emClass))).GetString());
		}
		else
		{
			m_pClassTextBox[i]->SetText(
				COMMENT::CHARCLASS[CharClassToIndex(static_cast<EMCHARCLASS> (sPartyLink.sPartyMem[i].emClass))].c_str());
		}
		m_pNameTextBox[i]->SetText(sPartyLink.sPartyMem[i].m_Name);
	}

	m_sPartyLink = sPartyLink;
	
	m_nLinkIndex = nLinkIndex;
}
