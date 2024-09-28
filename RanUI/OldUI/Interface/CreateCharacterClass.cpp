#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "CreateCharacterClass.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCreateCharacterClass::CCreateCharacterClass(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
    memset( m_pClass, NULL, sizeof( CUIControl* ) * GLCI_NUM_ETC );
}

CCreateCharacterClass::~CCreateCharacterClass()
{
}

void CCreateCharacterClass::CreateSubControl ()
{
    {
        CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
        pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pBasicLineBox->CreateBaseBoxOuterFat ( "CREATE_CHAR_CLASS_WINDOW_BACK" );
        RegisterControl ( pBasicLineBox );		
    }

    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    D3DCOLOR dwNameColor = NS_UITEXTCOLOR::DEFAULT;




    CString strClass[GLCI_NUM_ETC] = 
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


    for ( int i = 0; i < GLCI_NUM_ETC; ++i ) 
    {
        m_pClass[i] = CreateControl( strClass[i] );
    }

    m_pNameTextBox = CreateStaticControl ( "CREATE_CHAR_CLASS_NAME_TEXT", pFont, dwNameColor, TEXT_ALIGN_LEFT );
    m_pInfoTextBox = CreateStaticControl ( "CREATE_CHAR_CLASS_INFO_TEXT", pFont, dwNameColor, TEXT_ALIGN_LEFT );
}

void CCreateCharacterClass::ResetData()
{
    for ( int i = 0; i < GLCI_NUM_ETC; ++i ) 
    {
        m_pClass[i]->SetVisibleSingle( FALSE );
    }

    m_pNameTextBox->ClearText();
    m_pInfoTextBox->ClearText();
}

void CCreateCharacterClass::SetClassData( INT nSex, INT nClass )
{
    ResetData();

    if( nSex == 0 )
    {
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			switch( nClass )
			{
			case EXTREME:
				m_pClass[GLCI_EXTREME_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_EXTREME_M )).GetString() );
				break;
			case SWORDMAN:
				m_pClass[GLCI_ARMS_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_ARMS_M )).GetString() );
				break;
			case ARCHER:
				m_pClass[GLCI_ARCHER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_ARCHER_M )).GetString() );
				break;
			case BRAWLER:
				m_pClass[GLCI_FIGHTER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_FIGHTER_M )).GetString() );
				break;
			case SHAMAN:
				m_pClass[GLCI_SPIRIT_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_SPIRIT_M )).GetString() );
				break;
			case SCIENTIST:
				m_pClass[GLCI_SCIENTIST_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_SCIENTIST_M )).GetString() );
				break;
			case ASSASSIN:
				m_pClass[GLCI_ASSASSIN_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS",  CharClassToIndex( GLCC_ASSASSIN_M )).GetString() );
				break;
			case TRICKER:
				m_pClass[GLCI_TRICKER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_TRICKER_M )).GetString() );
				break;
			}
		}
		else
		{
			switch( nClass )
			{
			case ASSASSIN:
				m_pClass[GLCI_ASSASSIN_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ASSASSIN_M )].c_str() );
				break;
			case EXTREME:
				m_pClass[GLCI_EXTREME_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_EXTREME_M )].c_str() );
				break;
			case SWORDMAN:
				m_pClass[GLCI_ARMS_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ARMS_M )].c_str() );
				break;
			case ARCHER:
				m_pClass[GLCI_ARCHER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ARCHER_M )].c_str() );
				break;
			case BRAWLER:
				m_pClass[GLCI_FIGHTER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_FIGHTER_M )].c_str() );
				break;
			case SHAMAN:
				m_pClass[GLCI_SPIRIT_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_SPIRIT_M )].c_str() );
				break;
			case SCIENTIST:
				m_pClass[GLCI_SCIENTIST_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_SCIENTIST_M )].c_str() );
				break;
			case TRICKER:
				m_pClass[GLCI_TRICKER_M]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_TRICKER_M )].c_str() );
				break;
			}
		}
    }
    else if( nSex == 1 )
    {
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			switch( nClass )
			{
			case ASSASSIN:
				m_pClass[GLCI_ASSASSIN_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_ASSASSIN_W )).GetString() );
				break;
			case EXTREME:
				m_pClass[GLCI_EXTREME_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_EXTREME_W )).GetString() );
				break;
			case SWORDMAN:
				m_pClass[GLCI_ARMS_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_ARMS_W )).GetString() );
				break;
			case ARCHER:
				m_pClass[GLCI_ARCHER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_ARCHER_W )).GetString() );
				break;
			case BRAWLER:
				m_pClass[GLCI_FIGHTER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_FIGHTER_W )).GetString() );
				break;
			case SHAMAN:
				m_pClass[GLCI_SPIRIT_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_SPIRIT_W )).GetString() );
				break;
			case SCIENTIST:
				m_pClass[GLCI_SCIENTIST_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_SCIENTIST_W )).GetString() );
				break;
			case TRICKER:
				m_pClass[GLCI_TRICKER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex( GLCC_TRICKER_W )).GetString() );
				break;
			}
		}
		else
		{
			switch( nClass )
			{
			case ASSASSIN:
				m_pClass[GLCI_ASSASSIN_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ASSASSIN_W )].c_str() );
				break;
			case EXTREME:
				m_pClass[GLCI_EXTREME_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_EXTREME_W )].c_str() );
				break;
			case SWORDMAN:
				m_pClass[GLCI_ARMS_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ARMS_W )].c_str() );
				break;
			case ARCHER:
				m_pClass[GLCI_ARCHER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_ARCHER_W )].c_str() );
				break;
			case BRAWLER:
				m_pClass[GLCI_FIGHTER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_FIGHTER_W )].c_str() );
				break;
			case SHAMAN:
				m_pClass[GLCI_SPIRIT_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_SPIRIT_W )].c_str() );
				break;
			case SCIENTIST:
				m_pClass[GLCI_SCIENTIST_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_SCIENTIST_W )].c_str() );
				break;
			case TRICKER:
				m_pClass[GLCI_TRICKER_W]->SetVisibleSingle( TRUE );
				m_pNameTextBox->SetText( COMMENT::CHARCLASS[ CharClassToIndex( GLCC_TRICKER_W )].c_str() );
				break;
			}
		}
    }

    CString strTemp;
    strTemp.Format ( "NEW_CHAR_SELECT_CLASS_DEC%d", nClass );
    m_pInfoTextBox->ClearText();
    m_pInfoTextBox->SetText ( ID2GAMEEXTEXT(strTemp), NS_UITEXTCOLOR::WHITE );
}

CUIControl*	CCreateCharacterClass::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
    RegisterControl ( pControl );
    return pControl;
}

CBasicTextBox* CCreateCharacterClass::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
    CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
    pStaticText->CreateSub ( this, szControlKeyword );
    pStaticText->SetFont ( pFont );
    pStaticText->SetTextAlign ( nAlign );
    RegisterControl ( pStaticText );
    return pStaticText;
}

void CCreateCharacterClass::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCreateCharacterClass::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CCreateCharacterClass::SetVisibleSingle( BOOL bVisible )
{
    CUIGroup::SetVisibleSingle( bVisible );

    if ( !bVisible )
    {
        ResetData();
    }
}
