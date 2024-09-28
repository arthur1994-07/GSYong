#include "pch.h"
#include "GLMinimapClient.h"

#include "../../Enginelib/DeviceMan.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/Quest/GLQuestMan.h"

#include "../Char/GLCharClient.h"
#include "../GLGaeaClient.h"
#include "../Level/GLLevelFileClient.h"
#include "../Party/GLPartyClient.h"
#include "../Char/GLCharacter.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"

std::string GLMinimapClient::SfLargemap_MakeTooltip(const EMCROW emCrowType, const INT nCrowSubType, const DWORD dwID, const DWORD dwMapID)
{
	std::string strResult;

	switch (emCrowType)
	{
	case CROW_PC:	//Party
		{
			SfLargemap_MakeTooltip_Char(strResult, dwID, dwMapID );
		}
		break;
	case CROW_NPC:
		{
			SNpcTalk::EM_BASIC_SUB emSubType = (SNpcTalk::EM_BASIC_SUB)nCrowSubType;
			switch (emSubType)
			{
			case SNpcTalk::EM_STARTPOINT:
				SfLargemap_MakeTooltip_NPCDefaultTooltip(strResult, dwID, ID2GAMEINTEXT("RN_MAP_WINDOW_START_POS_TEXT"));
				break;
			case SNpcTalk::EM_STORAGE:
				SfLargemap_MakeTooltip_NPCDefaultTooltip(strResult, dwID, ID2GAMEINTEXT("RN_MAP_WINDOW_LOCKER_TEXT"));
				break;
			case SNpcTalk::EM_BUSSTATION:
				SfLargemap_MakeTooltip_NPCDefaultTooltip(strResult, dwID, ID2GAMEINTEXT("RN_MAP_WINDOW_BUS_STATION_TEXT"));
				break;
			default:
				SfLargemap_MakeTooltip_NormalNpc(strResult, dwID, dwMapID);
				break;
			}
		}
		break;
	case CROW_GATE:
		{
			SfLargemap_MakeTooltip_Gate(strResult, nCrowSubType, dwID, dwMapID);
		}
		break;
	case CROW_MOB:
		{
			SfLargemap_MakeTooltip_Mob(strResult, dwID);
		}
		break;
	}
	return strResult;
}

BOOL GLMinimapClient::SfLargemap_MakeTooltip_Char(std::string& strResult, const DWORD dwID, const DWORD dwMapID)
{
	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return FALSE;

	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	GLPartyClient* const pMyParty = pClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
		return FALSE;

	GLPartySlotClient* const pMember = pMyParty->GetMemberByGaeaID(dwID);
	if ( !pMember )
		return FALSE;

	std::tr1::shared_ptr<GLCharClient> pChar = pClient->GetChar(dwID);
	if ( !pChar )
		return FALSE;

	// Lv.250 gbgim
	// 검도부남
	std::string strLvName = stringformat("Lv.%1% %2%\n"
		, pMember->m_Level
		, pMember->m_szName);
	strLvName = NSWIDGET_SCRIPT::ToHtmlColorText(strLvName, NS_UITEXTCOLOR::WHITE);

	std::string strClass(COMMENT::CHARCLASS[ CharClassToIndex( pChar->GetClass() ) ]);
	strClass = NSWIDGET_SCRIPT::ToHtmlColorText(strClass, NS_UITEXTCOLOR::YELLOW);

	std::string strRes(strLvName);
	strRes.append(strClass);
	strResult = strRes;
	return TRUE;
}

BOOL GLMinimapClient::SfLargemap_MakeTooltip_NormalNpc(std::string& strResult, const DWORD dwID, const DWORD dwMapID)
{
	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
		return FALSE;

	// 퀘스트 정보;
	std::vector<std::string> vecQuestStringsStart, vecQuestStringsStep, vecQuestStringsFinish;
	SfGetCrowDataFromQuest(QUEST_START, &vecQuestStringsStart, dwID);
	SfGetCrowDataFromQuest(QUEST_STEP, &vecQuestStringsStep, dwID);
	SfGetCrowDataFromQuest(QUEST_FINISH, &vecQuestStringsFinish, dwID);

	// 대체, 상점 거래가능 여부;
	TRADE emTrade(NO_VISIBLE_TRADE);
	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwID );
	if ( pCrowData && haveInvenItem(pCrowData))
		emTrade = CAN_TRADE;

	// IsCrowDataFromNpcType	
	// 이름;
	std::string strRes = "";
	strRes.append(NSWIDGET_SCRIPT::ToHtmlColorText( pCrowData->GetName(), NS_UITEXTCOLOR::WHITE ));

	if ( IsCrowDataFromNpcType(dwID, NPC_TYPE_POSTBOX) )
		strRes = strRes + "\n\"" + NSWIDGET_SCRIPT::ToHtmlColorText(ID2GAMEINTEXT("RN_MAP_WINDOW_POSTBOX_TEXT"), NS_UITEXTCOLOR::WHITE )+"\""; 
	
	// 거래정보;
	if ( emTrade == CAN_TRADE )
		strRes = strRes + "\n" + NSWIDGET_SCRIPT::ToHtmlColorText( ID2GAMEINTEXT("RN_MAP_WINDOW_TRADE_ON"), NS_UITEXTCOLOR::YELLOW );

	// 퀘스트정보;
	for ( INT i = 0; i < vecQuestStringsStart.size(); ++i )
		strRes = strRes + "\n" + NSWIDGET_SCRIPT::ToHtmlColorText( vecQuestStringsStart[i].c_str(), NS_UITEXTCOLOR::WHITE );

	for ( INT i = 0; i < vecQuestStringsStep.size(); ++i )
		strRes = strRes + "\n" + NSWIDGET_SCRIPT::ToHtmlColorText( vecQuestStringsStep[i].c_str(), NS_UITEXTCOLOR::DARKGRAY );

	for ( INT i = 0; i < vecQuestStringsFinish.size(); ++i )
		strRes = strRes + "\n" + NSWIDGET_SCRIPT::ToHtmlColorText( vecQuestStringsFinish[i].c_str(), NS_UITEXTCOLOR::GREEN );

	strResult = strRes;

	return TRUE;
}

void GLMinimapClient::SfGetCrowDataFromQuest( QUEST_TYPE questType
											 , std::vector<std::string>* pQuestStringsVec
											 , DWORD dwCrowID )
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	const DWORD nPartyMember(pClient->GetMyPartyClient()->GetNMember());
	GLCharacter* pChar = pClient->GetCharacter();

	CTime cCurSvrTime = pClient->GetCurrentTime();
	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwCrowID );
	if ( !pCrowData )
		return;

	CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk( pCrowData->GetTalkFile() );
	if( NULL == pTalkMap )
		return;

	std::vector< std::string > questTitleVec;

	std::string ID2GAMEWORD_QUEST(ID2GAMEINTEXT("RN_MAP_WINDOW_QUEST")), strAdditionalExplan;
	switch ( questType )
	{
	case QUEST_START:		strAdditionalExplan = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_START");	break;
	case QUEST_STEP:		strAdditionalExplan = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_STEP");	break;
	case QUEST_FINISH:		strAdditionalExplan = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_FINISH");	break;
	}

	VEC_DWORD vecTalk;
	BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
	{
		std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
		if( NULL == spTalk )
			continue;

		bool isOK = false;
		switch( questType )
		{
		case QUEST_START :
			{
				if( SNpcTalk::EM_QUEST_START == spTalk->m_nACTION )
				{
					if( TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
					{
						isOK = true;

						for( int i = 0; i < MAX_QUEST_START; ++i )
						{
							if( INT_MAX != spTalk->m_dwQuestStartID[ i ] )
							{
								GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwQuestStartID[ i ] );
								if( NULL != pQuest && pQuestStringsVec )
								{
									if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
									{
										CString strKey;
										strKey.Format( "QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID );

										pQuestStringsVec->push_back( CGameTextMan::GetInstance().GetQuestText( strKey.GetString() ).GetString() );
									}
									else
									{
										pQuestStringsVec->push_back( pQuest->GetTITLE() );
									}
								}
							}
						}
					}
				}
			}
			break;

		case QUEST_STEP :
			{
				if( SNpcTalk::EM_QUEST_STEP == spTalk->m_nACTION )
				{
					if( TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
					{
						isOK = true;

						GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwACTION_PARAM1 );
						if( NULL != pQuest && pQuestStringsVec )
						{
							if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							{
								CString strKey;
								strKey.Format( "QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID );

								pQuestStringsVec->push_back( CGameTextMan::GetInstance().GetQuestText( strKey.GetString() ).GetString() );
							}
							else
							{
								pQuestStringsVec->push_back( pQuest->GetTITLE() );
							}
						}
					}
				}
			}
			break;

		case QUEST_FINISH:
			{
				if( SNpcTalk::EM_QUEST_STEP == spTalk->m_nACTION )
				{
					GLQUESTPROG* pQuestProg = pChar->m_cQuestPlay.FindProc( spTalk->m_dwACTION_PARAM1 );
					if( NULL != pQuestProg )
					{
						if( true == pQuestProg->IsLastStep() && TRUE == spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
						{
							isOK = true;

							GLQUEST* pQuest = GLQuestMan::GetInstance().Find( spTalk->m_dwACTION_PARAM1 );
							if( NULL != pQuest && pQuestStringsVec )
							{
								if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								{
									CString strKey;
									strKey.Format( "QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID );

									pQuestStringsVec->push_back( CGameTextMan::GetInstance().GetQuestText( strKey.GetString() ).GetString() );
								}
								else
								{
									pQuestStringsVec->push_back( pQuest->GetTITLE() );
								}
							}
						}
					}
				}
			}
			break;
		}
	}

	if (pQuestStringsVec)
	{
		std::vector<std::string>::iterator itr = pQuestStringsVec->begin()
			, itrEnd = pQuestStringsVec->end();
		for ( ; itr != itrEnd; ++itr )
		{
			std::string strQuest(stringformat( "%1% : %2%(%3%)", ID2GAMEWORD_QUEST, *itr, strAdditionalExplan));
			(*itr) = strQuest;
		}
	}
}

BOOL GLMinimapClient::IsCrowDataFromNpcType(DWORD dwCrowID, int nNpcType)
{
	const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(SNATIVEID(dwCrowID));
	if ( pCrowData == NULL || pCrowData->m_emNPCType != nNpcType )
		return false;

	return true;
}

BOOL GLMinimapClient::SfLargemap_MakeTooltip_Gate(std::string& strResult, const INT nSubType, const DWORD dwID, const DWORD dwMapID)
{
	GLGaeaClient* pClient = GLWidgetScript::GetInstance().m_pGaeaClient;
	GLLevelFileClient* pLevelFile = GetLevelClient( dwMapID );
	DxLandGateMan* pGateMan = pLevelFile->GetLandGateMan();
	PDXLANDGATE pGate = pGateMan->FindLandGate(dwID);
	if ( pGate == NULL )
		return FALSE;

	std::string strRes;
	strRes = strRes + "[" + pClient->GetMapNameStr(pGate->GetToMapID()) + "] ";
	strRes.append(ID2GAMEINTEXT("RN_MAP_WINDOW_GATE_TO_MOVE"));
	strRes.append("\n");
	strRes.append(ID2GAMEINTEXT("RN_MAP_WINDOW_GATE_TO_MOVE_TEXT"));
	strResult = NSWIDGET_SCRIPT::ToPrepareHTMLColorText(strRes.c_str(), NS_UITEXTCOLOR::WHITE);
	return TRUE;
}

BOOL GLMinimapClient::SfLargemap_MakeTooltip_NPCDefaultTooltip(std::string& strResult, const DWORD dwID, std::string strAdditionalExplan)
{
	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwID );
	if ( !pCrowData )
		return FALSE;

	std::string strRes;
	strRes.append(pCrowData->GetName());
	strRes = strRes + "\n\"" + strAdditionalExplan + "\"";
	strResult = NSWIDGET_SCRIPT::ToPrepareHTMLColorText(strRes.c_str(), NS_UITEXTCOLOR::WHITE);
	return TRUE;
}

BOOL GLMinimapClient::SfLargemap_MakeTooltip_Mob(std::string& strResult, const DWORD dwNativeID )
{
	SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( dwNativeID );
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();

	DWORD dwMobStrong(0);

	const GLQuestPlay::QUESTMOB_MAP& mapQuestMob = pChar->m_cQuestPlay.GetQuestMobProc();
	GLQuestPlay::QUESTMOB_MAP::const_iterator iter = mapQuestMob.find( pCrowData->GetId().dwID );
	if( iter != mapQuestMob.end() )
		dwMobStrong = (DWORD)MONSTER_TYPE_QUEST;

	dwMobStrong |= GetMobStrongType(pCrowData);
	
	LuaTable tbMobInfo( GLWidgetScript::GetInstance().GetLuaState() );
	tbMobInfo.set( "dwID", pCrowData->GetId().dwID );
	tbMobInfo.set( "strName", pCrowData->GetName() );
	tbMobInfo.set( "CrowType", pCrowData->m_sBasic.m_emCrow );
	tbMobInfo.set( "nType", (INT) dwMobStrong );
	tbMobInfo.set( "nLevel", (INT)pCrowData->GetLevel() );

	GLWidgetCallFuncValue _argMobInfo;
	_argMobInfo.SetTable( &tbMobInfo );
	VEC_WIDGET_CALLFUNCVALUE _vectorArgument;
	_vectorArgument.push_back( _argMobInfo );

	LuaTable tbResult = GLWidgetScript::GetInstance().LuaCallFunc< LuaTable >( 
		NSWIDGET_SCRIPT::g_strFunc_LargeMap_GetMobTooltip, _vectorArgument);

	strResult = tbResult.get<const char*>(1);
	return TRUE;
}