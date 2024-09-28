#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/Msg/GLContrlBaseMsg.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "SelectCharacterPageListItem.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterPageListItem::CSelectCharacterPageListItem ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIOuterWindow ( pInterface, pEngineDevice )
{
    memset( m_pClassOn, NULL, sizeof(CUIControl*) * GLCI_NUM_ETC );
    memset( m_pClassOff, NULL, sizeof(CUIControl*) * GLCI_NUM_ETC );
}

CSelectCharacterPageListItem::~CSelectCharacterPageListItem ()
{
}

void CSelectCharacterPageListItem::CreateSubControl ()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    D3DCOLOR dwNameColor = NS_UITEXTCOLOR::DEFAULT;

    CString strClassOn[GLCI_NUM_ETC] = 
    {
        "SELECT_CHAR_WINDOW_BRAWLER_MALE_ON",
        "SELECT_CHAR_WINDOW_SWORDSMAN_MALE_ON",
        "SELECT_CHAR_WINDOW_ARCHER_FEMALE_ON",
        "SELECT_CHAR_WINDOW_SHAMAN_FEMALE_ON",
        "SELECT_CHAR_WINDOW_EXTREME_MALE_ON",
        "SELECT_CHAR_WINDOW_EXTREME_FEMALE_ON",
        "SELECT_CHAR_WINDOW_BRAWLER_FEMALE_ON",
        "SELECT_CHAR_WINDOW_SWORDSMAN_FEMALE_ON",
        "SELECT_CHAR_WINDOW_ARCHER_MALE_ON",
        "SELECT_CHAR_WINDOW_SHAMAN_MALE_ON",
        "SELECT_CHAR_WINDOW_SCIENTIST_MALE_ON",
        "SELECT_CHAR_WINDOW_SCIENTIST_FEMALE_ON",
        "SELECT_CHAR_WINDOW_ASSASSIN_MALE_ON",
        "SELECT_CHAR_WINDOW_ASSASSIN_FEMALE_ON",
		"SELECT_CHAR_WINDOW_TRICKER_MALE_ON",
		"SELECT_CHAR_WINDOW_TRICKER_FEMALE_ON",
        "SELECT_CHAR_WINDOW_ETC_MALE_ON",
        "SELECT_CHAR_WINDOW_ETC_FEMALE_ON",
    };

    CString strClassOff[GLCI_NUM_ETC] =
    {
        "SELECT_CHAR_WINDOW_BRAWLER_MALE_OFF",
        "SELECT_CHAR_WINDOW_SWORDSMAN_MALE_OFF",
        "SELECT_CHAR_WINDOW_ARCHER_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_SHAMAN_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_EXTREME_MALE_OFF",
        "SELECT_CHAR_WINDOW_EXTREME_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_BRAWLER_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_SWORDSMAN_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_ARCHER_MALE_OFF",
        "SELECT_CHAR_WINDOW_SHAMAN_MALE_OFF",
        "SELECT_CHAR_WINDOW_SCIENTIST_MALE_OFF",
        "SELECT_CHAR_WINDOW_SCIENTIST_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_ASSASSIN_MALE_OFF",
        "SELECT_CHAR_WINDOW_ASSASSIN_FEMALE_OFF",
		"SELECT_CHAR_WINDOW_TRICKER_MALE_OFF",
		"SELECT_CHAR_WINDOW_TRICKER_FEMALE_OFF",
        "SELECT_CHAR_WINDOW_ETC_MALE_OFF",
        "SELECT_CHAR_WINDOW_ETC_FEMALE_OFF",
    };

    for ( int i = 0; i < GLCI_NUM_ETC; ++i ) 
    {
        m_pClassOn[i] = CreateControl( strClassOn[i].GetString() );
        m_pClassOff[i] = CreateControl( strClassOff[i].GetString() );
    }

    m_pLevelTextBox = CreateStaticControl ( "SELECT_CHAR_WINDOW_CHAR_LIST_LEVEL", pFont, dwNameColor, TEXT_ALIGN_LEFT );
    m_pClassTextBox = CreateStaticControl ( "SELECT_CHAR_WINDOW_CHAR_LIST_CLASS", pFont, dwNameColor, TEXT_ALIGN_LEFT );
    m_pNameTextBox = CreateStaticControl ( "SELECT_CHAR_WINDOW_CHAR_LIST_NAME", pFont, dwNameColor, TEXT_ALIGN_LEFT );

    m_pItemSelect = CreateControl ( "SELECT_CHAR_WINDOW_SELECT" );
}

CUIControl*	CSelectCharacterPageListItem::CreateControl( const char* szControl )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub ( this, szControl );
    RegisterControl ( pControl );
    return pControl;
}

CBasicTextBox* CSelectCharacterPageListItem::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
    CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
    pStaticText->CreateSub ( this, szControlKeyword );
    pStaticText->SetFont ( pFont );
    pStaticText->SetTextAlign ( nAlign );
    RegisterControl ( pStaticText );
    return pStaticText;
}

void CSelectCharacterPageListItem::ResetData()
{

    for ( int i = 0; i < GLCI_NUM_ETC; ++i ) 
    {
        m_pClassOn[i]->SetVisibleSingle( FALSE );
        m_pClassOff[i]->SetVisibleSingle( FALSE );		
    }

    m_pLevelTextBox->SetVisibleSingle( FALSE );
    m_pClassTextBox->SetVisibleSingle( FALSE );
    m_pNameTextBox->SetVisibleSingle( FALSE );

    m_pItemSelect->SetVisibleSingle( FALSE );

}

void CSelectCharacterPageListItem::SetListItemInfo( SCHARINFO_LOBBY* CharInfo, BOOL bSelect )
{
    ResetData();

    {
        CString strCombine;

        strCombine.Format( "Lv.%d", CharInfo->m_wLevel );
        m_pLevelTextBox->SetVisibleSingle( TRUE );
        m_pLevelTextBox->SetText( strCombine );
    }
    {
		CString strClassName;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			strClassName = CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(static_cast<EMCHARCLASS>(CharInfo->m_emClass))).GetString();
		else
			strClassName = COMMENT::CHARCLASS[CharClassToIndex(static_cast<EMCHARCLASS> (CharInfo->m_emClass))].c_str();

        m_pClassTextBox->SetVisibleSingle( TRUE );
        m_pClassTextBox->SetText( strClassName );
    }
    {	
        CString strTmp;
        strTmp.Format( _T( "%s" ), CharInfo->m_ChaName.c_str() );
        m_pNameTextBox->SetVisibleSingle( TRUE );
        m_pNameTextBox->SetText( strTmp );
    }

    EMCHARINDEX nChaIndex = CharClassToIndex(static_cast<EMCHARCLASS> (CharInfo->m_emClass));

    if ( bSelect ) m_pClassOn[nChaIndex]->SetVisibleSingle( TRUE );
    else m_pClassOff[nChaIndex]->SetVisibleSingle( TRUE );

    if( bSelect )
    {
        m_pItemSelect->SetVisibleSingle( TRUE );
    }
}

void CSelectCharacterPageListItem::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CSelectCharacterPageListItem::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}