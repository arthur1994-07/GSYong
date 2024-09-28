#include "StdAfx.h"
#include "CdmRankingDisplay.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
//#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCdmRankingDisplay::CCdmRankingDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )	
	, m_pCdmRankingText ( NULL )
{
	
}

CCdmRankingDisplay::~CCdmRankingDisplay()
{
}

void CCdmRankingDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	m_pCdmRankingText = new CBasicTextBox(m_pEngineDevice);
	m_pCdmRankingText->CreateSub ( this, "CDM_RANKING_TEXT" );
	m_pCdmRankingText->SetFont ( pFont );
	m_pCdmRankingText->SetTextAlign ( TEXT_ALIGN_LEFT );
	RegisterControl ( m_pCdmRankingText );	
}

void CCdmRankingDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCdmRankingDisplay::RefreashCdmRanking()
{
	if ( !m_pCdmRankingText ) return;

	m_pCdmRankingText->ClearText();

	const SCDM_RANK_EX& sMyRank = m_pGaeaClient->m_sMyCdmRank;

	CString strMyRanking;
	strMyRanking.Format( "%2d%s %s:%d %s:%d", sMyRank.wClubRanking, ID2GAMEWORD( "CDM_RANKING_DISPLAY", 0 ),
						ID2GAMEWORD( "CDM_RANKING_DISPLAY", 1),  sMyRank.wKillNum, 
						ID2GAMEWORD( "CDM_RANKING_DISPLAY", 2), sMyRank.wDeathNum );
	
	m_pCdmRankingText->AddText( strMyRanking, NS_UITEXTCOLOR::LIGHTSKYBLUE );

	int nRankNum = static_cast<int>(m_pGaeaClient->m_vecCdmRank.size());
	CString strRanking;

	CDM_RANK_VEC& vecCdmRank = m_pGaeaClient->m_vecCdmRank;

	for ( int i = 0; i < nRankNum; ++i )
	{
		strRanking.Format( "%2d%s %s", vecCdmRank[i].wClubRanking, 
							ID2GAMEWORD( "CDM_RANKING_DISPLAY", 0 ), vecCdmRank[i].szClubName );

		m_pCdmRankingText->AddText( strRanking, NS_UITEXTCOLOR::LIGHTGREY );
	}
}
