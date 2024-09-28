#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../Util/RnButton.h"
//#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/D3DFontX.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "RnCDMClubInfo.h"

namespace
{
	const char* szButtonNormal = "RNBUTTON_DEFAULT_TEXTURE";
	const char* szButtonOver = "RNBUTTON_OVER_TEXTURE";
	const char* szButtonDown = "RNBUTTON_CLICK_TEXTURE";
}

RnCDMClubInfo::RnCDMClubInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
:CUIWindowEx( pInterface, pEngineDevice )
, m_pInterface(pInterface)
{

}

RnCDMClubInfo::~RnCDMClubInfo()
{

}

void RnCDMClubInfo::CreateSubControl()
{
	SetTitleName("_(0/8)");
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	m_pBGLine = new CBasicLineBoxSmart(m_pEngineDevice);
	m_pBGLine->CreateSub( this, "RNCDM_CLUB_INFO_BG_POS" );
	m_pBGLine->CreateSubControl( "RNCDM_CLUB_INFO_BG" );
	RegisterControl( m_pBGLine );

	{
		CBasicTextBox* pClass = new CBasicTextBox(m_pEngineDevice);
		pClass->CreateSub ( this, "RNCDM_CLUB_MEMBER_CLASS_ROW", UI_FLAG_DEFAULT );
		pClass->SetFont ( pFont );
		pClass->SetTextAlign ( UI_FLAG_LEFT | UI_FLAG_CENTER_Y );	
		RegisterControl ( pClass );
		pClass->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_INFO_CLASS"), NS_UITEXTCOLOR::WHITE );
	}

	{
		CBasicTextBox* pMemberName = new CBasicTextBox(m_pEngineDevice);
		pMemberName->CreateSub ( this, "RNCDM_CLUB_MEMBER_NAME_ROW", UI_FLAG_DEFAULT );
		pMemberName->SetFont ( pFont );
		pMemberName->SetTextAlign ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );	
		RegisterControl ( pMemberName );
		pMemberName->SetOneLineText( ID2GAMEWORD("RN_CDM_CLUB_INFO_CHARACTER_NAME"), NS_UITEXTCOLOR::WHITE );
	}

	for (int i = 0; i < nMaxMember; i++ )
	{
		m_pMemberImage[i] = new CSwapImage(m_pEngineDevice);
		m_pMemberImage[i]->CreateSub ( this, sc::string::format( "RNCDM_CLUB_MEMBER_IMAGE_%d", i).c_str(), UI_FLAG_DEFAULT );
		RegisterControl ( m_pMemberImage[i] );
		//m_pMemberImage[i]->SetImage( "CREATE_CHAR_CLASS_BRAWLER_MALE" );
	}

	for (int i = 0; i < nMaxMember; i++ )
	{
		m_pMemberName[i] = new CBasicTextBox(m_pEngineDevice);
		m_pMemberName[i]->CreateSub ( this, sc::string::format( "RNCDM_CLUB_MEMBER_NAME_%d", i).c_str(), UI_FLAG_DEFAULT );
		m_pMemberName[i]->SetFont ( pFont );
		m_pMemberName[i]->SetTextAlign ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );	
		RegisterControl ( m_pMemberName[i] );
		//m_pMemberName[i]->SetOneLineText( "ÀÏÀÌ»ï»ç¿ÀÀ°Ä¥ÆÈ", NS_UITEXTCOLOR::WHITE );
	}

	//°Ë»ö ¹öÆ°
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",ID2GAMEWORD("CLOSE_BUTTON", 0));

		m_pCloseButton = new RnButton(m_pEngineDevice);
		m_pCloseButton->CreateSub(this, "RNCDM_CLUB_INFO_CLOSE_BUTTON", UI_FLAG_RIGHT, RNCDM_CLUB_INFO_CLOSE_BUTTON );
		m_pCloseButton->CreateSubControl(arg);
		RegisterControl(m_pCloseButton);
	}
}

void RnCDMClubInfo::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnCDMClubInfo::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch (cID)
	{
	case RNCDM_CLUB_INFO_CLOSE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			SetVisibleSingle(FALSE);
		}
		break;
	}
}

void RnCDMClubInfo::SetMemberImage( int nIndex, EMCHARINDEX emCharIndex )
{
	if ( nIndex >= nMaxMember )
		return;

	for ( int i = nIndex; i < nMaxMember; i++ )
	{
		m_pMemberImage[i]->SetVisibleSingle(FALSE);
	}


	//sc::string::format( "RNCDM_CLUB_MEMBER_IMAGE_%d", m_pMemberImage->si);

	//SetTitleName("_(0/8)");

	m_pMemberImage[nIndex]->SetVisibleSingle(TRUE);

	static const std::string strClass[GLCI_NUM_ETC] = 
	{
		"CREATE_CHAR_CLASS_BRAWLER_MALE",
		"CREATE_CHAR_CLASS_SWORDMAN_MALE",
		"CREATE_CHAR_CLASS_ARCHER_FEMALE",
		"CREATE_CHAR_CLASS_SHAMAN_FEMALE",
		"CREATE_CHAR_CLASS_EXTREME_MALE",
		"CREATE_CHAR_CLASS_EXTREME_FEMALE",
		"CREATE_CHAR_CLASS_BRAWLER_FEMALE",
		"CREATE_CHAR_CLASS_SWORDMAN_FEMALE",
		"CREATE_CHAR_CLASS_ARCHER_MALE",
		"CREATE_CHAR_CLASS_SHAMAN_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_MALE",
		"CREATE_CHAR_CLASS_SCIENTIST_FEMALE",
		"CREATE_CHAR_CLASS_ASSASSIN_MALE",
		"CREATE_CHAR_CLASS_ASSASSIN_FEMALE",
		"CREATE_CHAR_CLASS_TRICKER_MALE",
		"CREATE_CHAR_CLASS_TRICKER_FEMALE",
		"CREATE_CHAR_CLASS_ETC_MALE",
		"CREATE_CHAR_CLASS_ETC_FEMALE",
	};

	m_pMemberImage[nIndex]->SetImage( strClass[emCharIndex].c_str() );
}

void RnCDMClubInfo::SetMemberName( int nIndex, const CString strMemberName )
{
	if ( nIndex >= nMaxMember )
		return;

	for ( int i = nIndex; i < nMaxMember; i++ )
	{
		m_pMemberName[i]->ClearText();
	}
	m_pMemberName[nIndex]->SetOneLineText( strMemberName, NS_UITEXTCOLOR::WHITE );
}

void RnCDMClubInfo::SetTitleClubNameMemberNum( const CString strClubName, int nMembernum )
{
	SetTitleName( sc::string::format( "%s %s (%d/%d)", strClubName ,ID2GAMEWORD("MAP_CLUB"), nMembernum, nMaxMember).c_str() );
}

void RnCDMClubInfo::SetRnCDMClubInfoPos( int nPosX, int nPosY )
{
	SetGlobalPos( D3DXVECTOR2( (float)nPosX, (float)nPosY ) );
}

MyRnCDMClubInfo::MyRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: RnCDMClubInfo( pGaeaClient, pInterface, pEngineDevice )
{

}

void MyRnCDMClubInfo::CreateUIWindowAndRegisterOwnership()
{
	RnCDMClubInfo::Create( RNCDM_CLUB_INFO, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RnCDMClubInfo::CreateBaseWindowLightGray("RN_CDM_CLUB_INFO_WINDOW",  ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_TITLE") );
	RnCDMClubInfo::CreateSubControl();
	RnCDMClubInfo::SetAlignFlag( UI_FLAG_CENTER_X );
	RnCDMClubInfo::m_pInterface->UiRegisterControl( this, true );
	RnCDMClubInfo::m_pInterface->UiShowGroupFocus( RNCDM_CLUB_INFO );
}

void MyRnCDMClubInfo::SetMemberImage( int nIndex, EMCHARINDEX emCharIndex )
{
	RnCDMClubInfo::SetMemberImage( nIndex, emCharIndex );
}

void MyRnCDMClubInfo::SetMemberName( int nIndex, const CString strMemberName )
{
	RnCDMClubInfo::SetMemberName( nIndex, strMemberName );
}

void MyRnCDMClubInfo::SetTitleClubNameMemberNum( const CString strClubName, int nMembernum )
{
	RnCDMClubInfo::SetTitleClubNameMemberNum( strClubName, nMembernum );
}

void MyRnCDMClubInfo::SetRnCDMClubInfoPos( int nPosX, int nPosY )
{
	RnCDMClubInfo::SetRnCDMClubInfoPos( nPosX, nPosY );
}
