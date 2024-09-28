#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/InventoryWindow.h"
#include "../Item/RnInventoryWindow.h"

#include "StatsResetWindow.h"
#include "StatsResetComboBoxRollOver.h"
#include "StatsInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CStatsResetWindow::Recalculate()
{
	SCHARSTATS sBasic,sBasicAdded;

	CalBasicStats( sBasic      );
	CalAddedStats( sBasicAdded );

	m_pStats->SetBasicData( sBasic, sBasicAdded );
	m_pStats_Preview->SetBasicData( sBasic, sBasicAdded );

	ResetSkillControl();
	ResetTextControl();

  
}

void CStatsResetWindow::ResetProperty( const WORD wReturnStats, const UI::EMSTATSRESETWINDOW_TYPE emType, const EMITEM_TYPE emItemType )
{
	IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
	if( pInvenWindow )
		pInvenWindow->LockTrade();

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory )
		pInventory->LockTrade();

	m_wReturnStats	  = 0;
	m_wReturnStatsMax = wReturnStats;
	m_emType		  = emType;
	m_emItemType	  = emItemType;

	ResetStatsControl();
	ResetSkillControl();
	ResetTextControl();
}

void CStatsResetWindow::ResetStatsControl()
{
	GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();

	SCHARSTATS sBasic,sBasicAdded;

	CalBasicStats( sBasic      );
	CalAddedStats( sBasicAdded );

	m_pStats->Reset( sBasic, sBasicAdded );
	m_pStats_Preview->Reset( sBasic, sBasicAdded );

	memset( m_wResetStats, 0, sizeof(WORD)*EMSIZE );


	switch ( m_emType )
	{
	case UI::EMSTATSRESETWINDOW_TYPE_SKILL:
		{
			m_pStats_Preview->SetType( CStatsItem::EMSTATSITEMTYPE_NULL );
			m_wReturnStats	  = 0;
			m_wReturnStatsMax = 0;
		}
		break;

	case UI::EMSTATSRESETWINDOW_TYPE_STATS:
		{
			m_pStats_Preview->SetType( CStatsItem::EMSTATSITEMTYPE_NULL );

			WORD wEvenlyReset[EMSIZE];
			memset( wEvenlyReset, 0, sizeof(WORD)*EMSIZE);
			sCharData.GET_EVENLY_DECREASE_STATS( wEvenlyReset, m_wReturnStatsMax );

			m_wReturnStats = 0;
			for ( WORD i=0; i<EMSIZE; ++i )
			{
				m_wReturnStats += wEvenlyReset[i];
			}

			SCHARSTATS sAdd;
			sAdd.wPow = sBasicAdded.wPow - wEvenlyReset[EMPOW];
			sAdd.wDex = sBasicAdded.wDex - wEvenlyReset[EMDEX];
			sAdd.wSpi = sBasicAdded.wSpi - wEvenlyReset[EMSPI];
			sAdd.wStr = sBasicAdded.wStr - wEvenlyReset[EMSTR];
			sAdd.wSta = sBasicAdded.wSta - wEvenlyReset[EMSTA];

			m_pStats_Preview->SetAdd( sAdd ); 
		}
		break;

	case UI::EMSTATSRESETWINDOW_TYPE_STATS_SELECT:
		{
			m_pStats_Preview->SetType( CStatsItem::EMSTATSITEMTYPE_CONTROLLER_BUTTON );
			m_wReturnStats = 0;
		}
		break;
	}
}

void CStatsResetWindow::ResetSkillControl()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	

	if ( !pCharacter )
	{
		return;
	}

	{
		WORD		      wLEVEL    = pCharacter->m_wLevel;
		EMCHARINDEX       emINDEX   = CharClassToIndex(pCharacter->m_emClass);
		const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(pCharacter->m_wSchool,emINDEX);

		DWORD dwGiftSkill(0);

		GLQuestPlay::MAPQUEST&     sQuestEnd = pCharacter->m_cQuestPlay.GetQuestEnd();
		GLQuestPlay::MAPQUEST_ITER pos       = sQuestEnd.begin();
		GLQuestPlay::MAPQUEST_ITER end       = sQuestEnd.end();
		for ( ; pos!=end; ++pos )
		{
			GLQUESTPROG *pPROG = (*pos).second;
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find( pPROG->m_sNID.dwID );
			if ( !pQUEST )
                continue;

			dwGiftSkill += pQUEST->m_dwGiftSKILLPOINT;
		}

		if( emINDEX == GLCI_EXTREME_M || emINDEX == GLCI_EXTREME_W )
		{
			m_wReturnSkillPoint = (WORD)(sCHARDATA.m_dwSkillPoint + (wLEVEL-1)*GLCONST_CHAR::wLVL_2ndSKILL_P + dwGiftSkill);
		}
		else
		{
			m_wReturnSkillPoint = (WORD)(sCHARDATA.m_dwSkillPoint + (wLEVEL-1)*GLCONST_CHAR::wLVL_SKILL_P + dwGiftSkill);
		}

		m_wReturnSkillPoint -= static_cast<WORD>(pCharacter->m_dwSkillPoint);
	}

	{
		// GLCharDefine.h 
// 		enum
// 		{
// 			EMGRADE				= 31,
// 			EMMARBLE_MID		= 104,
// 
// 			EMGRADE_MIN			= 4,
// 		};

		WORD wNUM(0);
		WORD wGRADE[EMGRADE];
		memset(wGRADE,0,sizeof(WORD)*EMGRADE);

		GLCharacter::SKILL_MAP_ITER pos = pCharacter->m_ExpSkills.begin();
		GLCharacter::SKILL_MAP_ITER end = pCharacter->m_ExpSkills.end();

		for ( ; pos!=end; ++pos )
		{
			SNATIVEID sNID ( (*pos).first );

			PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sNID );
			if ( !pSKILL )									continue;
			if ( EMGRADE_MIN > pSKILL->m_sBASIC.dwGRADE )	continue;

			wGRADE[pSKILL->m_sBASIC.dwGRADE] += 1;
			++wNUM;
		}

		m_wReturnItem = wNUM;
//         m_wReqInven = 0;
//         for ( int i = 0; i < SLOT_NSIZE_S_2; ++i ) 
//         {
//             if ( pCharacter->m_PutOnItems[i].sNativeID != NATIVEID_NULL() ) 
//             {
//                 m_wReqInven++;
//             }
//         }
 
		if ( m_pReturn_Item_ComboBox )
		{
			CString strCombine;
			CString strTemp;

			m_pReturn_Item_ComboBox->ClearText();

			for ( WORD i=0; i<EMGRADE; ++i )
			{
				if ( !wGRADE[i] )
				{
					continue;
				}

				const SITEM *pITEM = GLogicData::GetInstance().GetItem( SNATIVEID(EMMARBLE_MID,i) );

				if ( !pITEM )
				{
					continue;
				}
//                 m_wReqInven +=  ( wGRADE[i] / pITEM->sDrugOp.wPileNum );
//                 if ( wGRADE[i] % pITEM->sDrugOp.wPileNum ) m_wReqInven++;
        
				//strCombine.Format ( "%s     %d°³", pITEM->GetName(), (INT32)wGRADE[i] );
				strTemp.Format( "%s", pITEM->GetName() );
				strCombine = strTemp;

				strTemp.Format( "%10d", (INT32)wGRADE[i] );
				strCombine += strTemp;
				strCombine += ID2GAMEWORD( "STATSRESET_RETURN_ITEM_TEXT_NUM" ); // °³

				m_pReturn_Item_ComboBox->AddText( strCombine );
			}
		}
//        m_wRemainInven = pCharacter->m_cInventory.GetAmountEmpty( false );
	}
}

void CStatsResetWindow::ResetTextControl()
{
	switch ( m_emType )
	{
	case UI::EMSTATSRESETWINDOW_TYPE_SKILL:
		{
			m_emInfoTextHead = EMINFOTEXTHEAD_SKILL_ONLY;
			if ( m_pReset_Button ) m_pReset_Button->SetVisibleSingle( FALSE );
		}
		break;

	case UI::EMSTATSRESETWINDOW_TYPE_STATS:
		{
			if ( m_wReturnStatsMax == USHRT_MAX )
			{
				m_emInfoTextHead = EMINFOTEXTHEAD_STATS_ALLSTATS;
			}
			else
			{
				m_emInfoTextHead = EMINFOTEXTHEAD_STATS_NONSELECTABLE;
			}

			if ( m_pReset_Button ) m_pReset_Button->SetVisibleSingle( FALSE );
		}
		break;

	case UI::EMSTATSRESETWINDOW_TYPE_STATS_SELECT:
		{
			m_emInfoTextHead = EMINFOTEXTHEAD_STATS_SELECTABLE;
			if ( m_pReset_Button ) m_pReset_Button->SetVisibleSingle( TRUE );
		}
		break;
	}

	if ( m_pInfo_Head_Text )
	{
		CString strInfo;

		switch ( m_emInfoTextHead )
		{
		case EMINFOTEXTHEAD_STATS_SELECTABLE:
			strInfo.Format( ID2GAMEINTEXT("STATSRESET_INFO_HEAD_TEXT_SELECTABLE"), m_wReturnStatsMax);
			break;

		case EMINFOTEXTHEAD_STATS_NONSELECTABLE:
			strInfo.Format( ID2GAMEINTEXT("STATSRESET_INFO_HEAD_TEXT_NONSELECT"), m_wReturnStatsMax);
			break;

		case EMINFOTEXTHEAD_STATS_ALLSTATS:
			strInfo = ID2GAMEINTEXT("STATSRESET_INFO_HEAD_TEXT_ALLSTATS");
			break;

		case EMINFOTEXTHEAD_SKILL_ONLY:
			strInfo = ID2GAMEINTEXT("STATSRESET_INFO_HEAD_TEXT_SKILLONLY");
			break;
		};

		m_pInfo_Head_Text->ClearText();
		m_pInfo_Head_Text->AddText( strInfo );
	}

	{
		const DWORD dwTopicColor = NS_UITEXTCOLOR::LIGHTYELLOW;

		CString strTopic;

		if ( m_pTopic_Stats_Current )
		{
			strTopic = ID2GAMEWORD("STATSRESET_TOPIC_STATS_CURRENT");
			m_pTopic_Stats_Current->ClearText();
			m_pTopic_Stats_Current->AddText(strTopic, dwTopicColor);
		}

		if ( m_pTopic_Stats_Preview )
		{
			switch ( m_emType )
			{
			case UI::EMSTATSRESETWINDOW_TYPE_SKILL:
			case UI::EMSTATSRESETWINDOW_TYPE_STATS:
				strTopic = ID2GAMEWORD("STATSRESET_TOPIC_STATS_PREVIEW_INFO");
				break;

			case UI::EMSTATSRESETWINDOW_TYPE_STATS_SELECT:
				strTopic = ID2GAMEWORD("STATSRESET_TOPIC_STATS_PREVIEW_SELECT");
				break;
			}

			m_pTopic_Stats_Preview->ClearText();
			m_pTopic_Stats_Preview->AddText(strTopic, dwTopicColor);
		}

		if ( m_pTopic_Item_Return )
		{
			strTopic = ID2GAMEWORD("STATSRESET_TOPIC_ITEM_RETURN");
			m_pTopic_Item_Return->ClearText();
			m_pTopic_Item_Return->AddText(strTopic, dwTopicColor);
		}
	}

	SetVariableTextControl ();

}

void CStatsResetWindow::CalBasicStats( SCHARSTATS& sOutStats )
{
	const GLCHARLOGIC&		 sCharData  = m_pGaeaClient->GetCharacterLogic();	
	const GLCONST_CHARCLASS& cCHARCONST = GLCONST_CHAR::cCONSTCLASS[sCharData.GETCHARINDEX()];	


	sOutStats = cCHARCONST.sBEGIN_STATS + cCHARCONST.sLVLUP_STATS*(sCharData.GETLEVEL()-1) + sCharData.m_sSUMITEM.sStats;
}

void CStatsResetWindow::CalAddedStats( SCHARSTATS& sOutStats )
{
	const GLCHARLOGIC& sCharData  = m_pGaeaClient->GetCharacterLogic();	
	sOutStats = sCharData.m_sStats;
}

void CStatsResetWindow::SetPointControl( const GLCHARLOGIC& sCharData )
{
	CString strCombine;


	strCombine.Format ( "%d", sCharData.m_wStatsPoint );
	if ( m_pPointText ) m_pPointText->SetOneLineText( strCombine, NS_UITEXTCOLOR::GOLD );


	strCombine.Format ( "%d", sCharData.m_wStatsPoint + m_wReturnStats );
	if ( m_pPointText_Preview ) m_pPointText_Preview->SetOneLineText( strCombine, NS_UITEXTCOLOR::PALEGREEN );
}

void CStatsResetWindow::SetVariableTextControl()
{
	switch ( m_emType )
	{
	case UI::EMSTATSRESETWINDOW_TYPE_SKILL:
	case UI::EMSTATSRESETWINDOW_TYPE_STATS:
		{
			m_emInfoTextTail = EMINFOTEXTTAIL_CONFIRMATION;
		}
		break;

	case UI::EMSTATSRESETWINDOW_TYPE_STATS_SELECT:
		{
			if ( m_wReturnStats != m_wReturnStatsMax )
			{
				m_emInfoTextTail = EMINFOTEXTTAIL_RESETSTATS_REMAINED;
			}
			else
			{
				m_emInfoTextTail = EMINFOTEXTTAIL_CONFIRMATION;
			}
		}
		break;
	}

	if ( m_pInfo_Tail_Text )
	{
		CString strInfo;

		m_pInfo_Tail_Text->ClearText();

		switch ( m_emInfoTextTail )
		{
		case EMINFOTEXTTAIL_CONFIRMATION:
			{
				strInfo = ID2GAMEINTEXT("STATSRESET_INFO_TAIL_TEXT_CONFIRMATION");
				m_pInfo_Tail_Text->ClearText();
				m_pInfo_Tail_Text->AddText( strInfo );
			}
			break;

		case EMINFOTEXTTAIL_RESETSTATS_REMAINED :
			{
				strInfo.Format( "%d ", (UINT32)(m_wReturnStatsMax - m_wReturnStats) );
				int nIndex = m_pInfo_Tail_Text->AddText( strInfo, NS_UITEXTCOLOR::RED );

				strInfo = ID2GAMEINTEXT("STATSRESET_INFO_TAIL_TEXT_REMAINED");
				m_pInfo_Tail_Text->AddString( nIndex, strInfo.GetString() );

				strInfo = ID2GAMEINTEXT("STATSRESET_INFO_TAIL_TEXT_CONFIRMATION");
				m_pInfo_Tail_Text->AddText( strInfo );
			}
			break;
		};
	}

	{
		const DWORD dwReturnColor = NS_UITEXTCOLOR::PALEGREEN;

		CString strCombine;
		int		nIndex = 0;

		strCombine = ID2GAMEINTEXT("STATSRESET_RETURN_STATS");
		m_pReturn_Stats_Text->ClearText();
		nIndex = m_pReturn_Stats_Text->AddText( strCombine.GetString(), dwReturnColor );

		switch ( m_emType )
		{
		case UI::EMSTATSRESETWINDOW_TYPE_SKILL:
		case UI::EMSTATSRESETWINDOW_TYPE_STATS:
			strCombine.Format ( " +%d", (INT32)m_wReturnStats );
			break;

		case UI::EMSTATSRESETWINDOW_TYPE_STATS_SELECT:
			strCombine.Format ( " +%3d/%d", (INT32)m_wReturnStats, (INT32)m_wReturnStatsMax );
		}

		m_pReturn_Stats_Text->AddString( nIndex, strCombine, dwReturnColor );

		strCombine = ID2GAMEINTEXT("STATSRESET_RETURN_SKILL");
		m_pReturn_Skill_Text->ClearText();
		nIndex = m_pReturn_Skill_Text->AddText( strCombine.GetString(), dwReturnColor );

		strCombine.Format ( " +%d", (INT32)m_wReturnSkillPoint );
		m_pReturn_Skill_Text->AddString( nIndex, strCombine, dwReturnColor );

		strCombine = ID2GAMEINTEXT("STATSRESET_RETURN_ITEM");
		m_pReturn_Item_Text->ClearText();
		nIndex = m_pReturn_Item_Text->AddText( strCombine.GetString(), dwReturnColor );

		strCombine.Format ( " %d", (INT32)m_wReturnItem );
		m_pReturn_Item_Text->AddString( nIndex, strCombine, dwReturnColor );

//         m_pReq_Inven_Text->ClearText();
//         
//         std::string strText = sc::string::format( ID2GAMEINTEXT("STATSRESET_REQ_INVEN"), m_wReqInven, m_wRemainInven );
// 
//         if( m_wReqInven > m_wRemainInven ) 
//         {
//             m_pReq_Inven_Text->AddText( strText.c_str(), NS_UITEXTCOLOR::DISABLE );
//         }
//         else
//         {   
//             m_pReq_Inven_Text->AddText( strText.c_str(), NS_UITEXTCOLOR::PALEGREEN );
//         }       
        
	}
}


