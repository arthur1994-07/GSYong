#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "CreateCharacterWindow.h"
#include "CreateCharacterMenu.h"
#include "CreateCharacterSchool.h"
#include "CreateCharacterSex.h"
#include "CreateCharacterClass.h"
#include "RotateCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCreateCharacterWindow::CCreateCharacterWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pCharMenu( NULL )
    , m_pCharSchool( NULL )
    , m_pCharSex( NULL )
    , m_pCharClass( NULL )
{
}

CCreateCharacterWindow::~CCreateCharacterWindow()
{
}

void CCreateCharacterWindow::CreateSubControl ()
{
	m_pCharMenu = new CCreateCharacterMenu(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pCharMenu->CreateSub ( this, "CREATE_CHAR_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE , CREATE_MENU_PAGE );
	m_pCharMenu->CreateSubControl ();
	RegisterControl ( m_pCharMenu );

	m_pCharSchool = new CCreateCharacterSchool(m_pEngineDevice);
	m_pCharSchool->CreateSub ( this, "CREATE_CHAR_SCHOOL_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE , CREATE_SCHOOL_PAGE );
	m_pCharSchool->CreateSubControl ();
	RegisterControl ( m_pCharSchool );

	m_pCharSex = new CCreateCharacterSex(m_pEngineDevice);
	m_pCharSex->CreateSub ( this, "CREATE_CHAR_SEX_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE , CREATE_SEX_PAGE );
	m_pCharSex->CreateSubControl ();
	RegisterControl ( m_pCharSex );

	m_pCharClass = new CCreateCharacterClass(m_pEngineDevice);
	m_pCharClass->CreateSub ( this, "CREATE_CHAR_CLASS_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE , CREATE_CLASS_PAGE );
	m_pCharClass->CreateSubControl ();
	RegisterControl ( m_pCharClass );

	m_pLeft = CreateControl( "CREATE_CHAR_SCHOOL_WINDOW_LEFT" );
	m_pRight = CreateControl( "CREATE_CHAR_SCHOOL_WINDOW_RIGHT" );
	m_pLeft->SetVisibleSingle( FALSE );
	m_pRight->SetVisibleSingle( FALSE );
}

CUIControl*	CCreateCharacterWindow::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CCreateCharacterWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CCreateCharacterWindow::ResetData()
{
	m_pCharMenu->SetVisibleSingle( TRUE );
	m_pCharMenu->ResetData();

	m_pCharSchool->SetVisibleSingle( FALSE );
	m_pCharSchool->ResetData();

	m_pCharSex->SetVisibleSingle( FALSE );
	m_pCharSex->ResetData();

	m_pCharClass->SetVisibleSingle( FALSE );
	m_pCharClass->ResetData();
}

BOOL CCreateCharacterWindow::IsCancelButton()
{	
	return m_pCharMenu->IsCancelButton();
}

void CCreateCharacterWindow::UseCancelButton()
{
	m_pCharMenu->UseCancelButton();
}

void CCreateCharacterWindow::SetData( INT nSchool, INT nSex, INT nClass )
{
	if( nSchool == -1 )
	{
		m_pCharSchool->SetVisibleSingle( FALSE );
		m_pCharSex->SetVisibleSingle( FALSE );
		m_pCharClass->SetVisibleSingle( FALSE );
		if( m_pInterface->GetRotateCharacter() )
		{
			m_pInterface->GetRotateCharacter()->SetVisibleSingle( FALSE );
		}
		return;
	}

	if( nSex == -1 )
	{
		m_pCharSchool->SetVisibleSingle( TRUE );
		m_pCharSchool->SetGlobalPos( m_pRight->GetGlobalPos() );
		m_pCharSchool->SetLocalPos( m_pRight->GetLocalPos() );
		m_pCharSchool->SetSchoolData( nSchool );

		m_pCharSex->SetVisibleSingle( FALSE );
		m_pCharClass->SetVisibleSingle( FALSE );
		if( m_pInterface->GetRotateCharacter() )
		{
			m_pInterface->GetRotateCharacter()->SetVisibleSingle( FALSE );
		}
		return;
	}

	if( nClass == -1 )
	{
		m_pCharSchool->SetVisibleSingle( TRUE );
		m_pCharSchool->SetGlobalPos( m_pLeft->GetGlobalPos() );
		m_pCharSchool->SetLocalPos( m_pLeft->GetLocalPos() );
		m_pCharSchool->SetSchoolData( nSchool );

		m_pCharSex->SetVisibleSingle( TRUE );
		m_pCharSex->SetSexualData( nSex );

		m_pCharClass->SetVisibleSingle( FALSE );
		if( m_pInterface->GetRotateCharacter() )
		{
			m_pInterface->GetRotateCharacter()->SetVisibleSingle( FALSE );
		}
		return;
	}

	if( nSchool != -1 && nSex != -1 && nClass != -1 )
	{
		m_pCharSchool->SetVisibleSingle( TRUE );
		m_pCharSchool->SetGlobalPos( m_pLeft->GetGlobalPos() );
		m_pCharSchool->SetLocalPos( m_pLeft->GetLocalPos() );
		m_pCharSchool->SetSchoolData( nSchool );

		m_pCharSex->SetVisibleSingle( TRUE );
		m_pCharSex->SetSexualData( nSex );

		m_pCharClass->SetVisibleSingle( TRUE );
		m_pCharClass->SetClassData( nSex, nClass );
		if( m_pInterface->GetRotateCharacter() )
		{
			m_pInterface->GetRotateCharacter()->SetVisibleSingle( TRUE );
		}
		return;
	}
}

void CCreateCharacterWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCreateCharacterWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CCreateCharacterWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		m_pCharMenu->SetVisibleSingle( TRUE );
		m_pCharMenu->ResetAll();
		m_pCharSchool->ResetData();
		m_pCharSex->ResetData();
		m_pCharClass->ResetData();
	}
	else
	{
		m_pCharMenu->SetVisibleSingle( bVisible );
	}
}