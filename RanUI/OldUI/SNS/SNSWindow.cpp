
// bjju.sns

#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"

#include "../../InnerInterface.h"
#include "../../ModalCallerID.h"
#include "../Util/ModalWindow.h"
#include "../Util/UIWindowEx.h"

#include "./SNSWindow.h"
#include "../Util/UIWindowObject.h"
#include "../Util/UIPageFrame.h"
#include "../Util/UIPage.h"

#include "./SNSMenuPage.h"
#include "./SNSFacebookPage.h"
#include "./SNSTwitterPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSNSWindow::CSNSWindow (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowObjectController (pInterface, pEngineDevice)
    , m_pGaeaClient             (pGaeaClient)
    , m_pUIObject		        ( NULL )
	, m_pPage_Menu	            ( NULL )
	, m_pPage_Facebook          ( NULL )
{
	// Blank
}

CSNSWindow::~CSNSWindow ()
{
	// Blank
}

void CSNSWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 윈도우 오브젝트
	{
		m_pUIObject = new CUIWindowObject(m_pInterface, m_pEngineDevice);
		m_pUIObject->Create					( SNS_WINDOW_OBJECT, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pUIObject->CreateBaseWindowBlack	( "SNS_WINDOW_OBJECT", "" );
		m_pUIObject->CreateSubControl		( pd3dDevice );
		m_pUIObject->SetAlignFlag			( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pUIObject->SetVisibleSingle		( FALSE );
		m_pUIObject->SetFlag				( CUIWindowObject::FLAG_PAGE_CHANGE_RESIZE_WINDOW_ANIMATION | CUIWindowObject::FLAG_PAGE_CHANGE_ANIMATION );
	
		if ( GLCONST_SNS::bANIMATION_FLAG )
		{
			m_pUIObject->SetWindowAniType		( CUIWindowObject::WINDOW_ANIMATION_LINEAR	);
			m_pUIObject->SetWindowAniSpeed		( 2.0f );
			m_pUIObject->SetPageAniType			( CUIPageFrame::PAGE_ANIMATION_ALPHA );
			m_pUIObject->SetPageAniSpeed		( 0.8f );
		}

		WindowRegister( m_pUIObject, SNS_WINDOW_OBJECT );
	}

	// Note : 페이지
	{
		m_pPage_Menu = new CSNSMenuPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Menu->Create ( "SNS_WINDOW_PAGE_MENU", ID2GAMEWORD("SNS_PAGE_TITLE", 0), SNS_WINDOW_PAGE_MENU );
		PageRegister( m_pPage_Menu );

		m_pPage_Facebook = new CSNSFacebookPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Facebook->Create ( "SNS_WINDOW_PAGE_FACEBOOK", ID2GAMEWORD("SNS_PAGE_TITLE", 1), SNS_WINDOW_PAGE_FACEBOOK );
		PageRegister( m_pPage_Facebook );

		m_pPage_Twitter = new CSNSTwitterPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pPage_Twitter->Create ( "SNS_WINDOW_PAGE_TWITTER", ID2GAMEWORD("SNS_PAGE_TITLE", 2), SNS_WINDOW_PAGE_TWITTER );
		PageRegister( m_pPage_Twitter );
	}

	SetOpenWindow	( SNS_WINDOW_OBJECT );
	SetOpenPage		( m_pPage_Menu );
}

CBasicLineBox* CSNSWindow::CreateBackgroundControl ( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicTextBox* CSNSWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CSNSWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowObjectController::SetVisibleSingle( bVisible );
}


void CSNSWindow::DoModalError ( const std::string& strError )
{
	m_pInterface->DoModal( strError, MODAL_ERROR, OK );
}

void CSNSWindow::DoModalMsg   ( const std::string& strMsg )
{
	m_pInterface->DoModal( strMsg, MODAL_INFOMATION, OK );
}

