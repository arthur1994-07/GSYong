#include "StdAfx.h"
#include "./RnCDMBattle.h"

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

#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

RnCDMBattle::RnCDMBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pTimeStart(NULL)
, m_pTimeEnd(NULL)
, m_pTimeProgress(NULL)
, m_pStatus(NULL)
, m_pJoinButton(NULL)
, m_emCTFType(ENUM_CTF_TYPE_NULL)
, m_nStartTime(0)
, m_nEndTime(0)
{
}

RnCDMBattle::~RnCDMBattle ()
{
}

void RnCDMBattle::CreateSubControl ()
{
	//! 라인박스
	{
		CBasicLineBoxSmart* pLineBox = NULL;
		pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
		pLineBox->CreateSub( this, "COMPETITION_CTF_PAGE_BATTLE_LINBOX" );
		pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO" );
		RegisterControl( pLineBox );
	}

	//! 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		const int nAlignLeftTop    = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
		const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
		const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_TITLE_STATICTEXT", pFont, nAlignLeftTop );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_STATIC", 0), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STARTTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_STATIC", 1), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_ENDTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_STATIC", 2), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_PROGRESSTIME_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_STATIC", 3), NS_UITEXTCOLOR::WHITE);

		pTextBox = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STATUS_STATICTEXT", pFont, nAlignCenter );
		pTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_STATIC", 4), NS_UITEXTCOLOR::WHITE);

		m_pTimeStart	= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STARTTIME_TEXT", pFont, nAlignCenter );
		m_pTimeEnd		= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_ENDTIME_TEXT", pFont, nAlignCenter );
		m_pTimeProgress = CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_PROGRESSTIME_TEXT", pFont, nAlignCenter );
		m_pStatus		= CreateStaticControl( "COMPETITION_CTF_PAGE_BATTLE_STATUS_TEXT", pFont, nAlignCenter );
	}

	//! 참여/취소 버튼
	{
		m_pJoinButton = new CBasicTextButton(m_pEngineDevice);
		m_pJoinButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, RNCDM_BATTLE_BUTTON);
		m_pJoinButton->CreateBaseButton( "COMPETITION_CTF_PAGE_BATTLE_BUTTON", CBasicTextButton::SIZE22, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
		RegisterControl( m_pJoinButton );
	}
}

void RnCDMBattle::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	Update();
}

void RnCDMBattle::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case RNCDM_BATTLE_BUTTON:
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

void RnCDMBattle::Update ()
{

	// 시작시간
	{
		//CTime cTime( sState.tSTARTTIME );
		//m_pGaeaClient->GetConvertServerTime( cTime );
		m_pTimeStart->SetOneLineText( sc::string::format("%d : %d", m_nStartTime, m_nStartTime).c_str() );
	}

	// 종료시간
	{
		//CTime cTime( sState.tSTARTTIME + sState.tPLAYTIME );
		//m_pGaeaClient->GetConvertServerTime( cTime );
		m_pTimeEnd->SetOneLineText( sc::string::format("%d : %d", m_nEndTime, m_nEndTime).c_str() );
	}

	// 진행 시간
	{
		m_pTimeProgress->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
	}
//
//	// 진행상태
//	{
//		switch( sState.emSTATE )
//		{
//			// 남은시간을 표시
//		case CTF_STATE_STOP:
//		case CTF_STATE_READY:
//			{
//				CTime tCurTime( m_pGaeaClient->GetCurrentTime() );
//
//				int nServerMinute = (int) tCurTime.GetMinute();
//				static int nServerMinuteBACK;
//				if ( nServerMinute != nServerMinuteBACK )
//				{
//					nServerMinuteBACK = nServerMinute;
//
//					//	서버 타임으로 변환( TimeZone 계산 )
//					m_pGaeaClient->GetConvertServerTime( tCurTime );
//
//					CTimeSpan sTimeLeft( sState.tSTARTTIME - tCurTime.GetTime() );
//
//					int nTotalMinutes = (int)(60 * sTimeLeft.GetTotalSeconds() / 3600);
//					int nTimeHour = nTotalMinutes / 60;
//					int nTimeMinutes = nTotalMinutes % 60;
//
//					std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 8), nTimeHour, nTimeMinutes);
//					m_pTimeProgress->SetOneLineText( strFormat.c_str(), NS_UITEXTCOLOR::WHITE );
//				}
//			}
//			break;
//
//			// 진행중
//		case CTF_STATE_START:
//		case CTF_STATE_REWARD:
//		case CTF_STATE_DONE:
//			{
//				m_pTimeProgress->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
//			}
//			break;
//		};
//	}
//
//	// 신청상태
//	{
//		if ( sState.IsJOINDENIED(pchar) )
//		{
//			m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
//			goto _STATUS_END;
//		}
//
//		switch( sState.emSTATE )
//		{
//		case CTF_STATE_REWARD:
//		case CTF_STATE_DONE:
//		case CTF_STATE_STOP:
//			{
//				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
//			}
//			break;
//
//		case CTF_STATE_READY:
//		case CTF_STATE_START:
//			{
//				if ( pCTFClient->GetProgressCTF() == m_emCTFType )
//				{
//					m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 4), NS_UITEXTCOLOR::WHITE );
//				}
//				else
//				{
//					if ( sState.bWAITJOIN )
//					{
//						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 5), NS_UITEXTCOLOR::ORANGE );
//					}
//					else
//					{
//						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 6), NS_UITEXTCOLOR::GREENYELLOW );
//					}
//				}
//			}
//			break;
//		};
//	}
//
//_STATUS_END:
//
//	//버튼
//	{
//		std::string strButtonLabel("");
//		bool bEnable = true;
//
//		if ( sState.IsJOINDENIED(pchar) )
//		{
//			EnableJoinButton( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0), false );
//			m_fnCallbackButton = boost::bind( &RnCDMBattle::ReqJoin, this );
//			goto _BUTTON_END;
//		}
//
//		// 이미 참여중이라면
//		if ( pCTFClient->GetProgressCTF() == m_emCTFType )
//		{
//			strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 2);
//			m_fnCallbackButton = boost::bind( &RnCDMBattle::ReqExit, this );
//		}
//		else
//		{
//			if ( sState.bWAITJOIN )
//			{
//				strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 1);
//				m_fnCallbackButton = boost::bind( &RnCDMBattle::ReqCancel, this );
//			}
//			else
//			{
//				strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0);
//				m_fnCallbackButton = boost::bind( &RnCDMBattle::ReqJoin, this );
//			}
//		}
//
//		if(sState.bConsecutivelyCapturel == FALSE)
//		{
//			if(sState.emSTATE == CTF_STATE_START)
//			{
//				GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = pCTFClient->GetPrevState( m_emCTFType );
//
//				if (! sPrevState.IsNULL() )
//				{
//					if(sPrevState.emAUTHSCHOOL == pchar->GetCharSchool() 
//						&& sState.bConsecutivelyCapturel == FALSE
//						&& sState.bisConsecutively == FALSE)
//					{
//						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
//						bEnable = false;	
//					}
//				}
//			}
//			else if( sState.emAUTHSCHOOL != SCHOOL_NONE )
//			{
//				if (! sState.IsNULL() )
//				{
//					if(sState.emAUTHSCHOOL == pchar->GetCharSchool() 
//						&& sState.bConsecutivelyCapturel == FALSE
//						&& sState.bisConsecutively == FALSE)
//					{
//						m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
//						bEnable = false;	
//					}
//				}
//			}
//
//		}
//
//
//		if(bEnable == true)
//		{
//			if ( sState.IsCANNOTJOIN(pchar->GetCharSchool()))
//			{
//				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
//				bEnable = false;
//
//			}
//			else
//			{
//				bEnable = true;
//			}
//		}
//		EnableJoinButton( strButtonLabel, bEnable );
//	}
//
//_BUTTON_END:

	return;
}

void RnCDMBattle::EnableJoinButton( const std::string& strLabel, const bool bEnable )
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

void RnCDMBattle::ReqJoin()
{
	if ( m_pGaeaClient )
	{
		if (m_pGaeaClient->GetCharacter()->IsDie())
			return; 
		m_pGaeaClient->ReqCaptureTheFieldJoin( m_emCTFType );
	}
}

void RnCDMBattle::ReqCancel()
{
	if ( m_pGaeaClient )
	{
		m_pGaeaClient->ReqCaptureTheFieldCancelJoin( m_emCTFType );
	}
}

void RnCDMBattle::ReqExit()
{
	if ( m_pGaeaClient )
	{
		m_pGaeaClient->ReqCaptureTheFieldQuit();
	}
}

CBasicTextBox* RnCDMBattle::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnCDMBattle::SetBattelState( int nStartTime, int nEndTime )
{
	 m_nStartTime = nStartTime;
	 m_nEndTime = nEndTime;
}
