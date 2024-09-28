#include "pch.h"

#include "../../RanLogicClient/Competition/Competition.h"

#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../RanGfxUI/GfxInterface.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../RanLogicClient/Tournament/MatchingJoinManagerClient.h"
#include "../../RanLogic/Tournament/GLTournamentClientInfo.h"

#include "../../RanLogic/Land/GLMapList.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../../RanLogicClient/PVE/GLPVEClient.h"
#include "../../RanLogicClient/TriggerSystem/ClientTriggerSystem.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogicClient/Level/GLLevelFileClient.h"
#include "../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

#include "../../RanLogicClient/ReferChar/ReferChar.h"

Competition::Competition(void)
{

}

Competition::Competition( const Competition& value )
{

}

Competition::~Competition(void)
{

}

Competition& Competition::GetInstance(void)
{
	static Competition m_Competition;
	return m_Competition;
}

bool Competition::FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable)
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLPVEClient* pPvEClient = pClient->GetPVEClient();
	if (pPvEClient)
	{
		return pPvEClient->FindTable(mid.dwID, nLevel, pTable);
	}

	return false;
}

bool Competition::GetSelCurTable(MAP_TABLE*& pTable, unsigned int nIndex)
{
	int nLevel = 0;
	TriggerMapInfo* pInfo = 0;

	if (GetSelCurInfo(pInfo, nLevel, nIndex))
	{
		return FindTable(pInfo->mapid.dwID, nLevel, pTable);
	}

	return false;
}

bool Competition::GetSelCurInfo(TriggerMapInfo*& info, int& level, unsigned int nIndex)
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLPVEClient* pPvEClient = pClient->GetPVEClient();
	if( pPvEClient )
	{
		size_t sel = nIndex;
		size_t cur = 0;

		TriggerSystem::TriggerMapInfoList& kMapInfo = pPvEClient->GetMapInfo();

		for (TriggerSystem::TriggerMapInfoListIt it = kMapInfo.begin(); it !=  kMapInfo.end(); ++it)
		{
			for (TriggerSystem::TriggerLevelInfoIt dit = it->levelinfo.begin(); dit != it->levelinfo.end(); ++dit, ++cur)
			{
				if (cur == sel)
				{
					info = &*it;
					level = dit->second.level;
					return true;
				}
			}
		}
	}

	return false;
}

//신규 인던 시스템으로 통합관리 되는 인던들에 대한 테이블 생성
LuaTable Competition::GetMatchingInstanceList ( const int _nContentsType )
{
	LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );
	
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	MatchingSystem::GLTouranmentClient* pTournament = pClient->GetTournamentClient();
	if( NULL == pTournament )
		return tbList;
	
	size_t nCount = 0;
	for( size_t i = 0; i < pTournament->m_VecMatchingInfo.size(); ++i )
	{
		LuaTable tbInstance( GLWidgetScript::GetInstance().GetLuaState() );

		const MatchingSystem::GLMatchingInfo* pInfo = pTournament->m_VecMatchingInfo[ i ];

		// PVP 나 PVE 가 아니라면 걸러준다;
		if ( ( pInfo->m_strContentsMainType == "RNCOMPETITION_MAIN_TYPE_PVP" && _nContentsType != 0 ) ||
			( pInfo->m_strContentsMainType == "RNCOMPETITION_MAIN_TYPE_PVE" && _nContentsType != 1 ) )
			continue;
		
		//XML ID
		tbInstance.set( 1, pInfo->m_strContentsMainType );	// 컨텐츠 대 분류 명칭;
		tbInstance.set( 2, pInfo->m_strContentsSubType );	// 컨텐츠 소 분류 명칭;
		tbInstance.set( 3, pInfo->m_strContentsName );		// 컨텐츠 이름;
		tbInstance.set( 4, pInfo->m_strMissionInfo );		// 컨텐츠 목표;
		tbInstance.set( 5, pInfo->m_strDetailInfo );		// 컨텐츠 상세 설명;
		tbInstance.set( 6, pInfo->m_strRewardInfo );		// 컨텐츠 보상;
		tbInstance.set( 7, pInfo->m_dwContentsMapID );		// 맵 이미지에 사용될 맵ID;
		tbInstance.set( 8, pInfo->m_ContentsType );			// 컨텐츠 타입 ( 일반, 토너먼트, 재입장 가능 일반 등 );
		tbInstance.set( 9, pInfo->m_ContentsStatus );		// 컨텐츠 상태 ( 신청 가능,불가능, 진행중 등 );
		tbInstance.set( 10, pInfo->m_ContentsDateType );	// 컨텐츠 진행시간 타입 ( 상시 식, 정기 식, 이벤트 식 등 );
		tbInstance.set( 11, pInfo->m_tContentsStartDate );	// 특정 시간 진행 (이벤트 식);
		tbInstance.set( 12, pInfo->m_tContentsEndDate );	// 특정 시간 종료 (이벤트 식);
		tbInstance.set( 13, pInfo->m_ScriptID );			// 스크립트 아이디(인던 입장 신청 용)

		//인던 시간 정보
		LuaTable tbTime( GLWidgetScript::GetInstance().GetLuaState() );
		for( size_t j = 0; j < pInfo->m_vecTimeInfo.size(); ++j )
		{
			LuaTable tbTimeInfo( GLWidgetScript::GetInstance().GetLuaState() );
			tbTimeInfo.set( 1, pInfo->m_vecTimeInfo[ j ].wWeekDay );
			tbTimeInfo.set( 2, pInfo->m_vecTimeInfo[ j ].wStartHour );
			tbTimeInfo.set( 3, pInfo->m_vecTimeInfo[ j ].wStartMin );
			tbTimeInfo.set( 4, pInfo->m_vecTimeInfo[ j ].dwDurationMin );
			tbTime.set( j + 1, tbTimeInfo );
		}
		tbInstance.set( 14, tbTime );
		
		bool isCompetitionJoined = false;
		MatchingSystem::MatchingClientInfo sTempMachingClientInfo;
		BOOST_FOREACH(MatchingSystem::MatchingClientInfo* m_ClientMatchingStatus, pTournament->m_ClientMatchingStatus  )
		{
			if( m_ClientMatchingStatus->ClientStatus == MatchingSystem::EMSTATUS_GROUP_IN )
				isCompetitionJoined = true;

			if( m_ClientMatchingStatus->ScriptID == pInfo->m_ScriptID )
			{
				sTempMachingClientInfo.ClientStatus = m_ClientMatchingStatus->ClientStatus;
				sTempMachingClientInfo.ScriptID = m_ClientMatchingStatus->ScriptID;
				break;
			}
		}
		
		if( pTournament->m_ClientMatchingStatus.size() == 0 )
		{
			sTempMachingClientInfo.ClientStatus = MatchingSystem::EMSTATUS_GROUP_OUT;
			sTempMachingClientInfo.ScriptID = pInfo->m_ScriptID;
		}
		
		tbInstance.set( 15, sTempMachingClientInfo.ClientStatus );
		tbInstance.set( 16, sTempMachingClientInfo.ScriptID );
		
		//추가 토너먼트 정보
		LuaTable tbTournamentInfo( GLWidgetScript::GetInstance().GetLuaState() );
		for( size_t j = 0; j < pInfo->m_GroupListInfo.size(); ++j )
		{
			LuaTable tbGroupInfo( GLWidgetScript::GetInstance().GetLuaState() );
			tbGroupInfo.set( 1, pInfo->m_GroupListInfo[ j ].dwGROUPID );
			tbGroupInfo.set( 2, pInfo->m_GroupListInfo[ j ].Position );
			tbGroupInfo.set( 3, pInfo->m_GroupListInfo[ j ].Index );
			tbGroupInfo.set( 4, pInfo->m_GroupListInfo[ j ].MatchingInstanceID );
			tbGroupInfo.set( 5, pInfo->m_GroupListInfo[ j ].ScriptID );
			tbGroupInfo.set( 6, pInfo->m_GroupListInfo[ j ].GroupName );
			tbGroupInfo.set( 7, pInfo->m_GroupListInfo[ j ].ClubName );
			tbGroupInfo.set( 8, pInfo->m_GroupListInfo[ j ].GroupSize );
			tbGroupInfo.set( 9, pInfo->m_GroupListInfo[ j ].GroupMaxSize );
			tbGroupInfo.set( 10, pInfo->m_GroupListInfo[ j ].CharLevel );
			tbGroupInfo.set( 11, pInfo->m_GroupListInfo[ j ].GroupClass );
			tbTournamentInfo.set( j + 1, tbGroupInfo );
		}
		tbInstance.set( 17, tbTournamentInfo );

		tbInstance.set( 18,  pInfo->m_dwMatchingSize );
		tbInstance.set( 19,  pInfo->m_dwCurrentRound );
		tbInstance.set( 20,  pInfo->m_bCanObserve );
		tbInstance.set( 21,  pInfo->m_bCanTOTO );

		int nNearestDayInterval = 0;	// 가장 가까운 진행까지 남은 날짜;
		int nNearestInterval = INT_MAX;	// 가장 가까운 진행까지 남은 분;
		int nNearestTime = 0;			// 가장 가까운 진행시간의 번호;

		switch( pInfo->m_ContentsDateType )
		{
		case MatchingSystem::EMMATCHING_TIME_ALWAYS:
			{
				nNearestTime = -1;
			}
			break;

		case MatchingSystem::EMMATCHING_TIME_PERIODIC :
			{
				CTime cCurTime = pClient->GetCurrentTime();
				// 서버 타임으로 변환( TimeZone 계산 );
				pClient->GetConvertServerTime( cCurTime );

				int nDayOfWeek = cCurTime.GetDayOfWeek ();
				int nDay = cCurTime.GetDay ();
				int nHour = cCurTime.GetHour();
				int nMinute = cCurTime.GetMinute ();

				const unsigned int numTimeInfo = pInfo->m_vecTimeInfo.size();
				for( unsigned int i = 0; i < numTimeInfo; ++i )
				{
					int nDayInterval = 0;
					if( pInfo->m_vecTimeInfo[ i ].wWeekDay != 0 )
					{
						nDayInterval = pInfo->m_vecTimeInfo[ i ].wWeekDay - nDayOfWeek;
						// 다음주에 열림;
						if( nDayInterval < 0 )
							nDayInterval += 7;
					}

					const int nHourInterval = pInfo->m_vecTimeInfo[ i ].wStartHour - nHour;
					const int nMinInterval = pInfo->m_vecTimeInfo[ i ].wStartMin - nMinute;

					int nInterval = nDayInterval * 24 * 60;
					nInterval += nHourInterval * 60;
					nInterval += nMinInterval;

					if( nInterval <= 0 )
					{
						// 진행중;
						if( -nInterval <= (int) pInfo->m_vecTimeInfo[ i ].dwDurationMin )
						{
							// 다음주 or 다음날 에 열림; 
							if( pInfo->m_ContentsType == MatchingSystem::EMMATCHING_CONTENTS_TOURNAMENT &&
								pInfo->m_ContentsStatus == MatchingSystem::EMMATCHING_CONTENTS_CLOSED )
							{
								nDayInterval = pInfo->m_vecTimeInfo[ i ].wWeekDay != 0 ? 7 : 1 ;
								nInterval += nDayInterval * 24 * 60;
							}
							else
							{
								nNearestTime = -1;
								break;
							}
						}
						// 진행시간 경과시;
						else
						{
							// 다음주 or 다음날 에 열림; 
							nDayInterval = pInfo->m_vecTimeInfo[ i ].wWeekDay != 0 ? 7 : 1 ;
							nInterval += nDayInterval * 24 * 60;
						}
					}

					if( nNearestInterval > nInterval )
					{
						nNearestDayInterval = nDayInterval;
						nNearestInterval = nInterval;
						nNearestTime = i;
					}
				}
			}
			break;
		}

		tbInstance.set( 22, nNearestDayInterval );
		tbInstance.set( 23, nNearestInterval );
		tbInstance.set( 24, nNearestTime );
		tbInstance.set( 25, pInfo->m_bEnableInfoButton );

		const DWORD dwJoinCount = MatchingSystem::CMatchingJoinManagerClient::Instance()
			->GetJoinCount( pInfo->m_ScriptID );
		DWORD dwCurJoinCount = pInfo->m_dwMaxJoinCount - dwJoinCount;
		if ( dwCurJoinCount < 0 )
			dwCurJoinCount = 0;

		tbInstance.set( 26, dwCurJoinCount );
		tbInstance.set( 27, pInfo->m_dwMaxJoinCount );
		tbInstance.set( 28, pInfo->m_bJoinInstanceDungeon );

		tbList.set( nCount, tbInstance );

		nCount++;
	}

	return tbList;
}

//신규 인던 시스템으로 통합관리 되지 않는 예외 인던들에 대한 테이블 생성
LuaTable Competition::GetExceptionPvPInstanceList(void)
{
	LuaTable tbInstance( GLWidgetScript::GetInstance().GetLuaState() );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	PVP::GLCaptureTheFieldClient* pCtf = pClient->GetCaptureTheField();

	// XML ID
	tbInstance.set( 1, "RNCOMPETITION_MAIN_TYPE_PVP" );				// 컨텐츠 대 분류 명칭;(예외 입력 신규인던 시스템으로 관리 안됨)
	tbInstance.set( 2, "RNCOMPETITION_SUB_TYPE_CTF" );				// 컨텐츠 소 분류 명칭;(예외 입력 신규인던 시스템으로 관리 안됨)
	tbInstance.set( 3, "RNCOMPETITION_CTF_OBJECT_INFO" );			// 컨텐츠 목표;
	tbInstance.set( 4, "COMPETITION_CTF_BATTLE_INFO_TEXT" );		// 컨텐츠 상세 설명;
	tbInstance.set( 5, "COMPETITION_CTF_BATTLE_AWARD_TEXT" );		// 컨텐츠 보상;

	// 컨텐츠 이름, 발전소 점령전 하나 뿐임
	PVP::GLCaptureTheFieldClient::CTFGLOBALSTATE& sState = pCtf->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	if( true == sState.IsNULL() )
		tbInstance.set( 6, "" );

	//인던 명칭
	GLLevelFileClient* pLevelFile = sState.spPVPMAPLEVELDATA.get();
	if( NULL != pLevelFile )
		tbInstance.set( 6, pClient->GetMapName ( pLevelFile->GetBaseMapID() )  );

	tbInstance.set( 7, pChar->GetCharSchool() );				// 현재 CTF 점령중인 학원
	tbInstance.set( 8, pCtf->GetProgressCTF() );				// 현재 CTF 진행중인 필드
	tbInstance.set( 9, PVP::ENUM_CTF_TYPE_POWERSTATION );		// 현재 CTF 타입
	tbInstance.set( 10, sState.IsNULL() );  
	tbInstance.set( 11, sState.nMAXPLAYERS );					// 진행 인원;
	tbInstance.set( 12, sState.wLEVELLIMIT );					// 참가 레벨;
	tbInstance.set( 13, sState.emSTATE );						// CTF 필드;
	tbInstance.set( 14, sState.emTYPE );						// CTF 타입;
	tbInstance.set( 15, sState.bConsecutivelyCapturel );		// 점령전 연속 가능 여부 
	tbInstance.set( 16, sState.bisConsecutively );				// 점령전 연속성
	tbInstance.set( 17, sState.emAUTHSCHOOL );					// 점령 학원

	// 입장가능여부 확인
	tbInstance.set( 18, sState.IsJOINDENIED( pChar ) );
	tbInstance.set( 19, sState.IsCANNOTJOIN( pChar->GetCharSchool() ) );
	tbInstance.set( 20, sState.bWAITJOIN );

	// 지난 CTF 상태
	const PVP::GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = pCtf->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	tbInstance.set( 21, sPrevState.IsNULL() );
	tbInstance.set( 22, sPrevState.emAUTHSCHOOL );		// 점령 학원

	// CTF 시간정보 
	CTime tCurTime( pClient->GetCurrentTime() );

	// 서버 타임으로 변환( TimeZone 계산 )
	pClient->GetConvertServerTime( tCurTime );

	CTimeSpan sTimeLeft( sState.tSTARTTIME - tCurTime.GetTime() );

	int nTotalMinutes = (int)(60 * sTimeLeft.GetTotalSeconds() / 3600);
	UINT nTimeHour = static_cast< UINT >( nTotalMinutes / 60 );
	UINT nTimeMinutes = static_cast< UINT >( nTotalMinutes % 60 );
	UINT nTimeSec = static_cast< UINT >( sTimeLeft.GetTotalSeconds() % 60 );

	//시간 출력 예외처리(서버에서 시간을 늦게 받아 올 시 쓰레기값 방지용)
	tbInstance.set( 23, (nTimeHour > 23 ? 0 : nTimeHour) );
	tbInstance.set( 24, (nTimeMinutes > 59 ? 0 : nTimeMinutes) );
	tbInstance.set( 25, nTimeSec );

	// 이전 CTF 랭커 정보
	LuaTable tbPrevRanker( GLWidgetScript::GetInstance().GetLuaState() );
	const PVP::VEC_CTF_RANKER& vecPrevRanker = sPrevState.vecRESULTDATA;

	PVP::VEC_CTF_RANKER::const_iterator iter = vecPrevRanker.begin();
	for( int i = 1; iter != vecPrevRanker.end(); ++iter, ++i )
	{
		LuaTable tbRanker( GLWidgetScript::GetInstance().GetLuaState() );
		tbRanker.set( 1, iter->wSchool );
		tbRanker.set( 2, iter->wClass );
		tbRanker.set( 3, iter->szName );

		tbPrevRanker.set( i, tbRanker );
	}
	tbInstance.set( 26, tbPrevRanker );

	// 이전 CTF 시간 정보
	int nPrevTime[ 4 ] = { 0 };
	if( false == sPrevState.IsNULL() && 0 != sPrevState.tSTARTTIME && 0 != sPrevState.tENDTIME )
	{
		CTime cStartTime( sPrevState.tSTARTTIME );
		CTime cEndTime( sPrevState.tENDTIME );

		pClient->GetConvertServerTime( cStartTime );
		pClient->GetConvertServerTime( cEndTime );

		nPrevTime[ 0 ] = cStartTime.GetHour();
		nPrevTime[ 1 ] = cStartTime.GetMinute();
		nPrevTime[ 2 ] = cEndTime.GetHour();
		nPrevTime[ 3 ] = cEndTime.GetMinute();
	}
	tbInstance.set( 27, nPrevTime[ 0 ] );
	tbInstance.set( 28, nPrevTime[ 1 ] );
	tbInstance.set( 29, nPrevTime[ 2 ] );
	tbInstance.set( 30, nPrevTime[ 3 ] );

	return tbInstance;
}

//기존 PvE 인던 관리 시스템으로 관리되는 항목들에 대한 테이블 생성
LuaTable Competition::GetPvEInstanceList(void)
{
	LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );
	// 단계형 인던 사용이 불가능하다면 세팅하지않고 던진다;
	if ( GLUseFeatures::GetInstance().IsUsingInfinityStairs() == false )
		return tbList;
	
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLPVEClient* pPvEClient = pClient->GetPVEClient();
	TriggerSystem::TriggerMapInfoList& kMapInfo = pPvEClient->GetMapInfo();

	TriggerSystem::TriggerMapInfoListIt iter = kMapInfo.begin();
	for( int i = 1; iter != kMapInfo.end(); ++iter, ++i )
	{
		LuaTable tbInstance( GLWidgetScript::GetInstance().GetLuaState() );
		
		// XML ID
		tbInstance.set( 1, "RNCOMPETITION_MAIN_TYPE_PVE" );		// 컨텐츠 대 분류 명칭;(예외 입력 신규인던 시스템으로 관리 안됨)
		tbInstance.set( 2, "RNCOMPETITION_SUB_TYPE_WAIL" );		// 컨텐츠 소 분류 명칭;(예외 입력 신규인던 시스템으로 관리 안됨)
		tbInstance.set( 3, iter->mapname );			// 컨텐츠 이름;
		tbInstance.set( 4, iter->mapObject );		// 컨텐츠 목표;
		tbInstance.set( 5, iter->mapinfoRn );		// 컨텐츠 상세 설명;
		tbInstance.set( 6, iter->mapreward );		// 컨텐츠 보상;
		tbInstance.set( 7, iter->dailylimit );		// 일일 입장 제한;
		tbInstance.set( 8, iter->owntime );			// 귀속 시간(초);
		tbInstance.set( 9, iter->person );			// 입장 인원 제한;
		tbInstance.set( 10, iter->waitentrance );   // 입장 확인 대기 시간;

		LuaTable tbLevel( GLWidgetScript::GetInstance().GetLuaState() );

		TriggerSystem::TriggerLevelInfoIt iterLevel = iter->levelinfo.begin();
		for( int j = 1; iterLevel != iter->levelinfo.end(); ++iterLevel, ++j )
		{
			int nCount = 0;
			_TIME_TYPE tTime = 0;

			MAP_TABLE* pTable = 0;
			if( true == FindTable( iter->mapid, iterLevel->second.level, pTable ) )
			{
				nCount = pTable->nInCount;
				tTime = pTable->tGenTime;
			}

			LuaTable tbLevelInfo( GLWidgetScript::GetInstance().GetLuaState() );
			tbLevelInfo.set( 1, iterLevel->second.level );
			tbLevelInfo.set( 2, iterLevel->second.minlevel );
			tbLevelInfo.set( 3, iterLevel->second.maxlevel );
			tbLevelInfo.set( 4, iterLevel->second.possible );
			tbLevelInfo.set( 5, nCount ); 
			tbLevelInfo.set( 6, tTime ); 

			tbLevel.set( j, tbLevelInfo );
		}

		tbInstance.set( 11, tbLevel );
		tbInstance.set( 12, iter->mapid.dwID );

		tbList.set( i, tbInstance );
	}

	return tbList;
}

bool Competition::IsJoined( DWORD dwScriptID )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	MatchingSystem::GLTouranmentClient* pTournament = pClient->GetTournamentClient();
	if( NULL == pTournament )
		return false;

	return pTournament->m_ClientMatchingStatus_Select.ScriptID == dwScriptID;
}

DWORD Competition::GetSelectStatus()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	MatchingSystem::GLTouranmentClient* pTournament = pClient->GetTournamentClient();
	if( NULL == pTournament )
		return -1;

	return pTournament->m_ClientMatchingStatus_Select.ClientStatus;
}

void Competition::SelectInstanceDungeon ( DWORD _dwMatchingBaseID, DWORD _dwStatus )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( NULL == pClient )
		return;

	MatchingSystem::GLTouranmentClient* pMatching = pClient->GetTournamentClient();
	if ( NULL == pMatching )
		return;

	pMatching->m_ClientMatchingStatus_Select.ScriptID = _dwMatchingBaseID;
	pMatching->m_ClientMatchingStatus_Select.ClientStatus = _dwStatus;
}

bool Competition::ReqCDMRankInfo( DWORD dwScriptID )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	if( dwScriptID != DWORD(-1) )
	{
		MatchingSystem::GLTouranmentClient* pTournament = pClient->GetTournamentClient();
		if( pTournament == NULL )
			return false;

		pTournament->SendCustomMessage( dwScriptID, MatchingSystem::EM_CUSTOM_MESSAGE_INFO_BUTTON, 0 );
		return true;
	}

	return false;
}

LuaTable Competition::GetClientTimeInfo(void)
{
	LuaTable tbTimeInfo( GLWidgetScript::GetInstance().GetLuaState() );
	
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	
	CTime cCurTime = pClient->GetCurrentTime();
	pClient->GetConvertServerTime( cCurTime );
	
	tbTimeInfo.set("nDayOfWeek", cCurTime.GetDayOfWeek () );
	tbTimeInfo.set("nDay", cCurTime.GetDay () );
	tbTimeInfo.set("nHour", cCurTime.GetHour() );
	tbTimeInfo.set("nMinute", cCurTime.GetMinute () );

	return tbTimeInfo;
}


void Competition::OnEnterance(const unsigned int nIndex)
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if (pClient)
	{
		int nLevel = 0;		
		TriggerMapInfo* pInfo = 0;

		if (GetSelCurInfo(pInfo, nLevel, nIndex) && pInfo->levelinfo[nLevel].possible)
		{
			MAP_TABLE* pTable = 0;

			if (GetSelCurTable(pTable, nIndex))
			{
				pClient->ReqPVEEntrance(pTable->dwCharDbNum, pInfo->mapid.dwID, pInfo->gateid, nLevel);
			}
			else
			{
				GLCharacter* pChar = pClient->GetCharacter();

				if (pChar)
				{
					pClient->ReqPVEEntrance(pChar->GetCharID(), pInfo->mapid.dwID, pInfo->gateid, nLevel);
				}
			}
		}
	}
}

void Competition::AckPVEEntrance( const bool bShow, DWORD dwMapID )
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Competition_PVE_AckEnternce,
		"-b, -dw", bShow, dwMapID );
}

void Competition::AckPVEEntranceConfirm( const BYTE byResult, const std::string &strChaName )
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Competition_PVE_AckEnternceConfirm, "-n, -s", byResult, strChaName.c_str() );
}

void Competition::OnEnteranceConfirm( const bool bEnterance )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if (pClient)
	{
		pClient->ReqPVEEntranceConfirm(bEnterance);
	}
}

void Competition::OnClearOfBelonging( const unsigned int nIndex )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if (pClient)
	{
		MAP_TABLE* pTable = 0;

		if (GetSelCurTable(pTable, nIndex) && pTable->tGenTime > 0.f)
		{
			pClient->ReqPVEClearOfBelonging(pTable->dwCharDbNum, pTable->dwMapID, pTable->nLevel);
		}
	}
}

void Competition::OnEnteranceTournament(const DWORD dwSelectID )
{
	if ( dwSelectID != DWORD(-1) )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		MatchingSystem::GLTouranmentClient* m_pLogic = pClient->GetTournamentClient();
		if(m_pLogic->m_ClientMatchingStatus.size() == 0)
		{
			m_pLogic->JoinTournament(dwSelectID);
			return;
		}

		BOOST_FOREACH(MatchingSystem::MatchingClientInfo* m_ClientMatchingStatus,m_pLogic->m_ClientMatchingStatus  )
		{
			if(m_ClientMatchingStatus->ScriptID == dwSelectID)
			{
				switch ( m_ClientMatchingStatus->ClientStatus )
				{
				case MatchingSystem::EMSTATUS_GROUP_IN:     // 입장 대기 중;
					m_pLogic->CanselJoinTournament();
					break;
				default:
					m_pLogic->JoinTournament(dwSelectID);
					break;
				}
				return;
			}
		}
	}
}

LuaTable Competition::GetCTFRewardFlag(void)
{
	LuaTable tbCTFInfo( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbWinnerReward( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbLoserReward( GLWidgetScript::GetInstance().GetLuaState() );
	
	tbWinnerReward.set( 1, PVP::ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER );
	tbWinnerReward.set( 2, PVP::ENUM_REWARD_TYPE_BUFF_PLAYER );
	tbCTFInfo.set( 1, tbWinnerReward );

	tbLoserReward.set( 1, PVP::ENUM_REWARD_TYPE_CONTRIBUTION_PLAYER );
	tbCTFInfo.set( 2, tbLoserReward );

	return tbCTFInfo;
}

void Competition::ReqCTFJoin()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( pClient )
	{
		if (pClient->GetCharacter()->IsDie())
			return; 
		pClient->ReqCaptureTheFieldJoin( PVP::ENUM_CTF_TYPE_POWERSTATION );
	}
}

void Competition::ReqCancel()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( pClient )
	{
		if (pClient->GetCharacter()->IsDie())
			return; 
		pClient->ReqCaptureTheFieldCancelJoin( PVP::ENUM_CTF_TYPE_POWERSTATION );
	}
}

void Competition::ReqExit()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( pClient )
	{
		if (pClient->GetCharacter()->IsDie())
			return; 
		pClient->ReqCaptureTheFieldQuit();
	}
}

LuaTable Competition::GetCTFRankInfo(void)
{
	LuaTable tbCTFRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	PVP::GLCaptureTheFieldClient* pCTFClient = pClient->GetCaptureTheField();
	PVP::GLCaptureTheFieldClient::CTFPREVSTATE& sState = pCTFClient->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	
	for ( UINT i=0; i<(UINT)sState.vecRESULTDATA.size(); ++i )
	{
		LuaTable tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
		PVP::CTF_RANKER_BASIC& sResultData = sState.vecRESULTDATA[i];
		tbRankInfo.set( "szName", sResultData.szName );
		tbRankInfo.set( "wClass", sResultData.wClass );
		tbRankInfo.set( "wSchool", sResultData.wSchool );

		tbCTFRankInfo.set( i+1, tbRankInfo );
	}

	return tbCTFRankInfo;
}

LuaTable Competition::GetCTFConsecutivelyCaptureSchool(void)
{
	LuaTable tbCTFConsecutivelyInfo( GLWidgetScript::GetInstance().GetLuaState() );
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	PVP::GLCaptureTheFieldClient* pCTFClient = pClient->GetCaptureTheField();
	PVP::GLCaptureTheFieldClient::CTFGLOBALSTATE& sState = pCTFClient->GetGlobalState(PVP::ENUM_CTF_TYPE_POWERSTATION);

	if ( sState.IsNULL() )
		return tbCTFConsecutivelyInfo;

	bool bConsecutively = false;
	PVP::GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = pCTFClient->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );

	if(sState.bConsecutivelyCapturel == FALSE)
	{
		if(sState.emSTATE == CTF_STATE_START)
		{
			if ( !sPrevState.IsNULL() )
			{
				if( sPrevState.emAUTHSCHOOL == pClient->GetCharacter()->GetCharSchool()
					&& sState.bConsecutivelyCapturel == FALSE
					&& sState.bisConsecutively == FALSE)
				{
					bConsecutively = true;	
				}
			}
		}
		else if( sState.emAUTHSCHOOL != SCHOOL_NONE )
		{
			if ( !sState.IsNULL() )
			{
				if( sState.emAUTHSCHOOL == pClient->GetCharacter()->GetCharSchool() 
					&& sState.bConsecutivelyCapturel == FALSE
					&& sState.bisConsecutively == FALSE)
				{
					bConsecutively = true;
				}
			}
		}
	}

	if ( bConsecutively == true )
	{
	//	if ( sPrevState.emAUTHSCHOOL == sState.emAUTHSCHOOL )
	//	{
			tbCTFConsecutivelyInfo.set( 1, sPrevState.emAUTHSCHOOL );
			tbCTFConsecutivelyInfo.set( 2, sState.dwConsecutivelyNumber );
	//	}
	}
	return tbCTFConsecutivelyInfo;
}

void Competition::ConfirmJoin_OK ()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( NULL == pClient )
		return;

	MatchingSystem::GLTouranmentClient* pMatching = pClient->GetTournamentClient();
	if ( NULL == pMatching )
		return;

	pMatching->AskJoinAnswer( true );
}

void Competition::ConfirmJoin_Cancel ()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( NULL == pClient )
		return;

	MatchingSystem::GLTouranmentClient* pMatching = pClient->GetTournamentClient();
	if ( NULL == pMatching )
		return;

	pMatching->AskJoinAnswer( false );
}

void Competition::SetCDMTopList (std::vector<DWORD> vecClubId,
									 LONGLONG lnStartTime,
									 LONGLONG lnEndTime,
									 std::vector<DWORD> vecRank )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	m_vecClubId.clear();
	m_vecRank.clear(); 

	m_vecClubId = vecClubId;
	m_lnStartTime = lnStartTime;
	m_lnEndTime = lnEndTime;
	m_vecRank = vecRank; 

	// CDM 이전 기록이 없다고 해도 CDM기록을 보여달라고함 (CDM참가자가 없었다는 정보를 알리기 위함);
	if ( //m_vecClubId.size() < 1 ||
		//m_vecRank.size() < 1 ||
		m_vecClubId.size()	!= m_vecRank.size() )
	{
		return;
	}

	GLMSG::SNET_CLUB_ID_2MASTERNAME_REQ_CA SendData;

	BOOST_FOREACH(DWORD dwClubId, m_vecClubId)
	{
		bool bFull = SendData.ADDID(dwClubId);

		if(!bFull)
			return;
	}

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);
	pClient->NETSENDTOAGENT(NET_MSG_CLUB_ID_2MASTERNAME_REQ_CA, SendBuffer);
}

void Competition::SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName )
{
	m_vecClubName.clear();
	m_vecMasterName.clear();
	m_vecClubName = vecClubName;
	m_vecMasterName = vecMasterName;
}

void Competition::SendCDMRankInfo()
{
	LuaTable tbCDMRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
	
	if( m_vecClubId.size() < 1 ||
		m_vecRank.size() < 1 ||
		m_vecClubName.size() < 1 ||
		m_vecMasterName.size() < 1 )
	{
		GLWidgetCallFuncValue arg;
		arg.SetTable( &tbCDMRankInfo );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Competition_CDMRankInfo, &arg, 1 );

		return;
	}

	struct STOPRANKDATA 
	{
		DWORD m_dwClubId;
		std::string m_strClubName;
		std::string m_strMasterName;

		STOPRANKDATA::STOPRANKDATA( DWORD dwClubId, std::string strClubName, std::string strMasterName )
			: m_dwClubId( dwClubId )
			, m_strClubName( strClubName )
			, m_strMasterName( strMasterName )
		{
		}
	};

	std::multimap< DWORD, STOPRANKDATA > mapTopRankData;
	for( unsigned i = 0; i < m_vecClubId.size(); ++i )
	{
		mapTopRankData.insert( std::make_pair( m_vecRank[ i ], 
			STOPRANKDATA( m_vecClubId[ i ], m_vecClubName[ i ], m_vecMasterName[ i ] ) ) );
	}

	std::multimap< DWORD, STOPRANKDATA >::iterator iter = mapTopRankData.begin();
	for( int i = 1; iter != mapTopRankData.end(); ++iter, ++i )
	{
		LuaTable tbRankInfo( GLWidgetScript::GetInstance().GetLuaState() );
		tbRankInfo.set( 1, iter->first );
		tbRankInfo.set( 2, iter->second.m_strClubName );
		tbRankInfo.set( 3, iter->second.m_strMasterName );
		tbRankInfo.set( 4, iter->second.m_dwClubId );
		
		tbCDMRankInfo.set( i, tbRankInfo );
	}
	
	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbCDMRankInfo );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_CDMRankInfo, &arg, 1 );
}

const bool Competition::ReqGetBettingInfo( const DWORD dwScriptID ) const
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * pTournament = pClient->GetTournamentClient();
		if(pTournament)
		{
			pTournament->GetBettingInfo(dwScriptID); 
		}
	}
	return false;
}

const bool Competition::ReqSetTournamentBettingMoney( const DWORD dwIndex, const LONGLONG llMoney, const DWORD dwScriptID ) const
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	
	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * pTournamentClient = pClient->GetTournamentClient();
		if(pTournamentClient)
		{
			if( 0 < llMoney )
			{	
				MatchingSystem::GLMatChingGroupInfo* __Bettingclient =
					pTournamentClient->GetBettingNormalInfo(dwIndex, dwScriptID);

				if(__Bettingclient == NULL)
					return false;

				if(__Bettingclient->dwGROUPID == -1)
					return false;

				//기존 로직을 변경하지 않게 하기 위해 현재 방식으로 변수 대입.
				pTournamentClient->m_bettingGroup = __Bettingclient->dwGROUPID;
				pTournamentClient->m_LeageId = __Bettingclient->ScriptID;

				pClient->GetTournamentClient()->SetBettingMoney(llMoney);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

const bool Competition::ReqCancelTournamentBettingMoney( const int nIndex ) const
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * Tourclient = pClient->GetTournamentClient();
		if(Tourclient)
		{
			MatchingSystem::GLMatChingGroupInfo* __Bettingclient =
				Tourclient->GetBettingNormalInfo( nIndex, Tourclient->m_SelectScriptID );

			if( __Bettingclient == NULL )
				return false;
			
			Tourclient->m_bettingGroup = __Bettingclient->dwGROUPID;
			Tourclient->m_LeageId = __Bettingclient->ScriptID;

			std::vector<MatchingSystem::Bettinginfo*>::iterator _bitor = Tourclient->m_MyBettingNormalInfo.begin();

			MatchingSystem::Bettinginfo* pMatchinginfo = NULL;

			for (; _bitor != Tourclient->m_MyBettingNormalInfo.end(); _bitor++)
			{
				MatchingSystem::Bettinginfo* _pMatchinginfo= NULL;
				_pMatchinginfo = *_bitor;
				if(_pMatchinginfo->ScriptID == __Bettingclient->ScriptID)
				{
					pMatchinginfo = _pMatchinginfo;
					break;
				}
			}

			if(__Bettingclient)
			{
				Tourclient->CanselBetting(__Bettingclient->ScriptID);
				return true;
			}
		}	
	}
	return false;
}

const bool Competition::ReqObserveTournament(const DWORD dwScriptID, const char* _nameCharacterTargetObserve)
{
	if(!_nameCharacterTargetObserve) 
		return false;

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * Tourclient = pClient->GetTournamentClient();
		if( Tourclient )
		{
			CString strTemp(_nameCharacterTargetObserve);
			Tourclient->ObserveTournament( dwScriptID, strTemp );
			return true;
		}
	}
	return false;
}

LuaTable Competition::GetBettingGroupInfo( const int nIndex ) const
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	LuaTable tbBettingInfo( GLWidgetScript::GetInstance().GetLuaState() );

	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * pTournamentClient = pClient->GetTournamentClient();
		if(pTournamentClient)
		{
			MatchingSystem::GLMatChingGroupInfo* pBettingclient =
				pTournamentClient->GetBettingNormalInfo( nIndex, pTournamentClient->m_SelectScriptID);

			if(pBettingclient)
			{
				tbBettingInfo.set( "nGROUPID", pBettingclient->dwGROUPID );
				tbBettingInfo.set( "dwPosition", pBettingclient->Position );
				tbBettingInfo.set( "dwIndex", pBettingclient->Index );
				tbBettingInfo.set( "dwMatchingInstanceID", pBettingclient->MatchingInstanceID );
				tbBettingInfo.set( "dwScriptID", pBettingclient->ScriptID );
				tbBettingInfo.set( "strGroupName", pBettingclient->GroupName );
				tbBettingInfo.set( "strClubName", pBettingclient->ClubName );
				tbBettingInfo.set( "dwGroupSize", pBettingclient->GroupSize );
				tbBettingInfo.set( "dwGroupMaxSize", pBettingclient->GroupMaxSize );
				tbBettingInfo.set( "dwCharLevel", pBettingclient->CharLevel );
				tbBettingInfo.set( "dwGroupClass", pBettingclient->GroupClass );
			}
		}	
	}

	return tbBettingInfo;
}

const bool Competition::ReqReferChar( const int nIndex, const DWORD dwScriptID ) const
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if(pClient)
	{
		MatchingSystem::GLTouranmentClient * pTournamentClient = pClient->GetTournamentClient();
		if(pTournamentClient)
		{
			pTournamentClient->m_SelectScriptID = dwScriptID;
			pTournamentClient->GetBettingInfo(dwScriptID);

			MatchingSystem::GLMatChingGroupInfo* pBettingclient =
				pTournamentClient->GetBettingNormalInfo( nIndex, pTournamentClient->m_SelectScriptID);

			if(pBettingclient)
			{
				pTournamentClient->GetBettingInfoEployee(pBettingclient->dwGROUPID);
				if( pBettingclient->GroupMaxSize == 1)
				{
					ReferChar::CClient* pReferChar = pClient->GetReferChar();
					if( !pReferChar )
						return false;

					pReferChar->Request( pClient->GetConnServerGroup(), pBettingclient->GroupName );
					return true;
				}
				else
				{
					//m_pInterface->OpenTournamentMoreInfoWindow(pBettingclient->dwGROUPID);
					return true;
				}
			}
		}
	}

	return false;
}
