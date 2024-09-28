#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"

// #include "../../../../RanLogic/Party/GLPartyDefine.h"
// #include "../../../../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../Util/RnProgressBar.h"
#include "../../Util/RnButton.h"

#include "ExpeditionCheckReadyWindow.h"

// MODAL_BUTTON 3 아니오;
// MODAL_BUTTON 2 예;
// EXPEDITION_CHECK_READY_TIME 확인 시간;


// EXPEDITION_CHECK_READY_TITLE
// EXPEDITION_CHECK_READY_CONTENT


ExpeditionCheckReadyWindow::ExpeditionCheckReadyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_dwElapsedTime(GLCONST_CHAR::dwExpeditionCheckReadyTime)
{
}
ExpeditionCheckReadyWindow::~ExpeditionCheckReadyWindow(void)
{
}
void ExpeditionCheckReadyWindow::CreateSubControl(void)
{
	CreateLineBox("EXPEDITION_CHECK_READY_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
//	CreateLineBox("EXPEDITION_CHECK_READY_WINDOW_CONTENT", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	RnProgressBar::CreateArg arg;
	arg.controlName = "EXPEDITION_CHECK_READY_WINDOW_CONTENT";
	arg.backgroundTextureName = "PARTY_TENDER_ITEM_PROGRESS_BG_TEXTURE";
	arg.overTextureName = "PARTY_TENDER_ITEM_PROGRESS_OVER_TEXTURE";

	m_pProgressBar = new RnProgressBar( m_pEngineDevice );
	m_pProgressBar->CreateSub( this, "EXPEDITION_CHECK_READY_WINDOW_CONTENT_REGION", UI_FLAG_DEFAULT);
	m_pProgressBar->CreateSubControl(arg);
	RegisterControl(m_pProgressBar);

	m_pCheckReadyContent = CreateText(
		ID2GAMEINTEXT("EXPEDITION_CHECK_READY_CONTENT"), 
		"EXPEDITION_CHECK_READY_WINDOW_CONTENT_TEXT_REGION", 
		TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

	{ // 네/아니오 버튼;
		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
		{ // 네 버튼;
			Arg.text = ID2GAMEWORD( "MODAL_BUTTON", 2);

			m_pButtonYes = new RnButton(m_pEngineDevice);
			m_pButtonYes->CreateSub(this,"EXPEDITION_CHECK_READY_BUUTON_YES", UI_FLAG_DEFAULT, BUTTON_YES);
			m_pButtonYes->CreateSubControl(Arg);
			RegisterControl(m_pButtonYes);
		}

		{ // 아니오 버튼;		
			Arg.text = ID2GAMEWORD( "MODAL_BUTTON", 3);

			m_pButtonNo = new RnButton(m_pEngineDevice);
			m_pButtonNo->CreateSub(this,"EXPEDITION_CHECK_READY_BUTTON_NO", UI_FLAG_DEFAULT, BUTTON_NO);
			m_pButtonNo->CreateSubControl(Arg);
			RegisterControl(m_pButtonNo);
		}
	}	
}

void ExpeditionCheckReadyWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	m_dwElapsedTime -= DWORD(fElapsedTime * 100.0f);

	m_pProgressBar->SetMax(GLCONST_CHAR::dwExpeditionCheckReadyTime);
	m_pProgressBar->SetNow(m_dwElapsedTime);

	if ( m_dwElapsedTime > GLCONST_CHAR::dwExpeditionCheckReadyTime )
		SetVisibleSingle(FALSE);
}

void ExpeditionCheckReadyWindow::TranslateUIMessage_CheckReady(const bool bReady)
{	
	CUIWindowEx::SetVisibleSingle(false);
	m_dwElapsedTime = GLCONST_CHAR::dwExpeditionCheckReadyTime + 1;	
	m_pGaeaClient->GetPartyManager()->CheckReadyAnswer(bReady);
}

void ExpeditionCheckReadyWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);

	switch ( ControlID )
	{
	case BUTTON_YES:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_CheckReady(true);
		break;
	case BUTTON_NO:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_CheckReady(false);
		break;
	}
}
void ExpeditionCheckReadyWindow::SetVisibleSingle(BOOL bVisible)
{
	if ( bVisible == FALSE )
	{
		if ( m_dwElapsedTime < GLCONST_CHAR::dwExpeditionCheckReadyTime )
			return;

		m_pGaeaClient->GetPartyManager()->ResetCheckReady();
		TranslateUIMessage_CheckReady(false);
	}
	
	const BOOL bMaster = m_pGaeaClient->GetPartyManager()->isExpeditionMaster();
	m_pButtonYes->SetVisibleSingle(!bMaster);
	m_pButtonNo->SetVisibleSingle(!bMaster);
	
	CUIWindowEx::SetVisibleSingle(bVisible);
	m_dwElapsedTime = GLCONST_CHAR::dwExpeditionCheckReadyTime;	
}





MyExpeditionCheckReadyWindow::MyExpeditionCheckReadyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: ExpeditionCheckReadyWindow(pGaeaClient, pInterface, pEngineDevice)
{
}
void MyExpeditionCheckReadyWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(EXPEDITION_CHECK_READY_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray( "EXPEDITION_CHECK_READY_WINDOW", (char*)ID2GAMEINTEXT("EXPEDITION_CHECK_READY_TITLE") );	
	CreateSubControl();
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupFocus(EXPEDITION_CHECK_READY_WINDOW);
}
