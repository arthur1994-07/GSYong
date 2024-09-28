#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "CreateCharacterSex.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCreateCharacterSex::CCreateCharacterSex(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pMale( NULL )
    , m_pFemale( NULL )
    , m_pNameTextBox( NULL )
{
}

CCreateCharacterSex::~CCreateCharacterSex()
{
}

void CCreateCharacterSex::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "CREATE_CHAR_SEX_WINDOW_BACK" );
		RegisterControl ( pBasicLineBox );		
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	D3DCOLOR dwNameColor = NS_UITEXTCOLOR::DEFAULT;

    m_pMale = CreateControl( "CREATE_CHAR_SEX_WINDOW_MALE" );
	m_pFemale = CreateControl( "CREATE_CHAR_SEX_WINDOW_FEMALE" );

	m_pNameTextBox = CreateStaticControl ( "CREATE_CHAR_SEX_NAME_TEXT", pFont, dwNameColor, TEXT_ALIGN_LEFT );
}

CUIControl*	CCreateCharacterSex::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CCreateCharacterSex::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CCreateCharacterSex::ResetData()
{
	m_pMale->SetVisibleSingle( FALSE );
	m_pFemale->SetVisibleSingle( FALSE );
	m_pNameTextBox->ClearText();
}

void CCreateCharacterSex::SetSexualData( INT nSex )
{
	ResetData();

	if( nSex == 0 )
	{
		m_pMale->SetVisibleSingle( TRUE );
		m_pNameTextBox->SetText( ID2GAMEWORD("BASIC_SEX", 0) );
	}
	else if( nSex == 1 )
	{
		m_pFemale->SetVisibleSingle( TRUE );
		m_pNameTextBox->SetText( ID2GAMEWORD("BASIC_SEX", 1) );
	}
}

void CCreateCharacterSex::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCreateCharacterSex::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CCreateCharacterSex::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( !bVisible )
	{
		ResetData();
	}
}