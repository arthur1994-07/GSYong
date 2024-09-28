#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Msg/GLContrlMatching.h"


#include "TournamentNextBattle.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;
using namespace MatchingSystem;

// ----------------------------------------------------------------------------

TournamentNextBattle::TournamentNextBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTimeStart(NULL)
, m_pTimeEnd(NULL)
, m_pTimeProgress(NULL)
, m_pStatus(NULL)
, m_pJoinButton(NULL)
, m_emCTFType(ENUM_CTF_TYPE_NULL)
{
}

TournamentNextBattle::~TournamentNextBattle ()
{
}

void TournamentNextBattle::CreateSubControl ()
{
	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = NULL;
		pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "COMPETITION_CTF_PAGE_BATTLE_LINBOX" );
		pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeftTop    = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

	//! 텍스트
	{
		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_DATETIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 0), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_STARTTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 1), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_ENDTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 2), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_PROGRESSTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 3), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_STATUS_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 4), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_VIEW_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 5), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "TOURNAMENT_PAGE_BATTLE_BETTING_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", 6), NS_UITEXTCOLOR::WHITE);

		m_pTimeStart	= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STARTTIME_TEXT", pFont, nAlignCenter );
		m_pTimeEnd		= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_ENDTIME_TEXT", pFont, nAlignCenter );
		m_pTimeProgress = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_PROGRESSTIME_TEXT", pFont, nAlignCenter );
		m_pStatus		= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STATUS_TEXT", pFont, nAlignCenter );
	}

	{
		CString DashBoardlist[8];
		int _DashItem = 0;
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_DATETIME_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_STARTTIME_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_ENDTIME_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_PROGRESSTIME_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_STATUS_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_VIEW_STATICTEXT_SUB";
		DashBoardlist[_DashItem++] = "TOURNAMENT_PAGE_BATTLE_BETTING_STATICTEXT_SUB";

		for(int i = 0 ; i < 7 ; i++)
		{
			pTextBox_SUB[i] = CreateStaticControl( DashBoardlist[i].GetString(), pFont, nAlignCenter );
			pTextBox_SUB[i]->SetOneLineText( ID2GAMEWORD("TOURNAMENT_BATTLE_STATIC", i), NS_UITEXTCOLOR::WHITE);
		}
	}
	
}
void TournamentNextBattle::OnSetDateTime(DWORD _time)
{
	CString strTemp;
	strTemp.Format(ID2GAMEWORD("TOURNAMENT_START_DATE",0), _time);
	pTextBox_SUB[0]->SetOneLineText( strTemp , NS_UITEXTCOLOR::WHITE);
}

void TournamentNextBattle::OnSetStartTime(DWORD _timeH,DWORD _timeM)
{
	CString strTemp;
	strTemp.Format("%02d : %02d", _timeH ,_timeM );
	pTextBox_SUB[1]->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE);
}

void TournamentNextBattle::OnSetEndTime(DWORD _timeH,DWORD _timeM)
{
	CString strTemp;
	strTemp.Format("%02d : %02d", _timeH ,_timeM );
	pTextBox_SUB[2]->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE);
}

void TournamentNextBattle::OnSetProgressTime(DWORD _timeH,DWORD _timeM)
{
	CString strTemp;
	strTemp.Format("%02d : %02d", _timeH ,_timeM );
	pTextBox_SUB[3]->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE);
}

void TournamentNextBattle::OnSetStatus(MATCHING_CONTENTS_STATUS _Status)
{
	CString strTemp;
	DWORD dwColor;
	switch(_Status)
	{
	case MatchingSystem::EMMATCHING_CONTENTS_OPENED:
		dwColor= NS_UITEXTUTIL::LIGHTGREEN;
		break;
	//case MatchingSystem::EMTORNAMENTSTATUS_PENDING_APPLICATION:
	//	dwColor= NS_UITEXTUTIL::LIGHTGREEN;
		break;
	case MatchingSystem::EMMATCHING_CONTENTS_STANDBY_JOIN:
		dwColor= NS_UITEXTUTIL::ORENGE;
		break;
	case MatchingSystem::EMMATCHING_CONTENTS_CLOSED:
		dwColor= NS_UITEXTUTIL::RED;
		break;
	case MatchingSystem::EMMATCHING_CONTENTS_IN_PROGRESS:
		dwColor= NS_UITEXTUTIL::WHITE;
		break;
	default:
		return;
	}

	int nWordIndex = _Status;
	pTextBox_SUB[4]->SetOneLineText( ID2GAMEWORD("TOURNAMENT_EM_TORNAMENT_STATUS", nWordIndex), dwColor);
}

void TournamentNextBattle::OnSetView(bool isCanObserve)
{

	//ITEM_INFO_TRADE_OPTION
	if (isCanObserve)
	{
		pTextBox_SUB[5]->SetOneLineText( ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0), NS_UITEXTCOLOR::WHITE);
	}
	else
	{
		pTextBox_SUB[5]->SetOneLineText( ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1), NS_UITEXTCOLOR::RED);
	}
	
}

void TournamentNextBattle::OnSetBetting(bool _IsCanTOTO)
{
	//ITEM_INFO_TRADE_OPTION
	if (_IsCanTOTO)
	{
		pTextBox_SUB[6]->SetOneLineText( ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0), NS_UITEXTCOLOR::WHITE);
	}
	else
	{
		pTextBox_SUB[6]->SetOneLineText( ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1), NS_UITEXTCOLOR::RED);
	}
}

void TournamentNextBattle::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	Update();
}

void TournamentNextBattle::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case CTF_BATTLE_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_fnCallbackButton.empty() )
				{
					break;
				}

				m_fnCallbackButton();
			}
		}
		break;
	}

}

void TournamentNextBattle::Update ()
{
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	GLCaptureTheFieldClient::CTFGLOBALSTATE& sState = pCTFClient->GetGlobalState( m_emCTFType );

	if ( sState.IsNULL() )
	{
		return;
	}

	GLCharacter* pchar = m_pGaeaClient->GetCharacter();
	if(!pchar)
		return;


	// 시작시간
	{
		CTime cTime( sState.tSTARTTIME );
		m_pGaeaClient->GetConvertServerTime( cTime );
		m_pTimeStart->SetOneLineText( sc::string::format("%02d : %02d", cTime.GetHour(), cTime.GetMinute()).c_str() );
	}

	// 종료시간
	{
		CTime cTime( sState.tSTARTTIME + sState.tPLAYTIME );
		m_pGaeaClient->GetConvertServerTime( cTime );
		m_pTimeEnd->SetOneLineText( sc::string::format("%02d : %02d", cTime.GetHour(), cTime.GetMinute()).c_str() );
	}

	// 진행상태
	{
		switch( sState.emSTATE )
		{
			// 남은시간을 표시
		case CTF_STATE_STOP:
		case CTF_STATE_READY:
			{
				CTime tCurTime( m_pGaeaClient->GetCurrentTime() );

				int nServerMinute = (int) tCurTime.GetMinute();
				static int nServerMinuteBACK;
				if ( nServerMinute != nServerMinuteBACK )
				{
					nServerMinuteBACK = nServerMinute;

					//	서버 타임으로 변환( TimeZone 계산 )
					m_pGaeaClient->GetConvertServerTime( tCurTime );

					CTimeSpan sTimeLeft( sState.tSTARTTIME - tCurTime.GetTime() );

					int nTotalMinutes = (int)(60 * sTimeLeft.GetTotalSeconds() / 3600);
					int nTimeHour = nTotalMinutes / 60;
					int nTimeMinutes = nTotalMinutes % 60;

					std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 8), nTimeHour, nTimeMinutes);
					m_pTimeProgress->SetOneLineText( strFormat.c_str(), NS_UITEXTCOLOR::WHITE );
				}
			}
			break;

			// 진행중
		case CTF_STATE_START:
		case CTF_STATE_REWARD:
		case CTF_STATE_DONE:
			{
				m_pTimeProgress->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
			}
			break;
		};
	}

	// 신청상태
	{
		if ( sState.IsJOINDENIED(pchar) )
		{
			m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
			goto _STATUS_END;
		}

		switch( sState.emSTATE )
		{
		case CTF_STATE_REWARD:
		case CTF_STATE_DONE:
		case CTF_STATE_STOP:
			{
				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
			}
			break;

		case CTF_STATE_READY:
		case CTF_STATE_START:
			{
				if ( pCTFClient->GetProgressCTF() == m_emCTFType )
				{
					m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 4), NS_UITEXTCOLOR::WHITE );
				}
				else
				{
					if ( sState.bWAITJOIN )
					{
						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 5), NS_UITEXTCOLOR::ORANGE );
					}
					else
					{
						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 6), NS_UITEXTCOLOR::GREENYELLOW );
					}
				}
			}
			break;
		};
	}

_STATUS_END:

	//버튼
	{
		std::string strButtonLabel("");
		bool bEnable = true;

		

		if(sState.bConsecutivelyCapturel == FALSE)
		{
			if(sState.emSTATE == CTF_STATE_START)
			{
				GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = pCTFClient->GetPrevState( m_emCTFType );

				if (! sPrevState.IsNULL() )
				{
					if(sPrevState.emAUTHSCHOOL == pchar->GetCharSchool() 
						&& sState.bConsecutivelyCapturel == FALSE
						&& sState.bisConsecutively == FALSE)
					{
						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
						bEnable = false;	
					}
				}
			}
			else if( sState.emAUTHSCHOOL != SCHOOL_NONE )
			{
				if (! sState.IsNULL() )
				{
					if(sState.emAUTHSCHOOL == pchar->GetCharSchool() 
						&& sState.bConsecutivelyCapturel == FALSE
						&& sState.bisConsecutively == FALSE)
					{
						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
						bEnable = false;	
					}
				}
			}

		}


		if(bEnable == true)
		{
			if ( sState.IsCANNOTJOIN(pchar->GetCharSchool()))
			{
				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
				bEnable = false;

			}
			else
			{
				bEnable = true;
			}
		}
		EnableJoinButton( strButtonLabel, bEnable );
	}

//_BUTTON_END: 참조되지 않은 레이블입니다.

	return;
}

void TournamentNextBattle::EnableJoinButton( const std::string& strLabel, const bool bEnable )
{
	if ( bEnable )
	{
		m_pJoinButton->SetOneLineText( strLabel.c_str() );
		m_pJoinButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
		m_pJoinButton->SetDiffuseAlpha( 0xFF );
		m_pJoinButton->SetNoMessage( FALSE );
	}
	else
	{
		m_pJoinButton->SetOneLineText( strLabel.c_str() );
		m_pJoinButton->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
		m_pJoinButton->SetDiffuseAlpha( 0xDD );
		m_pJoinButton->SetNoMessage( TRUE );
	}
}


CBasicTextBox* TournamentNextBattle::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}