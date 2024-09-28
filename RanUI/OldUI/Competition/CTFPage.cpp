
//bjju.PVP

#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

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

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Level/GLLevelFileClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogic/Land/GLMapAxisInfo.h"

#include "../../InnerInterface.h"

#include "./CTFPage.h"
#include "./CTFBattle.h"
#include "./CTFHistory.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

CCTFPage::CCTFPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CCompetitionPage(pGaeaClient, pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pMap(NULL)
, m_pText_Title(NULL)
, m_pText_Info(NULL)
, m_pText_Award(NULL)
, m_pBattle(NULL)
, m_pHistory(NULL)
, m_emCTFType(ENUM_CTF_TYPE_NULL)
{
	memset(m_pComp_SchoolImage, 0, sizeof(m_pComp_SchoolImage));
}

CCTFPage::~CCTFPage ()
{
}

void CCTFPage::CreateSubControl ()
{
	//! CTF 정보 라인박스
	{
		CreateLineBox  ( "COMPETITION_CTF_PAGE_INFO_LINEBOX_HEAD", "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY" );
		CreateLineBox  ( "COMPETITION_CTF_PAGE_INFO_LINEBOX_BODY", "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
		CreateLineBox  ( "COMPETITION_CTF_PAGE_INFO_LINEBOX_MAP",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );	

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "COMPETITION_CTF_PAGE_INFO_LINEBOX_BOTTOM" );
		RegisterControl ( pBasicLineBox );
	}

	//! CTF 정보 텍스트
	{
		CD3DFontPar* pFont		  = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

		const int nAlignLeftTop    = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
		const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	

		m_pText_Title = CreateStaticControl( "COMPETITION_CTF_PAGE_INFO_TITLE", pFont_Shadow, nAlignLeftCenter );
		m_pText_Info  = CreateStaticControl( "COMPETITION_CTF_PAGE_INFO_DESC_TEXT", pFont, nAlignLeftTop );
		m_pText_Award = CreateStaticControl( "COMPETITION_CTF_PAGE_INFO_AWARD_TEXT", pFont, nAlignLeftTop );
	}

	//! CTF 정보 맵 이미지
	{
		m_pMap =  new CSwapImage(m_pEngineDevice);
		m_pMap->CreateSub( this, "COMPETITION_CTF_PAGE_INFO_MAP" );
		RegisterControl( m_pMap );
	}

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

	//! CTF 전투
	{
		m_pBattle = new CCTFBattle(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pBattle->CreateSub( this, "COMPETITION_CTF_PAGE_BATTLE", UI_FLAG_DEFAULT, CTF_BATTLE );
		m_pBattle->CreateSubControl();
		RegisterControl( m_pBattle );
	}

	//! CTF 이전기록
	{
		m_pHistory = new CCTFHistory(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pHistory->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY", UI_FLAG_DEFAULT, CTF_HISTORY );
		m_pHistory->CreateSubControl( &m_pd3dDevice );
		RegisterControl( m_pHistory );
	}

	//! bjju.capturethefield
	// 현재 발전소 점령전밖에 존재하지 않는다.
	m_emCTFType = PVP::ENUM_CTF_TYPE_POWERSTATION;
}

void CCTFPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCTFPage::Update()
{
	UpdateInfo ();
	UpdateBattle ();
	UpdateHistory ();
}

void CCTFPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CCTFPage::BeginSubControl()
{
	Update();
}

void CCTFPage::EndSubControl()
{
	m_pInterface->CompetitionTopRankingClose();
}

void CCTFPage::UpdateInfo ()
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
		m_pText_Title->SetOneLineText( m_pGaeaClient->GetMapName ( pLevelFile->GetBaseMapID() ), NS_UITEXTCOLOR::ORANGE );
	}

	//! 정보 텍스트를 넣는다.
	{
		const UIRECT& rcParentPos = GetGlobalPos();
		const UIRECT& rcTextPos   = m_pText_Info->GetGlobalPos();

		CD3DFontPar* pFont = m_pText_Info->GetFont();
		GASSERT(pFont);

		SIZE szSpace;
		pFont->GetTextExtent(" ", szSpace);

		int nPart = 0;

		m_pText_Info->ClearText();
		
		// 정보
		{
			nPart = m_pText_Info->AddTextWithCRLF( ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 0), NS_UITEXTCOLOR::WHITE );
		}

		// 목표
		{
			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 1) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF( strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			const CBasicTextBox::STEXTPART& sPart1 = m_pText_Info->GetTextPart( nPart );
			float fRight = sPart1.m_rcPart.right;

			m_pText_Info->AddString( nPart, sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_OBJECTIVE_DESC", 0), 3).c_str(), NS_UITEXTCOLOR::WHITE );

			// 종료시까지 n개의 거점 점령 유지
			// 필요한 공백 갯수를 구한뒤 채워 넣는다.
			std::string strText = std::string((UINT) ceilf( (fRight - rcTextPos.left)/(float)szSpace.cx ), ' ')  
				+ sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_OBJECTIVE_DESC", 1), 2);

			m_pText_Info->AddTextWithCRLF(strText.c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 대진
		{
			for ( UINT i=0; i<SCHOOL_NUM; ++i )
			{
				m_pComp_SchoolImage[i]->SetVisibleSingle(FALSE);
			}

			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 2) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			//! 임시로 모든학원이 경쟁한다.
			UINT nCompetitionFlag = (EMTEAM_SM | EMTEAM_HA | EMTEAM_BH);

			if ( nCompetitionFlag & EMTEAM_SM )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pText_Info->GetTextPart( nPart );
				float fLocalX = sPart.m_rcPart.right - rcParentPos.left;
				float fLocalY = sPart.m_rcPart.top   - rcParentPos.top;

				m_pComp_SchoolImage[SCHOOL_SM]->SetVisibleSingle(TRUE);
				m_pComp_SchoolImage[SCHOOL_SM]->SetLocalPos( D3DXVECTOR2(fLocalX, fLocalY) );
				m_pComp_SchoolImage[SCHOOL_SM]->SetGlobalPos( D3DXVECTOR2(sPart.m_rcPart.right, sPart.m_rcPart.top) );
				
				const UIRECT& rcSize = m_pComp_SchoolImage[SCHOOL_SM]->GetLocalPos();

				//! 이미지의 사이즈 만큼 공백으로 채운다
				std::string strText = std::string((UINT) ceil( rcSize.sizeX/(float)szSpace.cx ), ' ')  
					+ std::string(" ") + std::string(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 3)) + std::string(" ");

				nPart = m_pText_Info->AddString(nPart, strText.c_str(), NS_UITEXTCOLOR::WHITE );
			}
			
			if ( nCompetitionFlag & EMTEAM_HA )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pText_Info->GetTextPart( nPart );
				float fLocalX = sPart.m_rcPart.right - rcParentPos.left;
				float fLocalY = sPart.m_rcPart.top   - rcParentPos.top;

				m_pComp_SchoolImage[SCHOOL_HA]->SetVisibleSingle(TRUE);
				m_pComp_SchoolImage[SCHOOL_HA]->SetLocalPos( D3DXVECTOR2(fLocalX, fLocalY) );
				m_pComp_SchoolImage[SCHOOL_HA]->SetGlobalPos( D3DXVECTOR2(sPart.m_rcPart.right, sPart.m_rcPart.top) );

				const UIRECT& rcSize = m_pComp_SchoolImage[SCHOOL_HA]->GetLocalPos();

				//! 이미지의 사이즈 만큼 공백으로 채운다
				std::string strText = std::string((UINT) ceil( rcSize.sizeX/(float)szSpace.cx ), ' ')  
					+ std::string(" ") + std::string(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 3)) + std::string(" ");

				nPart = m_pText_Info->AddString(nPart, strText.c_str(), NS_UITEXTCOLOR::WHITE );
			}

			if ( nCompetitionFlag & EMTEAM_BH )
			{
				const CBasicTextBox::STEXTPART& sPart = m_pText_Info->GetTextPart( nPart );
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
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			m_pText_Info->AddString(nPart, sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_PLAYERS", 0), nPlayersOfSchool).c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 참가레벨
		{
			UINT nLevelLimit = static_cast<UINT>(sState.wLEVELLIMIT);

			std::string strCaption = ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_TEXT", 5) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			m_pText_Info->AddString(nPart, sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_INFO_MAXLEVEL", 0), nLevelLimit).c_str(), NS_UITEXTCOLOR::WHITE );
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
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::RED );
		}
		
			
	}

	//! 보상 텍스트를 넣는다.
	{
		int nPart = 0;

		m_pText_Award->ClearText();
		m_pText_Award->AddTextWithCRLF( ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TEXT", 0), NS_UITEXTCOLOR::WHITE );

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
			nPart = m_pText_Award->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::GREENYELLOW );

			std::string strAward;
			UINT nFlag = nAwardFlag[i];

			if ( nFlag == 0 )
			{
				strAward = ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TYPE", 0);
				m_pText_Award->AddString(nPart, strAward.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
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
				m_pText_Award->AddString(nPart, strAward.c_str(), NS_UITEXTCOLOR::GREENYELLOW);
			}
		}
	}
}

void CCTFPage::UpdateBattle ()
{
	m_pBattle->SetCTFType( m_emCTFType );
	m_pBattle->Update();
}

void CCTFPage::UpdateHistory ()
{
	m_pHistory->SetCTFType( m_emCTFType );
	m_pHistory->Update();
}
