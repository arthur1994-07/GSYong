#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "SelectStylePage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CSelectStylePage::nLIMITCHAR = 16;

CSelectStylePage::CSelectStylePage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow ( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
	, m_nSex ( 0 )
	, m_nSchool ( 0 )
	, m_nClass ( 0 )
	, m_nHair ( 0 )
	, m_nFace ( 0 )
	, m_pSchoolTextBox ( NULL )
	, m_pClassTextBox ( NULL )
	, m_pSexTextBox ( NULL )
	, m_pFaceTextBox ( NULL )
	, m_pHairTextBox ( NULL )
//	, m_pOkButton ( NULL )
//	, m_CancleButton ( NULL )
	, m_pEditBox ( NULL ) 

{

	
}

CSelectStylePage::~CSelectStylePage()
{


}

void CSelectStylePage::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;

	float fontSize = 1.0f;

	LONG lResolution = m_pInterface->UiGetResolution ();
	WORD X_RES = HIWORD( lResolution );

	// 해상도가 작으면 폰트 사이즈를 줄인다. 
	if ( X_RES <= 800 ) fontSize = fFontRate;


	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, static_cast<DWORD>(9 * fontSize), _DEFAULT_FONT_FLAG );

	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_SCHOOLBACK_L" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_SCHOOLBACK_R" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_CLASSBACK_L" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_CLASSBACK_R" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_GENDERBACK_L" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_GENDERBACK_R" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_FACEBACK" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_HAIRBACK" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_NAMEBACK" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_NAMEEDIT_M" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_NAMEEDIT_L" );
	CreateControl ( "NEW_CHAR_SELECT_STYLE_PAGE_NAMEEDIT_R" );	


	CreateFlipButton ( "NEW_CHAR_SELECT_STYLE_PAGE_FACE_L", "NEW_CHAR_SELECT_STYLE_PAGE_FACE_L_F", SELECT_FACE_LEFT );
	CreateFlipButton ( "NEW_CHAR_SELECT_STYLE_PAGE_FACE_R", "NEW_CHAR_SELECT_STYLE_PAGE_FACE_R_F", SELECT_FACE_RIGHT );

	CreateFlipButton ( "NEW_CHAR_SELECT_STYLE_PAGE_HAIR_L", "NEW_CHAR_SELECT_STYLE_PAGE_HAIR_L_F", SELECT_HAIR_LEFT );
	CreateFlipButton ( "NEW_CHAR_SELECT_STYLE_PAGE_HAIR_R", "NEW_CHAR_SELECT_STYLE_PAGE_HAIR_R_F", SELECT_HAIR_RIGHT );

	m_pSchoolTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_SCHOOL_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	m_pClassTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_CLASS_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	m_pSexTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_GENDER_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	m_pFaceTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_FACE_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
	m_pHairTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_HAIR_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );

/*
	m_pOKButton = new CBasicTextButton(m_pRenderDevice);
	m_pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CREATE_PAGE_OK );
	m_pOKButton->CreateBaseButton ( "CREATE_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 0 ) );
	m_pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( m_pOKButton );
	m_pOKButton->SetVisibleSingle( FALSE );

	CBasicTextButton* pCancelButton = new CBasicTextButton(m_pRenderDevice);
	pCancelButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CREATE_PAGE_CANCEL );
	pCancelButton->CreateBaseButton ( "CREATE_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 1 ) );
	pCancelButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pCancelButton );
*/
	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_SCHOOLBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 0)  );

	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_CLASSBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 1)  );
	
	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_GENDERBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 6)  );

	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_FACEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 2)  );

	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_HAIRBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 3)  );

	pTextBox = CreateStaticControl ( "NEW_CHAR_SELECT_STYLE_PAGE_NAMEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 4)  );

	m_pEditBox = new CUIEditBox(m_pEngineDevice);
	m_pEditBox->CreateSub ( this, "NEW_CREATE_CHARACTER_NAMEEDIT_BOX", UI_FLAG_DEFAULT, NEWCHAR_NAME_EDIT );
	m_pEditBox->CreateCarrat ( "NEW_CREATE_CHARACTER_NAME_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMITCHAR );
	m_pEditBox->SetFont ( pFont9 );
	RegisterControl ( m_pEditBox );


	m_strClassHair = _T("HAIR_NAME_FIGHTER");
	m_strClassFace = _T("FACE_NAME_FIGHTER");




}
CBasicButton* CSelectStylePage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	RegisterControl ( pButton );

	return pButton;
}

void CSelectStylePage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient ();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_2"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CONNECTCLOSED );
		}
	}

	DxLobyStage *pLobyStage = pGlobalStage->GetLobyStage();
	const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();	
	const EMCHARINDEX& emCharIndex = sNEWINFO.m_emIndex;

	const CString& strHairName = ID2GAMEWORD (m_strClassHair.GetString(), sNEWINFO.m_wHair );
	const CString& strFaceName = ID2GAMEWORD (m_strClassFace.GetString(), sNEWINFO.m_wFace );		
	m_pHairTextBox->SetOneLineText ( strHairName );
	m_pFaceTextBox->SetOneLineText ( strFaceName );
}

void CSelectStylePage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	DxLobyStage *pLobyStage = pGlobalStage->GetLobyStage();

	switch ( ControlID )
	{
	case SELECT_FACE_LEFT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_PREV);
			}
		}
		break;

	case SELECT_FACE_RIGHT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case SELECT_HAIR_LEFT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_PREV);
			}
		}
		break;

	case SELECT_HAIR_RIGHT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case NEWCHAR_NAME_EDIT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ResetAll ();
			}
		}
		break;
	}
}

void CSelectStylePage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

void CSelectStylePage::SetSelectChar( int nSchool, int nSex, int nClass )
{
//	m_nSchool = nSchool;
//	m_nSex = nSex;
//	m_nClass = nClass;
//	m_nFace = 0;
//	m_nHair = 0;

	m_pSchoolTextBox->SetText( ID2GAMEEXTEXT("NEW_CHAR_SELECT_SCHOOL_NAME",nSchool ), NS_UITEXTCOLOR::WHITE );	
	m_pClassTextBox->SetText( ID2GAMEEXTEXT("NEW_CHAR_SELECT_CLASS_NAME",nClass ), NS_UITEXTCOLOR::WHITE );	
	m_pSexTextBox->SetText( ID2GAMEEXTEXT("NEW_CHAR_SELECT_SEX_NAME", nSex), NS_UITEXTCOLOR::WHITE );	
	ResetAll();
}

void CSelectStylePage::ResetAll ()
{
	m_pEditBox->EndEdit();
	m_pEditBox->ClearEdit();		
	if( !m_pEditBox->IsBegin() )
	{
		m_pEditBox->BeginEdit();
	}
}