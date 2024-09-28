#include "StdAfx.h"
#include "ClubAnnounce.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

#include "../Interface/BasicChat.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int nLIMIT_TEXT_LINE = EMCLUB_NOTICE_LEN_LINE;
 
CClubAnnounce::CClubAnnounce(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pTextBox(NULL)
	, m_pMultiEditBox(NULL)
{
}

CClubAnnounce::~CClubAnnounce ()
{
}

void CClubAnnounce::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// 뒷 배경
	{
		CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "CLUB_ANNOUNCE_LINEBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

		pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "CLUB_ANNOUNCE_LINETEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
	}

	//	텍스트 박스
	{
		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub ( this, "CLUB_ANNOUNCE_TEXTBOX", UI_FLAG_DEFAULT, CLUB_ANNOUNCE_TEXTBOX );
		pTextBox->SetFont ( pFont );
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;

		//int nTotalLine = pTextBox->GetVisibleLine ();

		////	스크롤바
		//CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pRenderDevice);
		//pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		//pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		//pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		//RegisterControl ( pScrollBar );
		//m_pScrollBarEx = pScrollBar;
	}

	{
		CUIMultiEditBox* pMultiEditBox = new CUIMultiEditBox(m_pEngineDevice);
		pMultiEditBox->CreateSub( this, "CLUB_ANNOUNCE_EDITBOX" );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L01, "CLUB_ANNOUNCE_EDITBOX_L01", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L02, "CLUB_ANNOUNCE_EDITBOX_L02", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L03, "CLUB_ANNOUNCE_EDITBOX_L03", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L04, "CLUB_ANNOUNCE_EDITBOX_L04", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L05, "CLUB_ANNOUNCE_EDITBOX_L05", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L06, "CLUB_ANNOUNCE_EDITBOX_L06", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L07, "CLUB_ANNOUNCE_EDITBOX_L07", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L08, "CLUB_ANNOUNCE_EDITBOX_L08", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L09, "CLUB_ANNOUNCE_EDITBOX_L09", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->CreateEditBox( CLUB_ANNOUNCE_EDITBOX_L10, "CLUB_ANNOUNCE_EDITBOX_L10", "CLUB_ANNOUNCE_CARRAT", TRUE, UINT_MAX, pFont );
		pMultiEditBox->SetLimitInput( nLIMIT_TEXT_LINE );
		RegisterControl( pMultiEditBox );
		m_pMultiEditBox = pMultiEditBox;
		m_pMultiEditBox->SetVisibleSingle( FALSE );
	}
}

void CClubAnnounce::SetAnnouncement()
{
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return;
	m_pTextBox->ClearText();
	m_pTextBox->AddTextWithCRLF(pMyClub->m_szNotice);
}

void CClubAnnounce::SetVisibleSingle ( BOOL bVisible ) // MEMO
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		SetAnnouncement ();
	}
}

void CClubAnnounce::BeginEdit()
{
	m_pTextBox->SetVisibleSingle( FALSE );
	m_pMultiEditBox->SetVisibleSingle( TRUE );

	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->Init ();
	m_pMultiEditBox->EndEdit ();
	m_pMultiEditBox->BeginEdit ();

	CString strNotice = m_pTextBox->GetText(0);
	m_pMultiEditBox->SetText( strNotice );
}

void CClubAnnounce::EndEdit(BOOL bOK)
{
	if (bOK)
	{
		// 확인
		CString strNotice;
		m_pMultiEditBox->GetText(strNotice);

		// 공지 설정
		//GLCharacter *pCharacter = m_pGaeaClient->GetCharacter();
		//pCharacter->ReqClubNotice( strNotice );
        m_pGaeaClient->ReqClubNotice(std::string(strNotice.GetString()));
		m_pTextBox->ClearText();
	}

	m_pTextBox->SetVisibleSingle(TRUE);
	m_pMultiEditBox->SetVisibleSingle(FALSE);
	m_pMultiEditBox->EndEdit();
}

void CClubAnnounce::ClearText()
{
	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->EndEdit();
	m_pMultiEditBox->Init();
	m_pMultiEditBox->BeginEdit();
}