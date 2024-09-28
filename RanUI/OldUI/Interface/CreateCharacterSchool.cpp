#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "CreateCharacterSchool.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCreateCharacterSchool::CCreateCharacterSchool(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CCreateCharacterSchool::~CCreateCharacterSchool()
{
}

void CCreateCharacterSchool::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "CREATE_CHAR_SCHOOL_WINDOW_BACK" );
		RegisterControl ( pBasicLineBox );		
	}

	CD3DFontPar* pFontName = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	D3DCOLOR dwNameColor = NS_UITEXTCOLOR::DEFAULT;

	m_pSM = CreateControl( "CREATE_CHAR_SCHOOL_WINDOW_SM" );
	m_pHA = CreateControl( "CREATE_CHAR_SCHOOL_WINDOW_HA" );
	m_pBH = CreateControl( "CREATE_CHAR_SCHOOL_WINDOW_BH" );

    m_pNameTextBox = CreateStaticControl ( "CREATE_CHAR_SCHOOL_NAME_TEXT", pFontName, dwNameColor, TEXT_ALIGN_LEFT );
	m_pInfoTextBox = CreateStaticControl ( "CREATE_CHAR_SCHOOL_INFO_TEXT", pFont, dwNameColor, TEXT_ALIGN_LEFT );
}

CUIControl*	CCreateCharacterSchool::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CCreateCharacterSchool::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CCreateCharacterSchool::ResetData()
{
	m_pSM->SetVisibleSingle( FALSE );
	m_pHA->SetVisibleSingle( FALSE );
	m_pBH->SetVisibleSingle( FALSE );

	m_pNameTextBox->ClearText();
	m_pInfoTextBox->ClearText();
}

void CCreateCharacterSchool::SetSchoolData( INT nSchool )
{
	ResetData();

	switch( nSchool )
	{
	case SCHOOL_SM:
        m_pSM->SetVisibleSingle( TRUE );
		break;
	case SCHOOL_HA:
		m_pHA->SetVisibleSingle( TRUE );
		break;
	case SCHOOL_BH:
		m_pBH->SetVisibleSingle( TRUE );
		break;
	}

	m_pNameTextBox->SetText( ID2GAMEEXTEXT( "NEW_CHAR_SELECT_SCHOOL_NAME", nSchool ), NS_UITEXTCOLOR::WHITE );	
	
	CString strTemp;
	strTemp.Format ( "NEW_CHAR_SELECT_SCHOOL_DEC%d", nSchool );
	m_pInfoTextBox->ClearText();
	m_pInfoTextBox->SetText ( ID2GAMEEXTEXT( strTemp ), NS_UITEXTCOLOR::WHITE );
}

void CCreateCharacterSchool::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCreateCharacterSchool::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CCreateCharacterSchool::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( !bVisible )
	{
		ResetData();
	}
}