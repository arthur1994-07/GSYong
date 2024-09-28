
#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Level/GLLevelFileClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../../RanLogic/Skill/GLSkill.h"

#include "../../InnerInterface.h"

#include "InstanceListBox.h"
#include "./RnCTF.h"

#include "../Skill/SkillImage.h"

#include "../../../RanLogic/GLogicDataMan.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;
// ----------------------------------------------------------------------------

#define InfoButtonDelayTime 1.0f
#define Button1_DelayTime 3.0f
//#define Button2_DelayTime 1.0f

RnCTFPage::RnCTFPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: RnCompetitionObject(pGaeaClient, pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pText_Title(NULL)
, m_emCTFType(ENUM_CTF_TYPE_NULL)
, m_pJoinButton(NULL)
, m_fJoinButton_Delay(0.0f)
{
	memset(m_pComp_SchoolImage, 0, sizeof(m_pComp_SchoolImage));
}

RnCTFPage::~RnCTFPage ()
{
}

void RnCTFPage::CreateSubControl ()
{
	RnCompetitionObject::CreateSubControlEx();

	//! CTF 정보 대진 학원 이미지
	{
		m_pComp_SchoolImage[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
		m_pComp_SchoolImage[SCHOOL_SM]->CreateSub(this, "COMPETITION_CTF_PAGE_INFO_RACE_SCHOOL0");
		RegisterControl(m_pComp_SchoolImage[SCHOOL_SM]);

		m_pComp_SchoolImage[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
		m_pComp_SchoolImage[SCHOOL_HA]->CreateSub(this, "COMPETITION_CTF_PAGE_INFO_RACE_SCHOOL1");
		RegisterControl(m_pComp_SchoolImage[SCHOOL_HA]);

		m_pComp_SchoolImage[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
		m_pComp_SchoolImage[SCHOOL_BH]->CreateSub(this, "COMPETITION_CTF_PAGE_INFO_RACE_SCHOOL2");
		RegisterControl(m_pComp_SchoolImage[SCHOOL_BH]);
	}

	//! 텍스트
	{
		CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		const int nAlignLeftTop    = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
		const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
		const int nAlignCenter	   = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

		SetNextWar(ID2GAMEWORD("RNCOMPETITION_NEXT_WAR"));
		SetRankText(ID2GAMEWORD("RNCOMPETITION_RANK"));
		SetInstanceListText(ID2GAMEWORD("RNCOMPETITION_INSTANCE_LIST"));
		SetObjectText(ID2GAMEWORD("RNCOMPETITION_OBJECT"));
		SetMoreInfoText(ID2GAMEWORD("RNCOMPETITION_MORE_INFO"));
		SetRewardText(ID2GAMEWORD("RNCOMPETITION_REWARD"));
        RnCompetitionObject::_SetEnableInfoButton(true);
        RnCompetitionObject::_SetEnableButton1(true);

	}

	//! 참여/취소 버튼
	{
		m_pJoinButton = new CBasicTextButton(m_pEngineDevice);
		m_pJoinButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CTF_BATTLE_BUTTON);
		m_pJoinButton->CreateBaseButton( "RNCOMPETITION_CTF_PAGE_BATTLE_BUTTON", CBasicTextButton::SIZE22, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
		RegisterControl( m_pJoinButton );
	}
	m_pButton1->SetVisibleSingle(FALSE);
	
	m_pInstanceListBox->AddItem(ID2GAMEINTEXT("MAP_222_0"));
	m_pInstanceListBox->SetCurSel(0);
	

	SetVisibleSingleContribution(FALSE);
	SetVisibleSingleExp(FALSE);
	SetVisibleSingleMoney(FALSE);
	SetVisibleSingleItem(FALSE);
	SetVisibleSingleBuff(FALSE);

	//! bjju.capturethefield
	// 현재 발전소 점령전밖에 존재하지 않는다.
	m_emCTFType = PVP::ENUM_CTF_TYPE_POWERSTATION;
}

void RnCTFPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	RnCompetitionObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	BattleUpdate();
	HistoryUpdate();

	if ( m_fJoinButton_Delay > 0.0f )
	{
		m_fJoinButton_Delay -= fElapsedTime;
		if ( m_fJoinButton_Delay < 0.0f )
		{
			m_fJoinButton_Delay = 0.0f;
		}
	}
}

void RnCTFPage::Update()
{
	UpdateInfo ();
	UpdateBattle ();
	UpdateHistory ();
}

void RnCTFPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	RnCompetitionObject::TranslateUIMessage ( ControlID, dwMsg );

	switch( ControlID )
	{
	case CTF_BATTLE_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				// 신청버튼의 버튼 딜레이가 남아 있다면
				if ( m_fJoinButton_Delay > 0.0f )
				{
					m_pInterface->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT",10));
					break;
				}

				m_fJoinButton_Delay = Button1_DelayTime;  // 신청 딜레이 3초;

				if ( m_fnCallbackButton.empty() )
				{
					break;
				}

				m_fnCallbackButton();
			}
		}
		break;

	case RnCompetitionObject::INFO_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->CompetitionTopRankingSetTextTime( m_strTimeText.c_str() );
				m_pInterface->CompetitionTopRankingOpen( m_emCTFType );
			}
			break;
		}
	}
}

void RnCTFPage::BeginSubControl()
{
	Update();
}

void RnCTFPage::EndSubControl()
{
	m_pInterface->CompetitionTopRankingClose();
}

void RnCTFPage::UpdateInfo ()
{
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	GLCaptureTheFieldClient::CTFGLOBALSTATE& sState = pCTFClient->GetGlobalState( m_emCTFType );

	if ( sState.IsNULL() )
	{
		return;
	}

	//! 맵이름과 이미지 정보를 구한다.
	GLLevelFileClient* pLevelFile = sState.spPVPMAPLEVELDATA.get();
	if ( pLevelFile )
	{
		m_pMap->SetImageTexture( pLevelFile->GetMapAxisInfo().GetMinMapTex() );
		SetInstanceName(m_pGaeaClient->GetMapName ( pLevelFile->GetBaseMapID() ));
	}

	//! 정보 텍스트를 넣는다.
	{
		const UIRECT& rcParentPos = GetGlobalPos();
		const UIRECT& rcTextPos   = m_pMoreInfoInfoTextBox->GetGlobalPos();

		CD3DFontPar* pFont = m_pMoreInfoInfoTextBox->GetFont();
		GASSERT(pFont);

		SIZE szSpace;
		pFont->GetTextExtent(" ", szSpace);

		int nPart = 0;

		m_pMoreInfoInfoTextBox->ClearText();

		// 목표
		{
			m_pObjectTextInfoBox->ClearText();
			std::string strCaption = sc::string::format( ID2GAMEINTEXT("RNCOMPETITION_CTF_OBJECT_INFO"), 3 );
			nPart = m_pObjectTextInfoBox->AddTextWithCRLF( strCaption.c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 대진
		{
			for ( UINT i=0; i<SCHOOL_NUM; ++i )
			{
				m_pComp_SchoolImage[i]->SetVisibleSingle(FALSE);
			}
			m_pMoreInfoInfoTextBox->ClearText();
			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 2) + std::string(" ");
			nPart = m_pMoreInfoInfoTextBox->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			//! 임시로 모든학원이 경쟁한다.
			UINT nCompetitionFlag = (EMTEAM_SM | EMTEAM_HA | EMTEAM_BH);

			if ( nCompetitionFlag & EMTEAM_SM )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pMoreInfoInfoTextBox->GetTextPart( nPart );
				float fLocalX = sPart.m_rcPart.right - rcParentPos.left;
				float fLocalY = sPart.m_rcPart.top   - rcParentPos.top;

				m_pComp_SchoolImage[SCHOOL_SM]->SetVisibleSingle(TRUE);
				m_pComp_SchoolImage[SCHOOL_SM]->SetLocalPos( D3DXVECTOR2(fLocalX, fLocalY) );
				m_pComp_SchoolImage[SCHOOL_SM]->SetGlobalPos( D3DXVECTOR2(sPart.m_rcPart.right, sPart.m_rcPart.top) );

				const UIRECT& rcSize = m_pComp_SchoolImage[SCHOOL_SM]->GetLocalPos();

				//! 이미지의 사이즈 만큼 공백으로 채운다
				std::string strText = std::string((UINT) ceil( rcSize.sizeX/(float)szSpace.cx ), ' ')  
					+ std::string(" ") + std::string(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 3)) + std::string(" ");

				nPart = m_pMoreInfoInfoTextBox->AddString(nPart, strText.c_str(), NS_UITEXTCOLOR::WHITE );
			}

			if ( nCompetitionFlag & EMTEAM_HA )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pMoreInfoInfoTextBox->GetTextPart( nPart );
				float fLocalX = sPart.m_rcPart.right - rcParentPos.left;
				float fLocalY = sPart.m_rcPart.top   - rcParentPos.top;

				m_pComp_SchoolImage[SCHOOL_HA]->SetVisibleSingle(TRUE);
				m_pComp_SchoolImage[SCHOOL_HA]->SetLocalPos( D3DXVECTOR2(fLocalX, fLocalY) );
				m_pComp_SchoolImage[SCHOOL_HA]->SetGlobalPos( D3DXVECTOR2(sPart.m_rcPart.right, sPart.m_rcPart.top) );

				const UIRECT& rcSize = m_pComp_SchoolImage[SCHOOL_HA]->GetLocalPos();

				//! 이미지의 사이즈 만큼 공백으로 채운다
				std::string strText = std::string((UINT) ceil( rcSize.sizeX/(float)szSpace.cx ), ' ')  
					+ std::string(" ") + std::string(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 3)) + std::string(" ");

				nPart = m_pMoreInfoInfoTextBox->AddString(nPart, strText.c_str(), NS_UITEXTCOLOR::WHITE );
			}

			if ( nCompetitionFlag & EMTEAM_BH )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pMoreInfoInfoTextBox->GetTextPart( nPart );
				float fLocalX = sPart.m_rcPart.right - rcParentPos.left;
				float fLocalY = sPart.m_rcPart.top   - rcParentPos.top;

				m_pComp_SchoolImage[SCHOOL_BH]->SetVisibleSingle(TRUE);
				m_pComp_SchoolImage[SCHOOL_BH]->SetLocalPos( D3DXVECTOR2(fLocalX, fLocalY) );
				m_pComp_SchoolImage[SCHOOL_BH]->SetGlobalPos( D3DXVECTOR2(sPart.m_rcPart.right, sPart.m_rcPart.top) );
			}
		}

		// 진행인원
		{
			UINT nPlayersOfSchool = sState.nMAXPLAYERS;

			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 4) + std::string(" ");
			nPart = m_pMoreInfoInfoTextBox->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			m_pMoreInfoInfoTextBox->AddString(nPart, sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_PLAYERS", 0), nPlayersOfSchool).c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 참가레벨
		{
			UINT nLevelLimit = static_cast<UINT>(sState.wLEVELLIMIT);

			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 5) + std::string(" ");
			nPart = m_pMoreInfoInfoTextBox->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			m_pMoreInfoInfoTextBox->AddString(nPart, sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_MAXLEVEL", 0), nLevelLimit).c_str(), NS_UITEXTCOLOR::WHITE );
		}

		GLCharacter * pchar =  m_pGaeaClient->GetCharacter();

		if( !pchar)
			return;

		// * %s 회 연속 점령학원입니다. (다음 점령전 참여가능)
		bool bConsecutively = false;
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
						bConsecutively = true;	
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
						bConsecutively = true;
					}
				}
			}

		}

		if(bConsecutively == true)
		{
			UINT nAuthLimit = static_cast<UINT>(sState.dwConsecutivelyNumber);
			std::string strCaption = sc::string::format(ID2GAMEINTEXT("CAPTURETHEFIELD_CONSECUTIVELY", 0) , nAuthLimit);
			nPart = m_pMoreInfoInfoTextBox->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::RED );
		}


	}

	//! 보상 텍스트를 넣는다.
	{
		int nPart = 0;

		m_pRewardInfoTextBox->ClearText();

		//! 임시로 보상을 설정
		UINT nAwardFlag[] = { 
			(ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER | ENUM_REWARD_TYPE_BUFF_PLAYER),
			(ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER)
		};

		std::string strAwardCaption[] = {
			ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TEXT", 1),
			ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TEXT", 2)
		};

		UINT nMax = sizeof(nAwardFlag)/sizeof(UINT);

		for ( UINT i=0; i<nMax; ++i )
		{
			std::string strCaption = strAwardCaption[i] + std::string(" ");
			nPart = m_pRewardInfoTextBox->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::GREENYELLOW );

			std::string strAward;
			UINT nFlag = nAwardFlag[i];

			if ( nFlag == 0 )
			{
				strAward = ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 0);
				m_pRewardInfoTextBox->AddString(nPart, strAward.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
				continue;
			}

			if ( nFlag&ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER )
			{
				if ( !strAward.empty() )
				{
					strAward += ", ";
				}

				strAward += ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 1);
			}

			if ( nFlag&ENUM_REWARD_TYPE_CONTRIBUTION_SCHOOL )
			{
				if ( !strAward.empty() )
				{
					strAward += ", ";
				}

				strAward += ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 2);
			}

			if ( nFlag&ENUM_REWARD_TYPE_BUFF_PLAYER )
			{
				if ( !strAward.empty() )
				{
					strAward += ", ";
				}

				//! 임시로 4개
				strAward += sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 3), 4);
			}

			if ( nFlag&ENUM_REWARD_TYPE_BUFF_SCHOOL )
			{
				if ( !strAward.empty() )
				{
					strAward += ", ";
				}

				//! 임시로 4개
				strAward += sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 4), 4);
			}

			if( !strAward.empty() )
			{
				m_pRewardInfoTextBox->AddString(nPart, strAward.c_str(), NS_UITEXTCOLOR::GREENYELLOW);
			}
		}
	}
}

void RnCTFPage::UpdateBattle ()
{
	SetCTFType( m_emCTFType );
	BattleUpdate();
}

void RnCTFPage::UpdateHistory ()
{
	SetCTFType( m_emCTFType );
	HistoryUpdate();
}

void RnCTFPage::BattleUpdate()
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
				static int nServerMinuteBACK(0);
				if ( nServerMinute != nServerMinuteBACK )
				{
					nServerMinuteBACK = nServerMinute;

					//	서버 타임으로 변환( TimeZone 계산 )
					m_pGaeaClient->GetConvertServerTime( tCurTime );

					CTimeSpan sTimeLeft( sState.tSTARTTIME - tCurTime.GetTime() );

					int nTotalMinutes = (int)(60 * sTimeLeft.GetTotalSeconds() / 3600);
					UINT nTimeHour = static_cast<UINT>( nTotalMinutes / 60 );
					UINT nTimeMinutes = static_cast<UINT>( nTotalMinutes % 60 );
					
					//시간 출력 예외처리(서버에서 시간을 늦게 받아 올 시 쓰레기값 방지용)
					nTimeHour		= ( nTimeHour>23? 0 : nTimeHour);
					nTimeMinutes	= ( nTimeMinutes>59? 0 : nTimeMinutes);

					std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 8), nTimeHour, nTimeMinutes);
					m_pNextWarTimeTextBox->SetOneLineText( strFormat.c_str(), NS_UITEXTCOLOR::WHITE );
				}
			}
			break;

			// 진행중 
		case CTF_STATE_START:
		case CTF_STATE_REWARD:
		case CTF_STATE_DONE:
			{
				m_pNextWarTimeTextBox->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
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

		if ( sState.IsJOINDENIED(pchar) )
		{
			EnableJoinButton( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0), false );
			m_fnCallbackButton = boost::bind( &RnCTFPage::ReqJoin, this );
			goto _BUTTON_END;
		}

		// 이미 참여중이라면
		if ( pCTFClient->GetProgressCTF() == m_emCTFType )
		{
			strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 2);
			m_fnCallbackButton = boost::bind( &RnCTFPage::ReqExit, this );
		}
		else
		{
			if ( sState.bWAITJOIN )
			{
				strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 1);
				m_fnCallbackButton = boost::bind( &RnCTFPage::ReqCancel, this );
			}
			else
			{
				strButtonLabel     = ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0);
				m_fnCallbackButton = boost::bind( &RnCTFPage::ReqJoin, this );
			}
		}

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

_BUTTON_END:

	return;
}

void RnCTFPage::EnableJoinButton( const std::string& strLabel, const bool bEnable )
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

void RnCTFPage::ReqJoin()
{
	if ( m_pGaeaClient )
	{
		if (m_pGaeaClient->GetCharacter()->IsDie())
			return; 
		m_pGaeaClient->ReqCaptureTheFieldJoin( m_emCTFType );
	}
}

void RnCTFPage::ReqCancel()
{
	if ( m_pGaeaClient )
	{
		m_pGaeaClient->ReqCaptureTheFieldCancelJoin( m_emCTFType );
	}
}

void RnCTFPage::ReqExit()
{
	if ( m_pGaeaClient )
	{
		m_pGaeaClient->ReqCaptureTheFieldQuit();
	}
}

CBasicTextBox* RnCTFPage::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnCTFPage::HistoryUpdate()
{
	GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	GLCaptureTheFieldClient::CTFPREVSTATE& sState = pCTFClient->GetPrevState( m_emCTFType );

	// 진행시간
	{
		if ( !sState.IsNULL() && sState.tSTARTTIME && sState.tENDTIME )
		{
			CTime cStartTime( sState.tSTARTTIME );
			CTime cEndTime( sState.tENDTIME );

			m_pGaeaClient->GetConvertServerTime( cStartTime );
			m_pGaeaClient->GetConvertServerTime( cEndTime );

			std::string strFormat = sc::string::format("(%02d:%02d~%02d:%02d)", 
				cStartTime.GetHour(), cStartTime.GetMinute(),
				cEndTime.GetHour(), cEndTime.GetMinute()
				);

			m_strTimeText = strFormat;
			//m_pText_Time->SetOneLineText(strFormat.c_str(), NS_UITEXTCOLOR::WHITE);
		}
		else
		{
			std::string strFormat("(--:--~--:--)");
			m_strTimeText = strFormat;
			//m_pText_Time->SetOneLineText("-- : -- ~ -- : --", NS_UITEXTCOLOR::WHITE);
		}
	}

}

void RnCTFPage::SetVisibleSingle( BOOL bVisible )
{
	if(bVisible)
	{
		CUIWindowObject::SetVisibleSingle(bVisible);
		RnCTFPage::Update();
	}
	else
	{
		CUIWindowObject::SetVisibleSingle(bVisible);
	}
}
