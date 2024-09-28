#include "pch.h"
#include "GLNpcTalkClient.h"

#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Inventory/GLInventory.h"
#include "../../RanLogic/Msg/GLContrlNpcMsg.h"
#include "../../RanLogic/GLogicEx.h"

#include "../GLGaeaClient.h"
#include "../GfxInterfaceBase.h"
#include "../Char/GLCharacter.h"
#include "../Club/GLClubClient.h"
#include "../Crow/GLCrowClient.h"
#include "../Land/GLLandManClient.h"
#include "../Party/GLPartyClient.h"

#include "../../RanLogicClient/Transport/GLBusStationClient.h"


const int GLNpcTalkClient::nSTARTINDEX = 1;

GLNpcTalkClient::GLNpcTalkClient(void)
:	m_dwNpcGlobalID( GAEAID_NULL )
,	m_dwRandomTime( 0 )
,	m_nRebuyTabIndex( -1 )
,	m_bEnabledMarket( false )
,	m_llTotalCartPrice( 0 )
,	m_emCrow( CROW_NPC )
{
}

GLNpcTalkClient::GLNpcTalkClient( const GLNpcTalkClient& value )
{
}

GLNpcTalkClient::~GLNpcTalkClient(void)
{
	m_vecTalk.clear();
	m_mapTalkIndex.clear();
	m_vecMarketCart.clear();
}

GLNpcTalkClient& GLNpcTalkClient::GetInstance()
{
	static GLNpcTalkClient Instance;
	return Instance;
}

void GLNpcTalkClient::SetTalkData( const SNATIVEID& sNpcID, DWORD dwNpcGlobalID, EMCROW emCrow )
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	PCROWDATA	pCrowData = NULL;
	
	switch( emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pClient->GetActiveMap()->GetSummon( dwNpcGlobalID );

			if( NULL == spCrow || CROW_SUMMON != spCrow->GETCROW() )	return;

			pCrowData = spCrow->m_pSummonCrowData;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pClient->GetActiveMap()->GetCrow( dwNpcGlobalID );

			if( NULL == spCrow || CROW_NPC != spCrow->GETCROW() )	return;

			pCrowData = spCrow->m_pCrowData;
		}
		break;
	default : return;
	}
	
	if( pCrowData == NULL ) return;

	m_emCrow		= emCrow;
	m_sNpcID		= sNpcID;
	m_dwNpcGlobalID = dwNpcGlobalID;

	//const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( spCrow->m_pCrowData->sNativeID );
	std::tr1::shared_ptr< NpcTalkMan > spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	if( NULL == spTalkMan )
		return;

	std::tr1::shared_ptr< CNpcDialogueSet > spDlgSet = spTalkMan->Find( pCrowData->GetTalkFile() );
	if( NULL == spDlgSet )
		return;

	m_spDlgSet = spDlgSet;

	LoadNode( nSTARTINDEX );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_NpcTalk_Open,
		"-b", true );
}

const bool GLNpcTalkClient::IsUseable()
{
	return IsUseableFromID( m_dwNpcGlobalID );
}

const bool GLNpcTalkClient::IsUseableFromID( DWORD dwNpcID )
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	GLLandManClient* pLandMan = pClient->GetActiveMap();

	if ( !pLandMan ) return false;

	const GLCharacter* const pChar = pClient->GetCharacter();

	if ( pChar == NULL ) return false;

	const D3DXVECTOR3 vCharPos( pChar->GetPosition() );
	const float fCharRadius( pChar->GETBODYRADIUS() );

	switch( m_emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pLandMan->GetSummon( dwNpcID );

			if( NULL == spCrow || CROW_SUMMON != spCrow->GETCROW() )
				return false;

			if ( spCrow->IsSummonNpcActionable(vCharPos, fCharRadius) == false )
				return false;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pLandMan->GetCrow( dwNpcID );

			if( NULL == spCrow || CROW_NPC != spCrow->GETCROW() )
				return false;

			if ( spCrow->IsNpcActionable(vCharPos, fCharRadius) == false )
				return false;
		}
		break;
	default : return false;
	}

	return true;
}

void GLNpcTalkClient::LoadNode( DWORD dwID, bool bRandom, DWORD dwRandomTime )
{
	m_spDlg = m_spDlgSet->GetDialogue( dwID );
	if( NULL == m_spDlg )
	{
		std::string strErrMsg = stringformat(
			"void GLNPCTalkClient::LoadNode - GetDialogue failed FileName : %1%, Index : %2%",
			m_spDlgSet->m_TalkName, dwID );
		sc::writeLogError( strErrMsg );

		return;
	}

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	if ( !pClient )
		return;

	GLCharacter* pChar = pClient->GetCharacter();
	CTime cCurSvrTime = pClient->GetCurrentTime();
	DWORD dwIndex = m_spDlg->FindConditionSatisfied( pChar, pClient->GetMyPartyClient()->GetNMember(), cCurSvrTime.GetHour() );
	if( UINT_MAX == dwIndex )
	{
		std::string strErrMsg = stringformat(
			"void GLNPCTalkClient::LoadNode - Satisfied failed FileName : %1%, Index : %2%",
			m_spDlgSet->m_TalkName, dwID );
		sc::writeLogError( strErrMsg );

		return;
	}

	m_spDlgCase = m_spDlg->GetDlgCase( dwIndex );
	if( NULL == m_spDlgCase )
		return;

	m_dwRandomTime = dwRandomTime;

	LoadTalkList( bRandom );
}

void GLNpcTalkClient::LoadTalkList( bool bRandom )
{
	m_vecTalk.clear();
	m_mapTalkIndex.clear();

	if( NULL == m_spDlgCase )
		return;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return;

	std::tr1::shared_ptr< CNpcTalkControl > spNpcTalkCtrl = m_spDlgCase->GetTalkControl();
	if( NULL == spNpcTalkCtrl )
		return;

	CNpcTalkControl::NPCMAP* pMap = spNpcTalkCtrl->GetTalkMap();
	m_vecTalk.reserve( pMap->size() );

	CNpcTalkControl::NPCMAP_IT iterNpcMap = pMap->begin();
	for( ; iterNpcMap != pMap->end(); ++iterNpcMap )
	{
		std::tr1::shared_ptr< SNpcTalk > spNpcTalk = iterNpcMap->second;
		if( NULL != spNpcTalk )
		{
			if ( true == GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if (NULL != m_spDlg && NULL != m_spDlgSet)
				{
					int nIndex = -1;
					const int nSize = static_cast< const int >( m_spDlg->m_vecCase.size() );
					for (UINT i = 0; i < nSize; ++i)
					{
						if (m_spDlgCase == m_spDlg->m_vecCase[i])
						{
							nIndex = i;
							break;
						}
					}

					if (-1 != nIndex)
					{
						CString strTalkKey;
						strTalkKey.Format("%s_%d_%d_%d",
							m_spDlgSet->m_TalkName.c_str(),
							m_spDlg->GetDlgNID(),
							nIndex,
							iterNpcMap->first);

						std::string strTalk = CGameTextMan::GetInstance().GetNPCTalkText(strTalkKey.GetString(), 0).GetString();
						spNpcTalk->SetTalk(strTalk);
					}
				}
			}
			
			m_vecTalk.push_back( spNpcTalk );
		}
	}

	if( true == bRandom )
		random_shuffle( m_vecTalk.begin(), m_vecTalk.end() );

	int nTalkIndex = 0;

	// 대화파일에 상점 조건에 대한 정보도 있기 때문에 여기서 설정해준다.
	m_bEnabledMarket = false;

	CTime cCurSvrTime = pClient->GetCurrentTime();

	std::vector< std::tr1::shared_ptr< SNpcTalk > >::iterator iterTalk = m_vecTalk.begin();
	for( ; iterTalk != m_vecTalk.end(); ++iterTalk, ++nTalkIndex )
	{
		std::tr1::shared_ptr< SNpcTalk > spNpcTalk = (*iterTalk);

		// 조건에 부합하지 못할 경우 추가하지 않음.
		DWORD dwPartyMemberNum = pClient->GetMyPartyClient()->GetNMember();
		if( FALSE == spNpcTalk->DoTEST( pChar, dwPartyMemberNum, cCurSvrTime.GetHour() ) )
		{
			if( true == CheckQuestProc( spNpcTalk, dwPartyMemberNum ) )
				m_mapTalkIndex.insert( std::make_pair( TALK_QUEST_ING, nTalkIndex ) );

			continue;
		}

		if( SNpcTalk::EM_BASIC == spNpcTalk->m_nACTION &&
			SNpcTalk::EM_MARKET == spNpcTalk->m_dwACTION_NO )
			m_bEnabledMarket = true;

		switch( spNpcTalk->m_nACTION )
		{
		case SNpcTalk::EM_QUEST_START :
			{
				m_mapTalkIndex.insert( std::make_pair( TALK_QUEST_START, nTalkIndex ) );
			}
			break;

		case SNpcTalk::EM_QUEST_STEP :
			{
				m_mapTalkIndex.insert( std::make_pair( TALK_QUEST_STEP, nTalkIndex ) );
			}
			break;

		case SNpcTalk::EM_BASIC :
			{
				if( spNpcTalk->m_dwACTION_NO != SNpcTalk::EM_MARKET )
					m_mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
			}
			break;

		case SNpcTalk::EM_PAGE_MOVE :
			{
				if( 1 == spNpcTalk->m_dwACTION_NO )
					m_mapTalkIndex.insert( std::make_pair( TALK_FIRST_TIME, nTalkIndex ) );
				else
					m_mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
			}
			break;

		default :
			{
				m_mapTalkIndex.insert( std::make_pair( TALK_BASIC_TALK, nTalkIndex ) );
			}
			break;
		}
	}
}

bool GLNpcTalkClient::CheckQuestProc( std::tr1::shared_ptr< SNpcTalk > spNpcTalk, DWORD dwPartyMemberNum )
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return false;

	if( NULL != spNpcTalk->m_spCondition )
	{
		CTime cCurSvrTime = pClient->GetCurrentTime();
		if( FALSE == spNpcTalk->m_spCondition->DoTEST( pChar, dwPartyMemberNum, cCurSvrTime.GetHour() ) )
			return false;
	}

	// 진행 중인 퀘스트는 보여주도록 한다.
	if( SNpcTalk::EM_QUEST_START == spNpcTalk->m_nACTION )
	{
		for( int i = 0; i < MAX_QUEST_START; ++i )
		{
			DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[i];
			if( UINT_MAX == dwQuestID )
				continue;

			GLQUESTPROG* pQUEST_PROG = pChar->m_cQuestPlay.FindProc( dwQuestID );

			if( NULL == pQUEST_PROG )
				continue;

			GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
			if( NULL == pQuest )
				continue;

			if( pQUEST_PROG->m_dwSTEP == pQuest->m_vecProgStep.size() - 1 )
				return false;
			else
				return true;
		}
	}
	else if( SNpcTalk::EM_QUEST_STEP == spNpcTalk->m_nACTION )
	{
		DWORD dwQuestID = spNpcTalk->m_dwACTION_PARAM1;

		GLQUESTPROG* pQUEST_PROG = pChar->m_cQuestPlay.FindProc( dwQuestID );
		if( NULL != pQUEST_PROG )
		{
			DWORD dwTalkStep = spNpcTalk->m_dwACTION_PARAM2;
			if( pQUEST_PROG->m_dwSTEP == dwTalkStep )
				return true;
		}
	}

	return false;
}

bool GLNpcTalkClient::SelectBasic( std::tr1::shared_ptr< SNpcTalk > spNpcTalk )
{
	if( NULL == spNpcTalk )
		return false;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();

	DWORD dwMsg = spNpcTalk->m_dwACTION_NO;
	DWORD dwTalkID = spNpcTalk->m_dwGLOB_ID;

	GLLandManClient* pLand = pClient->GetActiveMap();
	if( NULL == pLand )
		return false;

	switch( m_emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pLand->GetSummon( m_dwNpcGlobalID );

			if( NULL == spCrow ) return false;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pLand->GetCrow( m_dwNpcGlobalID );

			if( NULL == spCrow ) return false;
		}
		break;
	default : return false;
	}

	switch( dwMsg )
	{
	case SNpcTalk::EM_STORAGE :
	case SNpcTalk::EM_BUSSTATION :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_NpcTalk_Open,
				"-b", false );
		}
		break;
	}

	// ReqNpcTalkBasic() 대화문 유효성 체크 (서버)
	switch( dwMsg )
	{
	case SNpcTalk::EM_STORAGE :
	case SNpcTalk::EM_MARKET :
	case SNpcTalk::EM_CURE :
	case SNpcTalk::EM_STARTPOINT :
	case SNpcTalk::EM_CHAR_RESET :
	case SNpcTalk::EM_CLUB_NEW :
	case SNpcTalk::EM_CLUB_UP :
	case SNpcTalk::EM_CD_CERTIFY :
	case SNpcTalk::EM_COMMISSION :
	case SNpcTalk::EM_ITEM_REBUILD :
	case SNpcTalk::EM_ODDEVEN :
	case SNpcTalk::EM_RECOVERY_EXP :
	case SNpcTalk::EM_ITEMSEARCH_PAGE :
	case SNpcTalk::EM_ATTENDANCE_BOOK :
	case SNpcTalk::EM_ITEM_MIX :
		{
			pChar->ReqNpcTalkBasic( m_dwNpcGlobalID, dwMsg, dwTalkID );
		}
		return false;

	case SNpcTalk::EM_ITEM_TRADE :
		{
			pChar->ReqItemTrade( m_dwNpcGlobalID, dwTalkID, spNpcTalk );
		}
		return false;

	case SNpcTalk::EM_BUSSTATION :
		{
			//!! 버스 UI 오픈
			GLBusStationClient::GetInstance().OpenBusStation( m_dwNpcGlobalID, dwTalkID, spNpcTalk );
		}
		return false;

	case SNpcTalk::EM_CLUB_STORAGE :
		{
			GLClubClient* pMyClub = pClient->GetMyClub();
			bool bVALID_CLUB = pMyClub->IsValid();
			bool bMASTER = pMyClub->MasterCharDbNum() == pChar->m_CharDbNum;

			if( true == bVALID_CLUB && true == bMASTER )
			{
				pChar->ReqNpcTalkBasic( m_dwNpcGlobalID, dwMsg, dwTalkID );
			}
			else
			{
				// 채팅창 메시지 출력
				GfxInterfaceBase::GetInstance()->GetGaeaClient()->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE,
					ID2GAMEINTEXT( "ONLY_CLUB_MASTER_USE_CLUBSTORAGE" ) );
			}
		}
		return false;

	case SNpcTalk::EM_RANDOM_PAGE :
		{
			int nCnt = MAX_RANDOM_PAGE;
			for( int i = 0; i < MAX_RANDOM_PAGE; ++i )
			{
				if( UINT_MAX == spNpcTalk->m_dwRandomPageID[i] )
				{
					nCnt = i;
					break;
				}
			}

			if( 0 >= nCnt )
			{
				sc::writeLogError(
					sc::string::format(
					"Random Page Not Found [%1%]",
					m_spDlgSet->GetDlgSetName() ) );
				break;
			}

			int nChoice = int ( rand() % nCnt );
			LoadNode( spNpcTalk->m_dwRandomPageID[ nChoice ], true, spNpcTalk->m_dwRandomTime );
		}
		return true;

	case SNpcTalk::EM_HIRE_SUMMON:
		{
			pChar->ReqHireSummon(m_dwNpcGlobalID, dwTalkID, spNpcTalk );
		}
		return true;
	}

	return false;
}

void GLNpcTalkClient::SelectQuestStart( std::tr1::shared_ptr< SNpcTalk > spNpcTalk )
{
	GASSERT( spNpcTalk && "GLNpcTalkClient::SelectQuestStart" );

	if( NULL == spNpcTalk )
		return;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();

	GLLandManClient* pLand = pClient->GetActiveMap();
	if( NULL == pLand )
		return;

	switch( m_emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pLand->GetSummon( m_dwNpcGlobalID );

			if( NULL == spCrow ) return;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pLand->GetCrow( m_dwNpcGlobalID );

			if( NULL == spCrow ) return;
		}
		break;
	default : return;
	}

	CTime cCurSvrTime = pClient->GetCurrentTime();
	for( int i = 0; i < MAX_QUEST_START; ++i )
	{
		if( UINT_MAX == spNpcTalk->m_dwQuestStartID[ i ] ||
			FALSE == spNpcTalk->DoTEST( pChar, i, cCurSvrTime.GetHour() ) )
			continue;

		pChar->ReqQuestStart( m_dwNpcGlobalID, spNpcTalk->m_dwGLOB_ID, spNpcTalk->m_dwQuestStartID[ i ] );
	}
}

void GLNpcTalkClient::SelectQuestStep( std::tr1::shared_ptr< SNpcTalk > spNpcTalk )
{
	GASSERT( spNpcTalk && "GLNpcTalkClient::SelectQuestStep" );

	if( NULL == spNpcTalk )
		return;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	GLLandManClient* pLand = pClient->GetActiveMap();
	if( NULL == pLand )
		return;

	switch( m_emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pLand->GetSummon( m_dwNpcGlobalID );

			if( NULL == spCrow ) return;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pLand->GetCrow( m_dwNpcGlobalID );

			if( NULL == spCrow ) return;
		}
		break;
	default : return;
	}

	pClient->GetCharacter()->ReqQuestStepNpcTalk( m_dwNpcGlobalID, spNpcTalk->m_dwGLOB_ID,
		spNpcTalk->m_dwACTION_PARAM1, spNpcTalk->m_dwACTION_PARAM2 );
}

bool GLNpcTalkClient::CheckRecommendItem( const SITEM* pItemData, const SITEMCUSTOM& sItemCustom, 
										  bool bTypeCheck, DWORD dwIndex, WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	if( true == bTypeCheck )
	{
		EMITEM_TYPE emItemType = pItemData->BasicType();

		if( emItemType == ITEM_ARROW || emItemType == ITEM_CURE || emItemType == ITEM_EFFECT_RELEASE_CURE || 
			emItemType == ITEM_CHARM || emItemType == ITEM_PRIVATEMARKET ||
			emItemType == ITEM_CD || emItemType == ITEM_BULLET )
			return false;
	}

	if( true == RANPARAM::bCheckUsableItemMarketPage )
	{
		// 착용 레벨 검사
		{
			if( pItemData->sBasicOp.wReqLevelDW != 0 && pChar->m_wLevel < pItemData->sBasicOp.wReqLevelDW )
				return false;
			if( pItemData->sBasicOp.wReqLevelUP != 0 && pChar->m_wLevel > pItemData->sBasicOp.wReqLevelUP )
				return false;
		}

		// 착용 스탯 검사
		{
			const SCHARSTATS& rItemStats = pItemData->sBasicOp.sReqStats;
			//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
			const SCHARSTATS& rCharStats = pChar->m_sSUMSTATS_SKILL;	
			// 착용조건->힘
			if( rCharStats.wPow + 3 <= rItemStats.wPow )
				return false;

			// 착용조건->체력
			if( rCharStats.wStr + 3 <= rItemStats.wStr )
				return false;

			// 착용조건->정신력
			if( rCharStats.wSpi + 3 <= rItemStats.wSpi)
				return false;

			// 착용조건->민첩성
			if( rCharStats.wDex + 3 <= rItemStats.wDex )
				return false;

			// 착용조건->지력
			if( rCharStats.wInt + 3 <= rItemStats.wInt )
				return false;

			// 착용조건->근력
			if( rCharStats.wSta + 3 <= rItemStats.wSta )
				return false;
		}

		// 착용 클래스 검사
		{
			if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
			{
				const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
				if( NULL == pItemDisguise )
					return false;

				DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
				DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;
				if( !( (dwReqCharClass & pChar->m_emClass) && (dwReqCharClass_Disguise & pChar->m_emClass) ) )
					return false;
			}
			else
			{
				DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;
				if( !(dwReqCharClass & pChar->m_emClass) )
					return false;
			}
		}

		// 사용 가능 학원 검사
		{
			if( GLSCHOOL_ALL != pItemData->sBasicOp.dwReqSchool )
			{
				if( !(pItemData->sBasicOp.dwReqSchool & index2school( pChar->m_wSchool )) )
					return false;
			}
		}

		// 요구 속성 검사
		{
			if( BRIGHT_BOTH != pItemData->sBasicOp.emReqBright )
			{
				if( pItemData->sBasicOp.emReqBright != pChar->GETBRIGHT() )
					return false;
			}
		}

		// 스킬 문서일 경우 검사
		{
			if( ITEM_SKILL == pItemData->BasicType() )
			{
				SNATIVEID sNativeID = sItemCustom.GetNativeID();
				const SITEM* pItemData = GLogicData::GetInstance().GetItem( sNativeID );
				SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
				if( NULL == pSkill )
					return false;

				//	이미 배운 스킬
				if( NULL != pChar->GETLEARNED_SKILL( sSkillID ) )
					return false;

				//	요구치 정보
				const WORD wLevel = 0;
				SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
				SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[wLevel];

				//	1. 요구보유스킬
				SNATIVEID NeedSkillID = pSkill->m_sLEARN.sSKILL;			
				if( NeedSkillID != NATIVEID_NULL() )
				{
					bool bVALID = false;
					bool bNeedSkillLevel = false;

					std::string strNeedSkillName;
					std::string strNeedSkillLevel;

					PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData( NeedSkillID.wMainID, NeedSkillID.wSubID );
					bVALID = ( TRUE == pChar->ISLEARNED_SKILL( NeedSkillID ) );

					//	2. 요구보유스킬레벨
					if( 0 < sLVL.dwSKILL_LVL )
					{
						bNeedSkillLevel = true;

						SKILL_MAP& map = pChar->m_ExpSkills;				
						SKILL_MAP_ITER iter = map.find( NeedSkillID.dwID );
						//	색 조절
						if( iter != map.end() )
						{
							SCHARSKILL& rCharSkill = (*iter).second;
							bVALID = rCharSkill.wLevel >= sLVL.dwSKILL_LVL;						
						}
					}

					if( false == bVALID )
						return false;

					if( true == bNeedSkillLevel )
					{
						if( true == bVALID )
							return false;
					}
				}

				//	3. 요구경험치
				if( 0 < sLVL.dwSKP )
				{
					if( pChar->m_dwSkillPoint < sLVL.dwSKP )
						return false;
				}

				//	4. 요구레벨
				if( 0 < sLVL.dwLEVEL )
				{
					if ( pChar->GETLEVEL() < int(sLVL.dwLEVEL) )
						return false;
				}

				//	Stats
				//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
				//	1. 요구힘
				if( 0 < sLVL.sSTATS.wPow )
				{
					if( pChar->m_sSUMSTATS_SKILL.wPow < sLVL.sSTATS.wPow )
						return false;
				}

				//	2. 요구체력
				if( 0 < sLVL.sSTATS.wStr )
				{
					if( pChar->m_sSUMSTATS_SKILL.wStr < sLVL.sSTATS.wStr )
						return false;
				}

				//	3. 요구정신
				if( 0 < sLVL.sSTATS.wSpi )
				{
					if( pChar->m_sSUMSTATS_SKILL.wSpi < sLVL.sSTATS.wSpi )
						return false;
				}

				//	4. 요구민첩
				if( 0 < sLVL.sSTATS.wDex )
				{
					if( pChar->m_sSUMSTATS_SKILL.wDex < sLVL.sSTATS.wDex )
						return false;
				}

				//	5. 요구지력
				if( 0 < sLVL.sSTATS.wInt )
				{
					if( pChar->m_sSUMSTATS_SKILL.wInt < sLVL.sSTATS.wInt )
						return false;
				}

				//	6. 요구근력
				if( 0 < sLVL.sSTATS.wSta )
				{
					if( pChar->m_sSUMSTATS_SKILL.wPow < sLVL.sSTATS.wSta )
						return false;
				}
			}
		}
	}

	if( false == RANPARAM::bCheckBuyableItemMarketPage )
		return true;

	// 교환 조건 체크
	return CheckExchange( pItemData, dwIndex, wPosX, wPosY );
}

bool GLNpcTalkClient::CheckExchange( const SITEM* pItemData, DWORD dwIndex, WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	if( NULL == pCrowData )
		return false;

	GLInventorySale* pInven = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( dwIndex ) );
	if( NULL == pInven )
		return false;

	// 교환 조건 검사
	if( GLInventorySale::CROW_SALE_V_2 == pInven->GetSaleFuncType() )
	{
		stBuyInvenItem sBuyItem( wPosX, wPosY, 1 );

		VECBUYINVENITEM vecCheckInvenItem;
		vecCheckInvenItem.push_back( sBuyItem );

		EEXTYPE emType = pInven->IsBuyableItemList( vecCheckInvenItem, 
			&pChar->m_cInventory, pChar->PointShopPoint(), pChar->m_nLiving );
		if( EX_NONE != emType )
			return false;
	}
	// 구입 가격 검사
	else
	{
		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
		if( NULL == pCrowData )
			return false;

		volatile double fShopRate = pChar->GetBuyRate();

		LONGLONG llPrice = pCrowData->GetNpcSellPrice( pItemData->sBasicOp.sNativeID.dwID );
		if( 0 == llPrice )
		{
			LONGLONG llBuyPrice = static_cast< LONGLONG >( pItemData->sBasicOp.dwBuyPrice );
			llPrice = llBuyPrice * static_cast< LONGLONG >( fShopRate ) / 100;
		}

		if( pChar->GetInvenMoney() < llPrice )
			return false;
	}

	return true;
}

void GLNpcTalkClient::CalcCartItemPrice()
{
	GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	if( NULL == pCrowData )
		return;

	m_llTotalCartPrice = 0;
	float fShopRate = pChar->GetBuyRate();

	VEC_NPCMARKET_CART_ITER iter = m_vecMarketCart.begin();
	for( ; iter != m_vecMarketCart.end(); ++iter )
	{
		const SITEM* pItemData = GLogicData::GetInstance().GetItem( iter->sID );
		if( NULL == pItemData )
			continue;

		LONGLONG llPrice = pCrowData->GetNpcSellPrice( pItemData->sBasicOp.sNativeID.dwID );
		if( llPrice == 0 )
		{
			LONGLONG llBuyPrice = static_cast< LONGLONG >( pItemData->sBasicOp.dwBuyPrice );
			llPrice = llBuyPrice * static_cast< LONGLONG >( fShopRate ) / 100;
		}

		llPrice *= iter->wCount;
		m_llTotalCartPrice += llPrice;
	}
}

bool GLNpcTalkClient::SelectTalk( DWORD dwAnswerID )
{
	if( NULL == m_spDlgCase )
		return false;

	std::tr1::shared_ptr< CNpcTalkControl > spNpcTalkControl = m_spDlgCase->GetTalkControl();
	if( NULL == spNpcTalkControl )
		return false;

	std::tr1::shared_ptr< SNpcTalk > spNpcTalk = spNpcTalkControl->GetTalk( dwAnswerID );
	if( NULL == spNpcTalk )
		return false;

	switch( spNpcTalk->m_nACTION )
	{
	case SNpcTalk::EM_DO_NOTHING :
		{
		}
		return false;

	case SNpcTalk::EM_PAGE_MOVE :
		{
			LoadNode( spNpcTalk->m_dwACTION_NO );
		}
		return true;

	case SNpcTalk::EM_BASIC :
		{
			return SelectBasic( spNpcTalk );
		}
		break;

	case SNpcTalk::EM_QUEST_START :
		{
			LoadNode( spNpcTalk->m_dwACTION_NO );
			SelectQuestStart( spNpcTalk );
		}
		return true;

	case SNpcTalk::EM_QUEST_STEP :
		{
			LoadNode( spNpcTalk->m_dwACTION_NO );
			SelectQuestStep( spNpcTalk );
		}
		return true;
	}

	return false;
}

LuaTable GLNpcTalkClient::GetSaleItem( DWORD dwIndex )
{
	LuaTable tbSaleItemList( GLWidgetScript::GetInstance().GetLuaState() );

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	if( NULL == pCrowData )
		return tbSaleItemList;

	const GLInventory* pInven = pCrowData->GetSaleInven( dwIndex );
	if( NULL == pInven )
		return tbSaleItemList;

	const GLInventory::CELL_MAP* pItemList = pInven->GetItemList();
	if( ESaleTotalSlot < pItemList->size () )
	{
		GASSERT( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
		return tbSaleItemList;
	}

	GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();

	int nSaleItemListIndex = 1;

	GLInventory::CELL_MAP_CITER iter = pItemList->begin();
	for( ; iter != pItemList->end(); ++iter )
	{
		const SINVENITEM* pInvenItem = (*iter).second;
		const SITEMCUSTOM& itemCustom = pInvenItem->sItemCustom;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem( itemCustom.GetNativeID() );
		if( NULL == pItemData )
			continue;

		LONGLONG llPrice = pCrowData->GetNpcSellPrice( pItemData->sBasicOp.sNativeID.dwID );
		if( llPrice == 0 )
			llPrice = pItemData->sBasicOp.dwBuyPrice;

		EMINVEN_USE_ITEM emItemMsg = EMINVEN_USE_ITEM_OK;
		bool bUseable = TRUE == pChar->SIMPLE_CHECK_ITEM( itemCustom, emItemMsg ) ? true : false;
		bool bExchangeable = CheckExchange( pItemData, dwIndex, pInvenItem->wPosX, pInvenItem->wPosY );

		LuaTable tbSaleItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbSaleItem.set( "nPosX", pInvenItem->wPosX );
		tbSaleItem.set( "nPosY", pInvenItem->wPosY );
		tbSaleItem.set( "MID", itemCustom.Mid() );
		tbSaleItem.set( "SID", itemCustom.Sid() );
		tbSaleItem.set( "bUseable", bUseable );
		tbSaleItem.set( "nPrice", llPrice );
		tbSaleItem.set( "bExchangeable", bExchangeable );

		tbSaleItemList.set( nSaleItemListIndex++, tbSaleItem );
	}

	return tbSaleItemList;
}

LuaTable GLNpcTalkClient::GetRcommendSaleItem( WORD wMaxCount, DWORD dwIndex )
{
	LuaTable tbRecommendItemList( GLWidgetScript::GetInstance().GetLuaState() );

	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return tbRecommendItemList;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	if( NULL == pCrowData )
		return tbRecommendItemList;

	const GLInventorySale* pInven = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( dwIndex ) );
	if( NULL == pInven )
		return tbRecommendItemList;

	const GLInventory::CELL_MAP* pItemList = pInven->GetItemList();

	WORD wCount = 0;

	GLInventory::CELL_MAP_CITER iter = pItemList->begin();
	for( ; iter != pItemList->end(); ++iter )
	{
		if( wMaxCount < wCount )
			break;

		const SINVENITEM* pInvenItem = (*iter).second;
		const SITEMCUSTOM& itemCustom = pInvenItem->sItemCustom;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem( itemCustom.GetNativeID() );
		if( NULL == pItemData )
			continue;

		if( false == CheckRecommendItem( pItemData, itemCustom, true, dwIndex, pInvenItem->wPosX, pInvenItem->wPosY ) )
			continue;

		LONGLONG llPrice = pCrowData->GetNpcSellPrice( pItemData->sBasicOp.sNativeID.dwID );
		if( llPrice == 0 )
			llPrice = pItemData->sBasicOp.dwBuyPrice;

		LuaTable tbRecommendItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbRecommendItem.set( "MID", itemCustom.Mid() );
		tbRecommendItem.set( "SID", itemCustom.Sid() );
		tbRecommendItem.set( "strName", itemCustom.GETNAME().c_str() );
		tbRecommendItem.set( "nPrice", llPrice );
		tbRecommendItem.set( "fBuyRate", pChar->GetBuyRate() );
		tbRecommendItem.set( "nPosX", pInvenItem->wPosX );
		tbRecommendItem.set( "nPosY", pInvenItem->wPosY );
		tbRecommendItem.set( "bExchange", pInven->GetSaleFuncType() == GLInventorySale::CROW_SALE_V_2 );
		tbRecommendItemList.set( ++wCount, tbRecommendItem );
	}

	return tbRecommendItemList;
}

std::string GLNpcTalkClient::GetBasicTalk()
{
	if( NULL == m_spDlgCase )
		return "";

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 	{
		INT nIndex = -1;
		const INT nSize(m_spDlg->m_vecCase.size());
		for ( INT i = 0; i < nSize; ++i )
		{
			if ( m_spDlgCase == m_spDlg->m_vecCase[i] )
			{
				nIndex = i;
				break;
			}
		}
 		CString strTalkKey;
 		strTalkKey.Format( "%s_%d_%d"
 			, m_spDlgSet->m_TalkName.c_str()
 			, m_spDlg->GetDlgNID()
 			, nIndex );
		return CGameTextMan::GetInstance().GetNPCTalkText(strTalkKey.GetString(), 0).GetString();
 	}
 	else
		return m_spDlgCase->GetBasicTalk();
}

LuaTable GLNpcTalkClient::GetAnswerList()
{
	LuaTable tbAnswerList( GLWidgetScript::GetInstance().GetLuaState() );

	int nAnswerListIndex = 1;

	std::multimap< int, int >::iterator iter = m_mapTalkIndex.begin();
	for( ; iter != m_mapTalkIndex.end(); ++iter )
	{
		std::tr1::shared_ptr< SNpcTalk > spNpcTalk = m_vecTalk[ iter->second ];

		std::string strAnswerTalk = spNpcTalk->GetTalk();
		std::string strTooltip = "";
		DWORD dwQuestBeginMoney = 0;

		// 퀘스트 시작, 진행중이면 퀘스트 보상 툴팁 넣어준다.
		switch( iter->first )
		{
		case TALK_QUEST_START :
			{
				// 비용이 있다면 추가
				DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[ 0 ];
				GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
				if( NULL == pQuest )
				{
					sc::writeLogError(
						sc::string::format(
						"quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID ) );
					break;
				}

				dwQuestBeginMoney = pQuest->m_dwBeginMoney;
			}

		case TALK_QUEST_ING :
			{
				DWORD dwQuestID = spNpcTalk->m_dwQuestStartID[ 0 ];
				GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
				if( NULL == pQuest )
				{
					sc::writeLogError(
						sc::string::format(
						"quest id = %1% 의 정보를 가져오지 못하였습니다.", dwQuestID ) );
					break;
				}

				if( !( pQuest->m_lnGiftEXP || pQuest->m_dwGiftMONEY ||
					   pQuest->m_dwGiftELEMENT || pQuest->m_dwGiftLIFEPOINT ||
					   pQuest->m_dwGiftSKILLPOINT || pQuest->m_dwGiftSTATSPOINT ||
					   pQuest->m_vecGiftITEM.size () || pQuest->m_vecGiftSKILL.size () ) )
				{
					break;
				}

				CString strCombine;

				if( pQuest->m_lnGiftEXP )
				{
					strCombine.Format( "\n%s : %I64d", ID2GAMEWORD( "QUEST_GIFT", 0 ), pQuest->m_lnGiftEXP );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_dwGiftMONEY )
				{
					strCombine.Format( "\n%s : %d", ID2GAMEWORD( "QUEST_GIFT", 1 ), pQuest->m_dwGiftMONEY );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_dwGiftELEMENT )
				{
					strCombine.Format( "\n%s : %d", ID2GAMEWORD( "QUEST_GIFT", 2 ), pQuest->m_dwGiftELEMENT );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_dwGiftLIFEPOINT )
				{
					strCombine.Format( "\n%s : %d", ID2GAMEWORD( "QUEST_GIFT", 3 ), pQuest->m_dwGiftLIFEPOINT );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_dwGiftSKILLPOINT )
				{
					strCombine.Format( "\n%s : %d", ID2GAMEWORD( "QUEST_GIFT", 5 ), pQuest->m_dwGiftSKILLPOINT );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_dwGiftSTATSPOINT )
				{
					strCombine.Format( "\n%s : %d", ID2GAMEWORD( "QUEST_GIFT", 6 ), pQuest->m_dwGiftSTATSPOINT );
					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_vecGiftITEM.size() )
				{
					strCombine.Format( "\n%s : ", ID2GAMEWORD( "QUEST_GIFT", 7 ) );

					INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin();
					INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end();
					for( ; iter != iter_end; )
					{
						SITEMCUSTOM& sItemCustom = (*iter);

						const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
						if( NULL == pItem )
							continue;

						strCombine += sItemCustom.GETNAME().c_str();

						++iter;
						if( iter == iter_end )
							break;

						strCombine += ", ";
					}

					strTooltip.append( strCombine.GetBuffer() );
				}

				if( pQuest->m_vecGiftSKILL.size() )
				{
					strCombine.Format( "\n%s : ", ID2GAMEWORD( "QUEST_GIFT", 8 ) );

					DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin ();
					DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end ();
					for( ; iter != iter_end; )
					{
						DWORD dwID = (*iter);

						PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( SNATIVEID(dwID) );
						if( NULL == pSKILL )
							continue;

						strCombine += pSKILL->GetName();

						++iter;
						if( iter == iter_end )
							break;

						strCombine += ", ";
					}

					strTooltip.append( strCombine.GetBuffer() );
				}
			}
			break;
		}

		LuaTable tbAnswer( GLWidgetScript::GetInstance().GetLuaState() );
		tbAnswer.set( "nAnswerID", spNpcTalk->m_dwNID );
		tbAnswer.set( "nAnswerType", (int) iter->first );
		tbAnswer.set( "strAnswer", strAnswerTalk.c_str() );
		tbAnswer.set( "strTooltip", strTooltip.c_str() );
		tbAnswer.set( "dwQuestBeginMoney", dwQuestBeginMoney );
		tbAnswer.set( "dwActionNo", spNpcTalk->m_dwACTION_NO );

		tbAnswerList.set( nAnswerListIndex++, tbAnswer );
	}

	return tbAnswerList;
}

LuaTable GLNpcTalkClient::GetMarketTab()
{
	LuaTable tbMarket( GLWidgetScript::GetInstance().GetLuaState() );

	if( false == m_bEnabledMarket )
		return tbMarket;

	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();

	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return tbMarket;

	SCROWDATA* pCrowData = NULL;

	switch( m_emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr< GLSummonClient > spCrow = pClient->GetActiveMap()->GetSummon( m_dwNpcGlobalID );

			if( NULL == spCrow ) return tbMarket;

			// NPC 유효성 체크
			if( false == spCrow->IsSummonNpcActionable(pChar->GetPosition(), pChar->GETBODYRADIUS() ) )
				return tbMarket;

			pCrowData = spCrow->m_pSummonCrowData;
			if( NULL == pCrowData )
				return tbMarket;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow = pClient->GetActiveMap()->GetCrow( m_dwNpcGlobalID );

			if( NULL == spCrow ) return tbMarket;

			// NPC 유효성 체크
			if( false == spCrow->IsNpcActionable(pChar->GetPosition(), pChar->GETBODYRADIUS() ) )
				return tbMarket;

			pCrowData = spCrow->m_pCrowData;
			if( NULL == pCrowData )
				return tbMarket;
		}
		break;
	default : return tbMarket;
	}
	
	bool bMarketTab = false;
	int nCurTabIndex = 1;

	for( int i = 0; i < SCROWACTION::SALENUM; ++i )
	{
		GLInventorySale* pInven = dynamic_cast< GLInventorySale* >( pCrowData->GetSaleInven( i ) );
		if( NULL == pInven )
			continue;

		const std::string& strFileName = pCrowData->m_sAction.m_strSaleFile[ i ];
		if( 0 != strFileName.length() )
		{
			bMarketTab = true;

			LuaTable tbMarketData( GLWidgetScript::GetInstance().GetLuaState() );
			tbMarketData.set( "nID", i );
			tbMarketData.set( "strName", pCrowData->m_strSaleType[ i ].c_str() );
			tbMarketData.set( "bRebuy", false );
			tbMarketData.set( "bExchange", pInven->GetSaleFuncType() == GLInventorySale::CROW_SALE_V_2 );

			tbMarket.set( nCurTabIndex++, tbMarketData );
		}
	}

	if( true == bMarketTab )
	{
		m_nRebuyTabIndex = nCurTabIndex;

		LuaTable tbMarketData( GLWidgetScript::GetInstance().GetLuaState() );
		tbMarketData.set( "nID", m_nRebuyTabIndex );
		tbMarketData.set( "strName", " " );
		tbMarketData.set( "bRebuy", true );
		tbMarketData.set( "bExchange", false );

		tbMarket.set( nCurTabIndex, tbMarketData );
	}

	return tbMarket;
}

void GLNpcTalkClient::SetRebuySupplies( bool bValue )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqCheckRebuySupplies( bValue );
	RANPARAM::bCheckRebuySuppliesRebuyPage = bValue;
}

void GLNpcTalkClient::BuyItem( DWORD dwPageIndex, WORD wPosX, WORD wPosY, WORD wBuyNum )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	if( NULL == pCrowData )
		return;

	GLInventory* pInven = pCrowData->GetSaleInven( dwPageIndex );
	if( NULL == pInven )
		return;

	GLInventorySale* pSale = dynamic_cast< GLInventorySale* >( pInven );
	if( NULL == pSale)
		return;

	switch( pSale->GetSaleFuncType() )
	{
	case GLInventorySale::CROW_SALE_V_2 :
		{
			GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC sNetData;
			sNetData.sNID = m_sNpcID;
			sNetData.dwNpcGlobID = m_dwNpcGlobalID;
			sNetData.emCrow = m_emCrow;

			sNetData.itemlist[ 0 ].dwChannel = dwPageIndex;
			sNetData.itemlist[ 0 ].wPosX = wPosX;
			sNetData.itemlist[ 0 ].wPosY = wPosY;
			sNetData.itemlist[ 0 ].wBuyNum = wBuyNum;

			pChar->ReqExChangeNpcTo( &sNetData );
		}
		break;

	default :
		{
			pChar->ReqNpcTo( m_sNpcID, m_dwNpcGlobalID, dwPageIndex, wPosX, wPosY, wBuyNum, m_emCrow );
		}
		break;
	}
}

void GLNpcTalkClient::SellItem( WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqNpcSaleTo( m_sNpcID, m_dwNpcGlobalID, wPosX, wPosY, m_emCrow );
}

void GLNpcTalkClient::AddCart( DWORD dwPageIndex, WORD wMID, WORD wSID, WORD wPosX, WORD wPosY, WORD wCount )
{
	SNpcMarketCart sCart( dwPageIndex, SNATIVEID( wMID, wSID ), wPosX, wPosY, wCount );
	m_vecMarketCart.push_back( sCart );

	CalcCartItemPrice();
}

void GLNpcTalkClient::RemoveCart( DWORD dwIndex )
{
	if( m_vecMarketCart.size() <= dwIndex )
		return;

	DWORD dwCount = 0;
	VEC_NPCMARKET_CART_ITER iter = m_vecMarketCart.begin();
	for( ; iter != m_vecMarketCart.end(); ++iter, ++dwCount )
	{
		if( dwCount == dwIndex )
		{
			m_vecMarketCart.erase( iter );
			break;
		}
	}

	CalcCartItemPrice();
}

void GLNpcTalkClient::RemoveCartAll()
{
	m_vecMarketCart.clear();
	CalcCartItemPrice();
}

bool GLNpcTalkClient::BuyCartItem(WORD wTabPage)
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	if( NULL == pChar )
		return false;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcID );
	// 이럴 일이있나?;
	if ( !pCrowData )
		return false;

	GLInventorySale *pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven ( wTabPage ));
	if ( !pInven )
		return false;

	// 돈으로 아이템을 사는 경우 카트리스트의 총액과 보유한금액을 비교하고 필요 금액보다 돈이 적다면;
	// 순차적으로 돈이되는대까지 사는게아니라, 아예 구입 시도를 하지않는다;
	// 헌데 교환의 경우는 기존에 살수있는대까지 사도록되어있었다;
	// 둘이 동작방식이 다르면안되므로 교환도 전체적으로 검사하도록 수정함;
	// 서버쪽 소스는 수정하지않았음;
	// 교환;
	if ( pInven->GetSaleFuncType() == GLInventorySale::CROW_SALE_V_2 )
	{
		VECBUYINVENITEM vecCheckInvenItem;
		GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC sNetData;
		sNetData.sNID		 = m_sNpcID;
		sNetData.dwNpcGlobID = m_dwNpcGlobalID;
		//sNetData.dwChannel   = wTabPage;
		sNetData.emCrow      = m_emCrow;

		VEC_NPCMARKET_CART_ITER iter = m_vecMarketCart.begin();
		for( INT nCount(0); iter != m_vecMarketCart.end(); ++iter, ++nCount )
		{
			SNpcMarketCart& sMarketCartUnit = *iter;

			sNetData.itemlist[nCount].dwChannel = sMarketCartUnit.dwIndex;
			sNetData.itemlist[nCount].wPosX     = static_cast<WORD>(sMarketCartUnit.wPosX);
			sNetData.itemlist[nCount].wPosY     = static_cast<WORD>(sMarketCartUnit.wPosY);
			sNetData.itemlist[nCount].wBuyNum   = static_cast<WORD>(sMarketCartUnit.wCount);

			stBuyInvenItem sItem(sMarketCartUnit.wPosX, sMarketCartUnit.wPosY, sMarketCartUnit.wCount);
			vecCheckInvenItem.push_back(sItem);
		}

		switch ( pInven->IsBuyableItemList(vecCheckInvenItem, &pChar->m_cInventory, pChar->PointShopPoint(), pChar->m_nLiving) )
		{
		case EX_ITEM_TYPE  : GLWidgetScript::GetInstance().m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_ITEM_EXCHANGE") );   return false;
		case EX_POINT_TYPE : GLWidgetScript::GetInstance().m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_RANPT_EXCHANGE") );  return false;
		case EX_LVP_TYPE   : GLWidgetScript::GetInstance().m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_LVPT_EXCHANGE") );   return false;
		case EX_NONE:
			{
				pChar->ReqExChangeNpcTo(&sNetData);
			}
			break;
		}
		
	}
	// 돈;
	else
	{
		if ( GetCartTotalPrice() > pChar->GetInvenMoney() )
		{
			GLWidgetScript::GetInstance().m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOMONEY") );
			return false;
		}

		VEC_NPCMARKET_CART_ITER iter = m_vecMarketCart.begin();
		for( ; iter != m_vecMarketCart.end(); ++iter )
			pChar->ReqNpcTo( m_sNpcID, m_dwNpcGlobalID, iter->dwIndex, iter->wPosX, iter->wPosY, iter->wCount, m_emCrow );
	}

	m_vecMarketCart.clear();
	return true;
}

bool GLNpcTalkClient::Rebuy( WORD wIndex )
{
	GLGaeaClient* pClient = GfxInterfaceBase::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return false;

	const RebuyItem::REBUY_VEC& vecRebuy = pChar->RebuyList();
	if( vecRebuy.size() <= wIndex )
		return false;

	if( pChar->GetInvenMoney() < vecRebuy[ wIndex ].nPrice )
	{
		pClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_NOT_ENOUGH_MONEY" ) );
		return false;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( vecRebuy[ wIndex ].sItemCustom.Id() );
	if( NULL == pItem )
		return false;

	GLMSG::SNET_BUY_REBUY_ITEM_CA NetMsg( vecRebuy[ wIndex ].sItemCustom, vecRebuy[ wIndex ].nPrice );
	pClient->NETSENDTOAGENT( &NetMsg );

	return true;
}

LuaTable GLNpcTalkClient::GetCart()
{
	LuaTable tbCart( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableCount = 1;

	VEC_NPCMARKET_CART_ITER iter = m_vecMarketCart.begin();
	for( ; iter != m_vecMarketCart.end(); ++iter )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( iter->sID );
		if( NULL == pItem )
			continue;

		WORD wCount = iter->wCount;
		if( 1 == wCount && true == pItem->ISPILE() )
			wCount = pItem->GETAPPLYNUM();

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( "wMID", iter->sID.Mid() );
		tbItem.set( "wSID", iter->sID.Sid() );
		tbItem.set( "wCount", wCount );
		tbCart.set( nTableCount++, tbItem );
	}

	return tbCart;
}

LuaTable GLNpcTalkClient::GetRebuy()
{
	LuaTable tbRebuy( GLWidgetScript::GetInstance().GetLuaState() );

	GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return tbRebuy;

	int nTableCount = 1;
	const RebuyItem::REBUY_VEC& vecRebuy = pChar->RebuyList();
	
	RebuyItem::REBUY_VEC_CITER iter = vecRebuy.begin();
	for( ; iter != vecRebuy.end(); ++iter )
	{
		const SITEM* pItemData = GLogicData::GetInstance().GetItem( iter->sItemCustom.GetBoxWrappedItemID() );
		if( NULL == pItemData )
			continue;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( "wMID", iter->sItemCustom.Mid() );
		tbItem.set( "wSID", iter->sItemCustom.Sid() );
		tbItem.set( "strName", iter->sItemCustom.GETNAME().c_str() );
		tbItem.set( "nCount", iter->sItemCustom.TurnNum() );
		tbItem.set( "nPrice", iter->nPrice );

		tbRebuy.set( nTableCount++, tbItem );
	}

	return tbRebuy;
}

WORD GLNpcTalkClient::GetRebuyCount()
{
	GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return 0;

	const RebuyItem::REBUY_VEC& vecRebuy = pChar->RebuyList();
	return (WORD) vecRebuy.size();
}

DWORD GLNpcTalkClient::GetAdItemID()
{
	return GLCONST_CHAR::nidDIALOGUE_RECOMMEND_ITEM.dwID;
}

std::string GLNpcTalkClient::GetAdItemSearchString()
{
	return GLCONST_CHAR::strDIALOGUE_RECOMMEND_ITEM;
}