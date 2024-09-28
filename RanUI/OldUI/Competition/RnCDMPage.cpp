
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

#include "./RnCDMPage.h"
#include "./RnCDMBattle.h"
#include "./RnCDMHistory.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

RnCDMPage::RnCDMPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
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
, m_nTimeLimit(0)
, m_nPlayerLimite(0)
, m_nLevelLimit(0)
{
}

RnCDMPage::~RnCDMPage ()
{
}

void RnCDMPage::CreateSubControl ()
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

	//! CTF 전투
	{
		m_pBattle = new RnCDMBattle(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pBattle->CreateSub( this, "COMPETITION_CTF_PAGE_BATTLE", UI_FLAG_DEFAULT, RNCDM_BATTLE );
		m_pBattle->CreateSubControl();
		RegisterControl( m_pBattle );
	}

	//! CTF 이전기록
	{
		m_pHistory = new RnCDMHistory(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pHistory->CreateSub( this, "COMPETITION_CTF_PAGE_HISTORY", UI_FLAG_DEFAULT, RNCDM_HISTORY );
		m_pHistory->CreateSubControl( &m_pd3dDevice );
		RegisterControl( m_pHistory );
	}

	//! bjju.capturethefield
	// 현재 발전소 점령전밖에 존재하지 않는다.
	m_emCTFType = PVP::ENUM_CTF_TYPE_POWERSTATION;
}

void RnCDMPage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnCDMPage::Update()
{
	UpdateInfo ();
	UpdateBattle ();
	UpdateHistory ();
}

void RnCDMPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}

void RnCDMPage::BeginSubControl()
{
	Update();
}

void RnCDMPage::EndSubControl()
{
	m_pInterface->CompetitionTopRankingClose();
}

void RnCDMPage::UpdateInfo ()
{
	//! 맵이름과 이미지 정보를 구한다.
	m_pMap->SetImage(m_strMapImageName.c_str());
	m_pText_Title->SetOneLineText( m_strMapName.c_str(), NS_UITEXTCOLOR::ORANGE );

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
			nPart = m_pText_Info->AddTextWithCRLF( ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_TEXT", 0), NS_UITEXTCOLOR::WHITE );
		}

		// 목표
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_TEXT", 1) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF( strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			const CBasicTextBox::STEXTPART& sPart1 = m_pText_Info->GetTextPart( nPart );
			float fRight = sPart1.m_rcPart.right;

			m_pText_Info->AddString( nPart, sc::string::format(ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_OBJECTIVE_DESC", 0), m_nTimeLimit).c_str(), NS_UITEXTCOLOR::WHITE );

			// 종료시까지 n개의 거점 점령 유지
			// 필요한 공백 갯수를 구한뒤 채워 넣는다.
			std::string strText = std::string((UINT) ceilf( (fRight - rcTextPos.left)/(float)szSpace.cx ), ' ')  
				+ sc::string::format(ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_OBJECTIVE_DESC", 1), 2);

			m_pText_Info->AddTextWithCRLF(strText.c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 대진
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_TEXT", 2) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );
			m_pText_Info->AddString( nPart, ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_VS"), NS_UITEXTCOLOR::WHITE );
		}

		// 진행인원
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_TEXT", 3) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			m_pText_Info->AddString(nPart, sc::string::format(ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_PLAYERS", 0), m_nPlayerLimite).c_str(), NS_UITEXTCOLOR::WHITE );
		}

		// 참가레벨
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_TEXT", 4) + std::string(" ");
			nPart = m_pText_Info->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::WHITE );

			const CBasicTextBox::STEXTPART& sPart1 = m_pText_Info->GetTextPart( nPart );
			float fRight = sPart1.m_rcPart.right;

			m_pText_Info->AddString(nPart, sc::string::format(ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_MAXLEVEL", 0), m_nLevelLimit).c_str(), NS_UITEXTCOLOR::WHITE );

			std::string strText = std::string((UINT) ceilf( (fRight - rcTextPos.left)/(float)szSpace.cx ), ' ')  
				+ sc::string::format(ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_MAXLEVEL", 1), 2);

			m_pText_Info->AddTextWithCRLF(strText.c_str(), NS_UITEXTCOLOR::WHITE );
		}

	}

	//! 보상 텍스트를 넣는다.
	{
		int nPart = 0;

		m_pText_Award->ClearText();
		m_pText_Award->AddTextWithCRLF( ID2GAMEWORD("COMPETITION_CTF_BATTLE_AWARD_TEXT", 0), NS_UITEXTCOLOR::WHITE );

		//1위
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_REWARD", 0) + std::string(" ");
			nPart = m_pText_Award->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
		}

		//1~10위
		{
			std::string strCaption = ID2GAMEWORD("RNCOMPETITION_CDM_BATTLE_INFO_REWARD", 1) + std::string(" ");
			nPart = m_pText_Award->AddTextWithCRLF(strCaption.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
		}
	}
}

void RnCDMPage::UpdateBattle ()
{
	m_pBattle->SetCTFType( m_emCTFType );
	m_pBattle->Update();
}

void RnCDMPage::UpdateHistory ()
{
	m_pHistory->SetCTFType( m_emCTFType );
	m_pHistory->Update();
}

void RnCDMPage::SetMapinfo( std::string strMapName, int nTimeLimit, int nPlayerLimite, int nLevelLimit )
{
	m_strMapName = strMapName;
	m_nTimeLimit = nTimeLimit;
	m_nPlayerLimite = nPlayerLimite;
	m_nLevelLimit = nLevelLimit;

}

void RnCDMPage::SetMapImageName( std::string strMapImageName )
{
	m_strMapImageName = strMapImageName;
}

void RnCDMPage::BattelState( int nStartTime, int nEndTime )
{
	m_pBattle->SetBattelState( nStartTime, nEndTime );
}
