#include "StdAfx.h"


#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

#include "TournamentMulty.h"
#include "TournamentObserveWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

TournamentObserveWindow::TournamentObserveWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroupHelperNonClose(pEngineDevice,pInterface)
	, m_pGaeaClient(pGaeaClient) , m_timer(0) , m_SavedTime(0), m_isrevers(false)
	, m_nUnitsDigitMin(0),m_nTensDigitMin(0) , m_nUnitsDigitSec(0) , m_nTensDigitSec(0) , m_fElapsedTime(0)
	, m_pOk(NULL)
{

}

TournamentObserveWindow::~TournamentObserveWindow()
{
}

bool TournamentObserveWindow::CreateSubControl()
{
	CUIGroupHelperNonClose::CreateLightGrayMove("TOURNAMENT_OBSERVE");
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9 , _DEFAULT_FONT_SHADOW_FLAG);
	CD3DFontPar* pFontBIG_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 12 , _DEFAULT_FONT_SHADOW_FLAG);

	const int nAlignLeftTop     = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter  = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightCenter = TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignRightTop    = TEXT_ALIGN_RIGHT | TEXT_ALIGN_TOP;	
	const int nAlignCenter	    = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;

	CreateLineBox("TOURNAMENT_OBSERVE", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE");

	if (m_pOk == 0)
	{
		m_pOk = CreateButton("TOURNAMENT_OBSERVE_CANSEL_BUTTON", ID2GAMEWORD("CTF_RESULT_BUTTON", 4), CONTROL_ID_BACK);
		m_pOk->SetVisibleSingle(TRUE);
	}

	m_pMatchingLabel = CreateStaticControl("TOURNAMENT_OBSERVE_TEXT", pFont_Shadow, nAlignCenter,NO_ID);
	m_pMatchingLabel->SetOneLineText("OBSERVE", NS_UITEXTCOLOR::WHITE);

	m_pMatchingTimerLabel = CreateStaticControl("TOURNAMENT_OBSERVE_TIMERTEXT", pFont_Shadow, nAlignCenter,NO_ID);
	m_pMatchingTimerLabel->SetOneLineText("11:11", NS_UITEXTCOLOR::ORANGE);

	m_pTimerImage = new CSwapImage(m_pEngineDevice);
	m_pTimerImage->CreateSub ( this, "TOURNAMENT_OBSERVE_TIMER", UI_FLAG_DEFAULT );
	m_pTimerImage->SetImage( "STAGE_TIMER_IMAGE" );
	RegisterControl ( m_pTimerImage );

	return true;
}

void TournamentObserveWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelperNonClose::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	m_fElapsedTime += fElapsedTime;
	int _sec = (int)m_fElapsedTime;

	SetGameTime(m_timer + _sec);

	if(IsVisible())
	{
		if(m_pOk->IsVisible() == FALSE)
			m_pOk->SetVisibleSingle(TRUE);	
	}

}


void TournamentObserveWindow::SetText( const char* strText )
{
	m_pMatchingLabel->SetOneLineText(ID2GAMEINTEXT(strText, 0), NS_UITEXTCOLOR::WHITE);
}


void TournamentObserveWindow::SetTimer( int _timer, bool revers )
{
	m_timer = _timer;
	m_isrevers = revers;
	m_fElapsedTime = 0;

	SetGameTime(_timer);	
}

void TournamentObserveWindow::SetGameTime(int nTime)
{
	int nMin = nTime / 60;
	int nSec = nTime % 60;
	CString strTempLevel;
	strTempLevel.Format("%02d:%02d",nMin,nSec);

	m_pMatchingTimerLabel->SetOneLineText(strTempLevel, NS_UITEXTCOLOR::ORANGE);
}

void TournamentObserveWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelperNonClose::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CONTROL_ID_BACK:
		{
			if (CHECK_MOUSE_IN(dwMsg) && CHECK_LB_UP_LIKE(dwMsg))
			{
				if (m_pInterface)
				{
					m_pInterface->UiHideGroup(COMPETITION_TOURNAMENT_OBSERVE);
					MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();
					glTour->OutObserveTournament();
					
				}
			}
		}
		break;
	}
}

CBasicTextButton* TournamentObserveWindow::CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id)
{
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);

	if (pButton == 0)
	{
		return pButton;
	}

	pButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, id);
	pButton->CreateBaseButton(name, CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, text);
	RegisterControl(pButton);

	return pButton;
}



CBasicLineBoxSmart* TournamentObserveWindow::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );
	return pLineBox;
}

CBasicTextBox* TournamentObserveWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}