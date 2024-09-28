#include "StdAfx.h"

#include "../SigmaCore/Util/SystemInfo.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../EngineLib/Common/CommonWeb.h"
#include "../EngineLib/Common/SubPath.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLUseFeatures.h"

#include "OldUI/Util/ModalWindowOuter.h"

#include "OldUI/Interface/SelectServerPage.h"
#include "OldUI/Interface/SelectChinaAreaPage.h"
#include "OldUI/Interface/LoginPage.h"
#include "OldUI/Interface/PassKeyboard.h"
#include "OldUI/Interface/SelectCountryUI.h"
#include "OldUI/Interface/SelectContinentListUI.h"
#include "OldUI/Interface/SelectCountryListUI.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/SelectCharacterLeftPage.h"
#include "OldUI/Interface/CreateCharacterPage.h"
#include "OldUI/Interface/WaitDialogue.h"
#include "OldUI/Interface/SecPassSetPage.h"
#include "OldUI/Interface/SecPassCheckPage.h"
#include "OldUI/Interface/MessageWindow.h"
#include "OldUI/Interface/NewCreateCharPage.h"
#include "OldUI/Interface/CreateCharacterWindow.h"
#include "OldUI/Interface/CreateCharacterButtonPage.h"
#include "OldUI/Interface/RotateCharacter.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/SelectCharacterPageList.h"
#include "OldUI/Interface/LockSelectCharacterPreviewPage.h"
#include "OldUI/Interface/SelectCountryConfirmModal.h"
#include "OldUI/Interface/SelectCountryWindow.h"
#include "OldUI/Interface/SelectClubWindow.h"
#include "./StaticUIManager.h"
#include "OuterInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------
/*
COuterInterface& COuterInterface::GetInstance()
{
	static COuterInterface Instance;
	return Instance;
}
*/

const	float	COuterInterface::fLIST_LINE_INTERVAL = 5.0f;

CBasicScrollBarEx* g_pScrollBar = NULL;

COuterInterface::COuterInterface(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : OutInterfaceBase(pGaeaClient, pEngineDevice)
    , m_bUseWaitTime ( FALSE )
    , m_bBlockProgramFound ( false )
    , m_nServerGroup ( 0 )
    , m_nServerChannel ( 0 )
    , m_bCHANNEL ( false )
    , m_bLoginCancel( FALSE )
    , m_lResolutionBack( 0 )
    , m_uidMoalCallWindow(NO_ID)
    , m_Country(wb::UNKNOWN_COUNTRY)
	, m_bChaExtreme( FALSE )
	, m_bCharacterSelectLock( false )
{
	ResetControl();
}

COuterInterface::~COuterInterface ()
{
}

void COuterInterface::ResetControl()
{
	m_pModalWindow = NULL;
	m_pSelectServerPage = NULL;
	m_pLoginPage = NULL;
	m_pLoginMark = NULL;
	m_pSecPassSetPage = NULL;
	m_pSecPassCheckPage = NULL;
	m_pMessageWindow = NULL;
	m_pMessageWindowDummy = NULL;
	m_pPassKeyboard = NULL;
	m_pSelectCharacterPage = NULL;
	m_pSelectCharacterLeftPage = NULL;
	m_pSelectCountryUI = NULL;
	m_pSelectContinentListUI = NULL;
	m_pSelectCountryListUI = NULL;
	m_pCreateCharacterPage = NULL;
	m_pFullScreenButton = NULL;
	m_pFullScreenButtonDummy = NULL;
	m_pUpImage = NULL;
	m_pDownImage = NULL;
	m_pNewCreateCharPage = NULL;
	m_pCreateCharacterWindow = NULL;
	m_pCreateCharacterButtonPage = NULL;
	m_pInfoDisplay = NULL;
	m_pTitleText = NULL;
	m_pRotateChar = NULL;

	m_pWaitDisplay = NULL;
	m_pDxLobyStage = NULL;

    m_pLockSelectCharacterPreviewPage = NULL;
    m_pSelectCountryConfirmModal = NULL;
    m_pSelectCountryWindow = NULL;
    m_pCharacterCountryClubInfo = NULL;
    m_pSelectClubWindow = NULL;
}

void COuterInterface::UiSetResolution( long lResolution )
{
    CUIMan::SetResolution( lResolution );
}
long COuterInterface::UiGetResolution()
{
    return CUIMan::GetResolution();
}

HRESULT COuterInterface::OneTimeSceneInit ()
{
	switch ( RENDERPARAM::emSERVICE_TYPE )
	{
	case RENDERPARAM::EMSERVICE_MALAYSIA_CN:
	case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
	case RENDERPARAM::EMSERVICE_THAILAND:
	case RENDERPARAM::EMSERVICE_PHILIPPINES:
	case RENDERPARAM::EMSERVICE_VIETNAM:
	case RENDERPARAM::EMSERVICE_DEFAULT:
		CLoginPage::nLIMIT_ID = 14;
		CLoginPage::nLIMIT_PW = 14;
		m_bCHANNEL = true;
		break;
	case RENDERPARAM::EMSERVICE_FEYA:
		CLoginPage::nLIMIT_ID = 20;
		CLoginPage::nLIMIT_PW = 20;
		m_bCHANNEL = true;
		break;
	case RENDERPARAM::EMSERVICE_KOREA:
	case RENDERPARAM::EMSERVICE_INDONESIA:
		CLoginPage::nLIMIT_ID = 20;
		CLoginPage::nLIMIT_PW = 16;
		m_bCHANNEL = true;
		break;
	case RENDERPARAM::EMSERVICE_CHINA:
		CLoginPage::nLIMIT_ID = 12;
		CLoginPage::nLIMIT_PW = 19;
		m_bCHANNEL = true;
		break;

	case RENDERPARAM::EMSERVICE_JAPAN:
		CLoginPage::nLIMIT_ID = 16;
		CLoginPage::nLIMIT_PW = 16;
		m_bCHANNEL = true;
		break;

	case RENDERPARAM::EMSERVICE_GLOBAL:
		CLoginPage::nLIMIT_ID = 20;
		CLoginPage::nLIMIT_PW = 20;
		m_bCHANNEL = true;
		break;

	case RENDERPARAM::EMSERVICE_EU:
		CLoginPage::nLIMIT_ID = 200;
		CLoginPage::nLIMIT_PW = 200;
		m_bCHANNEL = true;
		break;

	case RENDERPARAM::EMSERVICE_US:
		CLoginPage::nLIMIT_ID = 20;
		CLoginPage::nLIMIT_PW = 20;
		m_bCHANNEL = true;
		break;

	default:
		CLoginPage::nLIMIT_ID = 12;
		CLoginPage::nLIMIT_PW = 12;
		m_bCHANNEL = true;
		break;
	};

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

//#ifdef CH_PARAM
	if (RENDERPARAM::emSERVICE_TYPE == RENDERPARAM::EMSERVICE_CHINA)
	{
		CSelectChinaAreaPage* pSelectChinaAreaPage = new CSelectChinaAreaPage(m_pGaeaClient, this, m_pEngineDevice);
		pSelectChinaAreaPage->CreateEx( SELECT_CHINA_AREA_PAGE, "OUTER_WINDOW" );
		pSelectChinaAreaPage->CreateBaseWidnow( "SELECT_AREA_PAGE", (char*)ID2GAMEWORD ( "SELECT_CHINA_AREA" ) );
		pSelectChinaAreaPage->CreateSubControl();
		pSelectChinaAreaPage->SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		UiRegisterControl( pSelectChinaAreaPage );	
		UiShowGroupFocus ( SELECT_CHINA_AREA_PAGE );
	}
//#endif

	{
		m_pSelectServerPage = new CSelectServerPage(m_pGaeaClient, this, m_pEngineDevice);
		m_pSelectServerPage->CreateEx ( SELECT_SERVER_PAGE, "OUTER_WINDOW" );
		m_pSelectServerPage->CreateBaseWidnow ( "SELECT_SERVER_PAGE", (char*) ID2GAMEWORD ( "SELECT_SERVER" ) );
		m_pSelectServerPage->CreateSubControl ();
		m_pSelectServerPage->SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		UiRegisterControl( m_pSelectServerPage );	
		UiShowGroupFocus ( SELECT_SERVER_PAGE );
	}

	{
		m_pLoginPage = new CLoginPage(m_pGaeaClient, this, m_pEngineDevice);
		m_pLoginPage->CreateEx ( LOGIN_PAGE, "OUTER_WINDOW" );
		m_pLoginPage->CreateBaseWidnow ( "LOGIN_PAGE", (char*) ID2GAMEWORD ( "LOGIN_PAGE" ) );
		m_pLoginPage->CreateSubControl ();
		m_pLoginPage->SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		UiRegisterControl( m_pLoginPage );
		UiShowGroupFocus ( LOGIN_PAGE );
	}

/*
//#if defined(KRT_PARAM) || defined(KR_PARAM)
#if defined ( JP_PARAM )	// 2차 패스워드
	{
		m_pSecPassSetPage = new CSecPassSetPage;
		m_pSecPassSetPage->CreateEx ( SECPASS_SETPAGE, "OUTER_WINDOW" );
		m_pSecPassSetPage->CreateBaseWidnow ( "SECPASS_SETPAGE", (char*) ID2GAMEWORD ( "SECPASS_SETPAGE" ) );
		m_pSecPassSetPage->CreateSubControl ();
		m_pSecPassSetPage->SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		UiRegisterControl( m_pSecPassSetPage );
		UiShowGroupFocus ( SECPASS_SETPAGE );
	}

	{
		m_pSecPassCheckPage = new CSecPassCheckPage;
		m_pSecPassCheckPage->CreateEx ( SECPASS_CHECKPAGE, "OUTER_WINDOW" );
		m_pSecPassCheckPage->CreateBaseWidnow ( "SECPASS_CHECKPAGE", (char*) ID2GAMEWORD ( "SECPASS_CHECKPAGE" ) );
		m_pSecPassCheckPage->CreateSubControl ();
		m_pSecPassCheckPage->SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		UiRegisterControl( m_pSecPassCheckPage );
		UiShowGroupFocus ( SECPASS_CHECKPAGE );
	}
#endif
*/

	{

		m_pMessageWindow = new CMessageWindow(m_pEngineDevice);
		m_pMessageWindow->Create ( MESSAGE_WINDOW, "BASIC_MESSAGE_DISPLAY", UI_FLAG_CENTER_X );
		m_pMessageWindow->CreateSubControl ();
		UiRegisterControl( m_pMessageWindow );
		UiShowGroupTop ( MESSAGE_WINDOW );
		UiHideGroup ( MESSAGE_WINDOW );

		m_pMessageWindowDummy = new CUIControl(m_pEngineDevice);
		m_pMessageWindowDummy->Create ( MESSAGE_WINDOW_DUMMY, "BASIC_MESSAGE_DISPLAY", UI_FLAG_CENTER_X );
        UiRegisterControl( m_pMessageWindowDummy );


	}

	{
		m_pPassKeyboard = new CPassKeyboard ( this, m_pEngineDevice );
		m_pPassKeyboard->Create( PASS_KEYBOARD, "PASS_KEYBOARD", UI_FLAG_CENTER_X );
		m_pPassKeyboard->CreateSubControl();
		UiRegisterControl( m_pPassKeyboard );
		UiShowGroupFocus( PASS_KEYBOARD );
	}

	{
		if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
		{
			// 국가 선택 UI;
			m_pSelectCountryUI = new CSelectCountryUI( m_pGaeaClient, this, m_pEngineDevice );
			m_pSelectCountryUI->Create( SELECT_COUNTRY_UI, "SELECT_COUNTRY_UI" );
			m_pSelectCountryUI->CreateSubControl();
			m_pSelectCountryUI->SetChangeSize( TRUE );
			UiRegisterControl( m_pSelectCountryUI );
			UiShowGroupFocus( SELECT_COUNTRY_UI );

			// 대륙 선택리스트 UI;
			m_pSelectContinentListUI = new CSelectContinentListUI( m_pGaeaClient, this, m_pEngineDevice );
			m_pSelectContinentListUI->CreateEx ( SELECT_CONTINENT_LIST_UI, "OUTER_WINDOW" );
			m_pSelectContinentListUI->CreateBaseWidnow ( "SELECT_CONTINENT_LIST_UI_REGION", (char*)ID2GAMEWORD("SELECT_COUNTRY_LIST_UI", 0 ) );
			m_pSelectContinentListUI->CreateSubControl();
			m_pSelectContinentListUI->SetChangeSize( TRUE );
			UiRegisterControl( m_pSelectContinentListUI );
			UiShowGroupFocus( SELECT_CONTINENT_LIST_UI );

			// 국가 선택리스트 UI;
			m_pSelectCountryListUI = new CSelectCountryListUI( m_pGaeaClient, this, m_pEngineDevice );
			m_pSelectCountryListUI->CreateEx ( SELECT_COUNTRY_LIST_UI, "OUTER_WINDOW" );
			m_pSelectCountryListUI->CreateBaseWidnow ( "SELECT_COUNTRY_LIST_UI_REGION", (char*)ID2GAMEWORD("SELECT_COUNTRY_LIST_UI", 1 ) );
			m_pSelectCountryListUI->CreateSubControl();
			m_pSelectCountryListUI->SetChangeSize( TRUE );
			UiRegisterControl( m_pSelectCountryListUI );
			UiShowGroupFocus( SELECT_COUNTRY_LIST_UI );
		}

		m_pSelectCharacterPage = new CSelectCharacterPage(m_pGaeaClient, this, m_pEngineDevice);
		m_pSelectCharacterPage->Create ( SELECT_CHARACTER_PAGE, "SELECT_CHARACTER_PAGE" );
		m_pSelectCharacterPage->CreateSubControl ();
		m_pSelectCharacterPage->SetChangeSize( TRUE );
		//m_pSelectCharacterPage->SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		UiRegisterControl( m_pSelectCharacterPage );
		UiShowGroupFocus ( SELECT_CHARACTER_PAGE );

		m_pSelectCharacterLeftPage = new CSelectCharacterLeftPage(m_pGaeaClient, this, m_pEngineDevice);
		m_pSelectCharacterLeftPage->Create ( SELECT_CHARACTER_LEFTPAGE, "SELECT_CHARACTER_LEFTPAGE" );
		m_pSelectCharacterLeftPage->CreateSubControl ();
		m_pSelectCharacterLeftPage->SetChangeSize( TRUE );		
		UiRegisterControl( m_pSelectCharacterLeftPage );
		UiShowGroupFocus ( SELECT_CHARACTER_LEFTPAGE );

        m_pCharacterCountryClubInfo = new CBasicTextBox(m_pEngineDevice);
        m_pCharacterCountryClubInfo->Create( CHARACTER_COUNTRY_CLUB_INFO, "CHARACTER_COUNTRY_CLUB_INFO" );
        m_pCharacterCountryClubInfo->SetFont( pFont9 );
        m_pCharacterCountryClubInfo->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        UiRegisterControl( m_pCharacterCountryClubInfo );
        UiShowGroupTop( CHARACTER_COUNTRY_CLUB_INFO );
	}

	{
		m_pCreateCharacterButtonPage = new CCreateChatacterButtonPage ( this, m_pEngineDevice );
		m_pCreateCharacterButtonPage->Create( CREATE_CHARACTER_BUTTON_PAGE, "CREATE_CHARACTER_BUTTON_WINDOW", UI_FLAG_DEFAULT );
		m_pCreateCharacterButtonPage->CreateSubControl();
		m_pCreateCharacterButtonPage->SetChangeSize( TRUE );
		UiRegisterControl( m_pCreateCharacterButtonPage );
		UiShowGroupFocus ( CREATE_CHARACTER_BUTTON_PAGE );
		UiHideGroup ( CREATE_CHARACTER_BUTTON_PAGE );
	}

	{
		m_pCreateCharacterWindow = new CCreateCharacterWindow(m_pGaeaClient, this, m_pEngineDevice);
		m_pCreateCharacterWindow->Create( CREATE_CHARACTER_PAGE, "CREATE_CHARACTER_WINDOW", UI_FLAG_DEFAULT );
		m_pCreateCharacterWindow->CreateSubControl();
		m_pCreateCharacterWindow->SetChangeSize( TRUE );
		UiRegisterControl( m_pCreateCharacterWindow );
		UiShowGroupFocus ( CREATE_CHARACTER_PAGE );
		UiHideGroup ( CREATE_CHARACTER_PAGE );
	}

	{
		m_pRotateChar = new CRotateCharacter(m_pGaeaClient, this, m_pEngineDevice);
		m_pRotateChar->Create( ROTATE_CHARACTER, "ROTATE_CHAR_WINDOW", UI_FLAG_DEFAULT );
		m_pRotateChar->CreateSubControl();
		UiRegisterControl( m_pRotateChar );
		UiShowGroupTop ( ROTATE_CHARACTER );
	}

//#else
/*
	{
		m_pCreateCharacterPage = new CCreateCharacterPage;
		m_pCreateCharacterPage->CreateEx ( CREATE_CHARACTER_PAGE, "OUTER_WINDOW" );
		m_pCreateCharacterPage->CreateBaseWidnow ( "CREATE_CHARACTER_PAGE", (char*) ID2GAMEWORD ( "CREATE_CHARACTER_PAGE" ) );
		m_pCreateCharacterPage->CreateSubControl ();
		m_pCreateCharacterPage->SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		UiRegisterControl( m_pCreateCharacterPage );
		UiShowGroupFocus ( CREATE_CHARACTER_PAGE );
	}
#endif 
*/
	{	// 로그인마크
        if ( m_pGaeaClient->GetServiceProvider() != SP_WORLD_BATTLE )
        {
		    m_pLoginMark = new CUIControl(m_pEngineDevice);
		    m_pLoginMark->Create ( LOGIN_MARK, "LOGIN_MARK", UI_FLAG_CENTER_X );
		    UiRegisterControl( m_pLoginMark );
		    UiShowGroupBottom ( LOGIN_MARK );
        }
        else
        {
            m_pLoginMark = new CUIControl(m_pEngineDevice);
            m_pLoginMark->Create ( LOGIN_MARK, "LOGIN_MARK_WB", UI_FLAG_CENTER_X );
            UiRegisterControl( m_pLoginMark );
            UiShowGroupBottom ( LOGIN_MARK );
        }
	}

	{	//	모달
		{
			m_pModalWindow = new CModalWindowOuter ( this, m_pEngineDevice );
			m_pModalWindow->Create ( MODAL_WINDOW_OUTER, "BASIC_MODAL_WINDOW", UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
			m_pModalWindow->CreateSubControl ();
			UiRegisterControl( m_pModalWindow );
			UiShowGroupFocus ( MODAL_WINDOW_OUTER );
		}
		//else
		//{
		//	CModalWindow* pModalWindow = new CModalWindow;
		//	pModalWindow->Create ( MODAL_WINDOW_OUTER, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		//	pModalWindow->CreateBaseModal ( "MODAL_WINDOW");
		//	pModalWindow->CreateSubControl ();
		//	pModalWindow->SetAlignFlag ( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		//	UiRegisterControl( pModalWindow );
		//	m_pModalWindow = pModalWindow;
		//	UiShowGroupFocus ( MODAL_WINDOW_OUTER );		
		//}
	}

    {	//툴팁
        m_pInfoDisplay = new CBasicVarTextBox(m_pEngineDevice);
        m_pInfoDisplay->Create( OUTER_INFO_DISPLAY, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pInfoDisplay->CreateSubControl();
        m_pInfoDisplay->SetVisibleSingle( FALSE );
        UiRegisterControl( m_pInfoDisplay );
        UiShowGroupTop( OUTER_INFO_DISPLAY );
    }

	{
		m_pUpImage = new CUIControl(m_pEngineDevice);
		m_pUpImage->Create ( OUTER_UP_IMAGE, "OUTER_UP_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );		
		UiRegisterControl( m_pUpImage );
		UiShowGroupBottom ( OUTER_UP_IMAGE );

//#ifdef CH_PARAM
		if (RENDERPARAM::emSERVICE_TYPE == RENDERPARAM::EMSERVICE_CHINA)
		{
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->Create ( CHINA_TITLE, "CHINA_TITLE", UI_FLAG_XSIZE );
			pTextBox->SetFont ( pFont9 );
			pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			pTextBox->SetLineInterval ( fLIST_LINE_INTERVAL );
			UiRegisterControl( pTextBox );		
			pTextBox->AddText ( ID2GAMEEXTEXT ( "CHINA_OUTER_UP_TITLE" ) );
			UiShowGroupBottom ( CHINA_TITLE );
		}
//#endif

		m_pDownImage = new CUIControl(m_pEngineDevice);
		m_pDownImage ->Create ( OUTER_DN_IMAGE, "OUTER_DN_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		UiRegisterControl( m_pDownImage  );
		UiShowGroupBottom ( OUTER_DN_IMAGE );

		{
			std::string strCOMPANY = ID2GAMEWORD ( "COPYRIGHT_COMPANY" );
			strCOMPANY += " ";
			strCOMPANY += sc::getFileVersion();

			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->Create ( COPYRIGHT, "COPYRIGHT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
			pTextBox->SetFont ( pFont9 );
			pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			pTextBox->SetLineInterval ( fLIST_LINE_INTERVAL );
			UiRegisterControl( pTextBox );		
			pTextBox->AddText ( ID2GAMEWORD ( "COPYRIGHT_TITLE" ) );
			pTextBox->AddText ( strCOMPANY.c_str() );
			UiShowGroupBottom ( COPYRIGHT );
		}

		{	
			CD3DFontPar* pFont12 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 12, _DEFAULT_FONT_FLAG );
			m_pTitleText = new CBasicTextBox(m_pEngineDevice);
			m_pTitleText->Create ( TITLE_TEXT, "LOBY_TITLE_TEXT", UI_FLAG_XSIZE );
			m_pTitleText->SetFont ( pFont12 );
			m_pTitleText->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
			UiRegisterControl( m_pTitleText );		
			UiShowGroupBottom ( TITLE_TEXT );
		}
	}

	{
		m_pFullScreenButton = new CUIControl(m_pEngineDevice);
		m_pFullScreenButton->Create ( FULLSCREEN_OUTER, "FULLSCREEN_OUTER", UI_FLAG_RIGHT );
		UiRegisterControl( m_pFullScreenButton );
		UiShowGroupTop ( FULLSCREEN_OUTER );

		m_pFullScreenButtonDummy = new CUIControl(m_pEngineDevice);
		m_pFullScreenButtonDummy->Create ( FULLSCREEN_OUTER_DUMMY, "FULLSCREEN_OUTER", UI_FLAG_RIGHT );
		m_pFullScreenButtonDummy->SetVisibleSingle ( FALSE );
		UiRegisterControl( m_pFullScreenButtonDummy );
	}

	{		
		m_pWaitDisplay = new CWaitDialogue ( this, m_pEngineDevice );
		m_pWaitDisplay->Create ( WAIT_DISPLAY, "WAITSERVER_DISPLAY", UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
		m_pWaitDisplay->CreateSubControl ();
		UiRegisterControl( m_pWaitDisplay );
		UiShowGroupFocus ( WAIT_DISPLAY );
	}

    {
        m_pLockSelectCharacterPreviewPage = new CLockSelectCharacterPreviewPage( m_pGaeaClient, this, m_pEngineDevice );
        m_pLockSelectCharacterPreviewPage->Create( LOCK_SELECT_CHARACTER_PREVIEW_PAGE, "LOCK_CHARACTER_PREVIEW_PAGE_BOX", UI_FLAG_DEFAULT );
        m_pLockSelectCharacterPreviewPage->CreateSubControl();
        UiRegisterControl( m_pLockSelectCharacterPreviewPage );
        UiShowGroupTop( LOCK_SELECT_CHARACTER_PREVIEW_PAGE );
    }

    if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
    {
        {
            m_pSelectCountryWindow = new CSelectCountryWindow( m_pGaeaClient, this, m_pEngineDevice );
            m_pSelectCountryWindow->CreateEx( SELECT_COUNTRY_WINDOW, "OUTER_WINDOW" );
            m_pSelectCountryWindow->CreateBaseWidnow( "SELCET_COUNTRY_WINDOW", "" );
            m_pSelectCountryWindow->CreateSubControl();
            m_pSelectCountryWindow->SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
            m_pSelectCountryWindow->SetTitleAlign( UI_FLAG_CENTER_X );
            UiRegisterControl( m_pSelectCountryWindow );
            UiShowGroupFocus( SELECT_COUNTRY_WINDOW );
            UiHideGroup( SELECT_COUNTRY_WINDOW );
        }
    }

    {
        m_pSelectCountryConfirmModal = new CSelectCountryConfirmModal( m_pGaeaClient, this, m_pEngineDevice );
        m_pSelectCountryConfirmModal->CreateEx( SELECT_COUNTRY_CONFIRM_MODAL, "OUTER_WINDOW" );
        m_pSelectCountryConfirmModal->SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y | UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pSelectCountryConfirmModal->CreateBaseWidnow( "SELCET_COUNTRY_CONFIRM_MODAL", (char*) ID2GAMEEXTEXT( "SELECT_COUNTRY_MODAL_TITLE" ) );
        m_pSelectCountryConfirmModal->CreateSubControl();
        UiRegisterControl( m_pSelectCountryConfirmModal );
        UiShowGroupFocus( SELECT_COUNTRY_CONFIRM_MODAL );
        UiHideGroup( SELECT_COUNTRY_CONFIRM_MODAL );
    }

    {
        m_pSelectClubWindow = new CSelectClubWindow( m_pGaeaClient, this, m_pEngineDevice );
        m_pSelectClubWindow->CreateEx( SELECT_CLUB_WINDOW, "OUTER_WINDOW" );
        m_pSelectClubWindow->CreateBaseWidnow( "SELCET_CLUB_WINDOW", (char*)ID2GAMEEXTEXT( "SELCET_CLUB_WINDOW_TITLE" ) );
        m_pSelectClubWindow->CreateSubControl();
        m_pSelectClubWindow->SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
        m_pSelectClubWindow->SetTitleAlign( UI_FLAG_CENTER_X );
        UiRegisterControl( m_pSelectClubWindow );
        UiShowGroupFocus( SELECT_CLUB_WINDOW );
        UiHideGroup( SELECT_CLUB_WINDOW );
    }

	CloseAllWindow ();
	
	//#ifdef CH_PARAM
	if (RENDERPARAM::emSERVICE_TYPE == RENDERPARAM::EMSERVICE_CHINA)
	{
		UiShowGroupFocus( SELECT_CHINA_AREA_PAGE );
	}
	else
	{
		//#else
		UiShowGroupFocus( SELECT_SERVER_PAGE );
	}
	//#endif

	return S_OK;
}

HRESULT COuterInterface::FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	ResetOnTheInterface ();

	UiHideGroup( OUTER_INFO_DISPLAY );

	bool bFULLSCREEN = RANPARAM::bScrWindowed;	// && RANPARAM::bScrWndHalfSize;	// 필요없는 데이터
	if ( bFULLSCREEN )	UiShowGroupTop( FULLSCREEN_OUTER );
	else				UiHideGroup( FULLSCREEN_OUTER );

	if ( UiIsVisibleGroup ( FULLSCREEN_OUTER ) )
	{
		const UIRECT& rcGlobalPos = m_pFullScreenButtonDummy->GetGlobalPos();
		m_pFullScreenButton->SetGlobalPos( D3DXVECTOR2(rcGlobalPos.left,rcGlobalPos.top) );
	}

	HRESULT hr;
	hr = uiman::GetOuterUIMan().FrameMove( fElapsedTime, this );
	if ( FAILED ( hr ) ) return hr;

	//	TIME UPDATE
	UPDATE_WAIT_TIME( fElapsedTime );

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !UiIsVisibleGroup( MODAL_WINDOW_OUTER ) )
		{
			if( !IsLoginCancel() )
			{
				DoModal( ID2GAMEEXTEXT("CHARACTERSTAGE_2"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CONNECTCLOSED );
			}
			else
			{
                if (pGlobalStage->GetNetClient()->ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) > NET_ERROR)
				{
					pGlobalStage->GetNetClient()->SndReqServerInfo();				
				}
				else
				{
					DoModal(ID2GAMEEXTEXT("SERVERSTAGE_3"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME);
				}
			}
		}
	}

    // TODO : 불법 프로그램 감지 테스트..
    /*if ( !IsBlockProgramFound() )
        SetBlockProgramFound( true );*/
    //

	if ( IsBlockProgramFound() )
	{
		DoModal ( ID2GAMEINTEXT("BLOCK_PROGRAM_FOUND"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
	}

	if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_PRESSED )
	{
		if ( DxInputDevice::GetInstance().GetKeyState ( DIK_F4 ) & DXKEY_DOWN )
		{
			DoModal ( ID2GAMEINTEXT("CLOSE_GAME"), UI::MODAL_QUESTION, UI::OKCANCEL, OUTER_MODAL_CLOSEGAME );
		}
	}

	if ( UiIsVisibleGroup ( SELECT_CHARACTER_PAGE ) )
	{
		CUIFocusContainer::UICONTROL_FOCUSLIST focus_list = uiman::GetOuterUIMan().GetFocusList()->GetFocusList();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = focus_list.rbegin ();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = focus_list.rend ();

		bool bLeftPageShow = false;
		for ( ; riter != riter_end; ++riter )
		{
			UIGUID cID = riter->cID;
			if ( SELECT_CHARACTER_PAGE == cID )
			{
				if ( bLeftPageShow )
				{
					uiman::GetOuterUIMan().GetFocusList()->InsertAfter ( SELECT_CHARACTER_LEFTPAGE, SELECT_CHARACTER_PAGE, riter->pUIControl );
					uiman::GetOuterUIMan().GetFocusList()->InsertAfter ( SELECT_COUNTRY_UI, SELECT_CHARACTER_PAGE, riter->pUIControl );
				}
				break;
			}
			else if (
				SELECT_CHARACTER_LEFTPAGE == cID ||
				SELECT_COUNTRY_UI == cID )
			{
				bLeftPageShow = true;
			}
		}
	}

	{
		// Note : 위아래 바의 크기를 조절한다. 
		LONG lResolution = CUIMan::GetResolution ();

		//if( m_lResolutionBack != lResolution )
		{
			WORD X_RES = HIWORD( lResolution );
			WORD Y_RES = LOWORD( lResolution );

			INT nHeight = static_cast<INT>(Y_RES * 0.1f);
			INT nTop = Y_RES - nHeight;

			if ( m_pUpImage && m_pDownImage )
			{
				m_pUpImage->SetGlobalPos( UIRECT( 0, 0, (float)X_RES, (float)nHeight ) );
				m_pDownImage->SetGlobalPos( UIRECT( 0, (float)nTop, (float)X_RES, (float)nHeight ) );
			}

			// Note : 캐릭터 선택, 생성 윈도우의 위치를 변경한다.
			UIRECT rcLocalPos;

			if( m_pSelectCharacterLeftPage && m_pSelectCharacterLeftPage->IsVisible() )
			{
				rcLocalPos = m_pSelectCharacterLeftPage->GetLocalPos();
				rcLocalPos.left = X_RES - rcLocalPos.sizeX;
				rcLocalPos.top = nTop - rcLocalPos.sizeY;
				m_pSelectCharacterLeftPage->SetGlobalPos( rcLocalPos );
			}

			if( m_pSelectCountryUI && m_pSelectCountryUI->IsVisible() )
			{
				rcLocalPos = m_pSelectCountryUI->GetLocalPos();
				rcLocalPos.SetPosition(
					X_RES/2 - rcLocalPos.sizeX/2,
					nTop - rcLocalPos.sizeY );
				m_pSelectCountryUI->SetGlobalPos( rcLocalPos );
			}

			if ( m_pSelectContinentListUI && m_pSelectContinentListUI->IsVisible() )
			{
				rcLocalPos = m_pSelectContinentListUI->GetLocalPos();
				rcLocalPos.SetPosition(
					X_RES/2 - rcLocalPos.sizeX,
					Y_RES/2 - rcLocalPos.sizeY/2 );
				m_pSelectContinentListUI->SetGlobalPos( rcLocalPos );
			}

			if ( m_pSelectCountryListUI && m_pSelectCountryListUI->IsVisible() )
			{
				rcLocalPos.SetPosition(
					X_RES/2,
					Y_RES/2 - rcLocalPos.sizeY/2 );
				m_pSelectCountryListUI->SetGlobalPos( rcLocalPos );
			}

			if( m_pSelectCharacterPage && m_pSelectCharacterPage->IsVisible() )
			{
				rcLocalPos = m_pSelectCharacterPage->GetLocalPos();
				rcLocalPos.left = X_RES - rcLocalPos.sizeX - 6.0f;
				rcLocalPos.top = static_cast<float>(nHeight);
				m_pSelectCharacterPage->SetGlobalPos( rcLocalPos );
			}

            if ( m_pCharacterCountryClubInfo && m_pCharacterCountryClubInfo->IsVisible() )
            {
                rcLocalPos = m_pCharacterCountryClubInfo->GetLocalPos();
                rcLocalPos.left = (X_RES - rcLocalPos.sizeX) * 0.5f;
                rcLocalPos.top = m_pSelectCharacterLeftPage->GetGlobalPos().bottom;
                m_pCharacterCountryClubInfo->SetGlobalPos( rcLocalPos );
            }
/*
			if( m_pCreateCharacterPage && m_pCreateCharacterPage->IsVisible() )
			{
				rcLocalPos = m_pCreateCharacterPage->GetLocalPos();
				rcLocalPos.left = X_RES - rcLocalPos.sizeX;
				rcLocalPos.top = nTop - rcLocalPos.sizeY - 6.0f;
				m_pCreateCharacterPage->SetGlobalPos( rcLocalPos );
			}
*/

			if( m_pCreateCharacterWindow && m_pCreateCharacterWindow->IsVisible() )
			{
				rcLocalPos = m_pCreateCharacterWindow->GetLocalPos();
				rcLocalPos.left = X_RES - rcLocalPos.sizeX - 6.0f;
				rcLocalPos.top = static_cast<float>(nHeight);
				m_pCreateCharacterWindow->SetGlobalPos( rcLocalPos );
			}

			if( m_pCreateCharacterButtonPage && m_pCreateCharacterButtonPage->IsVisible() )
			{
				rcLocalPos = m_pCreateCharacterButtonPage->GetLocalPos();
				rcLocalPos.left = X_RES - rcLocalPos.sizeX;
				rcLocalPos.top = nTop - rcLocalPos.sizeY;
				m_pCreateCharacterButtonPage->SetGlobalPos( rcLocalPos );
			}
/*
			if( m_pNewCreateCharPage && m_pNewCreateCharPage->IsVisible() )
			{
				UIRECT rcGlobalPosNew, rcGlobalPosOld;
				rcLocalPos = m_pNewCreateCharPage->GetLocalPos();

				LONG lResolutionBack = GetResolutionBack ();
				WORD X_RES_BACK = HIWORD( lResolutionBack );
				WORD Y_RES_BACK = LOWORD( lResolutionBack );

				if ( X_RES_BACK != X_RES || Y_RES_BACK != Y_RES )
				{		

					rcGlobalPosOld.left = 0;								
					rcGlobalPosOld.top = 0;
					rcGlobalPosOld.right = X_RES_BACK;
					rcGlobalPosOld.bottom = Y_RES_BACK;
					rcGlobalPosOld.sizeX = X_RES_BACK;
					rcGlobalPosOld.sizeY = Y_RES_BACK;

					rcGlobalPosNew.left = 0;			
					rcGlobalPosNew.right = X_RES;
					rcGlobalPosNew.sizeX = X_RES;
					rcGlobalPosNew.top = 0;
					rcGlobalPosNew.bottom = Y_RES;
					rcGlobalPosNew.sizeY = Y_RES;

					m_pNewCreateCharPage->ReSizeControl( rcGlobalPosOld, rcGlobalPosNew );

					SetResolutionBack ( MAKELONG(Y_RES, X_RES) );
				}
			}
*/
			// Note : 패스워드 키보드의 위치를 조정한다.
			if( m_pPassKeyboard && m_pPassKeyboard->IsVisible() )
			{
				rcLocalPos = m_pPassKeyboard->GetGlobalPos();

				if ( m_pLoginPage->IsVisible() )
					rcLocalPos.top = m_pLoginPage->GetGlobalPos().bottom + 13.0f;
/*
//#if defined(KRT_PARAM) || defined(KR_PARAM)
#if defined ( JP_PARAM )	// 2차 패스워드
				else if ( m_pSecPassSetPage->IsVisible() ) 
					rcLocalPos.top = m_pSecPassSetPage->GetGlobalPos().bottom + 13.0f;
				else if ( m_pSecPassCheckPage->IsVisible() )
					rcLocalPos.top = m_pSecPassCheckPage->GetGlobalPos().bottom + 13.0f;
#endif
*/
				m_pPassKeyboard->SetGlobalPos( rcLocalPos );
			}

			if( m_pLoginMark && m_pLoginMark->IsVisible() )
			{
				rcLocalPos = m_pLoginMark->GetLocalPos();
				rcLocalPos.left = ( X_RES - rcLocalPos.sizeX ) / 2;
				rcLocalPos.top = nHeight * 2.0f;
				m_pLoginMark->SetGlobalPos( rcLocalPos );
			}

			if( m_pTitleText && m_pTitleText->IsVisible() )
			{
				rcLocalPos = m_pTitleText->GetLocalPos();
				rcLocalPos.left = ( X_RES - rcLocalPos.sizeX ) / 2;
				rcLocalPos.top = nHeight * 0.5f;
				m_pTitleText->SetGlobalPos( rcLocalPos );
			}

			if( m_pRotateChar && m_pRotateChar->IsVisible() )
			{	
				//rcLocalPos = m_pRotateChar->GetLocalPos();
				//rcLocalPos.left = ( X_RES / 2 ) * 0.5;
				//rcLocalPos.top = nTop - rcLocalPos.sizeY;
				//m_pRotateChar->SetGlobalPos( rcLocalPos );
				UIRECT rcGlobalPosNew, rcGlobalPosOld;
				rcLocalPos = m_pRotateChar->GetLocalPos();

				LONG lResolutionBack = GetResolutionBack ();
				WORD X_RES_BACK = HIWORD( lResolutionBack );
				WORD Y_RES_BACK = LOWORD( lResolutionBack );

				if ( X_RES_BACK != X_RES || Y_RES_BACK != Y_RES )
				{		

					rcGlobalPosOld.left = 0;								
					rcGlobalPosOld.top = 0;
					rcGlobalPosOld.right = X_RES_BACK;
					rcGlobalPosOld.bottom = Y_RES_BACK;
					rcGlobalPosOld.sizeX = X_RES_BACK;
					rcGlobalPosOld.sizeY = Y_RES_BACK;

					rcGlobalPosNew.left = 0;			
					rcGlobalPosNew.right = X_RES;
					rcGlobalPosNew.sizeX = X_RES;
					rcGlobalPosNew.top = 0;
					rcGlobalPosNew.bottom = Y_RES;
					rcGlobalPosNew.sizeY = Y_RES;

					m_pRotateChar->ReSizeControl( rcGlobalPosOld, rcGlobalPosNew );

					SetResolutionBack ( MAKELONG(Y_RES, X_RES) );
				}
			}
		}
	}

	return S_OK;
}

HRESULT COuterInterface::DeleteDeviceObjects()
{
	ResetControl();
    return uiman::GetOuterUIMan().DeleteDeviceObjects ();
}

HRESULT COuterInterface::IInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return uiman::GetOuterUIMan().InitDeviceObjects( pd3dDevice );
}

HRESULT COuterInterface::IInvalidateDeviceObjects()
{
    return uiman::GetOuterUIMan().InvalidateDeviceObjects();
}

HRESULT COuterInterface::IRestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return uiman::GetOuterUIMan().RestoreDeviceObjects( pd3dDevice );
}

void COuterInterface::IRender( LPDIRECT3DDEVICEQ pd3dDevice )
{
    uiman::GetOuterUIMan().Render( pd3dDevice );
}

BOOL COuterInterface::UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep ) 
{
    return uiman::GetOuterUIMan().RegisterControl(pNewControl, bPosisionKeep);
}

BOOL COuterInterface::UiIsVisibleGroup(UIGUID ControlID)
{
    return uiman::GetOuterUIMan().IsVisibleGroup(ControlID);
}

void COuterInterface::UiShowGroupTop(UIGUID ControlID, bool bMakeMsg )
{
    uiman::GetOuterUIMan().ShowGroupTop(ControlID, bMakeMsg);
}

void COuterInterface::UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg )
{
    uiman::GetOuterUIMan().ShowGroupBottom(ControlID, bMakeMsg);
}

void COuterInterface::UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg )
{
    uiman::GetOuterUIMan().ShowGroupFocus(ControlID, bMakeMsg);
}

void COuterInterface::UiHideGroup(UIGUID ControlID, bool bMakeMsg )
{
    uiman::GetOuterUIMan().HideGroup(ControlID, bMakeMsg);
}
/*
// 중국 MMOSPEED
// 중국 네트워크 체크 프로그램 실행
void COuterInterface::RunMMOSpeed(const SUBPATH* pPath)
{
	CString str;
	//str.Format ( "%s\\%s", SUBPATH::APP_ROOT, "mmospeed.exe");
    str.Format ( "%s\\%s", pPath->AppRoot().c_str(), "mmospeed.exe");
	
	CString strCmd;
	std::string strUserID = RANPARAM::GETUSERID_DEC();

	// UserID, 게임 식별번호, 지역번호
	strCmd.Format( "%s %d %d", strUserID.c_str(), 1, RANPARAM::nChinaRegion+1 );

    HINSTANCE result = ShellExecute( NULL , "open", str.GetString() ,strCmd.GetString(), NULL, SW_SHOW );

	if( result < reinterpret_cast<HINSTANCE>(HINSTANCE_ERROR)  )
	{
		// 에러처리
		return;
	}
	
	return;
}
*/

void COuterInterface::ToggleUISelectCountry( const bool bToggle )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingSelectCountry() )
		return;

	if ( bToggle )
	{
		UiShowGroupFocus( SELECT_COUNTRY_UI );
	}
	else
	{
		UiHideGroup( SELECT_COUNTRY_UI );
	}
}

void COuterInterface::ToggleUIJoinWorldBattle( const bool bToggle )
{
	if ( bToggle )
	{
		UiShowGroupFocus( JOIN_WORLDBATTLE_BUTTON );
	}
	else
	{
		UiHideGroup( JOIN_WORLDBATTLE_BUTTON );
	}
}

// 2차비번
void COuterInterface::DoModal(const std::string& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide, BOOL bPassKey, BOOL bTwian)
{
    UIGUID RenderStage = GetRenderStage ();
	CModalWindow* pModalWindow = GetModalWindow();
	if (pModalWindow)
	{
		// UiHideGroup ( MODAL_WINDOW, true );
		if (UiIsVisibleGroup(MODAL_WINDOW_OUTER))
		{
			// GASSERT ( 0 && "이미 켜져있습니다." );
			UiHideGroup(MODAL_WINDOW_OUTER, true);
		}

        pModalWindow->SetPrevPageID ( RenderStage );
		pModalWindow->SetHide(bHide);
		pModalWindow->DoModal(strText, nModalTitle, nModalType, CallerID, bPassKey, bTwian);
		UiShowGroupFocus(MODAL_WINDOW_OUTER, true);
	}
}
////////////////////////////////////////////////

void COuterInterface::SetModalData ( int nData1, int nData2 )
{
	CModalWindow* pModalWindow = GetModalWindow ();
	if ( pModalWindow ) pModalWindow->SetModalData ( nData1, nData2 );
}

void COuterInterface::SetModalData ( D3DXVECTOR3 vData)
{
	CModalWindow* pModalWindow = GetModalWindow ();
	if ( pModalWindow ) pModalWindow->SetModalData ( vData );
}

void COuterInterface::SetSelectCharIndex( DWORD dwCharSelect, INT nCharStart ) 
{ 
    GetSelectCharacterPage()->GetSelectCharPageList()->SetSelectCharIndex( dwCharSelect, nCharStart );
}

void COuterInterface::ISetLockImagePos( const D3DXVECTOR3& vCharPos, int nIndex )
{
    if ( m_pLockSelectCharacterPreviewPage )
        m_pLockSelectCharacterPreviewPage->SetLockImagePos( vCharPos, nIndex );
}

void COuterInterface::ISetVisibleLockImage( bool bVisible, int nIndex )
{
    if ( m_pLockSelectCharacterPreviewPage )
    {
        m_pLockSelectCharacterPreviewPage->SetVisibleLockImage( bVisible, nIndex );
    }
}

void COuterInterface::SetCreateExtreme( const bool bCreateExtreme )
{
	if ( GLUseFeatures::GetInstance().IsUsingExtremeClass() )
	{
		m_bChaExtreme = bCreateExtreme;
	}
	else
	{
		m_bChaExtreme = false;
	}
}

BOOL COuterInterface::IsCreateExtreme()
{ 
	if ( GLUseFeatures::GetInstance().IsUsingExtremeClass() )
	{
		return m_bChaExtreme;
	}

	return false;
}

void COuterInterface::SetCountry( wb::EM_COUNTRY Country )
{
    m_Country = Country;

    if ( m_Country != wb::UNKNOWN_COUNTRY )
    {
        //	로그인 성공 & 캐릭터 선택 화면으로 이동
        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if ( !pGlobalStage )
        {
            GASSERT( 0 && "void CSelectCountryConfirmModal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) : pGlobalSage is null");
                return;
        }

        pGlobalStage->OnInitLogin();
        pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
		// 로그인 프로세스 확인을 위한 로그
		if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info (Country)"));

		if( RANPARAM::VALIDIDPARAM() )
			ToSelectCharacterPage( SELECT_SERVER_PAGE );
		else
			ToSelectCharacterPage( LOGIN_PAGE );
    }
}

void COuterInterface::ShowBeforeSelectModal()
{
    if ( m_pSelectCountryConfirmModal )
    {
        m_pSelectCountryConfirmModal->SetBeforeSelectState();

        UiShowGroupFocus( SELECT_COUNTRY_CONFIRM_MODAL );
    }
}

void COuterInterface::ShowAfterSelectModal( const wb::COUNTRY_INFO& CountryInfo )
{
    if ( UiIsVisibleGroup( SELECT_COUNTRY_WINDOW ) )
        UiHideGroup( SELECT_COUNTRY_WINDOW );

    if ( m_pSelectCountryConfirmModal )
    {
        m_pSelectCountryConfirmModal->SetAfterSelectState( CountryInfo );

        UiShowGroupFocus( SELECT_COUNTRY_CONFIRM_MODAL );
    }
}

void COuterInterface::ShowSelectWorldBattleClub()
{
    if ( m_pSelectCountryConfirmModal )
    {
        if ( GLUseFeatures::GetInstance().IsUsingEssentialSelectClub() )
        {
            m_pSelectCountryConfirmModal->SetEssentialSelectClub();
        }
        else
        {
            m_pSelectCountryConfirmModal->SetNonEssentialSelectClub();
        }

        UiShowGroupFocus( SELECT_COUNTRY_CONFIRM_MODAL );
    }
}

void COuterInterface::ShowCharacterClubInfo()
{
    if ( m_pCharacterCountryClubInfo )
    {
        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if ( !pGlobalStage )
            return;
        SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
        if ( !pCharInfo )
            return;

        if ( pCharInfo->m_ClubDbNum != CLUB_NULL )
        {
            std::string strClubInfo = sc::string::format( ID2GAMEEXTEXT("CHARACTER_COUNTY_CLUB_INFO"),
                pCharInfo->GetClubName(), pCharInfo->GetClubRank() );
            m_pCharacterCountryClubInfo->SetText( strClubInfo.c_str() );
        }
        else
            m_pCharacterCountryClubInfo->SetText( ID2GAMEEXTEXT("CHARACTER_COUNTY_NO_CLUB_INFO") );

        UiShowGroupTop( CHARACTER_COUNTRY_CLUB_INFO );
    }
}

void COuterInterface::ShowCountryClubCountInSelectCountryWindow()
{
    if ( m_pSelectCountryWindow )
        m_pSelectCountryWindow->ShowCountryClubCount();
}

void COuterInterface::ReloadClubList()
{
    if ( m_pSelectClubWindow )
        m_pSelectClubWindow->ReloadClubList();
}

void COuterInterface::SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order )
{
    if ( m_pSelectClubWindow )
        m_pSelectClubWindow->SetClubListOrder( Type, Order );
}

void COuterInterface::SearchClubList()
{
    if ( m_pSelectClubWindow )
        m_pSelectClubWindow->SearchClub();
}

void COuterInterface::SelectClubList( int nIndex )
{
    if ( m_pSelectClubWindow )
        m_pSelectClubWindow->SelectClubList( nIndex );
}
// 통합전장 로그인 W.B.
//void COuterInterface::LoginWorldBattle()
//{
//    if ( m_pSelectServerPage )
//        m_pSelectServerPage->LoginWorldBattle();
//}


 // 통합전장 서버 연결
BOOL COuterInterface::LoginWorldBattle ()
{
	int nServerGroup = 1;
	int nServerNumber = 0;
	int nServerChannel = 0;
	
	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    pGlobalStage->SetChannel( nServerChannel );

	//m_pInterface->SetConnectServerInfo ( nServerGroup, nServerNumber );
	SetConnectServerInfo ( nServerGroup, nServerNumber );

	m_pGaeaClient->SetConnServerGroup( nServerGroup );
	m_pGaeaClient->SetConnChannel( nServerNumber );

	//	접속할 서버 아이피,포트
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( pNetClient->ConnectGameServer ( nServerGroup, nServerNumber ) <= NET_ERROR )
	{
		//접속 실패 에러
		//m_pInterface->DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_6"), UI::MODAL_INFOMATION );					
		DoModal ( ID2GAMEEXTEXT ("SERVERSTAGE_6"), UI::MODAL_INFOMATION );					
		return FALSE;
	}
    
    m_pGaeaClient->SetServerGroup(nServerGroup);
	return TRUE;

}
