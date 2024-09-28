#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"
#include "ClubBattle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CClubBattle::CClubBattle(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pTextBox(NULL)
	, m_pScrollBarEx(NULL)
	, m_nSelectIndex(-1)
	, m_bSelAlliance(false)
{
}

CClubBattle::~CClubBattle ()
{
}

void CClubBattle::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "CLUB_LIST_LINEBOX" );
		RegisterControl ( pBasicLineBox );		
	}

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "CLUB_BATTLE_LIST_TEXTBOX", UI_FLAG_DEFAULT, CLUB_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, CLUB_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "CLUB_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pScrollBarEx = pScrollBar;
	}
}

void CClubBattle::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pTextBox->SetUseOverColor( FALSE );
	if( IsSelectedIndex() )
		m_pTextBox->SetUseTextColor( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( IsSelectedIndex() )
	{
		m_pTextBox->SetUseTextColor( m_nSelectIndex, TRUE );
		m_pTextBox->SetTextColor( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBarEx->GetThumbFrame ();

		const int nTotalLine = m_pTextBox->GetTotalLine ();
		const int nLinePerOneView = m_pTextBox->GetVisibleLine ();
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pTextBox->SetCurLine ( nPos );			
		}
	}
}

void CClubBattle::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CLUB_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pTextBox->GetSelectPos ();
				if( nIndex < 0 || m_pTextBox->GetCount () <= nIndex ) return;

				m_pTextBox->SetUseOverColor ( TRUE );
				m_pTextBox->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				ShowClubBattleInfo( nIndex );

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;

					const DWORD dwClubaID = m_pTextBox->GetTextData( m_nSelectIndex );

					GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

					CLUB_BATTLE_ITER iter = pMyClub->m_mapBattle.find( dwClubaID );

					GLCLUBBATTLE& sClubBattle = iter->second;
					m_strSelectName = sClubBattle.m_szClubName;
					m_bSelAlliance = sClubBattle.m_bAlliance;
				}
			}
		}
		break;
	}
}

void CClubBattle::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadClubBattleList();
	}
}

void CClubBattle::LoadClubBattleList()
{
	
	m_pTextBox->ClearText (); // 텍스트를 모두 지운다.

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return;

	CLUB_BATTLE_ITER iter = pMyClub->m_mapBattle.begin();
	CLUB_BATTLE_ITER iter_end = pMyClub->m_mapBattle.end();

	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBBATTLE& sClubBattle = iter->second;
		const DWORD dwID = sClubBattle.m_dwCLUBID;

		CString strCombine;

		strCombine = "[VS]";
		if ( sClubBattle.m_bAlliance ) strCombine += ID2GAMEINTEXT( "CLUB_BATTLE_INFO", 4 );
		
		strCombine += " ";
		strCombine += sClubBattle.m_szClubName;

		CString strTemp;
		strTemp = sc::string::format( ID2GAMEINTEXT( "CLUB_BATTLE_INFO", 0 ), sClubBattle.m_wKillPoint, 
						 sClubBattle.m_wKillPointTemp, sClubBattle.m_wDeathPoint, 
						 sClubBattle.m_wDeathPointTemp ).c_str();

		strCombine += " ";
		strCombine += strTemp;	

		int nIndex = m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );	
		m_pTextBox->SetTextData ( nIndex, dwID );
	}

	{
		m_pTextBox->SetCurLine ( 0 );

		const float fPercent = m_pScrollBarEx->GetThumbFrame()->GetPercent ();

		const int nTotal = m_pTextBox->GetCount ();
		const int nViewPerPage = m_pTextBox->GetVisibleLine ();	
		m_pScrollBarEx->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
		m_pScrollBarEx->GetThumbFrame()->SetPercent ( fPercent );
	}

}

BOOL CClubBattle::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex ) && ( m_nSelectIndex < m_pTextBox->GetCount() );
}

const DWORD CClubBattle::GetBattleClubID()
{
	if ( !IsSelectedIndex() )
		return NATIVEID_NULL().dwID;

	return m_pTextBox->GetTextData( m_nSelectIndex );
}

void CClubBattle::ShowClubBattleInfo( int nIndex )
{
	if( nIndex < 0 || m_pTextBox->GetCount () <= nIndex )
        return;

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

	int nClubBattleID = m_pTextBox->GetTextData( nIndex );

	GLCLUBBATTLE* pClubBattle = pMyClub->GetClubBattle( nClubBattleID );
	if ( !pClubBattle )
        return;

	if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI() )
	{
		CString strTemp;
		CTime cTime;
		
		cTime = pClubBattle->m_tStartTime;

		//	서버 타임으로 변환( TimeZone 계산 )
		m_pGaeaClient->GetConvertServerTime( cTime );

		strTemp = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",1), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();
		m_pInterface->ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		cTime = pClubBattle->m_tEndTime;

		//	서버 타임으로 변환( TimeZone 계산 )
		m_pGaeaClient->GetConvertServerTime( cTime );
		
		strTemp = sc::string::format(  ID2GAMEINTEXT("CLUB_BATTLE_INFO",2), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();
		m_pInterface->ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME,0);
		cTime = pClubBattle->m_tStartTime;		

		//	서버 타임으로 변환( TimeZone 계산 )
		m_pGaeaClient->GetConvertServerTime( cTime );
		
		cTime += tDisTimeSpan;
		
		strTemp = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_INFO",3), cTime.GetYear (), cTime.GetMonth (), 
						cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();
		m_pInterface->ADD_COMMON_LINEINFO_MULTI( strTemp, NS_UITEXTCOLOR::WHITE  );

		m_pInterface->END_COMMON_LINEINFO_MULTI();
	}

	return;
}
