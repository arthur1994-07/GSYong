#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"

#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/DxResponseMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../MfcExLib/RanFilter.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "CreateCharacterPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CCreateCharacterPage::nLIMITCHAR = 16;

CCreateCharacterPage::CCreateCharacterPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow ( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
	, m_pSchoolTextBox ( NULL )
	, m_pClassTextBox ( NULL )
	, m_pFaceTextBox ( NULL )
	, m_pHairTextBox ( NULL )
	, m_bUseCancel ( TRUE )
	, m_wClassMax ( GLCI_NUM )
{
}

CCreateCharacterPage::~CCreateCharacterPage ()
{
}

void CCreateCharacterPage::CreateSubControl ()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CreateControl ( "CREATE_PAGE_SCHOOLBACK_M" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_M" );
	CreateControl ( "CREATE_PAGE_FACEBACK_M" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_M" );
	CreateControl ( "CREATE_PAGE_SCHOOLBACK_L" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_L" );
	CreateControl ( "CREATE_PAGE_FACEBACK_L" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_L" );
	CreateControl ( "CREATE_PAGE_SCHOOLBACK_R" );
	CreateControl ( "CREATE_PAGE_CLASSBACK_R" );
	CreateControl ( "CREATE_PAGE_FACEBACK_R" );
	CreateControl ( "CREATE_PAGE_HAIRBACK_R" );

	CreateControl ( "CREATE_PAGE_NAMEBACK_M" );
	CreateControl ( "CREATE_PAGE_NAMEBACK_L" );
	CreateControl ( "CREATE_PAGE_NAMEBACK_R" );
	
	CreateControl ( "CREATE_PAGE_NAMEEDIT_M" );
	CreateControl ( "CREATE_PAGE_NAMEEDIT_L" );
	CreateControl ( "CREATE_PAGE_NAMEEDIT_R" );	

	CBasicButton* pButton = NULL;
	CreateFlipButton ( "CREATE_PAGE_SCHOOL_L", "CREATE_PAGE_SCHOOL_L_F", CREATE_SCHOOL_L );
	CreateFlipButton ( "CREATE_PAGE_SCHOOL_R", "CREATE_PAGE_SCHOOL_R_F", CREATE_SCHOOL_R );
	
	CreateFlipButton ( "CREATE_PAGE_CLASS_L", "CREATE_PAGE_CLASS_L_F", CREATE_CLASS_L );
	CreateFlipButton ( "CREATE_PAGE_CLASS_R", "CREATE_PAGE_CLASS_R_F", CREATE_CLASS_R );
	
	CreateFlipButton ( "CREATE_PAGE_FACE_L", "CREATE_PAGE_FACE_L_F", CREATE_FACE_L );
	CreateFlipButton ( "CREATE_PAGE_FACE_R", "CREATE_PAGE_FACE_R_F", CREATE_FACE_R );

	CreateFlipButton ( "CREATE_PAGE_HAIR_L", "CREATE_PAGE_HAIR_L_F", CREATE_HAIR_L );
	CreateFlipButton ( "CREATE_PAGE_HAIR_R", "CREATE_PAGE_HAIR_R_F", CREATE_HAIR_R );

	m_pSchoolTextBox = CreateStaticControl ( "CREATE_PAGE_SCHOOL_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pClassTextBox = CreateStaticControl ( "CREATE_PAGE_CLASS_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pFaceTextBox = CreateStaticControl ( "CREATE_PAGE_FACE_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pHairTextBox = CreateStaticControl ( "CREATE_PAGE_HAIR_TEXTBOX", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );


	m_pOKButton = new CBasicTextButton(m_pEngineDevice);
	m_pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CREATE_PAGE_OK );
	m_pOKButton->CreateBaseButton ( "CREATE_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 0 ) );
	m_pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( m_pOKButton );
	m_pOKButton->SetVisibleSingle( FALSE );

	CBasicTextButton* pCancelButton = new CBasicTextButton(m_pEngineDevice);
	pCancelButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, CREATE_PAGE_CANCEL );
	pCancelButton->CreateBaseButton ( "CREATE_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CREATE_CHARACTER_PAGE_OKCANCEL", 1 ) );
	pCancelButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pCancelButton );
	
	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "CREATE_PAGE_SCHOOLBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 0)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_CLASSBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 1)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_FACEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 2)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_HAIRBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 3)  );

	pTextBox = CreateStaticControl ( "CREATE_PAGE_NAMEBACK_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("CREATE_CHARACTER_PAGE_STATIC", 4)  );

	m_pEditBox = new CUIEditBox(m_pEngineDevice);
	m_pEditBox->CreateSub ( this, "CREATE_CHARACTER_NAMEEDIT_BOX", UI_FLAG_DEFAULT, CREATE_PAGE_NAME_EDIT );
	m_pEditBox->CreateCarrat ( "CREATE_CHARACTER_NAME_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMITCHAR );
	m_pEditBox->SetFont ( pFont9 );
	RegisterControl ( m_pEditBox );

	m_strClassHair = _T("HAIR_NAME_FIGHTER");
	m_strClassFace = _T("FACE_NAME_FIGHTER");

}


void CCreateCharacterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	//CUIWindow::TranslateUIMessage ( ControlID, dwMsg );
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	DxLobyStage *pLobyStage = pGlobalStage->GetLobyStage();

	switch ( ControlID )
	{
	case CREATE_PAGE_OK:
		{
			if (CHECK_KEYFOCUSED(dwMsg) || CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
                DXInputString* pInputString = m_pEngineDevice->GetInputString();
				if (CHECK_KEYFOCUSED(dwMsg) && !pInputString->CheckEnterKeyDown())
                    return;

				if (!m_pEditBox)
                    return;

				if (!m_pEditBox->GetEditLength())
                    return;

				CString strTemp = m_pEditBox->GetEditString ();				
				m_pEditBox->EndEdit();
				
				if ( !CheckString ( strTemp ) )
				{
					m_pEditBox->ClearEdit();
					return;
				}

				if ( CRanFilter::GetInstance().NameFilter ( strTemp.GetString () ) )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT("CHARACTER_BADNAME") );
					return ;
				}

				const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();

				if ( sNEWINFO.CHECKVALID () )
				{
                    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
					CNetClient* pNetClient = pGlobalStage->GetNetClient();
					pNetClient->SndCreateChaInfo
					(
						sNEWINFO.m_emIndex,		/*부서*/
						sNEWINFO.m_wSchool,		/*학교*/
						sNEWINFO.m_wFace,		/*얼굴*/
						sNEWINFO.m_wHair,		/*헤어*/
						sNEWINFO.m_wHairColor,	/*헤어컬러*/
						sNEWINFO.m_wSex,		/*성별*/
						strTemp.GetString ()
					);	

//					m_pInterface->UiHideGroup ( GetWndID () );
//					m_pInterface->UiShowGroupFocus ( SELECT_CHARACTER_PAGE );
				}

				m_bUseCancel = FALSE;
			}
		}
		break;

	case CREATE_PAGE_CANCEL:
		{
			if( m_bUseCancel )
			{
				if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					m_pEditBox->EndEdit();
					m_pInterface->ToSelectCharacterPage ( GetWndID () );
				}
			}
		}
		break;

	case CREATE_SCHOOL_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharSchool(DxLobyStage::EM_PREV);
				m_pInterface->UiHideGroup( CREATE_CHARACTER_WEB );
			}
		}
		break;

	case CREATE_SCHOOL_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharSchool(DxLobyStage::EM_NEXT);
				m_pInterface->UiHideGroup( CREATE_CHARACTER_WEB );
			}
		}
		break;

	case CREATE_CLASS_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				WORD nIndex = pLobyStage->ShiftCharClass(DxLobyStage::EM_PREV, m_wClassMax);
			}
		}
		break;

	case CREATE_CLASS_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				WORD nIndex = pLobyStage->ShiftCharClass(DxLobyStage::EM_NEXT, m_wClassMax);
			}
		}
		break;

	case CREATE_FACE_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_PREV);
			}
		}
		break;

	case CREATE_FACE_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharFace(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case CREATE_HAIR_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_PREV);
			}
		}
		break;

	case CREATE_HAIR_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				pLobyStage->ShiftCharHair(DxLobyStage::EM_NEXT);
			}
		}
		break;

	case CREATE_PAGE_NAME_EDIT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ResetAll ();
			}
		}
		break;
	}
}

CBasicButton* CCreateCharacterPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	RegisterControl ( pButton );

	return pButton;
}

void CCreateCharacterPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		ResetAll ();

		if( m_pInterface->IsCreateExtreme() )
		{
			m_wClassMax = GLCI_NUM_EX;
		}
		else
		{
			m_wClassMax = GLCI_NUM;
		}
	}
}

void CCreateCharacterPage::ResetAll ()
{
	m_pEditBox->EndEdit();
	m_pEditBox->ClearEdit();		
	if( !m_pEditBox->IsBegin() )
	{
		m_pEditBox->BeginEdit();
	}
}

void CCreateCharacterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient ();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_2"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CONNECTCLOSED );
		}
	}

	m_pClassTextBox->ClearText ();
	m_pHairTextBox->ClearText ();
	m_pFaceTextBox->ClearText ();

	DxLobyStage *pLobyStage = pGlobalStage->GetLobyStage();
	const GLCHAR_NEWINFO& sNEWINFO = pLobyStage->GetNewCharInfo ();	
	const EMCHARINDEX& emCharIndex = sNEWINFO.m_emIndex;

	if( emCharIndex < m_wClassMax )
	{
		CString strClassName;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			strClassName = CGameTextMan::GetInstance().GetCommentText("CHARCLASS", emCharIndex).GetString();
		else
			strClassName = COMMENT::CHARCLASS[emCharIndex].c_str();

		const CString& strHairName = ID2GAMEWORD (m_strClassHair.GetString(), sNEWINFO.m_wHair );
		const CString& strFaceName = ID2GAMEWORD (m_strClassFace.GetString(), sNEWINFO.m_wFace );		
		
		m_pClassTextBox->SetOneLineText ( strClassName );
		m_pHairTextBox->SetOneLineText ( strHairName );
		m_pFaceTextBox->SetOneLineText ( strFaceName );

		m_pOKButton->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pOKButton->SetVisibleSingle( FALSE );
	}

	WORD wSchool = pLobyStage->GetCreateSchool ();
	const CString& strSchoolName = ID2GAMEWORD ("ACADEMY_NAME", wSchool );
	m_pSchoolTextBox->SetOneLineText ( strSchoolName );
}

BOOL CCreateCharacterPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();



	// 태국어 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString(strTemp) )
	    {
		    m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CREATESTAGE_9" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_THAI_CHECK_STRING_ERROR );
		    return FALSE;
	    }
    }

	// 베트남 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if( STRUTIL::CheckVietnamString( strTemp ) )
	    {
		    m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CREATESTAGE_10" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_THAI_CHECK_STRING_ERROR );
		    return FALSE;
	    }
    }
	
	if( STRUTIL::CheckString( strTemp ) )
	{
		m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CREATE_CHARACTER_NAME_ERROR" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_NAME_ERROR );
		return FALSE;
	}

    return TRUE;
}
